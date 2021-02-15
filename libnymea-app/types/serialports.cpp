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

#include "serialports.h"

SerialPorts::SerialPorts(QObject *parent) : QAbstractListModel(parent)
{

}

int SerialPorts::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_serialPorts.count();
}

QVariant SerialPorts::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case RoleSystmLocation:
        return m_serialPorts.at(index.row())->systemLocation();
    case RoleManufacturer:
        return m_serialPorts.at(index.row())->manufacturer();
    case RoleDescription:
        return m_serialPorts.at(index.row())->description();
    case RoleSerialNumber:
        return m_serialPorts.at(index.row())->serialNumber();
    }
    return QVariant();
}

QHash<int, QByteArray> SerialPorts::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(RoleSystmLocation, "systemLocation");
    roles.insert(RoleManufacturer, "manufacturer");
    roles.insert(RoleDescription, "description");
    roles.insert(RoleSerialNumber, "serialNumber");
    return roles;
}

void SerialPorts::addSerialPort(SerialPort *serialPort)
{
    serialPort->setParent(this);

    beginInsertRows(QModelIndex(), m_serialPorts.count(), m_serialPorts.count());
    m_serialPorts.append(serialPort);
    endInsertRows();

    emit countChanged();
}

void SerialPorts::removeSerialPort(const QString &systemLocation)
{
    for (int i = 0; i < m_serialPorts.count(); i++) {
        if (m_serialPorts.at(i)->systemLocation() == systemLocation) {
            beginRemoveRows(QModelIndex(), i, i);
            m_serialPorts.takeAt(i)->deleteLater();
            endRemoveRows();
            emit countChanged();
            return;
        }
    }
}

void SerialPorts::clear()
{
    beginResetModel();
    qDeleteAll(m_serialPorts);
    m_serialPorts.clear();
    endResetModel();
    emit countChanged();
}

SerialPort *SerialPorts::get(int index) const
{
    if (index < 0 || index >= m_serialPorts.count()) {
        return nullptr;
    }

    return m_serialPorts.at(index);
}
