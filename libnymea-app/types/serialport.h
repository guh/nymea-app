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

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QVariant>

class SerialPort : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString systemLocation READ systemLocation CONSTANT)
    Q_PROPERTY(QString manufacturer READ manufacturer CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString serialNumber READ serialNumber CONSTANT)

public:
    enum SerialPortParity {
        SerialPortParityNoParity = 0,
        SerialPortParityEvenParity = 2,
        SerialPortParityOddParity = 3,
        SerialPortParitySpaceParity = 4,
        SerialPortParityMarkParity = 5,
        SerialPortParityUnknownParity = -1
    };
    Q_ENUM(SerialPortParity)

    enum SerialPortDataBits {
        SerialPortDataBitsData5 = 5,
        SerialPortDataBitsData6 = 6,
        SerialPortDataBitsData7 = 7,
        SerialPortDataBitsData8 = 8,
        SerialPortDataBitsUnknownDataBits = -1
    };
    Q_ENUM(SerialPortDataBits)

    enum SerialPortStopBits {
        SerialPortStopBitsOneStop = 1,
        SerialPortStopBitsOneAndHalfStop = 3,
        SerialPortStopBitsTwoStop = 2,
        SerialPortStopBitsUnknownStopBits = -1
    };
    Q_ENUM(SerialPortStopBits)

    explicit SerialPort(const QString &systemLocation, const QString &manufacturer, const QString &description, const QString &serialNumber, QObject *parent = nullptr);

    QString systemLocation() const;
    QString manufacturer() const;
    QString description() const;
    QString serialNumber() const;

    static SerialPort *unpackSerialPort(const QVariantMap &serialPortMap, QObject *parent);
    static SerialPort::SerialPortParity stringToSerialPortParity(const QString &parityString);
    static SerialPort::SerialPortDataBits stringToSerialPortDataBits(const QString &dataBitsString);
    static SerialPort::SerialPortStopBits stringToSerialPortStopBits(const QString &stopBitsString);

private:
    QString m_systemLocation;
    QString m_manufacturer;
    QString m_description;
    QString m_serialNumber;

};

#endif // SERIALPORT_H
