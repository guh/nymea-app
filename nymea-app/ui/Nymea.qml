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
import QtQuick.Layouts 1.2
import Qt.labs.settings 1.0
import Qt.labs.folderlistmodel 2.2
import QtQuick.Window 2.3
import Nymea 1.0

ApplicationWindow {
    id: app
    visible: true
    width: settings.windowWidth
    height: settings.windowHeight
    x: 0
    y: 0
    minimumWidth: 350
    minimumHeight: 480
    visibility: kioskMode ? ApplicationWindow.FullScreen : settings.viewMode
    color: Material.background

    // Those variables must be present in the Style
    title: appName
    Material.primary: primaryColor
    Material.accent: accentColor
    Material.foreground: foregroundColor

    property int margins: 16
    property int bigMargins: 20
    property int extraSmallFont: 10
    property int smallFont: 13
    property int mediumFont: 16
    property int largeFont: 20
    property int iconSize: 30
    property int delegateHeight: 60
    property color backgroundColor: Material.background

    readonly property bool landscape: app.width > app.height

    readonly property var settings: Settings {
        property int viewMode: ApplicationWindow.AutomaticVisibility
        property int windowWidth: 1024
        property int windowHeight: 768
        property bool returnToHome: false
        property string graphStyle: "bars"
        property string style: "light"
        property bool showHiddenOptions: false
        property string cloudEnvironment: "Community"
        property bool showConnectionTabs: false
        property int tabCount: 1
        property string units: "metric" // or "imperial"
    }

    property string privacyPolicyUrl: "https://nymea.io/privacy-statement/en/nymea_privacy.html"

    Component.onCompleted: {
        styleController.setSystemFont(app.font)
        PlatformHelper.topPanelColor = app.primaryColor
        PlatformHelper.bottomPanelColor = Material.background
    }

    Binding {
        target: Types
        property: "unitSystem"
        value: settings.units === "metric" ? Types.UnitSystemMetric : Types.UnitSystemImperial
    }

    RootItem {
        id: rootItem
        anchors.fill: parent
        anchors.bottomMargin: keyboardRect.height
    }

    NymeaDiscovery {
        id: discovery
        objectName: "discovery"
        awsClient: AWSClient
        //        discovering: pageStack.currentItem.objectName === "discoveryPage"
    }
    property alias _discovery: discovery

    property var supportedInterfaces: [
        "light",
        "media",
        "awning",
        "shutter",
        "blind",
        "garagedoor",
        "powersocket",
        "heating",
        "smartlock",
        "doorbell",
        "irrigation",
        "ventilation",
        "sensor",
        "weather",
        "evcharger",
        "smartmeter",
        "fingerprintreader",
        "notifications",
        "barcodescanner",
        "button",
        "inputtrigger",
        "outputtrigger",
        "gateway",
        "account"
    ]

    function interfaceToString(name) {
        switch(name) {
        case "light":
            return qsTr("Lighting")
        case "weather":
            return qsTr("Weather")
        case "sensor":
            return qsTr("Sensors")
        case "media":
            return qsTr("Media")
        case "button":
        case "powerswitch":
            return qsTr("Switches")
        case "gateway":
            return qsTr("Gateways")
        case "notifications":
            return qsTr("Notifications")
        case "temperaturesensor":
            return qsTr("Temperature");
        case "humiditysensor":
            return qsTr("Humidity");
        case "pressuresensor":
            return qsTr("Pressure");
        case "noisesensor":
            return qsTr("Noise level");
        case "co2sensor":
            return qsTr("CO2 level")
        case "inputtrigger":
            return qsTr("Incoming Events");
        case "outputtrigger":
            return qsTr("Events");
        case "shutter":
        case "extendedshutter":
            return qsTr("Shutters");
        case "blind":
        case "extendedblind":
            return qsTr("Blinds");
        case "awning":
        case "extendedawning":
            return qsTr("Awnings");
        case "garagedoor":
            return qsTr("Garage doors");
        case "accesscontrol":
            return qsTr("Access control");
        case "fingerprintreader":
            return qsTr("Fingerprint reader");
        case "smartmeter":
        case "smartmeterproducer":
        case "smartmeterconsumer":
        case "extendedsmartmeterproducer":
        case "extendedsmartmeterconsumer":
            return qsTr("Smart meters");
        case "heating":
            return qsTr("Heating");
        case "evcharger":
            return qsTr("EV-chargers");
        case "powersocket":
            return qsTr("Power sockets")
        case "doorbell":
            return qsTr("Doorbells");
        case "account":
            return qsTr("Accounts");
        case "smartlock":
            return qsTr("Smartlocks")
        case "irrigation":
            return qsTr("Irrigation");
        case "ventilation":
            return qsTr("Ventilation")
        case "barcodescanner":
            return qsTr("Barcode scanners");
        case "uncategorized":
            return qsTr("Uncategorized")
        default:
            console.warn("interfaceToString unhandled interface:", name)
        }
        return ""
    }

    function interfacesToIcon(interfaces) {
//        print("finding icon for interfaces:", interfaces)
        for (var i = 0; i < interfaces.length; i++) {
            var icon = interfaceToIcon(interfaces[i]);
            if (icon !== "") {
                return icon;
            }
        }
        return Qt.resolvedUrl("images/select-none.svg")
    }

    function interfaceToIcon(name) {
//        print("finding icon for interface:", name)
        switch (name) {
        case "light":
        case "colorlight":
        case "dimmablelight":
        case "colortemperaturelight":
            return Qt.resolvedUrl("images/light-on.svg")
        case "sensor":
            return Qt.resolvedUrl("images/sensors.svg")
        case "temperaturesensor":
            return Qt.resolvedUrl("images/sensors/temperature.svg")
        case "humiditysensor":
            return Qt.resolvedUrl("images/sensors/humidity.svg")
        case "moisturesensor":
            return Qt.resolvedUrl("images/sensors/moisture.svg")
        case "lightsensor":
            return Qt.resolvedUrl("images/sensors/light.svg")
        case "conductivitysensor":
            return Qt.resolvedUrl("images/sensors/conductivity.svg")
        case "pressuresensor":
            return Qt.resolvedUrl("images/sensors/pressure.svg")
        case "noisesensor":
            return Qt.resolvedUrl("images/sensors/noise.svg");
        case "co2sensor":
            return Qt.resolvedUrl("images/sensors/co2.svg")
        case "daylightsensor":
            return Qt.resolvedUrl("images/sensors/light.svg")
        case "presencesensor":
            return Qt.resolvedUrl("images/sensors/presence.svg")
        case "closablesensor":
            return Qt.resolvedUrl("images/sensors/closable.svg")
        case "windspeedsensor":
            return Qt.resolvedUrl("images/sensors/windspeed.svg")
        case "media":
        case "mediacontroller":
        case "extendedmediacontroller":
        case "mediaplayer":
            return Qt.resolvedUrl("images/media.svg")
        case "powersocket":
            return Qt.resolvedUrl("images/powersocket.svg")
        case "button":
        case "longpressbutton":
        case "simplemultibutton":
        case "longpressmultibutton":
        case "powerswitch":
            return Qt.resolvedUrl("images/system-shutdown.svg")
        case "weather":
            return Qt.resolvedUrl("images/weather-app-symbolic.svg")
        case "gateway":
            return Qt.resolvedUrl("images/connections/network-wired.svg")
        case "notifications":
            return Qt.resolvedUrl("images/messaging-app-symbolic.svg")
        case "inputtrigger":
            return Qt.resolvedUrl("images/attention.svg")
        case "outputtrigger":
            return Qt.resolvedUrl("images/send.svg")
        case "shutter":
        case "extendedshutter":
            return Qt.resolvedUrl("images/shutter/shutter-040.svg")
        case "blind":
        case "extendedblind":
            return Qt.resolvedUrl("images/shutter/shutter-060.svg")
        case "garagedoor":
        case "impulsegaragedoor":
        case "statefulgaragedoor":
        case "extendedstatefulgaragedoor":
        case "garagegate":
            return Qt.resolvedUrl("images/garage/garage-100.svg")
        case "awning":
        case "extendedawning":
            return Qt.resolvedUrl("images/awning/awning-100.svg")
        case "battery":
            return Qt.resolvedUrl("images/battery/battery-050.svg")
        case "uncategorized":
            return Qt.resolvedUrl("images/select-none.svg")
        case "simpleclosable":
            return Qt.resolvedUrl("images/closable-move.svg")
        case "fingerprintreader":
            return Qt.resolvedUrl("images/fingerprint.svg")
        case "accesscontrol":
            return Qt.resolvedUrl("images/lock-closed.svg");
        case "smartmeter":
        case "smartmeterconsumer":
        case "smartmeterproducer":
        case "extendedsmartmeterconsumer":
        case "extendedsmartmeterproducer":
            return Qt.resolvedUrl("images/smartmeter.svg")
        case "heating":
        case "extendedheating":
            return Qt.resolvedUrl("images/radiator.svg")
        case "thermostat":
            return Qt.resolvedUrl("images/dial.svg")
        case "evcharger":
        case "extendedevcharger":
            return Qt.resolvedUrl("images/ev-charger.svg")
        case "doorbell":
            return Qt.resolvedUrl("images/notification.svg")
        case "connectable":
            return Qt.resolvedUrl("images/stock_link.svg")
        case "irrigation":
            return Qt.resolvedUrl("images/irrigation.svg")
        case "ventilation":
            return Qt.resolvedUrl("images/ventilation.svg")
        case "power":
            return Qt.resolvedUrl("images/system-shutdown.svg")
        case "account":
            return Qt.resolvedUrl("images/account.svg")
        case "smartlock":
            return Qt.resolvedUrl("images/smartlock.svg")
        case "navigationpad":
        case "extendednavigationpad":
            return Qt.resolvedUrl("images/navigationpad.svg")
        case "volumecontroller":
        case "extendedvolumecontroller":
            return Qt.resolvedUrl("images/audio-speakers-symbolic.svg")
        case "shufflerepeat":
            return Qt.resolvedUrl("images/media-playlist-shuffle.svg")
        case "alert":
            return Qt.resolvedUrl("images/notification.svg")
        case "barcodescanner":
            return Qt.resolvedUrl("images/qrcode.svg")
        default:
            console.warn("InterfaceToIcon: Unhandled interface", name)
        }
        return "";
    }

    property var fallbackColors: {
        "temperaturesensor": "red",
        "humiditysensor": "deepskyblue",
        "moisturesensor":"blue",
        "lightsensor": "orange",
        "conductivitysensor": "green",
        "pressuresensor": "grey",
        "noisesensor": "darkviolet",
        "co2sensor": "turquoise",
        "daylightsensor": "gold",
        "presencesensor": "darkblue",
        "closablesensor": "green",
        "smartmeterproducer": "lightgreen",
        "smartmeterconsumer": "orange",
        "extendedsmartmeterproducer": "blue",
        "extendedsmartmeterconsumer": "blue",
        "heating" : "gainsboro",
        "thermostat": "dodgerblue",
        "irrigation": "lightblue",
        "windspeedsensor": "blue",
        "ventilation": "lightblue"
    }

    function interfaceToColor(name) {
        // Try to load color map from style
        if (interfaceColors[name]) {
            return interfaceColors[name];
        }

        if (fallbackColors[name]) {
            return fallbackColors[name];
        }

        return "grey";
    }

    function interfaceToDisplayName(name) {
        switch (name) {
        case "light":
        case "dimmablelight":
        case "colorlight":
        case "colortemperaturelight":
            //: Select ...
            return qsTr("light")
        case "sensor":
            //: Select ...
            return qsTr("sensor")
        case "battery":
            //: Select ...
            return qsTr("battery powered thing")
        case "connectable":
            //: Select ...
            return qsTr("connectable thing")
        case "irrigation":
            //: Select ...
            return qsTr("irrigation");
        case "ventilation":
            //: Select ...
            return qsTr("ventilation");
        case "power":
            //: Select ...
            return qsTr("switchable thing")
        case "daylightsensor":
            //: Select ...
            return qsTr("daylight sensor")
        case "presencesensor":
            //: Select ...
            return qsTr("presence sensor")
        case "doorbell":
            //: Select ...
            return qsTr("doorbell")
        case "alert":
            //: Select ...
            return qsTr("alert")
        case "simplemultibutton":
        case "simplebutton":
        case "button":
            //: Select ...
            return qsTr("button")
        case "accesscotrol":
            //: Select ...
            return qsTr("access control")
        case "smartmeter":
        case "smartmeterproducer":
        case "smartmeterconsumer":
        case "extendedsmartmeterproducer":
        case "extendedsmartmeterconsumer":
            //: Select ...
            return qsTr("smart meter");
        case "media":
        case "mediaplayer":
            //: Select ...
            return qsTr("media player");
        case "moisturesensor":
            //: Select ...
            return qsTr("moisture sensor");
        case "notifications":
            //: Select ...
            return qsTr("thing to notify")
        case "smartlock":
            //: Select ...
            return qsTr("smartlock");
        default:
            console.warn("Unhandled interfaceToDisplayName:", name)
        }
    }


    function pad(num, size) {
        var s = "000000000" + num;
        return s.substr(s.length-size);
    }

    // Handle the Android close event that happens when the back button is pressed
    // It's hard to handle the key press, because we might not have focus all the time
    // So let's handle the window's onClosing signal instad.
    // The problem is, we cannot distinguish between the back button being pressed
    // or the bottom swipe gesture is being used to switch apps. Let's try to figure that out
    // by checking if the app becomes inactive right after the event. If not, it's probably a back
    // button press and we close ourselves.
    onClosing: {
        if (Qt.platform.os == "android") {
            var handled = rootItem.handleAndroidBackButton();
            if (!handled) {
                closeTimer.start()
            }
            close.accepted = false;
        }
    }
    Timer {
        id: closeTimer
        interval: 300
        onTriggered: Qt.quit();
    }
    Connections {
        target: Qt.application
        onStateChanged: closeTimer.stop()
    }

    FolderListModel {
        id: availableMainViews
        folder: "mainviews"
        showFiles: false
    }

    // NOTE: If using a Dialog, make sure closePolicy does not contain Dialog.CloseOnPressOutside
    // or the virtual keyboard will close when pressing it...

    // https://bugreports.qt.io/browse/QTBUG-56918
    KeyboardLoader {
        id: keyboardRect
        parent: app.overlay
        z: 1
        anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
    }
}
