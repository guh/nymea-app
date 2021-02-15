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

import QtQuick 2.8
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import "../components"
import Nymea 1.0

SettingsPageBase {
    id: root

    property ModbusRtuManager modbusRtuManager
    property ModbusRtuMaster modbusRtuMaster
    property ListModel serialPortBaudrateModel
    property ListModel serialPortParityModel
    property ListModel serialPortDataBitsModel
    property ListModel serialPortStopBitsModel

    header: NymeaHeader {
        text: qsTr("Reconfigure modbus RTU master")
        backButtonVisible: true
        onBackPressed: pageStack.pop()
    }

    SettingsPageSectionHeader {
        text: qsTr("Serial ports")
    }

    Label {
        Layout.fillWidth: true
        Layout.topMargin: app.margins
        Layout.leftMargin: app.margins; Layout.rightMargin: app.margins
        wrapMode: Text.WordWrap
        text: modbusRtuManager.serialPorts.count !== 0 ? qsTr("Please select one of the following serial ports detected on the system.") : qsTr("There are no serial ports available.\n\nPlease make sure the modbus RTU interface is connected to the system.")
    }

    Repeater {
        model: modbusRtuManager.serialPorts
        delegate: NymeaSwipeDelegate {
            Layout.fillWidth: true
            iconName: "../images/stock_usb.svg"
            text:  model.description + (model.manufacturer === "" ? "" : " - " + model.manufacturer)
            subText: model.systemLocation + (model.serialNumber === "" ? "" : " - " + model.serialNumber)
            onClicked: pageStack.push(reconfigureNewModbusRtuMasterPage, { modbusRtuManager: modbusRtuManager, serialPort: modbusRtuManager.serialPorts.get(index), modbusRtuMaster: modbusRtuMaster })
        }
    }

    Component {
        id: reconfigureNewModbusRtuMasterPage

        SettingsPageBase {
            id: root

            property ModbusRtuManager modbusRtuManager
            property SerialPort serialPort
            property ModbusRtuMaster modbusRtuMaster

            busy: d.pendingCommandId != -1

            header: NymeaHeader {
                text: qsTr("Reconfigure modbus RTU master")
                backButtonVisible: true
                onBackPressed: pageStack.pop()
            }

            QtObject {
                id: d
                property int pendingCommandId: -1

                function reconfigureModbusRtuMaster(modbusUuid, serialPort, baudRate, parity, dataBits, stopBits) {
                    d.pendingCommandId = root.modbusRtuManager.reconfigureModbusRtuMaster(modbusUuid, serialPort, baudRate, parity, dataBits, stopBits)
                }
            }

            Connections {
                target: root.modbusRtuManager
                onReconfigureModbusRtuMasterReply: {
                    if (commandId === d.pendingCommandId) {
                        d.pendingCommandId = -1
                        if (modbusRtuManager.handleModbusError(error)) {
                            pageStack.pop();
                            pageStack.pop();
                        }
                    }
                }
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("UUID")
                subText: modbusRtuMaster.modbusUuid
                progressive: false
                prominentSubText: false
            }

            SettingsPageSectionHeader {
                text: qsTr("Serial port")
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("System location")
                subText: serialPort.systemLocation
                progressive: false
                prominentSubText: false
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("Description")
                subText: serialPort.description
                progressive: false
                prominentSubText: false
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("Manufacturer")
                subText: serialPort.manufacturer
                progressive: false
                prominentSubText: false
                visible: serialPort.manufacturer !== ""
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("Serialnumber")
                subText: serialPort.serialNumber
                progressive: false
                prominentSubText: false
                visible: serialPort.serialNumber !== ""
            }

            SettingsPageSectionHeader {
                text: qsTr("Configuration")
            }

            RowLayout {
                Layout.leftMargin: app.margins; Layout.rightMargin: app.margins
                Label {
                    text: qsTr("Baud rate")
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: baudRateComboBox
                    Layout.minimumWidth: 250
                    enabled: !root.busy
                    textRole: "text"
                    model: serialPortBaudrateModel
                    onActivated: console.log("Selected baudrate", currentText, model.get(currentIndex).value)
                    Component.onCompleted: {
                        for (var i = 0; i < serialPortBaudrateModel.count; i++) {
                            if (serialPortBaudrateModel.get(i).value === modbusRtuMaster.baudrate) {
                                currentIndex = i
                            }
                        }
                    }
                }
            }

            RowLayout {
                Layout.leftMargin: app.margins; Layout.rightMargin: app.margins

                Label {
                    text: qsTr("Parity")
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: parityComboBox
                    textRole: "text"
                    enabled: !root.busy
                    Layout.minimumWidth: 250
                    onActivated: console.log("Selected parity", currentText, model.get(currentIndex).value)
                    model: serialPortParityModel
                    Component.onCompleted: {
                        for (var i = 0; i < serialPortParityModel.count; i++) {
                            if (serialPortParityModel.get(i).value === modbusRtuMaster.parity) {
                                currentIndex = i
                            }
                        }
                    }
                }
            }

            RowLayout {
                Layout.leftMargin: app.margins; Layout.rightMargin: app.margins

                Label {
                    text: qsTr("Data bits")
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: dataBitsComboBox
                    textRole: "text"
                    enabled: !root.busy
                    Layout.minimumWidth: 250
                    onActivated: console.log("Selected data bits", currentText, model.get(currentIndex).value)
                    model: serialPortDataBitsModel
                    Component.onCompleted: {
                        for (var i = 0; i < serialPortDataBitsModel.count; i++) {
                            if (serialPortDataBitsModel.get(i).value === modbusRtuMaster.dataBits) {
                                currentIndex = i
                            }
                        }
                    }
                }
            }

            RowLayout {
                Layout.leftMargin: app.margins; Layout.rightMargin: app.margins

                Label {
                    text: qsTr("Stop bits")
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: stopBitsComboBox
                    textRole: "text"
                    enabled: !root.busy
                    Layout.minimumWidth: 250
                    onActivated: console.log("Selected stop bits", currentText, model.get(currentIndex).value)
                    model: serialPortStopBitsModel
                    Component.onCompleted: {
                        for (var i = 0; i < serialPortStopBitsModel.count; i++) {
                            if (serialPortStopBitsModel.get(i).value === modbusRtuMaster.stopBits) {
                                currentIndex = i
                            }
                        }
                    }
                }
            }

            Button {
                Layout.fillWidth: true
                Layout.leftMargin: app.margins
                Layout.rightMargin: app.margins
                text: qsTr("Apply")
                enabled: !root.busy
                onClicked: {
                    var baudrate = serialPortBaudrateModel.get(baudRateComboBox.currentIndex).value
                    var parity = serialPortParityModel.get(parityComboBox.currentIndex).value
                    var dataBits = serialPortDataBitsModel.get(dataBitsComboBox.currentIndex).value
                    var stopBits = serialPortStopBitsModel.get(stopBitsComboBox.currentIndex).value

                    console.log("Reconfigure modbus RTU", modbusRtuMaster.modbusUuid, "with", serialPort.systemLocation, baudrate, parity, dataBits, stopBits)
                    d.reconfigureModbusRtuMaster(modbusRtuMaster.modbusUuid, serialPort.systemLocation, baudrate, parity, dataBits, stopBits)
                }
            }
        }
    }
}
