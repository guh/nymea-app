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

#ifndef SIGV4UTILS_H
#define SIGV4UTILS_H

#include <QString>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

class SigV4Utils
{
public:
    SigV4Utils();

    // Signes a request by adding the "X-AMZ-Date" (if not present) and "X-AMZ-Signature" headers
    static void signRequest(QNetworkAccessManager::Operation operation, QNetworkRequest &request, const QString &region, const QString &service, const QByteArray &accessKeyId, const QByteArray &secretAccessKey, const QByteArray &sessionToken = QByteArray(), const QByteArray &payload = QByteArray());


    static QByteArray getCurrentDateTime();


    static QByteArray getCanonicalQueryString(const QNetworkRequest &request, const QByteArray &accessKeyId, const QByteArray &secretAccessKey, const QByteArray &sessionToken, const QByteArray &region, const QByteArray &service, const QByteArray &payload);
    static QByteArray getCanonicalRequest(QNetworkAccessManager::Operation operation, const QNetworkRequest &request, const QByteArray &payload);
    static QByteArray getCanonicalHeaders(const QNetworkRequest &request);
    static QByteArray getCredentialScope(const QByteArray &dateTime, const QByteArray &region, const QByteArray &service);
    static QByteArray getStringToSign(const QByteArray &canonicalRequest, const QByteArray &dateTime, const QByteArray &region, const QByteArray &service);
    static QByteArray getSignatureKey(const QByteArray &key, const QByteArray &date, const QByteArray &region, const QByteArray &service);
    static QByteArray getSignature(const QByteArray &stringToSign, const QByteArray &secretAccessKey, const QByteArray &dateTime, const QString &region, const QString &service);
    static QByteArray getAuthorizationHeader(const QByteArray &accessKeyId, const QByteArray &dateTime, const QString &region, const QString &service, const QNetworkRequest &request, const QByteArray &signature);

};

#endif // SIGV4UTILS_H
