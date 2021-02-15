/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2021, nymea GmbH
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

#ifndef MODBUSRTUMASTERS_H
#define MODBUSRTUMASTERS_H

#include <QObject>
#include <QAbstractListModel>

#include "modbusrtumaster.h"

class ModbusRtuMasters : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        RoleUuid,
        RoleSerialPort,
        RoleBaudrate,
        RoleParity,
        RoleDataBits,
        RoleStopBits,
        RoleConnected
    };
    Q_ENUM(Roles)

    explicit ModbusRtuMasters(QObject *parent = nullptr);
    virtual ~ModbusRtuMasters() override = default;

    QList<ModbusRtuMaster *> modbusRtuMasters() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addModbusRtuMaster(ModbusRtuMaster *modbusRtuMaster);
    void removeModbusRtuMaster(const QUuid &modbusUuid);

    void clear();

    Q_INVOKABLE virtual ModbusRtuMaster *get(int index) const;
    Q_INVOKABLE ModbusRtuMaster *getModbusRtuMaster(const QUuid &modbusUuid) const;

signals:
    void countChanged();

private:
    QList<ModbusRtuMaster *> m_modbusRtuMasters;

};

#endif // MODBUSRTUMASTERS_H
