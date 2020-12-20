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

#ifndef IOCONNECTIONS_H
#define IOCONNECTIONS_H

#include "ioconnection.h"

#include <QAbstractListModel>

class IOConnections : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum Roles {
        RoleInputThingId,
        RoleInputStateTypeId,
        RoleOutputThingId,
        RoleOutputStateTypeId,
        RoleInverted
    };
    Q_ENUM(Roles)

    explicit IOConnections(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addIOConnection(IOConnection *ioConnection);
    void removeIOConnection(const QUuid &ioConnectionId);
    void clearModel();

    Q_INVOKABLE IOConnection* getIOConnection(const QUuid &ioConnectionId) const;

    Q_INVOKABLE IOConnection* findIOConnectionByInput(const QUuid &inputThingId, const QUuid &inputStateTypeId) const;
    Q_INVOKABLE IOConnection* findIOConnectionByOutput(const QUuid &outputThingId, const QUuid &outputStateTypeId) const;

signals:
    void countChanged();

private:
    QList<IOConnection*> m_list;
};

#endif // IOCONNECTIONS_H
