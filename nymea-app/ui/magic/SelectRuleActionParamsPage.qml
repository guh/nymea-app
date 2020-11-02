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
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.2
import "../components"
import "../delegates"
import Nymea 1.0

Page {
    id: root
    // Needs to be set and have rule.ruleActions filled in with deviceId and actionTypeId or interfaceName and interfaceAction
    property RuleAction ruleAction: null

    // optionally a rule which will be used to propose events params as param values
    property var rule: null

    readonly property Device device: ruleAction && ruleAction.deviceId ? engine.deviceManager.devices.getDevice(ruleAction.deviceId) : null
    readonly property var iface: ruleAction && ruleAction.interfaceName ? Interfaces.findByName(ruleAction.interfaceName) : null
    readonly property var actionType: device ? device.deviceClass.actionTypes.getActionType(ruleAction.actionTypeId)
                                            : iface ? iface.actionTypes.findByName(ruleAction.interfaceAction) : null

    signal backPressed();
    signal completed();

    header: NymeaHeader {
        text: actionType.displayName
        onBackPressed: root.backPressed();
    }

    Flickable {
        anchors.fill: parent
        contentHeight: contentCol.height

        ColumnLayout {
            id: contentCol
            width: parent.width

            Repeater {
                id: delegateRepeater
                model: root.actionType.paramTypes
                delegate: ColumnLayout {
                    Layout.fillWidth: true
                    property string type: {
                        if (staticParamRadioButton.checked) {
                            return "static"
                        }
                        if (eventParamRadioButton.checked) {
                            return "event"
                        }
                        if (stateValueRadioButton.checked) {
                            return "state"
                        }
                        return ""
                    }

                    property alias paramType: paramDelegate.paramType
                    property alias value: paramDelegate.value
                    property alias eventType: eventParamsComboBox.eventType
                    property alias eventParamTypeId: eventParamsComboBox.currentParamTypeId
                    property alias stateDeviceId: statePickerDelegate.deviceId
                    property alias stateTypeId: statePickerDelegate.stateTypeId

                    GroupBox {
                        Layout.fillWidth: true
                        Layout.margins: app.margins
                        title: paramType.displayName
                        ColumnLayout {
                            anchors.fill: parent
                            RadioButton {
                                id: staticParamRadioButton
                                text: qsTr("Use static value as parameter")
                                checked: true
                                font.pixelSize: app.smallFont
                                visible: eventParamRadioButton.visible || stateValueRadioButton.visible
                            }
                            RadioButton {
                                id: eventParamRadioButton
                                text: qsTr("Use event parameter")
                                visible: eventParamsComboBox.count > 0
                                font.pixelSize: app.smallFont
                            }
                            RadioButton {
                                id: stateValueRadioButton
                                text: qsTr("Use a thing's state value")
                                font.pixelSize: app.smallFont
                                visible: engine.jsonRpcClient.ensureServerVersion("2.0")
                            }

                            ThinDivider {
                                visible: staticParamRadioButton.visible
                            }

                            ParamDelegate {
                                id: paramDelegate
                                Layout.fillWidth: true
                                hoverEnabled: false
                                padding: 0
                                paramType: root.actionType.paramTypes.get(index)
                                enabled: staticParamRadioButton.checked
                                nameVisible: false
                                value: root.ruleAction.ruleActionParams.get(index).value
                                visible: staticParamRadioButton.checked
                                placeholderText: qsTr("Insert value here")
                            }

                            ComboBox {
                                id: eventParamsComboBox
                                Layout.fillWidth: true
                                visible: eventParamRadioButton.checked && count > 0
                                Component.onCompleted: currentIndex = 0;
                                property var eventDescriptor: root.rule.eventDescriptors.count === 1 ? root.rule.eventDescriptors.get(0) : null
                                property var device: eventDescriptor ? engine.deviceManager.devices.getDevice(eventDescriptor.deviceId) : null
                                property var deviceClass: device ? engine.deviceManager.deviceClasses.getDeviceClass(device.deviceClassId) : null
                                property var eventType: deviceClass ? deviceClass.eventTypes.getEventType(eventDescriptor.eventTypeId) : null

                                property var currentParamDescriptor: eventType.paramTypes.get(eventParamsComboBox.currentIndex)
                                property var currentParamTypeId: currentParamDescriptor.id

                                model: eventType.paramTypes
                                delegate: ItemDelegate {
                                    width: parent.width
                                    text: eventParamsComboBox.device.name + " - " + eventParamsComboBox.eventType.displayName + " - " + eventParamsComboBox.eventType.paramTypes.getParamType(model.id).displayName
                                }
                                contentItem: Label {
                                    id: eventParamsComboBoxContentItem
                                    anchors.fill: parent
                                    anchors.margins: app.margins
                                    text: eventParamsComboBox.device.name + " - " + eventParamsComboBox.eventType.displayName + " - " + eventParamsComboBox.currentParamDescriptor.displayName
                                    elide: Text.ElideRight
                                }
                            }

                            NymeaListItemDelegate {
                                id: statePickerDelegate
                                Layout.fillWidth: true
                                text: deviceId === null || stateTypeId === null
                                      ? qsTr("Select a state")
                                      : dev.name + " - " + dev.deviceClass.stateTypes.getStateType(stateTypeId).displayName
                                visible: stateValueRadioButton.checked

                                property var deviceId: null
                                property var stateTypeId: null

                                readonly property Device dev: engine.deviceManager.devices.getDevice(deviceId)

                                onClicked: {
                                    var page = pageStack.push(Qt.resolvedUrl("SelectThingPage.qml"), {showStates: true, showEvents: false, showActions: false });
                                    page.thingSelected.connect(function(device) {
                                        print("Thing selected", device.name);
                                        statePickerDelegate.deviceId = device.id
                                        var selectStatePage = pageStack.replace(Qt.resolvedUrl("SelectStatePage.qml"), {device: device})
                                        selectStatePage.stateSelected.connect(function(stateTypeId) {
                                            print("State selected", stateTypeId)
                                            pageStack.pop();
                                            statePickerDelegate.stateTypeId = stateTypeId;
                                        })
                                    })
                                    page.backPressed.connect(function() {
                                        pageStack.pop();
                                    })
                                }
                            }
                        }
                    }

//                    Label {
//                        id: paramNameLabel
//                        Layout.fillWidth: true
//                        Layout.leftMargin: app.margins
//                        Layout.rightMargin: app.margins
//                        Layout.topMargin: app.margins
//                        elide: Text.ElideRight
//                        text: paramType.displayName
//                        font.pixelSize: app.largeFont
//                    }


//                    ThinDivider {}
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            Button {
                text: qsTr("OK")
                Layout.fillWidth: true
                Layout.margins: app.margins
                onClicked: {
                    var params = [];
                    for (var i = 0; i < delegateRepeater.count; i++) {
                        var paramDelegate = delegateRepeater.itemAt(i);
                        print("Working on parameter", paramDelegate.paramType, paramDelegate.paramType.id, paramDelegate.value, paramDelegate.eventType, paramDelegate.eventParamTypeId, paramDelegate.stateDeviceId, paramDelegate.stateTypeId)

                        if (paramDelegate.type === "static") {
                            if (root.device) {
                                print("Setting static value rule action param", paramDelegate.paramType.id, paramDelegate.value)
                                root.ruleAction.ruleActionParams.setRuleActionParam(paramDelegate.paramType.id, paramDelegate.value)
                            } else if (root.iface) {
                                print("Setting static value rule action param by name", root.actionType.paramTypes.get(i).name, paramDelegate.value)
                                root.ruleAction.ruleActionParams.setRuleActionParamByName(root.actionType.paramTypes.get(i).name, paramDelegate.value)
                            }
                        } else if (paramDelegate.type === "event") {
                            if (root.device) {
                                print("adding event based rule action param", paramDelegate.paramType.id, paramDelegate.eventType.id, paramDelegate.eventParamTypeId)
                                root.ruleAction.ruleActionParams.setRuleActionParamEvent(paramDelegate.paramType.id, paramDelegate.eventType.id, paramDelegate.eventParamTypeId)
                            } else if (root.iface) {
                                print("adding event based rule action param by name", root.actionType.paramTypes.get(i).name, paramDelegate.eventType.id, paramDelegate.eventParamTypeId)
                                root.ruleAction.ruleActionParams.setRuleActionParamEventByName(root.actionType.paramTypes.get(i).name, paramDelegate.eventType.id, paramDelegate.eventParamTypeId)
                            }
                        } else if (paramDelegate.type === "state") {
                            if (root.device) {
                                print("adding state value based rule action param", paramDelegate.paramType.id, paramDelegate.stateDeviceId, paramDelegate.stateTypeId)
                                root.ruleAction.ruleActionParams.setRuleActionParamState(paramDelegate.paramType.id, paramDelegate.stateDeviceId, paramDelegate.stateTypeId)
                            } else if (root.iface) {
                                print("adding state value based rule action param by name", root.actionType.paramTypes.get(i).name, paramDelegate.stateDeviceId, paramDelegate.stateTypeId)
                                root.ruleAction.ruleActionParams.setRuleActionParamStateByName(root.actionType.paramTypes.get(i).name, paramDelegate.stateDeviceId, paramDelegate.stateTypeId)
                            }
                        }
                    }
                    root.completed()
                }
            }
        }
    }
}
