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

#ifndef MODBUSRTUMASTER_H
#define MODBUSRTUMASTER_H

#include <QUuid>
#include <QObject>

#include "types/serialport.h"

class ModbusRtuMaster : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid modbusUuid READ modbusUuid CONSTANT)
    Q_PROPERTY(QString serialPort READ serialPort NOTIFY serialPortChanged)
    Q_PROPERTY(qint32 baudrate READ baudrate NOTIFY baudrateChanged)
    Q_PROPERTY(SerialPort::SerialPortParity parity READ parity NOTIFY parityChanged)
    Q_PROPERTY(SerialPort::SerialPortDataBits dataBits READ dataBits NOTIFY dataBitsChanged)
    Q_PROPERTY(SerialPort::SerialPortStopBits stopBits READ stopBits NOTIFY stopBitsChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

public:
    explicit ModbusRtuMaster(QObject *parent = nullptr);

    QUuid modbusUuid() const;
    void setModbusUuid(const QUuid &modbusUuid);

    QString serialPort() const;
    void setSerialPort(const QString &serialPort);

    qint32 baudrate() const;
    void setBaudrate(qint32 baudrate);

    SerialPort::SerialPortParity parity() const;
    void setParity(SerialPort::SerialPortParity parity);

    SerialPort::SerialPortDataBits dataBits() const;
    void setDataBits(SerialPort::SerialPortDataBits dataBits);

    SerialPort::SerialPortStopBits stopBits() const;
    void setStopBits(SerialPort::SerialPortStopBits stopBits);

    bool connected() const;
    void setConnected(bool connected);

signals:
    void connectedChanged(bool connected);
    void serialPortChanged(const QString &serialPort);
    void baudrateChanged(quint32 baudrate);
    void parityChanged(SerialPort::SerialPortParity parity);
    void dataBitsChanged(SerialPort::SerialPortDataBits dataBits);
    void stopBitsChanged(SerialPort::SerialPortStopBits stopBits);

private:
    QUuid m_modbusUuid;
    QString m_serialPort;
    qint32 m_baudrate;
    SerialPort::SerialPortParity m_parity;
    SerialPort::SerialPortDataBits m_dataBits;
    SerialPort::SerialPortStopBits m_stopBits;
    bool m_connected = false;

};

#endif // MODBUSRTUMASTER_H
