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

#include "modbusrtumaster.h"

ModbusRtuMaster::ModbusRtuMaster(QObject *parent) : QObject(parent)
{

}

QUuid ModbusRtuMaster::modbusUuid() const
{
    return m_modbusUuid;
}

void ModbusRtuMaster::setModbusUuid(const QUuid &modbusUuid)
{
    m_modbusUuid = modbusUuid;
}

QString ModbusRtuMaster::serialPort() const
{
    return m_serialPort;
}

void ModbusRtuMaster::setSerialPort(const QString &serialPort)
{
    if (m_serialPort == serialPort)
        return;

    m_serialPort = serialPort;
    emit serialPortChanged(m_serialPort);
}

qint32 ModbusRtuMaster::baudrate() const
{
    return m_baudrate;
}

void ModbusRtuMaster::setBaudrate(qint32 baudrate)
{
    if (m_baudrate == baudrate)
        return;

    m_baudrate = baudrate;
    emit baudrateChanged(m_baudrate);
}

SerialPort::SerialPortParity ModbusRtuMaster::parity() const
{
    return m_parity;
}

void ModbusRtuMaster::setParity(SerialPort::SerialPortParity parity)
{
    if (m_parity == parity)
        return;

    m_parity = parity;
    emit parityChanged(m_parity);
}

SerialPort::SerialPortDataBits ModbusRtuMaster::dataBits() const
{
    return m_dataBits;
}

void ModbusRtuMaster::setDataBits(SerialPort::SerialPortDataBits dataBits)
{
    if (m_dataBits == dataBits)
        return;

    m_dataBits = dataBits;
    emit dataBitsChanged(m_dataBits);
}

SerialPort::SerialPortStopBits ModbusRtuMaster::stopBits() const
{
    return m_stopBits;
}

void ModbusRtuMaster::setStopBits(SerialPort::SerialPortStopBits stopBits)
{
    if (m_stopBits == stopBits)
        return;

    m_stopBits = stopBits;
    emit stopBitsChanged(m_stopBits);
}

bool ModbusRtuMaster::connected() const
{
    return m_connected;
}

void ModbusRtuMaster::setConnected(bool connected)
{
    if (m_connected == connected)
        return;

    m_connected = connected;
    emit connectedChanged(m_connected);
}
