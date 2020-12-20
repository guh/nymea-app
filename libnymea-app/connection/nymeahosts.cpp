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

#include "nymeahosts.h"
#include "connection/discovery/nymeadiscovery.h"
#include "nymeahost.h"
#include "jsonrpc/jsonrpcclient.h"
#include <QUuid>

NymeaHosts::NymeaHosts(QObject *parent) :
    QAbstractListModel(parent)
{
}

int NymeaHosts::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_hosts.count();
}

QVariant NymeaHosts::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_hosts.count())
        return QVariant();

    NymeaHost *host = m_hosts.at(index.row());
    switch (role) {
    case UuidRole:
        return host->uuid();
    case NameRole:
        return host->name();
    case VersionRole:
        return host->version();
    }
    return QVariant();
}

void NymeaHosts::addHost(NymeaHost *host)
{
    for (int i = 0; i < m_hosts.count(); i++) {
        if (m_hosts.at(i)->uuid() == host->uuid()) {
            qWarning() << "Host already added. Update existing host instead.";
            return;
        }
    }
    host->setParent(this);
    connect(host, &NymeaHost::connectionChanged, this, &NymeaHosts::hostChanged);

    beginInsertRows(QModelIndex(), m_hosts.count(), m_hosts.count());
    m_hosts.append(host);
    endInsertRows();
    emit hostAdded(host);
    emit countChanged();
}

void NymeaHosts::removeHost(NymeaHost *host)
{
    int idx = m_hosts.indexOf(host);
    if (idx == -1) {
        qWarning() << "Cannot remove NymeaHost" << host << "as its nit in the model";
        return;
    }
    beginRemoveRows(QModelIndex(), idx, idx);
    m_hosts.takeAt(idx);
    endRemoveRows();
    emit hostRemoved(host);
    emit countChanged();
}

NymeaHost *NymeaHosts::createCloudHost(const QString &name, const QUrl &url)
{
    return createHost(name, url, Connection::BearerTypeCloud);
}

NymeaHost *NymeaHosts::createLanHost(const QString &name, const QUrl &url)
{
    if (QHostAddress(url.host()).isLoopback()) {
        return createHost(name, url, Connection::BearerTypeLoopback);
    }
    return createHost(name, url, Connection::BearerTypeLan);
}

NymeaHost *NymeaHosts::createWanHost(const QString &name, const QUrl &url)
{
    return createHost(name, url, Connection::BearerTypeWan);
}

NymeaHost *NymeaHosts::createHost(const QString &name, const QUrl &url, Connection::BearerType bearerType)
{
    NymeaHost *host = new NymeaHost(this);
    host->setName(name);
    Connection *connection = new Connection(url, bearerType, false, url.toString(), host);
    connection->setOnline(true);
    host->connections()->addConnection(connection);
    addHost(host);
    return host;
}

NymeaHost *NymeaHosts::get(int index) const
{
    if (index < 0 || index >= m_hosts.count()) {
        return nullptr;
    }
    return m_hosts.at(index);
}

NymeaHost *NymeaHosts::find(const QUuid &uuid)
{
    foreach (NymeaHost *dev, m_hosts) {
        if (dev->uuid() == uuid) {
            return dev;
        }
    }
    return nullptr;
}

void NymeaHosts::clearModel()
{
    beginResetModel();
    m_hosts.clear();
    endResetModel();
    emit countChanged();
}

QHash<int, QByteArray> NymeaHosts::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UuidRole] = "uuid";
    roles[NameRole] = "name";
    roles[VersionRole] = "version";
    return roles;
}

NymeaHostsFilterModel::NymeaHostsFilterModel(QObject *parent):
    QSortFilterProxyModel(parent)
{

}

NymeaDiscovery *NymeaHostsFilterModel::discovery() const
{
    return m_nymeaDiscovery;
}

void NymeaHostsFilterModel::setDiscovery(NymeaDiscovery *discovery)
{
    if (m_nymeaDiscovery != discovery) {
        m_nymeaDiscovery = discovery;
        setSourceModel(discovery->nymeaHosts());
        emit discoveryChanged();

        connect(discovery->nymeaHosts(), &NymeaHosts::hostChanged, this, [this](){
//            qDebug() << "Host Changed!";
            invalidateFilter();
            emit countChanged();
        });

        emit countChanged();
    }
}

JsonRpcClient *NymeaHostsFilterModel::jsonRpcClient() const
{
    return m_jsonRpcClient;
}

void NymeaHostsFilterModel::setJsonRpcClient(JsonRpcClient *jsonRpcClient)
{
    if (m_jsonRpcClient != jsonRpcClient) {
        m_jsonRpcClient = jsonRpcClient;
        emit jsonRpcClientChanged();

        connect(m_jsonRpcClient, &JsonRpcClient::availableBearerTypesChanged, this, [this](){
//            qDebug() << "Bearer Types Changed!";
            invalidateFilter();
            emit countChanged();
        });

        invalidateFilter();
        emit countChanged();
    }
}

bool NymeaHostsFilterModel::showUnreachableBearers() const
{
    return m_showUneachableBearers;
}

void NymeaHostsFilterModel::setShowUnreachableBearers(bool showUnreachableBearers)
{
    if (m_showUneachableBearers != showUnreachableBearers) {
        m_showUneachableBearers = showUnreachableBearers;
        emit showUnreachableBearersChanged();
        invalidateFilter();
        emit countChanged();
    }
}

bool NymeaHostsFilterModel::showUnreachableHosts() const
{
    return m_showUneachableHosts;
}

void NymeaHostsFilterModel::setShowUnreachableHosts(bool showUnreachableHosts)
{
    if (m_showUneachableHosts != showUnreachableHosts) {
        m_showUneachableHosts = showUnreachableHosts;
        emit showUnreachableHostsChanged();
        invalidateFilter();
        emit countChanged();
    }
}

NymeaHost *NymeaHostsFilterModel::get(int index) const
{
    return m_nymeaDiscovery->nymeaHosts()->get(mapToSource(this->index(index, 0)).row());
}

bool NymeaHostsFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)
    NymeaHost *host = m_nymeaDiscovery->nymeaHosts()->get(sourceRow);
    if (m_jsonRpcClient && !m_showUneachableBearers) {
        bool hasReachableConnection = false;
        for (int i = 0; i < host->connections()->rowCount(); i++) {
//            qDebug() << "checking host for available bearer" << host->name() << host->connections()->get(i)->url() << "available bearer types:" << m_nymeaConnection->availableBearerTypes() << "hosts bearer types" << host->connections()->get(i)->bearerType();
            // Either enable a connection when the Bearer type is directly available
            switch (host->connections()->get(i)->bearerType()) {
            case Connection::BearerTypeLan:
                hasReachableConnection |= m_jsonRpcClient->availableBearerTypes().testFlag(NymeaConnection::BearerTypeEthernet);
                hasReachableConnection |= m_jsonRpcClient->availableBearerTypes().testFlag(NymeaConnection::BearerTypeWiFi);
                break;
            case Connection::BearerTypeWan:
            case Connection::BearerTypeCloud:
                hasReachableConnection |= m_jsonRpcClient->availableBearerTypes().testFlag(NymeaConnection::BearerTypeEthernet);
                hasReachableConnection |= m_jsonRpcClient->availableBearerTypes().testFlag(NymeaConnection::BearerTypeWiFi);
                hasReachableConnection |= m_jsonRpcClient->availableBearerTypes().testFlag(NymeaConnection::BearerTypeMobileData);
                break;
            case Connection::BearerTypeBluetooth:
                hasReachableConnection |= m_jsonRpcClient->availableBearerTypes().testFlag(NymeaConnection::BearerTypeBluetooth);
                break;
            case Connection::BearerTypeUnknown:
            case Connection::BearerTypeLoopback:
                hasReachableConnection = true;
                break;
            case Connection::BearerTypeNone:
                break;
            }
        }
        if (!hasReachableConnection) {
            return false;
        }
    }
    if (!m_showUneachableHosts) {
        bool isOnline = false;
        for (int i = 0; i < host->connections()->rowCount(); i++) {
            if (host->connections()->get(i)->online()) {
                isOnline = true;
                break;
            }
        }
        if (!isOnline) {
            return false;
        }
    }
    return true;
}
