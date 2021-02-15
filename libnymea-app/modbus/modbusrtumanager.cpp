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

#include "modbusrtumanager.h"
#include "engine.h"
#include "modbusrtumaster.h"
#include "modbusrtumasters.h"

#include "jsonrpc/jsonrpcclient.h"

#include <QMetaEnum>

ModbusRtuManager::ModbusRtuManager(QObject *parent) :
    JsonHandler(parent),
    m_serialPorts(new SerialPorts(this)),
    m_modbusRtuMasters(new ModbusRtuMasters(this))
{
    qRegisterMetaType<SerialPort::SerialPortParity>();
    qRegisterMetaType<SerialPort::SerialPortDataBits>();
    qRegisterMetaType<SerialPort::SerialPortStopBits>();
}

ModbusRtuManager::~ModbusRtuManager()
{
    if (m_engine) {
        m_engine->jsonRpcClient()->unregisterNotificationHandler(this);
    }
}

QString ModbusRtuManager::nameSpace() const
{
    return "ModbusRtu";
}

Engine *ModbusRtuManager::engine() const
{
    return m_engine;
}

void ModbusRtuManager::setEngine(Engine *engine)
{
    if (m_engine == engine)
        return;

    if (m_engine) {
        m_engine->jsonRpcClient()->unregisterNotificationHandler(this);
    }

    m_engine = engine;
    emit engineChanged();

    if (m_engine) {
        init();
    }
}

bool ModbusRtuManager::supported() const
{
    return m_supported;
}

SerialPorts *ModbusRtuManager::serialPorts() const
{
    return m_serialPorts;
}

ModbusRtuMasters *ModbusRtuManager::modbusRtuMasters() const
{
    return m_modbusRtuMasters;
}

int ModbusRtuManager::addModbusRtuMaster(const QString &serialPort, qint32 baudrate, SerialPort::SerialPortParity parity, SerialPort::SerialPortDataBits dataBits, SerialPort::SerialPortStopBits stopBits)
{
    QVariantMap params;
    params.insert("serialPort", serialPort);
    params.insert("baudrate", baudrate);
    params.insert("parity", QMetaEnum::fromType<SerialPort::SerialPortParity>().valueToKey(parity));
    params.insert("dataBits", QMetaEnum::fromType<SerialPort::SerialPortDataBits>().valueToKey(dataBits));
    params.insert("stopBits", QMetaEnum::fromType<SerialPort::SerialPortStopBits>().valueToKey(stopBits));
    return m_engine->jsonRpcClient()->sendCommand("ModbusRtu.AddModbusRtuMaster", params, this, "addModbusRtuMasterResponse");
}

int ModbusRtuManager::removeModbusRtuMaster(const QUuid &modbusUuid)
{
    QVariantMap params;
    params.insert("modbusUuid", modbusUuid);
    return m_engine->jsonRpcClient()->sendCommand("ModbusRtu.RemoveModbusRtuMaster", params, this, "removeModbusRtuMasterResponse");
}

int ModbusRtuManager::reconfigureModbusRtuMaster(const QUuid &modbusUuid, const QString &serialPort, qint32 baudrate, SerialPort::SerialPortParity parity, SerialPort::SerialPortDataBits dataBits, SerialPort::SerialPortStopBits stopBits)
{
    QVariantMap params;
    params.insert("modbusUuid", modbusUuid);
    params.insert("serialPort", serialPort);
    params.insert("baudrate", baudrate);
    params.insert("parity", QMetaEnum::fromType<SerialPort::SerialPortParity>().valueToKey(parity));
    params.insert("dataBits", QMetaEnum::fromType<SerialPort::SerialPortDataBits>().valueToKey(dataBits));
    params.insert("stopBits", QMetaEnum::fromType<SerialPort::SerialPortStopBits>().valueToKey(stopBits));
    return m_engine->jsonRpcClient()->sendCommand("ModbusRtu.ReconfigureModbusRtuMaster", params, this, "reconfigureModbusRtuMasterResponse");
}

void ModbusRtuManager::init()
{
    m_serialPorts->clear();
    m_modbusRtuMasters->clear();

    m_engine->jsonRpcClient()->registerNotificationHandler(this, "notificationReceived");
    m_engine->jsonRpcClient()->sendCommand("ModbusRtu.GetModbusRtuMasters", this, "getModbusRtuMastersResponse");
}

ModbusRtuMaster *ModbusRtuManager::unpackModbusRtuMaster(const QVariantMap &modbusRtuMasterMap)
{
    ModbusRtuMaster *modbusMaster = new ModbusRtuMaster(this);
    modbusMaster->setModbusUuid(modbusRtuMasterMap.value("modbusUuid").toUuid());
    modbusMaster->setConnected(modbusRtuMasterMap.value("connected").toBool());
    modbusMaster->setSerialPort(modbusRtuMasterMap.value("serialPort").toString());
    modbusMaster->setBaudrate(modbusRtuMasterMap.value("baudrate").toInt());
    modbusMaster->setParity(SerialPort::stringToSerialPortParity(modbusRtuMasterMap.value("parity").toString()));
    modbusMaster->setStopBits(SerialPort::stringToSerialPortStopBits(modbusRtuMasterMap.value("stopBits").toString()));
    modbusMaster->setDataBits(SerialPort::stringToSerialPortDataBits(modbusRtuMasterMap.value("dataBits").toString()));
    return modbusMaster;
}

void ModbusRtuManager::notificationReceived(const QVariantMap &notification)
{
    QString notificationString = notification.value("notification").toString();
    qDebug() << "Received notification" << notificationString << endl << notification;
    if (notificationString == "ModbusRtu.SerialPortAdded") {
        QVariantMap serialPortMap = notification.value("params").toMap().value("serialPort").toMap();
        m_serialPorts->addSerialPort(SerialPort::unpackSerialPort(serialPortMap, m_serialPorts));
        return;
    }

    if (notificationString == "ModbusRtu.SerialPortRemoved") {
        QVariantMap serialPortMap = notification.value("params").toMap().value("serialPort").toMap();
        SerialPort *serialPort = SerialPort::unpackSerialPort(serialPortMap, this);
        m_serialPorts->removeSerialPort(serialPort->systemLocation());
        serialPort->deleteLater();
        return;
    }

    if (notificationString == "ModbusRtu.ModbusRtuMasterAdded") {
        QVariantMap modbusRtuMasterMap = notification.value("params").toMap().value("modbusRtuMaster").toMap();
        ModbusRtuMaster *modbusRtuMaster = unpackModbusRtuMaster(modbusRtuMasterMap);
        m_modbusRtuMasters->addModbusRtuMaster(modbusRtuMaster);
        return;
    }

    if (notificationString == "ModbusRtu.ModbusRtuMasterRemoved") {
        QUuid modbusUuid = notification.value("params").toMap().value("modbusUuid").toUuid();
        m_modbusRtuMasters->removeModbusRtuMaster(modbusUuid);
        return;
    }

    if (notificationString == "ModbusRtu.ModbusRtuMasterChanged") {
        QVariantMap modbusRtuMasterMap = notification.value("params").toMap().value("modbusRtuMaster").toMap();
        ModbusRtuMaster *modbusRtuMaster = unpackModbusRtuMaster(modbusRtuMasterMap);
        qDebug() << "Modbus master changed" << modbusRtuMaster;
        ModbusRtuMaster *currentModbusRtuMaster = m_modbusRtuMasters->getModbusRtuMaster(modbusRtuMaster->modbusUuid());
        if (!currentModbusRtuMaster) {
            qWarning() << "Got modbus changed signal but there is no such modbus interface. Ignoring notification";
            return;
        }

        qDebug() << "Update modbus values" << currentModbusRtuMaster;
        currentModbusRtuMaster->setSerialPort(modbusRtuMaster->serialPort());
        currentModbusRtuMaster->setBaudrate(modbusRtuMaster->baudrate());
        currentModbusRtuMaster->setParity(modbusRtuMaster->parity());
        currentModbusRtuMaster->setDataBits(modbusRtuMaster->dataBits());
        currentModbusRtuMaster->setStopBits(modbusRtuMaster->stopBits());
        currentModbusRtuMaster->setConnected(modbusRtuMaster->connected());
        modbusRtuMaster->deleteLater();

        return;
    }
}

void ModbusRtuManager::getSerialPortsResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Get serial ports response" << commandId << params;
    m_serialPorts->clear();

    foreach (const QVariant &serialPortVariant, params.value("serialPorts").toList()) {
        m_serialPorts->addSerialPort(SerialPort::unpackSerialPort(serialPortVariant.toMap(), m_serialPorts));
    }
}

void ModbusRtuManager::getModbusRtuMastersResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Get modbus RTU masters response" << commandId << params;

    QString error = params.value("modbusError").toString();
    if (error == "ModbusRtuErrorNoError") {
        m_supported = true;
        emit supportedChanged(m_supported);

        m_modbusRtuMasters->clear();
        foreach (const QVariant &modbusRtuMasterVariant, params.value("modbusRtuMasters").toList()) {
            m_modbusRtuMasters->addModbusRtuMaster(unpackModbusRtuMaster(modbusRtuMasterVariant.toMap()));
        }

        m_engine->jsonRpcClient()->sendCommand("ModbusRtu.GetSerialPorts", this, "getSerialPortsResponse");
    } else {
        qWarning() << "Modbus is not supported on this platform";
    }
}

void ModbusRtuManager::addModbusRtuMasterResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Add modbus RTU master response" << commandId << params;
    emit addModbusRtuMasterReply(commandId, params.value("modbusError").toString(), params.value("modbusUuid").toUuid());
}

void ModbusRtuManager::removeModbusRtuMasterResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Remove modbus RTU master response" << commandId << params;
    emit removeModbusRtuMasterReply(commandId, params.value("modbusError").toString());
}

void ModbusRtuManager::reconfigureModbusRtuMasterResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Reconfigure modbus RTU master response" << commandId << params;
    emit reconfigureModbusRtuMasterReply(commandId, params.value("modbusError").toString());
}
