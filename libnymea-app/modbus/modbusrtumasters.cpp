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

#include "modbusrtumasters.h"

ModbusRtuMasters::ModbusRtuMasters(QObject *parent) : QAbstractListModel(parent)
{

}

QList<ModbusRtuMaster *> ModbusRtuMasters::modbusRtuMasters() const
{
    return m_modbusRtuMasters;
}

int ModbusRtuMasters::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_modbusRtuMasters.count();
}

QVariant ModbusRtuMasters::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case RoleUuid:
        return m_modbusRtuMasters.at(index.row())->modbusUuid();
    case RoleSerialPort:
        return m_modbusRtuMasters.at(index.row())->serialPort();
    case RoleBaudrate:
        return m_modbusRtuMasters.at(index.row())->baudrate();
    case RoleParity:
        return m_modbusRtuMasters.at(index.row())->parity();
    case RoleDataBits:
        return m_modbusRtuMasters.at(index.row())->dataBits();
    case RoleStopBits:
        return m_modbusRtuMasters.at(index.row())->stopBits();
    case RoleConnected:
        return m_modbusRtuMasters.at(index.row())->connected();
    }
    return QVariant();
}

QHash<int, QByteArray> ModbusRtuMasters::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(RoleUuid, "modbusUuid");
    roles.insert(RoleSerialPort, "serialPort");
    roles.insert(RoleBaudrate, "baudrate");
    roles.insert(RoleParity, "parity");
    roles.insert(RoleDataBits, "dataBits");
    roles.insert(RoleStopBits, "stopBits");
    roles.insert(RoleConnected, "connected");
    return roles;
}

void ModbusRtuMasters::addModbusRtuMaster(ModbusRtuMaster *modbusRtuMaster)
{
    modbusRtuMaster->setParent(this);

    connect(modbusRtuMaster, &ModbusRtuMaster::serialPortChanged, this, [=](const QString &serialPort) {
        Q_UNUSED(serialPort)
        QModelIndex idx = index(m_modbusRtuMasters.indexOf(modbusRtuMaster), 0);
        emit dataChanged(idx, idx, {RoleSerialPort});
    });

    connect(modbusRtuMaster, &ModbusRtuMaster::baudrateChanged, this, [=](qint32 baudrate) {
        Q_UNUSED(baudrate)
        QModelIndex idx = index(m_modbusRtuMasters.indexOf(modbusRtuMaster), 0);
        emit dataChanged(idx, idx, {RoleBaudrate});
    });

    connect(modbusRtuMaster, &ModbusRtuMaster::parityChanged, this, [=](SerialPort::SerialPortParity parity) {
        Q_UNUSED(parity)
        QModelIndex idx = index(m_modbusRtuMasters.indexOf(modbusRtuMaster), 0);
        emit dataChanged(idx, idx, {RoleParity});
    });

    connect(modbusRtuMaster, &ModbusRtuMaster::dataBitsChanged, this, [=](SerialPort::SerialPortDataBits dataBits) {
        Q_UNUSED(dataBits)
        QModelIndex idx = index(m_modbusRtuMasters.indexOf(modbusRtuMaster), 0);
        emit dataChanged(idx, idx, {RoleDataBits});
    });

    connect(modbusRtuMaster, &ModbusRtuMaster::stopBitsChanged, this, [=](SerialPort::SerialPortStopBits stopBites) {
        Q_UNUSED(stopBites)
        QModelIndex idx = index(m_modbusRtuMasters.indexOf(modbusRtuMaster), 0);
        emit dataChanged(idx, idx, {RoleStopBits});
    });

    connect(modbusRtuMaster, &ModbusRtuMaster::connectedChanged, this, [=](bool connected) {
        Q_UNUSED(connected)
        QModelIndex idx = index(m_modbusRtuMasters.indexOf(modbusRtuMaster), 0);
        emit dataChanged(idx, idx, {RoleConnected});
    });

    beginInsertRows(QModelIndex(), m_modbusRtuMasters.count(), m_modbusRtuMasters.count());
    m_modbusRtuMasters.append(modbusRtuMaster);
    endInsertRows();

    emit countChanged();
}

void ModbusRtuMasters::removeModbusRtuMaster(const QUuid &modbusUuid)
{
    for (int i = 0; i < m_modbusRtuMasters.count(); i++) {
        if (m_modbusRtuMasters.at(i)->modbusUuid() == modbusUuid) {
            beginRemoveRows(QModelIndex(), i, i);
            m_modbusRtuMasters.takeAt(i)->deleteLater();
            endRemoveRows();
            emit countChanged();
            return;
        }
    }
}

void ModbusRtuMasters::clear()
{
    beginResetModel();
    qDeleteAll(m_modbusRtuMasters);
    m_modbusRtuMasters.clear();
    endResetModel();
    emit countChanged();
}

ModbusRtuMaster *ModbusRtuMasters::get(int index) const
{
    if (index < 0 || index >= m_modbusRtuMasters.count()) {
        return nullptr;
    }
    return m_modbusRtuMasters.at(index);
}

ModbusRtuMaster *ModbusRtuMasters::getModbusRtuMaster(const QUuid &modbusUuid) const
{
    foreach (ModbusRtuMaster *modbusRtuMaster, m_modbusRtuMasters) {
        if (modbusRtuMaster->modbusUuid() == modbusUuid) {
            return modbusRtuMaster;
        }
    }

    return nullptr;
}
