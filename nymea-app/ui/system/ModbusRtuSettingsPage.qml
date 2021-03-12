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

import QtQuick 2.8
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import "../components"
import Nymea 1.0

SettingsPageBase {
    id: root

    header: NymeaHeader {
        text: qsTr("Modbus RTU")
        backButtonVisible: true
        onBackPressed: pageStack.pop()

        HeaderButton {
            imageSource: "../images/add.svg"
            text: qsTr("Add Modbus RTU master")
            onClicked: pageStack.push(Qt.resolvedUrl("ModbusRtuAddMasterPage.qml"), {
                                          modbusRtuManager: modbusRtuManager,
                                          serialPortBaudrateModel: serialPortBaudrateModel,
                                          serialPortParityModel: serialPortParityModel,
                                          serialPortDataBitsModel: serialPortDataBitsModel,
                                          serialPortStopBitsModel: serialPortStopBitsModel
                                      })
            enabled: modbusRtuManager.supported
        }
    }

    ModbusRtuManager {
        id: modbusRtuManager
        engine: _engine

        function handleModbusError(error) {
            var props = {};
            switch (error) {
            case "ModbusRtuErrorNoError":
                return true;
            case "ModbusRtuErrorNotAvailable":
                props.text = qsTr("The serial port is not available any more.");
                break;
            case "ModbusRtuErrorNotSupported":
                props.text = qsTr("Modbus is not supported on this platform.");
                break;
            case "ModbusRtuErrorHardwareNotFound":
                props.text = qsTr("The modbus RTU hardware could not be found.");
                break;
            case "ModbusRtuErrorUuidNotFound":
                props.text = qsTr("The selected modbus RTU master does not exist any more.");
                break;
            case "ModbusRtuErrorConnectionFailed":
                props.text = qsTr("Unable to connect to the modbus RTU master.\n\nMaybe the hardware is already in use.");
                break;
            default:
                props.errorCode = error;
            }
            var comp = Qt.createComponent("../components/ErrorDialog.qml")
            var popup = comp.createObject(app, props)
            popup.open();

            return false;
        }
    }

    ListModel {
        id: serialPortBaudrateModel
        ListElement { value: 9600; text: qsTr("9600 Bd") }
        ListElement { value: 14400; text: qsTr("14400 Bd") }
        ListElement { value: 19200; text: qsTr("19200 Bd") }
        ListElement { value: 38400; text: qsTr("38400 Bd") }
        ListElement { value: 57600; text: qsTr("57600 Bd") }
        ListElement { value: 115200; text: qsTr("115200 Bd") }
        ListElement { value: 128000; text: qsTr("128000 Bd") }
        ListElement { value: 230400; text: qsTr("230400 Bd") }
        ListElement { value: 256000; text: qsTr("256000 Bd") }

        function getText(baudrate) {
            for (var index = 0; index < serialPortBaudrateModel.count; index++) {
                if (serialPortBaudrateModel.get(index).value === baudrate) {
                    return serialPortBaudrateModel.get(index).text
                }
            }

            return qsTr("Unknown baud rate")
        }
    }

    ListModel {
        id: serialPortParityModel
        ListElement { value: SerialPort.SerialPortParityNoParity; text: qsTr("No parity") }
        ListElement { value: SerialPort.SerialPortParityEvenParity; text: qsTr("Even parity") }
        ListElement { value: SerialPort.SerialPortParityOddParity; text: qsTr("Odd parity") }
        ListElement { value: SerialPort.SerialPortParitySpaceParity; text: qsTr("Space parity") }
        ListElement { value: SerialPort.SerialPortParityMarkParity; text: qsTr("Mark parity") }

        function getText(parity) {
            for (var index = 0; index < serialPortParityModel.count; index++) {
                if (serialPortParityModel.get(index).value === parity) {
                    return serialPortParityModel.get(index).text
                }
            }

            return qsTr("Unknown parity")
        }
    }

    ListModel {
        id: serialPortDataBitsModel
        ListElement { value: SerialPort.SerialPortDataBitsData5; text: qsTr("5 data bits") }
        ListElement { value: SerialPort.SerialPortDataBitsData6; text: qsTr("6 data bits") }
        ListElement { value: SerialPort.SerialPortDataBitsData7; text: qsTr("7 data bits") }
        ListElement { value: SerialPort.SerialPortDataBitsData8; text: qsTr("8 data bits") }

        function getText(dataBits) {
            for (var index = 0; index < serialPortDataBitsModel.count; index++) {
                if (serialPortDataBitsModel.get(index).value === dataBits) {
                    return serialPortDataBitsModel.get(index).text
                }
            }

            return qsTr("Unknown data bits")
        }
    }

    ListModel {
        id: serialPortStopBitsModel
        ListElement { value: SerialPort.SerialPortStopBitsOneStop; text: qsTr("One stop bit") }
        ListElement { value: SerialPort.SerialPortStopBitsOneAndHalfStop; text: qsTr("One and a half stop bits") }
        ListElement { value: SerialPort.SerialPortStopBitsTwoStop; text: qsTr("Two stop bits") }

        function getText(stopBits) {
            for (var index = 0; index < serialPortStopBitsModel.count; index++) {
                if (serialPortStopBitsModel.get(index).value === stopBits) {
                    return serialPortStopBitsModel.get(index).text
                }
            }

            return qsTr("Unknown stop bits")
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: root.height
        visible: modbusRtuManager.modbusRtuMasters.count == 0 && modbusRtuManager.supported

        EmptyViewPlaceholder {
            width: parent.width - app.margins * 2
            anchors.centerIn: parent
            title: qsTr("Modbus RTU masters")
            text: qsTr("There are no Modbus RTU masters set up yet.\nIn order to have Modbus RTU available as resource in the system, please add a new Modbus RTU master.")
            imageSource: "/ui/images/modbus.svg"
            buttonText: qsTr("Add Modbus RTU master")
            onButtonClicked: {
                pageStack.push(Qt.resolvedUrl("ModbusRtuAddMasterPage.qml"), {
                                   modbusRtuManager: modbusRtuManager,
                                   serialPortBaudrateModel: serialPortBaudrateModel,
                                   serialPortParityModel: serialPortParityModel,
                                   serialPortDataBitsModel: serialPortDataBitsModel,
                                   serialPortStopBitsModel: serialPortStopBitsModel
                               })
            }
        }
    }

    Label {
        Layout.fillWidth: true
        Layout.leftMargin: app.margins; Layout.rightMargin: app.margins
        wrapMode: Text.WordWrap
        text: qsTr("Modbus RTU is not supported on this platorm.")
        visible: !modbusRtuManager.supported
    }

    // Modbus RTU masters
    Repeater {
        enabled: modbusRtuManager.supported
        model: modbusRtuManager.modbusRtuMasters
        delegate: NymeaSwipeDelegate {
            Layout.fillWidth: true
            iconName: "../images/modbus.svg"
            text: model.serialPort + " " + model.baudrate
            subText: model.connected ? qsTr("Connected") : qsTr("Disconnected")
            onClicked: pageStack.push(modbusDetailsComponent, { modbusRtuManager: modbusRtuManager, modbusRtuMaster: modbusRtuManager.modbusRtuMasters.get(index) })
        }
    }

    Component {
        id: modbusDetailsComponent

        SettingsPageBase {
            id: root

            property ModbusRtuManager modbusRtuManager
            property ModbusRtuMaster modbusRtuMaster

            busy: d.pendingCommandId !== -1

            header: NymeaHeader {
                text: qsTr("Modbus RTU master")
                backButtonVisible: true
                onBackPressed: pageStack.pop()

                HeaderButton {
                    imageSource: "../images/delete.svg"
                    text: qsTr("Remove modbus RTU master")
                    enabled: modbusRtuManager.supported
                    onClicked: {
                        var dialog = removeModbusMasterDialogComponent.createObject(app, {modbusRtuMaster: root.modbusRtuMaster})
                        dialog.open()
                    }
                }
            }

            Component {
                id: removeModbusMasterDialogComponent

                MeaDialog {
                    id: removeModbusMasterDialog

                    property ModbusRtuMaster modbusRtuMaster

                    headerIcon: "../images/modbus.svg"
                    title: qsTr("Remove modbus RTU master")
                    text: qsTr("Are you sure you want to remove this modbus RTU master?")
                    standardButtons: Dialog.Ok | Dialog.Cancel

                    Label {
                        text: qsTr("Please note that all related things will stop working until you assign a new modbus RTU master to them.")
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    onAccepted: {
                        d.removeModbusRtuMaster(modbusRtuMaster.modbusUuid)
                    }
                }
            }

            QtObject {
                id: d
                property int pendingCommandId: -1

                function removeModbusRtuMaster(modbusUuid) {
                    d.pendingCommandId = root.modbusRtuManager.removeModbusRtuMaster(modbusUuid)
                }
            }

            Connections {
                target: root.modbusRtuManager
                onRemoveModbusRtuMasterReply: {
                    if (commandId === d.pendingCommandId) {
                        d.pendingCommandId = -1
                        if (modbusRtuManager.handleModbusError(error)) {
                            // FIXME: the page does not work if I pop the page here
                            //pageStack.pop()
                        }
                    }
                }
            }

            SettingsPageSectionHeader {
                text: qsTr("Information")
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("UUID")
                subText: modbusRtuMaster ? modbusRtuMaster.modbusUuid : ""
                progressive: false
                prominentSubText: false
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("System location")
                subText: modbusRtuMaster ? modbusRtuMaster.serialPort : ""
                progressive: false
                prominentSubText: false
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("Connection status")
                subText: modbusRtuMaster && modbusRtuMaster.connected ? qsTr("Connected") : qsTr("Disconnected")
                progressive: false
                prominentSubText: false
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("Baud rate")
                subText: modbusRtuMaster ? serialPortBaudrateModel.getText(modbusRtuMaster.baudrate) : ""
                progressive: false
                prominentSubText: false
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("Parity")
                subText: modbusRtuMaster ? serialPortParityModel.getText(modbusRtuMaster.parity) : ""
                progressive: false
                prominentSubText: false
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("Data bits")
                subText: modbusRtuMaster ? serialPortDataBitsModel.getText(modbusRtuMaster.dataBits) : ""
                progressive: false
                prominentSubText: false
            }

            NymeaSwipeDelegate {
                Layout.fillWidth: true
                text: qsTr("Stop bits")
                subText: modbusRtuMaster ? serialPortStopBitsModel.getText(modbusRtuMaster.stopBits) : ""
                progressive: false
                prominentSubText: false
            }

            Button {
                id: reconfigureButton
                Layout.fillWidth: true
                Layout.leftMargin: app.margins
                Layout.rightMargin: app.margins
                text: qsTr("Reconfigure")
                enabled: !root.busy
                onClicked: pageStack.push(Qt.resolvedUrl("ModbusRtuReconfigureMasterPage.qml"), {
                                              modbusRtuManager: modbusRtuManager,
                                              modbusRtuMaster: root.modbusRtuMaster,
                                              serialPortBaudrateModel: serialPortBaudrateModel,
                                              serialPortParityModel: serialPortParityModel,
                                              serialPortDataBitsModel: serialPortDataBitsModel,
                                              serialPortStopBitsModel: serialPortStopBitsModel
                                          })
            }
        }
    }
}

