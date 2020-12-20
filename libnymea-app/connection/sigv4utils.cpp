/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU General Public License as published by the Free Software
* Foundation, GNU version 3. This project is distributed in the hope that it
* will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "sigv4utils.h"

#include <QDateTime>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QtDebug>
#include <QUrlQuery>
#include <QList>

SigV4Utils::SigV4Utils()
{

}

void SigV4Utils::signRequest(QNetworkAccessManager::Operation operation, QNetworkRequest &request, const QString &region, const QString &service, const QByteArray &accessKeyId, const QByteArray &secretAccessKey, const QByteArray &sessionToken, const QByteArray &payload)
{
    QByteArray dateTime;
    if (request.rawHeaderList().contains("X-Amz-Date")) {
        dateTime = request.rawHeader("X-AMZ-Date");
    } else {
        dateTime = SigV4Utils::getCurrentDateTime();
        request.setRawHeader("X-Amz-Date", dateTime);
    }

    if (!sessionToken.isEmpty()) {
        request.setRawHeader("x-amz-security-token", sessionToken);
    }

    QByteArray canonicalRequest = SigV4Utils::getCanonicalRequest(operation, request, payload);
//    qDebug() << "canonical request:" << qUtf8Printable(canonicalRequest);
    QByteArray stringToSign = SigV4Utils::getStringToSign(canonicalRequest, dateTime, region.toUtf8(), service.toUtf8());
//    qDebug() << "string to sign:" << stringToSign;
    QByteArray signature = SigV4Utils::getSignature(stringToSign, secretAccessKey, dateTime, region, service);
//    qDebug() << "signature:" << signature;
    QByteArray authorizeHeader = SigV4Utils::getAuthorizationHeader(accessKeyId, dateTime, region, service, request, signature);

    request.setRawHeader("Authorization", authorizeHeader);
}

QByteArray SigV4Utils::getCurrentDateTime()
{
    return QDateTime::currentDateTime().toUTC().toString("yyyyMMddThhmmssZ").toUtf8();
}

QByteArray SigV4Utils::getCanonicalQueryString(const QNetworkRequest &request, const QByteArray &accessKeyId, const QByteArray &secretAccessKey, const QByteArray &sessionToken, const QByteArray &region, const QByteArray &service, const QByteArray &payload)
{
    QByteArray algorithm = "AWS4-HMAC-SHA256";
    QByteArray dateTime = getCurrentDateTime();
    QByteArray credentialScope = getCredentialScope(dateTime, region, service);

    QByteArray canonicalQueryString;
    canonicalQueryString += "X-Amz-Algorithm=AWS4-HMAC-SHA256";
    canonicalQueryString += "&X-Amz-Credential=" + QByteArray(accessKeyId + '/' + credentialScope).toPercentEncoding();
    canonicalQueryString += "&X-Amz-Date=" + dateTime;
    if (request.rawHeaderList().count() > 0){
        canonicalQueryString += "&X-Amz-SignedHeaders=" + request.rawHeaderList().join(';').toLower();
    }

    QByteArray canonicalRequest = getCanonicalRequest(QNetworkAccessManager::GetOperation, request, payload);

    QByteArray stringToSign = getStringToSign(canonicalRequest, dateTime, region, service);
    QByteArray signature = getSignature(stringToSign, secretAccessKey, dateTime, region, service);

    canonicalQueryString += "&X-Amz-Signature=" + signature;

    if (!sessionToken.isEmpty()) {
        canonicalQueryString += "&X-Amz-Security-Token=" + sessionToken.toPercentEncoding();
    }

    return canonicalQueryString;
}

QByteArray SigV4Utils::getSignatureKey(const QByteArray &key, const QByteArray &date, const QByteArray &region, const QByteArray &service)
{
    QCryptographicHash::Algorithm hashAlgorithm = QCryptographicHash::Sha256;
    return QMessageAuthenticationCode::hash("aws4_request",
           QMessageAuthenticationCode::hash(service,
           QMessageAuthenticationCode::hash(region,
           QMessageAuthenticationCode::hash(date, "AWS4"+key,
           hashAlgorithm), hashAlgorithm), hashAlgorithm), hashAlgorithm);
}

QByteArray SigV4Utils::getCanonicalRequest(QNetworkAccessManager::Operation operation, const QNetworkRequest &request, const QByteArray &payload)
{
    QByteArray canonicalRequest;

    QByteArray method;
    switch (operation) {
    case QNetworkAccessManager::GetOperation:
        method = "GET";
        break;
    case QNetworkAccessManager::PostOperation:
        method = "POST";
        break;
    default:
        Q_ASSERT_X(false, "Network operation not implemented", "SigV4Utils");
    }
    QByteArray uri = request.url().path(QUrl::FullyEncoded).toUtf8();
    QUrlQuery query(request.url());
    QList<QPair<QString, QString> > queryItems = query.queryItems();
    QStringList queryItemStrings;
    for (int i = 0; i < queryItems.count(); i++) {
        QPair<QString, QString> queryItem = queryItems.at(i);
        queryItemStrings.append(queryItem.first + '=' + queryItem.second);
    }
    queryItemStrings.sort(Qt::CaseInsensitive);

    QByteArray canonicalQueryString = queryItemStrings.join('&').toUtf8();

    QByteArray canonicalHeaders;
    foreach(const QByteArray &headerName, request.rawHeaderList()) {
        canonicalHeaders += headerName.toLower() + ':' + request.rawHeader(headerName) + '\n';
    }

    QByteArray payloadHash = QCryptographicHash::hash(payload, QCryptographicHash::Sha256).toHex();

    canonicalRequest = method + '\n' + uri + '\n' + canonicalQueryString + '\n' + canonicalHeaders + '\n' + request.rawHeaderList().join(';').toLower() + '\n' + payloadHash;
    return canonicalRequest;
}

QByteArray SigV4Utils::getCredentialScope(const QByteArray &dateTime, const QByteArray &region, const QByteArray &service)
{
    QByteArray credentialScope = dateTime.left(8) + '/' + region + '/' + service + "/aws4_request";
    return credentialScope;
}

QByteArray SigV4Utils::getStringToSign(const QByteArray &canonicalRequest, const QByteArray &dateTime, const QByteArray &region, const QByteArray &service)
{
    QByteArray algorithm = "AWS4-HMAC-SHA256";
    QByteArray credentialScope = getCredentialScope(dateTime, region, service);

    QByteArray stringToSign = algorithm + '\n' + dateTime + '\n' + credentialScope + '\n' + QCryptographicHash::hash(canonicalRequest, QCryptographicHash::Sha256).toHex();
    return stringToSign;
}

QByteArray SigV4Utils::getSignature(const QByteArray &stringToSign, const QByteArray &secretAccessKey, const QByteArray &dateTime, const QString &region, const QString &service)
{
    QByteArray signingKey = getSignatureKey(secretAccessKey, dateTime.left(8), region.toUtf8(), service.toUtf8());
    QByteArray signature = QMessageAuthenticationCode::hash(stringToSign, signingKey, QCryptographicHash::Sha256).toHex();
    return signature;
}

QByteArray SigV4Utils::getAuthorizationHeader(const QByteArray &accessKeyId, const QByteArray &dateTime, const QString &region, const QString &service, const QNetworkRequest &request, const QByteArray &signature)
{
    QByteArray authHeader = "AWS4-HMAC-SHA256 Credential=" + accessKeyId + '/' + dateTime.left(8) + '/' + region.toUtf8() + '/' + service.toUtf8() + '/' + "aws4_request, SignedHeaders=" + request.rawHeaderList().join(';').toLower() + ", Signature=" + signature;
    return authHeader;
}
