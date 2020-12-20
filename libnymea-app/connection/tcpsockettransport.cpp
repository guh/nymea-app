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

#include "tcpsockettransport.h"

#include <QUrl>
#include <QSslConfiguration>

TcpSocketTransport::TcpSocketTransport(QObject *parent) : NymeaTransportInterface(parent)
{
    QObject::connect(&m_socket, &QSslSocket::connected, this, &TcpSocketTransport::onConnected);
    QObject::connect(&m_socket, &QSslSocket::encrypted, this, &TcpSocketTransport::onEncrypted);
    typedef void (QSslSocket:: *sslErrorsSignal)(const QList<QSslError> &);
    QObject::connect(&m_socket, static_cast<sslErrorsSignal>(&QSslSocket::sslErrors), this, &TcpSocketTransport::sslErrors);
    QObject::connect(&m_socket, &QSslSocket::readyRead, this, &TcpSocketTransport::socketReadyRead);
#if QT_VERSION >=  QT_VERSION_CHECK(5, 15, 0)
    QObject::connect(&m_socket, &QSslSocket::errorOccurred, this, &TcpSocketTransport::error);
#else
    typedef void (QSslSocket:: *errorSignal)(QAbstractSocket::SocketError);
    QObject::connect(&m_socket, static_cast<errorSignal>(&QSslSocket::error), this, &TcpSocketTransport::error);
#endif
    QObject::connect(&m_socket, &QSslSocket::stateChanged, this, &TcpSocketTransport::onSocketStateChanged);

}

void TcpSocketTransport::sendData(const QByteArray &data)
{
    qint64 ret = m_socket.write(data);
    if (ret != data.length()) {
        qWarning() << "Error writing data to socket.";
    }
}

void TcpSocketTransport::ignoreSslErrors(const QList<QSslError> &errors)
{
    m_socket.ignoreSslErrors(errors);
}

bool TcpSocketTransport::isEncrypted() const
{
    return m_socket.isEncrypted();
}

QSslCertificate TcpSocketTransport::serverCertificate() const
{
    return m_socket.peerCertificate();
}

void TcpSocketTransport::onConnected()
{
    if (m_url.scheme() == "nymea") {
        qDebug() << "TCP socket connected";
        emit connected();
    }
}

void TcpSocketTransport::onEncrypted()
{
    qDebug() << "TCP socket encrypted";
    emit connected();
}

bool TcpSocketTransport::connect(const QUrl &url)
{
    m_url = url;
    if (url.scheme() == "nymeas") {
        qDebug() << "TCP socket connecting to" << url.host() << url.port();
        m_socket.connectToHostEncrypted(url.host(), static_cast<quint16>(url.port()));
        return true;
    } else if (url.scheme() == "nymea") {
        m_socket.connectToHost(url.host(), static_cast<quint16>(url.port()));
        return true;
    }
    qWarning() << "TCP socket: Unsupported scheme";
    return false;
}

QUrl TcpSocketTransport::url() const
{
    return m_url;
}

NymeaTransportInterface::ConnectionState TcpSocketTransport::connectionState() const
{
    switch (m_socket.state()) {
    case QAbstractSocket::ConnectedState:
        return NymeaTransportInterface::ConnectionStateConnected;
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::HostLookupState:
        return NymeaTransportInterface::ConnectionStateConnecting;
    default:
        return NymeaTransportInterface::ConnectionStateDisconnected;
    }
}

void TcpSocketTransport::disconnect()
{
    qDebug() << "closing socket";
    m_socket.disconnectFromHost();
    m_socket.close();
    // QTcpSocket might endlessly wait for a timeout if we call connectToHost() for an IP which isn't
    // reable at all (e.g. has disappeared from the network). Closing the socket is not enough, we need
    // abort the exiting connection attempts too.
    m_socket.abort();
}

void TcpSocketTransport::socketReadyRead()
{
    QByteArray data = m_socket.readAll();
    emit dataReady(data);
}

void TcpSocketTransport::onSocketStateChanged(const QAbstractSocket::SocketState &state)
{
    qDebug() << "Socket state changed -->" << state;
    if (state == QAbstractSocket::UnconnectedState) {
        emit disconnected();
    }
}

NymeaTransportInterface *TcpSocketTransportFactory::createTransport(QObject *parent) const
{
    return new TcpSocketTransport(parent);
}

QStringList TcpSocketTransportFactory::supportedSchemes() const
{
    return {"nymea", "nymeas"};
}
