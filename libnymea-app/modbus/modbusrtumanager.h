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

#ifndef MODBUSRTUMANAGER_H
#define MODBUSRTUMANAGER_H

#include <QObject>

#include "types/serialports.h"
#include "jsonrpc/jsonhandler.h"

class Engine;
class JsonRpcClient;
class ModbusRtuMaster;
class ModbusRtuMasters;

class ModbusRtuManager : public JsonHandler
{
    Q_OBJECT
    Q_PROPERTY(Engine *engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(SerialPorts *serialPorts READ serialPorts CONSTANT)
    Q_PROPERTY(ModbusRtuMasters *modbusRtuMasters READ modbusRtuMasters CONSTANT)
    Q_PROPERTY(bool supported READ supported NOTIFY supportedChanged)

public:
    explicit ModbusRtuManager(QObject *parent = nullptr);
    ~ModbusRtuManager();

    QString nameSpace() const override;

    Engine *engine() const;
    void setEngine(Engine *engine);

    bool supported() const;

    SerialPorts *serialPorts() const;
    ModbusRtuMasters *modbusRtuMasters() const;

    Q_INVOKABLE int addModbusRtuMaster(const QString &serialPort, qint32 baudrate, SerialPort::SerialPortParity parity, SerialPort::SerialPortDataBits dataBits, SerialPort::SerialPortStopBits stopBits);
    Q_INVOKABLE int removeModbusRtuMaster(const QUuid &modbusUuid);
    Q_INVOKABLE int reconfigureModbusRtuMaster(const QUuid &modbusUuid, const QString &serialPort, qint32 baudrate, SerialPort::SerialPortParity parity, SerialPort::SerialPortDataBits dataBits, SerialPort::SerialPortStopBits stopBits);

signals:
    void engineChanged();
    void supportedChanged(bool supported);
    void addModbusRtuMasterReply(int commandId, const QString &error, const QUuid &modbusUuid);
    void removeModbusRtuMasterReply(int commandId, const QString &error);
    void reconfigureModbusRtuMasterReply(int commandId, const QString &error);

private:
    Engine* m_engine = nullptr;
    SerialPorts *m_serialPorts = nullptr;
    ModbusRtuMasters *m_modbusRtuMasters = nullptr;
    bool m_supported = false;

    void init();

    ModbusRtuMaster *unpackModbusRtuMaster(const QVariantMap &modbusRtuMasterMap);

    Q_INVOKABLE void notificationReceived(const QVariantMap &notification);

    Q_INVOKABLE void getSerialPortsResponse(int commandId, const QVariantMap &params);
    Q_INVOKABLE void getModbusRtuMastersResponse(int commandId, const QVariantMap &params);
    Q_INVOKABLE void addModbusRtuMasterResponse(int commandId, const QVariantMap &params);
    Q_INVOKABLE void removeModbusRtuMasterResponse(int commandId, const QVariantMap &params);
    Q_INVOKABLE void reconfigureModbusRtuMasterResponse(int commandId, const QVariantMap &params);


};

#endif // MODBUSRTUMANAGER_H
