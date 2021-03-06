import QtQuick 2.0

Item {
    property color backgroundColor: "#fafafa"
    property color foregroundColor: "#202020"

    property color accentColor: "#ff57baae"
    property color iconColor: "#808080"

    property color headerBackgroundColor: "#ffffff"
    property color headerForegroundColor: "#202020"

    property color tileBackgroundColor: Qt.tint(backgroundColor, Qt.rgba(foregroundColor.r, foregroundColor.g, foregroundColor.b, 0.05))
    property color tileForegroundColor: foregroundColor
    property color tileOverlayColor: Qt.tint(backgroundColor, Qt.rgba(foregroundColor.r, foregroundColor.g, foregroundColor.b, 0.1))
    property color tileOverlayForegroundColor: foregroundColor
    property color tileOverlayIconColor: iconColor
    property int tileRadius: 6


    // Note: Font files need to be provided in a "fonts" folder in the style
    property string fontFamily: "Ubuntu"

    // Icon/graph colors for various interfaces
    property var interfaceColors: {
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
        "extendedsmartmeterproducer": "lightgreen",
        "smartmeterconsumer": "deepskyblue",
        "extendedsmartmeterconsumer": "deepskyblue",
        "heating" : "gainsboro",
        "thermostat": "dodgerblue",
        "irrigation": "lightblue",
        "windspeedsensor": "blue",
        "ventilation": "lightblue",
        "watersensor": "aqua"
    }

    property var stateColors: {
        "totalEnergyConsumed": "orange",
        "totalEnergyProduced": "lightgreen",
        "currentPower": "deepskyblue",
    }
}
