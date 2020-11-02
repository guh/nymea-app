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
    // Needs to be set and filled in with thingId and eventTypeId
    property StateDescriptor stateDescriptor: null

    readonly property Device thing: stateDescriptor && stateDescriptor.thingId ? engine.thingManager.things.getThing(stateDescriptor.thingId) : null
    readonly property Interface iface: stateDescriptor && stateDescriptor.interfaceName ? Interfaces.findByName(stateDescriptor.interfaceName) : null
    readonly property StateType stateType: thing ? thing.thingClass.stateTypes.getStateType(stateDescriptor.stateTypeId)
                                                 : iface ? iface.stateTypes.findByName(stateDescriptor.interfaceState) : null

    signal backPressed();
    signal completed();

    header: NymeaHeader {
        text: qsTr("Condition")
        onBackPressed: root.backPressed();
    }

    QtObject {
        id: d
        property var value: root.stateType.defaultValue
    }

    GroupBox {
        anchors.fill: parent
        GridLayout {
            anchors.fill: parent
            columns: width > 600 ? 2 : 1

            Label {
                Layout.fillWidth: true
                text: "%1, %2".arg(root.thing.name).arg(root.stateType.displayName)
            }

            ComboBox {
                id: operatorComboBox
                Layout.fillWidth: true
                property bool isNumeric: {
                    switch (root.stateType.type.toLowerCase()) {
                    case "bool":
                    case "string":
                    case "qstring":
                    case "color":
                        return false;
                    case "int":
                    case "double":
                        return true;
                    }
                    console.warn("ParamDescriptorDelegate: Unhandled data type:", root.stateType.type.toLowerCase());
                    return false;
                }

                model: isNumeric ?
                           [qsTr("is equal to"), qsTr("is not equal to"), qsTr("is smaller than"), qsTr("is greater than"), qsTr("is smaller or equal than"), qsTr("is greater or equal than")]
                         : [qsTr("is"), qsTr("is not")];
            }

            GroupBox {
                Layout.columnSpan: parent.columns
                Layout.fillWidth: true

                GridLayout {
                    anchors.fill: parent
                    columns: root.width > 600 ? 2 : 1
                    RadioButton {
                        id: staticValueRadioButton
                        Layout.fillWidth: true
                        checked: true
                        text: qsTr("a static value:")
                        font.pixelSize: app.smallFont
                    }
                    RadioButton {
                        id: stateValueRadioButton
                        Layout.fillWidth: true
                        text: qsTr("another thing's state:")
                        font.pixelSize: app.smallFont
                        visible: engine.jsonRpcClient.ensureServerVersion("5.3")
                    }

                    ThinDivider { Layout.columnSpan: parent.columns }

                    ParamDelegate {
                        Layout.fillWidth: true
                        hoverEnabled: false
                        padding: 0
                        paramType: root.thing.thingClass.eventTypes.getEventType(root.stateType.id).paramTypes.getParamType(root.stateType.id)
                        enabled: staticValueRadioButton.checked
                        nameVisible: false
                        value: d.value
                        visible: staticValueRadioButton.checked
                        placeholderText: qsTr("Insert value here")
                    }

                    NymeaListItemDelegate {
                        id: statePickerDelegate
                        Layout.fillWidth: true
                        text: thingId === null || stateTypeId === null
                              ? qsTr("Select a state")
                              : thing.name + " - " + thing.thingClass.stateTypes.getStateType(stateTypeId).displayName
                        visible: stateValueRadioButton.checked

                        property var thingId: null
                        property var stateTypeId: null

                        readonly property Device thing: engine.thingManager.things.getThing(thingId)

                        onClicked: {
                            var page = pageStack.push(Qt.resolvedUrl("SelectThingPage.qml"), {showStates: true, showEvents: false, showActions: false });
                            page.thingSelected.connect(function(thing) {
                                print("Thing selected", thing.name);
                                statePickerDelegate.thingId = thing.id
                                var selectStatePage = pageStack.replace(Qt.resolvedUrl("SelectStatePage.qml"), {thing: thing})
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

            Button {
                text: qsTr("OK")
                Layout.fillWidth: true
                Layout.margins: app.margins
                onClicked: {
                    root.stateDescriptor.valueOperator = operatorComboBox.currentIndex
                    if (staticValueRadioButton.checked) {
                        root.stateDescriptor.value = d.value
                    } else {
                        root.stateDescriptor.valueThingId = statePickerDelegate.thingId
                        root.stateDescriptor.valueStateTypeId = statePickerDelegate.stateTypeId
                    }
                    root.completed()
                }
            }

        }
    }
}
