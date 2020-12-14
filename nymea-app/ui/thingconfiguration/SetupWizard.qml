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
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Nymea 1.0

import "../components"
import "../delegates"

Page {
    id: root

    property ThingClass thingClass: thing ? thing.thingClass : null
    property alias deviceClass: root.thingClass

    // Optional: If set, it will be reconfigred, otherwise a new one will be created
    property Thing thing: null
    property alias device: root.thing // Transitional, use thing instead

    signal done();

    header: NymeaHeader {
        text: root.device ? qsTr("Reconfigure %1").arg(root.device.name) : qsTr("Set up %1").arg(root.deviceClass.displayName)
        onBackPressed: {
            if (internalPageStack.depth > 1) {
                internalPageStack.pop();
            } else {
                pageStack.pop();
            }
        }

        HeaderButton {
            imageSource: "../images/close.svg"
            onClicked: root.done();
        }
    }

    QtObject {
        id: d
        property var vendorId: null
        property ThingDescriptor thingDescriptor: null
        property alias deviceDescriptor: d.thingDescriptor
        property var discoveryParams: []
        property string deviceName: ""
        property int pairRequestId: 0
        property var pairingTransactionId: null
        property int addRequestId: 0
    }

    Component.onCompleted: {
        print("Starting setup wizard. Create Methods:", root.deviceClass.createMethods, "Setup method:", root.deviceClass.setupMethod)
        if (root.deviceClass.createMethods.indexOf("CreateMethodDiscovery") !== -1) {
            print("CreateMethodDiscovery")
            if (deviceClass["discoveryParamTypes"].count > 0) {
                print("Discovery params:", deviceClass.discoveryParamTypes.count)
                internalPageStack.push(discoveryParamsPage)
            } else {
                print("Starting discovery...")
                discovery.discoverDevices(deviceClass.id)
                internalPageStack.push(discoveryPage, {deviceClass: deviceClass})
            }
        } else if (root.deviceClass.createMethods.indexOf("CreateMethodUser") !== -1) {
            print("CreateMethodUser")
            // Setting up a new device
            if (!root.thing) {
                print("New thing setup")
                internalPageStack.push(paramsPage)

            // Reconfigure
            } else if (root.device) {
                print("Existing device")
                // There are params. Open params page in any case
                if (root.deviceClass.paramTypes.count > 0) {
                    print("Params:", root.deviceClass.paramTypes.count)
                    internalPageStack.push(paramsPage)

                // No params... go straight to reconfigure/repair
                } else {
                    print("no params")
                    switch (root.deviceClass.setupMethod) {
                    case 0:
                        print("reconfiguring...")
                        // This totally does not make sense... Maybe we should hide the reconfigure button if there are no params?
                        engine.deviceManager.reconfigureDevice(root.device.id, [])
                        busyOverlay.shown = true;
                        break;
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        print("re-pairing", root.device.id)
                        engine.deviceManager.rePairDevice(root.device.id, []);
                        break;
                    default:
                        console.warn("Unhandled setup method!")
                    }
                }
            }
        }
    }

    Connections {
        target: engine.deviceManager
        onPairDeviceReply: {
            busyOverlay.shown = false
            if (params["deviceError"] !== "DeviceErrorNoError") {
                busyOverlay.shown = false;
                internalPageStack.push(resultsPage, {deviceError: params["deviceError"], message: params["displayMessage"]});
                return;

            }

            d.pairingTransactionId = params["pairingTransactionId"];

            switch (params["setupMethod"]) {
            case "SetupMethodPushButton":
            case "SetupMethodDisplayPin":
            case "SetupMethodUserAndPassword":
                internalPageStack.push(pairingPageComponent, {text: params["displayMessage"], setupMethod: params["setupMethod"]})
                break;
            case "SetupMethodOAuth":
                internalPageStack.push(oAuthPageComponent, {oAuthUrl: params["oAuthUrl"]})
                break;
            default:
                print("Setup method reply not handled:", JSON.stringify(params));
            }
        }
        onConfirmPairingReply: {
            busyOverlay.shown = false
            internalPageStack.push(resultsPage, {deviceError: params["deviceError"], deviceId: params["deviceId"], message: params["displayMessage"]})
        }
        onAddDeviceReply: {
            print("Device added:", JSON.stringify(params))
            busyOverlay.shown = false;
            internalPageStack.push(resultsPage, {deviceError: params["deviceError"], deviceId: params["deviceId"], message: params["displayMessage"]})
        }
        onReconfigureDeviceReply: {
            busyOverlay.shown = false;
            internalPageStack.push(resultsPage, {deviceError: params["deviceError"], deviceId: params["deviceId"], message: params["displayMessage"]})
        }
    }

    DeviceDiscovery {
        id: discovery
        engine: _engine
    }

    StackView {
        id: internalPageStack
        anchors.fill: parent
    }

    Component {
        id: discoveryParamsPage
        Page {
            id: discoveryParamsView

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10

                Flickable {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    ColumnLayout {
                        width: parent.width

                        Repeater {
                            id: paramRepeater
                            model: root.deviceClass ? root.deviceClass["discoveryParamTypes"] : null
                            Loader {
                                Layout.fillWidth: true
                                sourceComponent: searchStringEntryComponent
                                property var discoveryParams: model
                                property var value: item ? item.value : null
                            }
                        }
                        Button {
                            Layout.fillWidth: true
                            text: "Next"
                            onClicked: {
                                var paramTypes = root.deviceClass["discoveryParamTypes"];
                                d.discoveryParams = [];
                                for (var i = 0; i < paramTypes.count; i++) {
                                    var param = {};
                                    param["paramTypeId"] = paramTypes.get(i).id;
                                    param["value"] = paramRepeater.itemAt(i).value
                                    d.discoveryParams.push(param);
                                }
                                discovery.discoverDevices(root.deviceClass.id, d.discoveryParams)
                                internalPageStack.push(discoveryPage, {deviceClass: root.deviceClass})
                            }
                        }
                    }
                }

                Component {
                    id: searchStringEntryComponent
                    ColumnLayout {
                        property alias value: searchTextField.text
                        Label {
                            text: discoveryParams.displayName
                            Layout.fillWidth: true
                        }
                        TextField {
                            id: searchTextField
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }

    Component {
        id: discoveryPage

        Page {
            id: discoveryView

            property var deviceClass: null

            ColumnLayout {
                anchors.fill: parent
                anchors.bottomMargin: app.margins

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: DeviceDiscoveryProxy {
                        id: discoveryProxy
                        deviceDiscovery: discovery
                        showAlreadyAdded: root.device !== null
                        showNew: root.device === null
                        filterDeviceId: root.device ? root.device.id : ""
                    }
                    delegate: NymeaSwipeDelegate {
                        width: parent.width
                        height: app.delegateHeight
                        text: model.name
                        subText: model.description
                        iconName: app.interfacesToIcon(discoveryView.deviceClass.interfaces)
                        onClicked: {
                            d.deviceDescriptor = discoveryProxy.get(index);
                            d.deviceName = model.name;
                            internalPageStack.push(paramsPage)
                        }
                    }
                }
                Button {
                    id: retryButton
                    Layout.fillWidth: true
                    Layout.leftMargin: app.margins; Layout.rightMargin: app.margins
                    text: qsTr("Search again")
                    onClicked: discovery.discoverDevices(root.deviceClass.id, d.discoveryParams)
                    visible: !discovery.busy
                }

                Button {
                    id: manualAddButton
                    Layout.fillWidth: true
                    Layout.leftMargin: app.margins; Layout.rightMargin: app.margins;
                    visible: root.deviceClass.createMethods.indexOf("CreateMethodUser") >= 0
                    text: qsTr("Add thing manually")
                    onClicked: internalPageStack.push(paramsPage)
                }
            }


            ColumnLayout {
                anchors.centerIn: parent
                width: parent.width - app.margins * 2
                visible: discovery.busy
                spacing: app.margins * 2
                Label {
                    text: qsTr("Searching for things...")
                    Layout.fillWidth: true
                    font.pixelSize: app.largeFont
                    horizontalAlignment: Text.AlignHCenter
                }
                BusyIndicator {
                    running: visible
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            ColumnLayout {
                anchors.centerIn: parent
                width: parent.width - app.margins * 2
                visible: !discovery.busy && discoveryProxy.count === 0
                spacing: app.margins * 2
                Label {
                    text: qsTr("Too bad...")
                    font.pixelSize: app.largeFont
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }
                Label {
                    text: qsTr("No things of this kind could be found...")
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: discovery.displayMessage.length === 0 ?
                              qsTr("Make sure your things are set up and connected, try searching again or go back and pick a different kind of thing.")
                            : discovery.displayMessage
                    wrapMode: Text.WordWrap
                }
            }
        }
    }

    Component {
        id: paramsPage

        Page {
            id: paramsView
            Flickable {
                anchors.fill: parent
                contentHeight: paramsColumn.implicitHeight

                ColumnLayout {
                    id: paramsColumn
                    width: parent.width

                    ColumnLayout {
//                        visible: root.device === null
                        Label {
                            Layout.leftMargin: app.margins
                            Layout.rightMargin: app.margins
                            Layout.topMargin: app.margins
                            Layout.fillWidth: true
                            text: qsTr("Name the thing:")
                        }
                        TextField {
                            id: nameTextField
                            text: (d.deviceName ? d.deviceName : root.deviceClass.displayName)
                                  + (root.thingClass.id.toString().match(/\{?f0dd4c03-0aca-42cc-8f34-9902457b05de\}?/) ? " (" + PlatformHelper.machineHostname + ")" : "")
                            Layout.fillWidth: true
                            Layout.leftMargin: app.margins
                            Layout.rightMargin: app.margins
                        }

                        ThinDivider {
                            visible: paramRepeater.count > 0
                        }
                    }

                    Repeater {
                        id: paramRepeater
                        model: engine.jsonRpcClient.ensureServerVersion("1.12") || d.deviceDescriptor == null ?  root.deviceClass.paramTypes : null
                        delegate: ParamDelegate {
//                            Layout.preferredHeight: 60
                            Layout.fillWidth: true
                            enabled: !model.readOnly
                            paramType: root.deviceClass.paramTypes.get(index)
                            //visible: root.thingClass.id.toString().match(/\{?f0dd4c03-0aca-42cc-8f34-9902457b05de\}?/) === null
                            value: {
                                // Discovery, use params from discovered descriptor
                                if (d.thingDescriptor && d.thingDescriptor.params.getParam(paramType.id)) {
                                    return d.thingDescriptor.params.getParam(paramType.id).value
                                }

                                // Special hook for push notifications as we need to provide the token etc implicitly
                                print("Setting up params for thing class:", root.thingClass.id, root.thingClass.name)
                                if (root.thingClass.id.toString().match(/\{?f0dd4c03-0aca-42cc-8f34-9902457b05de\}?/)) {
                                    if (paramType.id.toString().match(/\{?3cb8e30e-2ec5-4b4b-8c8c-03eaf7876839\}?/)) {
                                        return PushNotifications.service;
                                    }
                                    if (paramType.id.toString().match(/\{?12ec06b2-44e7-486a-9169-31c684b91c8f\}?/)) {
                                        return PushNotifications.token;
                                    }
                                    if (paramType.id.toString().match(/\{?d76da367-64e3-4b7d-aa84-c96b3acfb65e\}?/)) {
                                        return PushNotifications.clientId;
                                    }
                                }

                                // Manual setup, use default value from thing class
                                return root.thingClass.paramTypes.get(index).defaultValue
                            }
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        Layout.leftMargin: app.margins
                        Layout.rightMargin: app.margins

                        text: "OK"
                        onClicked: {
                            print("setupMethod", root.deviceClass.setupMethod)

                            var params = []
                            for (var i = 0; i < paramRepeater.count; i++) {
                                var param = {}
                                var paramType = paramRepeater.itemAt(i).paramType
                                if (!paramType.readOnly) {
                                    param.paramTypeId = paramType.id
                                    param.value = paramRepeater.itemAt(i).value
                                    print("adding param", param.paramTypeId, param.value)
                                    params.push(param)
                                }
                            }

                            switch (root.deviceClass.setupMethod) {
                            case 0:
                                if (root.device) {
                                    if (d.deviceDescriptor) {
                                        engine.deviceManager.reconfigureDiscoveredDevice(root.device.id, d.deviceDescriptor.id, params);
                                    } else {
                                        engine.deviceManager.reconfigureDevice(root.device.id, params);
                                    }
                                } else {
                                    if (d.deviceDescriptor) {
                                        engine.deviceManager.addDiscoveredDevice(root.deviceClass.id, d.deviceDescriptor.id, nameTextField.text, params);
                                    } else {
                                        engine.deviceManager.addDevice(root.deviceClass.id, nameTextField.text, params);
                                    }
                                }
                                break;
                            case 1:
                            case 2:
                            case 3:
                            case 4:
                            case 5:
                                if (root.device) {
                                    if (d.deviceDescriptor) {
                                        engine.deviceManager.pairDiscoveredDevice(root.deviceClass.id, d.deviceDescriptor.id, params, nameTextField.text);
                                    } else {
                                        engine.deviceManager.rePairDevice(root.device.id, params, nameTextField.text);
                                    }
                                    return;
                                } else {
                                    if (d.deviceDescriptor) {
                                        engine.deviceManager.pairDiscoveredDevice(root.deviceClass.id, d.deviceDescriptor.id, params, nameTextField.text);
                                    } else {
                                        engine.deviceManager.pairDevice(root.deviceClass.id, params, nameTextField.text);
                                    }
                                }

                                break;
                            }

                            busyOverlay.shown = true;

                        }
                    }
                }
            }
        }
    }

    Component {
        id: pairingPageComponent
        Page {
            id: pairingPage
            property alias text: textLabel.text

            property string setupMethod

            ColumnLayout {
                anchors.centerIn: parent
                width: parent.width - app.margins * 2
                spacing: app.margins * 2

                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    font.pixelSize: app.largeFont
                    text: qsTr("Pairing...")
                    color: Style.accentColor
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    id: textLabel
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }

                TextField {
                    id: usernameTextField
                    Layout.fillWidth: true
                    visible: pairingPage.setupMethod === "SetupMethodUserAndPassword"
                }

                PasswordTextField {
                    id: pinTextField
                    Layout.fillWidth: true
                    visible: pairingPage.setupMethod === "SetupMethodDisplayPin" || pairingPage.setupMethod === "SetupMethodUserAndPassword"
                    signup: false
                }


                Button {
                    Layout.fillWidth: true
                    text: "OK"
                    onClicked: {
                        engine.deviceManager.confirmPairing(d.pairingTransactionId, pinTextField.password, usernameTextField.displayText);
                        busyOverlay.shown = true;
                    }
                }
            }
        }
    }

    Component {
        id: oAuthPageComponent
        Page {
            id: oAuthPage
            property string oAuthUrl

            ColumnLayout {
                anchors.centerIn: parent
                width: parent.width - app.margins * 2
                spacing: app.margins * 2

                Label {
                    Layout.fillWidth: true
                    text: qsTr("OAuth is not supported on this platform. Please use this app on a different device to set up this thing.")
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    Layout.fillWidth: true
                    text: qsTr("In order to use OAuth on this platform, make sure qml-module-qtwebview is installed.")
                    wrapMode: Text.WordWrap
                    font.pixelSize: app.smallFont
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Item {
                id: webViewContainer
                anchors.fill: parent

                Component.onCompleted: {
                    // This might fail if qml-module-qtwebview isn't around
                    Qt.createQmlObject(webViewString, webViewContainer);
                }

                property string webViewString:
                    '
                    import QtQuick 2.8;
                    import QtWebView 1.1;
                    WebView {
                        id: oAuthWebView
                        anchors.fill: parent
                        url: oAuthPage.oAuthUrl

                        onUrlChanged: {
                            print("OAUTH URL changed", url)
                            if (url.toString().indexOf("https://127.0.0.1") == 0) {
                                print("Redirect URL detected!");
                                engine.deviceManager.confirmPairing(d.pairingTransactionId, url)
                            }
                        }
                    }
                    '
            }
        }
    }

    Component {
        id: resultsPage

        Page {
            id: resultsView

            property string deviceId
            property string deviceError
            property string message

            readonly property bool success: deviceError === "DeviceErrorNoError"

            readonly property var device: root.device ? root.device : engine.deviceManager.devices.getDevice(deviceId)

            ColumnLayout {
                width: parent.width - app.margins * 2
                anchors.centerIn: parent
                spacing: app.margins * 2
                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    text: resultsView.success ? root.device ? qsTr("Thing reconfigured!") : qsTr("Thing added!") : qsTr("Uh oh")
                    font.pixelSize: app.largeFont
                    color: Style.accentColor
                }
                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: resultsView.success ? qsTr("All done. You can now start using %1.").arg(resultsView.device.name) : qsTr("Something went wrong setting up this thing...");
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: resultsView.message
                }

                Button {
                    Layout.fillWidth: true
                    text: qsTr("Ok")
                    onClicked: {
                        root.done();
                    }
                }
            }
        }
    }

    BusyOverlay {
        id: busyOverlay
    }
}
