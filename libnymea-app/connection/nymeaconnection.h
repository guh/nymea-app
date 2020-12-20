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

#ifndef NYMEACONNECTION_H
#define NYMEACONNECTION_H

// QNetworkConfigurationManager is deprecated as of 5.15 and there is no replacement yet
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <QObject>
#include <QHash>
#include <QSslError>
#include <QAbstractSocket>
#include <QUrl>
#include <QNetworkConfigurationManager>

#include "nymeahost.h"

class NymeaTransportInterface;
class NymeaTransportInterfaceFactory;

class NymeaConnection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(NymeaHost* currentHost READ currentHost WRITE setCurrentHost NOTIFY currentHostChanged)
    Q_PROPERTY(Connection* currentConnection  READ currentConnection NOTIFY currentConnectionChanged)
    Q_PROPERTY(NymeaConnection::BearerTypes availableBearerTypes READ availableBearerTypes NOTIFY availableBearerTypesChanged)
    Q_PROPERTY(ConnectionStatus connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)

public:
    enum BearerType {
        BearerTypeNone = 0x0,
        BearerTypeEthernet = 0x1,
        BearerTypeWiFi = 0x2,
        BearerTypeMobileData = 0x4,
        BearerTypeBluetooth = 0x8,
        BearerTypeAll = 0xF
    };
    Q_ENUM(BearerType)
    Q_DECLARE_FLAGS(BearerTypes, BearerType)
    Q_FLAG(BearerTypes)

    enum ConnectionStatus {
        ConnectionStatusUnconnected,
        ConnectionStatusConnecting,
        ConnectionStatusNoBearerAvailable,
        ConnectionStatusBearerFailed,
        ConnectionStatusHostNotFound,
        ConnectionStatusConnectionRefused,
        ConnectionStatusRemoteHostClosed,
        ConnectionStatusTimeout,
        ConnectionStatusSslError,
        ConnectionStatusSslUntrusted,
        ConnectionStatusUnknownError,
        ConnectionStatusConnected
    };
    Q_ENUM(ConnectionStatus)
    explicit NymeaConnection(QObject *parent = nullptr);

    void registerTransport(NymeaTransportInterfaceFactory *transportFactory);

    Q_INVOKABLE void connectToHost(NymeaHost* nymeaHost, Connection *connection = nullptr);
    Q_INVOKABLE void disconnectFromHost();

    bool isEncrypted() const;
    QSslCertificate sslCertificate() const;

    NymeaConnection::BearerTypes availableBearerTypes() const;

    bool connected();
    ConnectionStatus connectionStatus() const;

    NymeaHost* currentHost() const;
    void setCurrentHost(NymeaHost *host);

    Connection* currentConnection() const;


    void sendData(const QByteArray &data);

signals:
    void availableBearerTypesChanged();
    void verifyConnectionCertificate(const QString &url, const QStringList &issuerInfo, const QByteArray &fingerprint, const QByteArray &pem);
    void currentHostChanged();
    void connectedChanged(bool connected);
    void connectionStatusChanged();
    void currentConnectionChanged();
    void dataAvailable(const QByteArray &data);

private slots:
    void onSslErrors(const QList<QSslError> &errors);
    void onError(QAbstractSocket::SocketError error);
    void onConnected();
    void onDisconnected();

    void updateActiveBearers();
private:
    void connectInternal(NymeaHost *host);
    bool connectInternal(Connection *connection);

    NymeaConnection::BearerType qBearerTypeToNymeaBearerType(QNetworkConfiguration::BearerType type) const;

    bool isConnectionBearerAvailable(Connection::BearerType connectionBearerType) const;

private:
    ConnectionStatus m_connectionStatus = ConnectionStatusUnconnected;
    QNetworkConfigurationManager *m_networkConfigManager = nullptr;
    NymeaConnection::BearerTypes m_availableBearerTypes = BearerTypeNone;

    QHash<QString, NymeaTransportInterfaceFactory*> m_transportFactories;
    QHash<NymeaTransportInterface*, Connection*> m_transportCandidates;
    NymeaTransportInterface *m_currentTransport = nullptr;
    NymeaHost *m_currentHost = nullptr;
    Connection *m_preferredConnection = nullptr;
};

#pragma GCC diagnostic pop

#endif // NYMEACONNECTION_H
