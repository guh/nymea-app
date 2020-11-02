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
import Nymea 1.0
import "../components"

SwipeDelegate {
    id: root
    Layout.fillWidth: true
    clip: true

    property StateEvaluator stateEvaluator: null
    property bool showChilds: false

    readonly property Thing device: stateEvaluator ? engine.deviceManager.devices.getDevice(stateEvaluator.stateDescriptor.deviceId) : null
    readonly property ThingClass deviceClass: device ? engine.deviceManager.deviceClasses.getDeviceClass(device.deviceClassId) : null
    readonly property Interface iface: stateEvaluator ? Interfaces.findByName(stateEvaluator.stateDescriptor.interfaceName) : null
    readonly property StateType stateType: deviceClass ? deviceClass.stateTypes.getStateType(stateEvaluator.stateDescriptor.stateTypeId)
                                                 : iface ? iface.stateTypes.findByName(stateEvaluator.stateDescriptor.interfaceState) : null

    signal deleteClicked();

    Rectangle {
        anchors.fill: parent
        border.color: "black"
        border.width: 1
        color: "transparent"
    }

    contentItem: ColumnLayout {
        RowLayout {
            Layout.fillWidth: true
            ColorIcon {
                Layout.preferredHeight: childEvaluatorsRepeater.count > 0 ? app.iconSize * .6 : app.iconSize
                Layout.preferredWidth: height
                name: root.stateEvaluator && root.stateEvaluator.stateDescriptor.interfaceName.length === 0 ? "../images/state.svg" : "../images/state-interface.svg"
                color: app.accentColor
            }

            Label {
                Layout.fillWidth: true
                font.pixelSize: childEvaluatorsRepeater.count > 0 ? app.smallFont : app.mediumFont
                wrapMode: Text.WordWrap
                property string operatorString: {
                    if (!root.stateEvaluator) {
                        return "";
                    }

                    switch (root.stateEvaluator.stateDescriptor.valueOperator) {
                    case StateDescriptor.ValueOperatorEquals:
                        return "=";
                    case StateDescriptor.ValueOperatorNotEquals:
                        return "!=";
                    case StateDescriptor.ValueOperatorGreater:
                        return ">";
                    case StateDescriptor.ValueOperatorGreaterOrEqual:
                        return ">=";
                    case StateDescriptor.ValueOperatorLess:
                        return "<";
                    case StateDescriptor.ValueOperatorLessOrEqual:
                        return "<=";
                    }
                    return "FIXME"
                }

                text: {
                    if (!root.stateType) {
                        return qsTr("Press to edit condition")
                    }

                    var valueText;
                    if (root.stateEvaluator.stateDescriptor.value !== undefined) {
                        valueText = root.stateEvaluator.stateDescriptor.value;
                        switch (root.stateType.type.toLowerCase()) {
                        case "bool":
                            valueText = root.stateEvaluator.stateDescriptor.value === true ? qsTr("True") : qsTr("False")
                            break;
                        }
                    } else {
                        print("value thing id:", root.stateEvaluator.stateDescriptor.valueThingId)
                        var valueThing = engine.thingManager.things.getThing(root.stateEvaluator.stateDescriptor.valueThingId)
                        valueText = valueThing.name
                        valueText += ", " + valueThing.thingClass.stateTypes.getStateType(root.stateEvaluator.stateDescriptor.valueStateTypeId).displayName
                    }

                    if (root.device) {
                        return "%1, %2 %3 %4".arg(root.device.name).arg(root.stateType.displayName).arg(operatorString).arg(valueText)
                    } else if (root.iface) {
                        return "%1, %2 %3 %4".arg(root.iface.displayName).arg(root.stateType.displayName).arg(operatorString).arg(valueText)
                    }
                    return "--";
                }
            }
        }

        Repeater {
            id: childEvaluatorsRepeater
            model: root.showChilds ? root.stateEvaluator.childEvaluators : null
            delegate: RowLayout {
                id: childEvaluatorDelegate
                Layout.fillWidth: true

                property var stateEvaluator: root.stateEvaluator.childEvaluators.get(index)
                property var stateDescriptor: stateEvaluator.stateDescriptor
                readonly property var device: engine.deviceManager.devices.getDevice(stateDescriptor.deviceId)
                readonly property var deviceClass: device ? engine.deviceManager.deviceClasses.getDeviceClass(device.deviceClassId) : null
                readonly property var iface: Interfaces.findByName(stateEvaluator.stateDescriptor.interfaceName)
                readonly property var stateType: device ? deviceClass.stateTypes.getStateType(stateDescriptor.stateTypeId)
                                                        : iface ? iface.stateTypes.findByName(stateEvaluator.stateDescriptor.interfaceState)
                                                                : null

                ColorIcon {
                    Layout.preferredHeight: app.iconSize * .6
                    Layout.preferredWidth: height
                    name: childEvaluatorDelegate.stateDescriptor.interfaceName.length === 0 ? "../images/state.svg" : "../images/state-interface.svg"
                    color: app.accentColor
                }
                Label {
                    font.pixelSize: app.smallFont
                    Layout.fillWidth: true

                    property string operatorString: {
                        switch (childEvaluatorDelegate.stateDescriptor.valueOperator) {
                        case StateDescriptor.ValueOperatorEquals:
                            return "=";
                        case StateDescriptor.ValueOperatorNotEquals:
                            return "!=";
                        case StateDescriptor.ValueOperatorGreater:
                            return ">";
                        case StateDescriptor.ValueOperatorGreaterOrEqual:
                            return ">=";
                        case StateDescriptor.ValueOperatorLess:
                            return "<";
                        case StateDescriptor.ValueOperatorLessOrEqual:
                            return "<=";
                        }
                        return "FIXME"
                    }
                    text: device ? ("%1 %2: %3 %4 %5%6").arg(root.stateEvaluator.stateOperator === StateEvaluator.StateOperatorAnd ? "and" : "or").arg(childEvaluatorDelegate.device.name).arg(childEvaluatorDelegate.stateType.displayName).arg(operatorString).arg(childEvaluatorDelegate.stateDescriptor.value).arg(childEvaluatorDelegate.stateEvaluator.childEvaluators.count > 0 ? "..." : "")
                                 : iface ? ("%1 %2: %3 %4 %5%6").arg(root.stateEvaluator.stateOperator === StateEvaluator.StateOperatorAnd ? "and" : "or").arg(childEvaluatorDelegate.iface.displayName).arg(childEvaluatorDelegate.stateType.displayName).arg(operatorString).arg(childEvaluatorDelegate.stateDescriptor.value).arg(childEvaluatorDelegate.stateEvaluator.childEvaluators.count > 0 ? "..." : "")
                                         : "???"
                }
            }
        }
    }

    onPressAndHold: swipe.open(SwipeDelegate.Right)
    swipe.right: MouseArea {
        height: parent.height
        width: height
        anchors.right: parent.right
        Rectangle {
            anchors.fill: parent
            color: "red"
        }

        ColorIcon {
            anchors.fill: parent
            anchors.margins: app.margins
            name: "../images/delete.svg"
            color: "white"
        }
        onClicked: {
            swipe.close()
            root.deleteClicked();
        }
    }
}
