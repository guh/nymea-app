TARGET = mea-core
TEMPLATE = lib
CONFIG += staticlib

include(../mea.pri)

QT -= gui
QT += websockets bluetooth

INCLUDEPATH += $$top_srcdir/libnymea-common

SOURCES += \
    engine.cpp \
    nymeainterface.cpp \
    devicemanager.cpp \
    websocketinterface.cpp \
    jsonrpc/jsontypes.cpp \
    jsonrpc/jsonrpcclient.cpp \
    jsonrpc/jsonhandler.cpp \
    discovery/nymeahost.cpp \
    discovery/nymeahosts.cpp  \
    discovery/upnpdiscovery.cpp \
    devices.cpp \
    devicesproxy.cpp \
    deviceclasses.cpp \
    deviceclassesproxy.cpp \
    devicediscovery.cpp \
    vendorsproxy.cpp \
    pluginsproxy.cpp \
    tcpsocketinterface.cpp \
    nymeaconnection.cpp \
    interfacesmodel.cpp \
    discovery/zeroconfdiscovery.cpp \
    discovery/discoverydevice.cpp \
    discovery/discoverymodel.cpp \
    rulemanager.cpp \
    models/rulesfiltermodel.cpp \
    models/logsmodel.cpp \
    models/valuelogsproxymodel.cpp \
    discovery/nymeadiscovery.cpp \
    logmanager.cpp \
    basicconfiguration.cpp \
    models/eventdescriptorparamsfiltermodel.cpp \
    wifisetup/bluetoothdevice.cpp \
    wifisetup/bluetoothdeviceinfo.cpp \
    wifisetup/bluetoothdeviceinfos.cpp \
    wifisetup/bluetoothdiscovery.cpp \
    wifisetup/wirelessaccesspoint.cpp \
    wifisetup/wirelessaccesspoints.cpp \
    wifisetup/wirelesssetupmanager.cpp \
    wifisetup/networkmanagercontroler.cpp \

HEADERS += \
    engine.h \
    nymeainterface.h \
    devicemanager.h \
    websocketinterface.h \
    jsonrpc/jsontypes.h \
    jsonrpc/jsonrpcclient.h \
    jsonrpc/jsonhandler.h \
    discovery/nymeahost.h \
    discovery/nymeahosts.h \
    discovery/upnpdiscovery.h \
    devices.h \
    devicesproxy.h \
    deviceclasses.h \
    deviceclassesproxy.h \
    devicediscovery.h \
    vendorsproxy.h \
    pluginsproxy.h \
    tcpsocketinterface.h \
    nymeaconnection.h \
    interfacesmodel.h \
    discovery/zeroconfdiscovery.h \
    discovery/discoverydevice.h \
    discovery/discoverymodel.h \
    rulemanager.h \
    models/rulesfiltermodel.h \
    models/logsmodel.h \
    models/valuelogsproxymodel.h \
    discovery/nymeadiscovery.h \
    logmanager.h \
    basicconfiguration.h \
    models/eventdescriptorparamsfiltermodel.h \
    wifisetup/bluetoothdevice.h \
    wifisetup/bluetoothdeviceinfo.h \
    wifisetup/bluetoothdeviceinfos.h \
    wifisetup/bluetoothdiscovery.h \
    wifisetup/wirelessaccesspoint.h \
    wifisetup/wirelessaccesspoints.h \
    wifisetup/wirelesssetupmanager.h \
    wifisetup/networkmanagercontroler.h \
    libmea-core.h

withavahi {
DEFINES += WITH_AVAHI

LIBS +=  -lavahi-client -lavahi-common

HEADERS += discovery/avahi/avahiserviceentry.h \
    discovery/avahi/qt-watch.h \
    discovery/avahi/qtavahiclient.h \
    discovery/avahi/qtavahiservice_p.h \
    discovery/avahi/qtavahiservice.h \
    discovery/avahi/qtavahiservicebrowser_p.h \
    discovery/avahi/qtavahiservicebrowser.h \

SOURCES += discovery/avahi/avahiserviceentry.cpp \
    discovery/avahi/qt-watch.cpp \
    discovery/avahi/qtavahiclient.cpp \
    discovery/avahi/qtavahiservice_p.cpp \
    discovery/avahi/qtavahiservice.cpp \
    discovery/avahi/qtavahiservicebrowser_p.cpp \
    discovery/avahi/qtavahiservicebrowser.cpp \

}

unix {
    target.path = /usr/lib
    INSTALLS += target
}