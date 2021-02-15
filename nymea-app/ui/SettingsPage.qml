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

import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.1
import Nymea 1.0
import "components"

Page {
    id: root
    header: NymeaHeader {
        text: qsTr("System settings")
        backButtonVisible: true
        onBackPressed: pageStack.pop()
    }

    Flickable {
        anchors.fill: parent
        contentHeight: layout.implicitHeight + (layout.isGrid ? app.margins * 2 : 0)

        GridLayout {
            id: layout
            property bool isGrid: columns > 1
            anchors { left: parent.left; top: parent.top; right: parent.right; margins: isGrid ? app.margins : 0 }
            columns: Math.max(1, Math.floor(parent.width / 300))
            rowSpacing: isGrid ? app.margins : 0
            columnSpacing: isGrid ? app.margins : 0

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0
                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/configure.svg"
                    text: qsTr("General")
                    subText: qsTr("Change system name and time zone")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/GeneralSettingsPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0
                padding: 0
                visible: engine.jsonRpcClient.ensureServerVersion("4.2")
                         && engine.jsonRpcClient.authenticated
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/account.svg"
                    text: qsTr("User settings")
                    subText: qsTr("Configure who can log in")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/UsersSettingsPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/connections/network-wifi.svg"
                    text: qsTr("Networking")
                    subText: qsTr("Configure the system's network connection")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/NetworkSettingsPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/connections/cloud.svg"
                    text: qsTr("Cloud")
                    subText: qsTr("Connect this %1:core to %1:cloud").arg(app.systemName)
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/CloudSettingsPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/connections/network-vpn.svg"
                    text: qsTr("API interfaces")
                    prominentSubText: false
                    wrapTexts: false
                    subText: qsTr("Configure how clients interact with this system")
                    onClicked: pageStack.push(Qt.resolvedUrl("system/ConnectionInterfacesPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0
                visible: engine.jsonRpcClient.ensureServerVersion("1.11")

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/mqtt.svg"
                    text: qsTr("MQTT broker")
                    subText: qsTr("Configure the MQTT broker")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/MqttBrokerSettingsPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0
                visible: engine.jsonRpcClient.ensureServerVersion("5.3")

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/zigbee.svg"
                    text: qsTr("ZigBee")
                    subText: qsTr("Configure ZigBee networks")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/ZigbeeSettingsPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/stock_website.svg"
                    text: qsTr("Web server")
                    subText: qsTr("Configure the web server")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/WebServerSettingsPage.qml"))
                }

            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0
                visible: engine.jsonRpcClient.ensureServerVersion("5.4")

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/modbus.svg"
                    text: qsTr("Modbus RTU")
                    subText: qsTr("Configure Modbus RTU master interfaces")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/ModbusRtuSettingsPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/plugin.svg"
                    text: qsTr("Plugins")
                    subText: qsTr("List and cofigure installed plugins")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked:pageStack.push(Qt.resolvedUrl("system/PluginsPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/sdk.svg"
                    text: qsTr("Developer tools")
                    subText: qsTr("Access tools for debugging and error reporting")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/DeveloperTools.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0
                visible: engine.jsonRpcClient.ensureServerVersion("2.1") && engine.systemController.updateManagementAvailable

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/system-update.svg"
                    text: qsTr("System update")
                    subText: qsTr("Update your %1:core system").arg(app.systemName)
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/SystemUpdatePage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/logs.svg"
                    text: qsTr("Log viewer")
                    subText: qsTr("View system log")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/LogViewerPage.qml"))
                }
            }

            Pane {
                Layout.fillWidth: true
                Material.elevation: layout.isGrid ? 1 : 0

                padding: 0
                NymeaSwipeDelegate {
                    width: parent.width
                    iconName: "../images/info.svg"
                    text: qsTr("About %1:core").arg(app.systemName)
                    subText: qsTr("Find server UUID and versions")
                    prominentSubText: false
                    wrapTexts: false
                    onClicked: pageStack.push(Qt.resolvedUrl("system/AboutNymeaPage.qml"))
                }
            }
        }
    }
}
