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

import QtQuick 2.9
import QtGraphicalEffects 1.0
import Nymea 1.0
import "../utils"

Item {
    id: root
    implicitWidth: orientation == Qt.Horizontal ? 300 : app.hugeIconSize
    implicitHeight: orientation == Qt.Horizontal ? app.hugeIconSize : 300

    property Thing thing: null

    readonly property StateType colorTemperatureStateType: root.thing.thingClass.stateTypes.findByName("brightness")

    property int value: thing.stateByName("brightness").value

    property int orientation: Qt.Horizontal

    ActionQueue {
        id: actionQueue
        thing: root.thing
        stateType: root.colorTemperatureStateType
    }

    Rectangle {
        id: clipRect
        anchors.fill: parent
        radius: Style.tileRadius
        color: Style.tileOverlayColor
    }

    LinearGradient {
        anchors.fill: parent
        anchors.rightMargin: root.orientation == Qt.Horizontal ?
                                 parent.width - (dragHandle.x + dragHandle.width / 2)
                               : 0
        anchors.topMargin: root.orientation == Qt.Vertical ?
                                 dragHandle.y + dragHandle.height / 2
                               : 0
        start: root.orientation == Qt.Horizontal ? Qt.point(0,0) : Qt.point(0, height)
        end: root.orientation == Qt.Horizontal ? Qt.point(width, 0) : Qt.point(0, 0)
        source: clipRect
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: "#55ffffff" }
        }
    }

    Rectangle {
        id: dragHandle
        x: root.orientation === Qt.Horizontal ?
               (actionQueue.pendingValue || root.value) * (root.width - dragHandle.width) / 100
             : 0
        y: root.orientation === Qt.Vertical ?
               root.height - dragHandle.height - ((actionQueue.pendingValue || root.value) * (root.height - dragHandle.height) / 100)
             : 0
        height: root.orientation === Qt.Horizontal ? parent.height : 8
        width: root.orientation === Qt.Horizontal ? 8 : parent.width
        radius: 4
        color: Style.foregroundColor
    }

    MouseArea {
        anchors.fill: parent
        onPositionChanged: {
            var minCt = root.colorTemperatureStateType.minValue;
            var maxCt = root.colorTemperatureStateType.maxValue
            var ct;
            if (root.orientation == Qt.Horizontal) {
                ct = Math.min(maxCt, Math.max(minCt, (mouseX * (maxCt - minCt) / (width - dragHandle.width)) + minCt))
            } else {
                ct = Math.min(maxCt, Math.max(minCt, ((height - mouseY) * (maxCt - minCt) / (height - dragHandle.height)) + minCt))
            }
            actionQueue.sendValue(ct);
        }
    }
}

