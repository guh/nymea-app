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

#include "serialport.h"

SerialPort::SerialPort(const QString &systemLocation, const QString &manufacturer, const QString &description, const QString &serialNumber, QObject *parent) :
    QObject(parent),
    m_systemLocation(systemLocation),
    m_manufacturer(manufacturer),
    m_description(description),
    m_serialNumber(serialNumber)
{

}

QString SerialPort::systemLocation() const
{
    return m_systemLocation;
}

QString SerialPort::manufacturer() const
{
    return m_manufacturer;
}

QString SerialPort::description() const
{
    return m_description;
}

QString SerialPort::serialNumber() const
{
    return m_serialNumber;
}

SerialPort *SerialPort::unpackSerialPort(const QVariantMap &serialPortMap, QObject *parent)
{
    return new SerialPort(serialPortMap.value("systemLocation").toString(),
                          serialPortMap.value("manufacturer").toString(),
                          serialPortMap.value("description").toString(),
                          serialPortMap.value("serialNumber").toString(), parent);
}

SerialPort::SerialPortParity SerialPort::stringToSerialPortParity(const QString &parityString)
{
    if (parityString == "SerialPortParityNoParity") {
        return SerialPort::SerialPortParityNoParity;
    } else if (parityString == "SerialPortParityEvenParity") {
        return SerialPort::SerialPortParityEvenParity;
    } else if (parityString == "SerialPortParityOddParity") {
        return SerialPort::SerialPortParityOddParity;
    } else if (parityString == "SerialPortParitySpaceParity") {
        return SerialPort::SerialPortParitySpaceParity;
    } else if (parityString == "SerialPortParityMarkParity") {
        return SerialPort::SerialPortParityMarkParity;
    }

    return SerialPort::SerialPortParityUnknownParity;
}

SerialPort::SerialPortDataBits SerialPort::stringToSerialPortDataBits(const QString &dataBitsString)
{
    if (dataBitsString == "SerialPortDataBitsData5") {
        return SerialPort::SerialPortDataBitsData5;
    } else if (dataBitsString == "SerialPortDataBitsData6") {
        return SerialPort::SerialPortDataBitsData6;
    } else if (dataBitsString == "SerialPortDataBitsData7") {
        return SerialPort::SerialPortDataBitsData7;
    } else if (dataBitsString == "SerialPortDataBitsData8") {
        return SerialPort::SerialPortDataBitsData7;
    }

    return SerialPort::SerialPortDataBitsUnknownDataBits;
}

SerialPort::SerialPortStopBits SerialPort::stringToSerialPortStopBits(const QString &stopBitsString)
{
    if (stopBitsString == "SerialPortStopBitsOneStop") {
        return SerialPort::SerialPortStopBitsOneStop;
    } else if (stopBitsString == "SerialPortStopBitsOneAndHalfStop") {
        return SerialPort::SerialPortStopBitsOneAndHalfStop;
    } else if (stopBitsString == "SerialPortStopBitsTwoStop") {
        return SerialPort::SerialPortStopBitsTwoStop;
    }

    return SerialPort::SerialPortStopBitsUnknownStopBits;
}
