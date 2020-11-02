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
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import Nymea 1.0
import "../components"

ItemDelegate {
    id: root

    property ParamType paramType: null
    property alias value: d.value
    property Param param: Param {
        id: d
        paramTypeId: paramType.id
        value: paramType.defaultValue
    }
    property bool writable: true
    property alias nameVisible: nameLabel.visible
    property string placeholderText: ""

    topPadding: 0
    bottomPadding: 0
    contentItem: ColumnLayout {
        id: contentItemColumn
        RowLayout {
            spacing: app.margins
            Label {
                id: nameLabel
//                Layout.fillWidth: true
//                Layout.minimumWidth: parent.width / 2
                text: root.paramType.displayName
                elide: Text.ElideRight
            }
            Loader {
                id: loader
                Layout.fillWidth: true// sourceComponent === textFieldComponent || sourceComponent === stringComponent
                sourceComponent: {
                    print("Loading ParamDelegate");
                    print("Writable:", root.writable, "type:", root.paramType.type, "min:", root.paramType.minValue, "max:", root.paramType.maxValue, "value:", root.param.value)
                    if (!root.writable) {
                        return stringComponent;
                    }

                    switch (root.paramType.type.toLowerCase()) {
                    case "bool":
                        return boolComponent;
                    case "uint":
                    case "int":
                        if (root.paramType.name == "colorTemperature") {
                            return null;
                        }
                    case "double":
                        if (root.paramType.allowedValues.length > 0) {
                            return comboBoxComponent;
                        } else if (root.paramType.minValue !== undefined && root.paramType.maxValue !== undefined
                                   && (root.paramType.maxValue - root.paramType.minValue <= 100)) {
                            return sliderComponent;
                        } else {
                            return spinnerComponent;
                        }
                    case "string":
                    case "qstring":
                        if (root.paramType.allowedValues.length > 0) {
                            return comboBoxComponent;
                        }
                        return textFieldComponent;
                    case "color":
                        return colorPreviewComponent;
                    }
                    console.warn("Param Delegate: Fallback to stringComponent", root.paramType.name, root.paramType.type)
                    return stringComponent;
                }
            }
        }
        Loader {
            Layout.fillWidth: true
            sourceComponent: {
                if (root.paramType.name == "colorTemperature") {
                    return colorTemperaturePickerComponent;
                }

                switch (root.paramType.type.toLowerCase()) {
                case "color":
                    return colorPickerComponent
                }
                return null;
            }
        }
    }

    Component {
        id: stringComponent
        Label {
            text: {
                switch (root.paramType.type.toLowerCase()) {
                case "int":
                    return Math.round(root.param.value);
                }
                return root.param.value;
            }
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideRight
        }
    }
    Component {
        id: boolComponent
        Item {
            implicitHeight: theSwitch.implicitHeight
            implicitWidth: theSwitch.implicitWidth
            Switch {
                id: theSwitch
                anchors { top: parent.top; right: parent.right; bottom: parent.bottom }
                width: Math.min(parent.width, implicitiWidth)
                checked: root.param.value === true
                Component.onCompleted: {
                    if (root.param.value === undefined) {
                        root.param.value = checked;
                    }
                }

                onClicked: {
                    root.param.value = checked;
                }
            }
        }

    }
    Component {
        id: sliderComponent
        RowLayout {
            spacing: app.margins

            Slider {
                id: slider
                Layout.fillWidth: true
                from: root.paramType.minValue
                to: root.paramType.maxValue
                value: root.param.value
                Component.onCompleted: {
                    if (root.param.value === undefined) {
                        if (root.paramType.defaultValue !== undefined) {
                            root.param.value = root.paramType.defaultValue
                        } else {
                            root.param.value = root.paramType.minValue
                        }
                    }
                }

                stepSize: {
                    var ret = 1
                    for (var i = 0; i < decimals; i++) {
                        ret /= 10;
                    }
                    return ret;
                }
                property int decimals: root.paramType.type.toLocaleLowerCase() === "double" ? 1 : 0

                onMoved: {
                    var newValue
                    switch (root.paramType.type.toLowerCase()) {
                    case "int":
                        newValue = Math.round(value)
                        break;
                    default:
                        newValue = Math.round(value * 10) / 10
                    }
                    root.param.value = newValue;
                }
            }
            Label {
                text: Types.toUiValue(root.param.value, root.paramType.unit).toFixed(slider.decimals) + Types.toUiUnit(root.paramType.unit)
            }
        }

    }

    Component {
        id: spinnerComponent
        RowLayout {
            spacing: app.margins

            SpinBox {
                value: root.param.value ? root.param.value : 0
                from: root.paramType.minValue !== undefined
                      ? root.paramType.minValue
                      : root.paramType.type.toLowerCase() === "uint"
                        ? 0
                        : -2000000000
                to: root.paramType.maxValue !== undefined
                    ? root.paramType.maxValue
                    : 2000000000
                editable: true
                width: 150
                onValueModified: root.param.value = value
                textFromValue: function(value) {
                    return Types.toUiValue(value, root.paramType.unit)
                }
                Component.onCompleted: {
                    print("from:", from, "min", root.paramType.minValue)
                    if (root.value === undefined) {
                        root.value = value
                    }
                }
            }
            Label {
                text: Types.toUiUnit(root.paramType.unit)
                visible: text.length > 0
            }
        }
    }

    Component {
        id: textFieldComponent
        TextField {
            text: root.param.value !== undefined
                  ? root.param.value
                  : root.paramType.defaultValue
                    ? root.paramType.defaultValue
                    : ""
            onEditingFinished: {
                root.param.value = text
            }
            Component.onCompleted: {
                if (root.param.value === undefined) {
                    root.param.value = text;
                }
            }
            placeholderText: root.placeholderText
        }
    }

    Component {
        id: comboBoxComponent
        ComboBox {
            id: control
            Layout.fillWidth: true
            model: root.paramType.allowedValues
            displayText: currentText + ( root.paramType.unit != Types.UnitNone ? " " + Types.toUiUnit(root.paramType.unit) : "")
            currentIndex: root.paramType.allowedValues.indexOf(root.param.value !== undefined ? root.param.value : root.paramType.defaultValue)
            delegate: ItemDelegate {
                width: control.width
                text: Types.toUiValue(modelData, root.paramType.unit) + ( root.paramType.unit != Types.UnitNone ? " " + Types.toUiUnit(root.paramType.unit) : "")
                highlighted: control.highlightedIndex === index
            }
            onActivated: {
                root.param.value = root.paramType.allowedValues[index]
            }
            Component.onCompleted: {
                if (root.value === undefined) {
                    root.value = model[0]
                }
            }
        }
    }

    Component {
        id: colorPickerComponent
        ColorPicker {
            id: colorPicker
            implicitHeight: 200
            //            color: root.param.value

            Binding {
                target: colorPicker
                property: "color"
                value: root.param.value
                when: !colorPicker.pressed
            }

            onColorChanged: {
                root.param.value = color;
            }

            touchDelegate: Rectangle {
                height: 15
                width: height
                radius: height / 2
                color: Material.accent


                Rectangle {
                    color: colorPicker.hovered || colorPicker.pressed ? "#11000000" : "transparent"
                    anchors.centerIn: parent
                    height: 30
                    width: height
                    radius: width / 2
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
            }
        }
    }

    Component {
        id: colorTemperaturePickerComponent
        ColorPickerCt {
            id: colorPickerCt
            implicitHeight: 50
            minCt: root.paramType.minValue
            maxCt: root.paramType.maxValue
            ct: root.param.value !== undefined
                  ? root.param.value
                  : root.paramType.defaultValue
                    ? root.paramType.defaultValue
                    : root.paramType.minValue

            onCtChanged: {
                root.param.value = ct
            }


            touchDelegate: Rectangle {
                height: colorPickerCt.height
                width: 5
                color: app.accentColor
            }
        }
    }

    Component {
        id: colorPreviewComponent
        Rectangle {
            implicitHeight: app.mediumFont
            implicitWidth: implicitHeight
            color: root.param.value
            radius: width / 4
        }
    }
}
