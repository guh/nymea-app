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

#ifndef LIBNYMEAAPPCORE_H
#define LIBNYMEAAPPCORE_H

#include "engine.h"
#include "connection/nymeahosts.h"
#include "connection/nymeahost.h"
#include "connection/discovery/nymeadiscovery.h"
#include "vendorsproxy.h"
#include "thingclassesproxy.h"
#include "thingsproxy.h"
#include "pluginsproxy.h"
#include "thingdiscovery.h"
#include "interfacesmodel.h"
#include "rulemanager.h"
#include "models/rulesfiltermodel.h"
#include "types/ruleactions.h"
#include "types/ruleaction.h"
#include "types/ruleactionparams.h"
#include "types/ruleactionparam.h"
#include "types/eventdescriptors.h"
#include "types/eventdescriptor.h"
#include "types/timedescriptor.h"
#include "types/timeeventitems.h"
#include "types/timeeventitem.h"
#include "types/repeatingoption.h"
#include "types/calendaritems.h"
#include "types/calendaritem.h"
#include "types/rule.h"
#include "types/interfaces.h"
#include "types/interface.h"
#include "types/statedescriptor.h"
#include "types/stateevaluator.h"
#include "types/stateevaluators.h"
#include "types/browseritems.h"
#include "types/browseritem.h"
#include "models/logsmodel.h"
#include "models/logsmodelng.h"
#include "models/barseriesadapter.h"
#include "models/xyseriesadapter.h"
#include "models/interfacesproxy.h"
#include "configuration/nymeaconfiguration.h"
#include "configuration/serverconfiguration.h"
#include "configuration/serverconfigurations.h"
#include "configuration/mqttpolicy.h"
#include "configuration/mqttpolicies.h"
#include "wifisetup/btwifisetup.h"
#include "types/wirelessaccesspoint.h"
#include "types/wirelessaccesspoints.h"
#include "models/wirelessaccesspointsproxy.h"
#include "tagsmanager.h"
#include "models/tagsproxymodel.h"
#include "models/taglistmodel.h"
#include "types/tag.h"
#include "ruletemplates/ruletemplates.h"
#include "ruletemplates/ruletemplate.h"
#include "ruletemplates/eventdescriptortemplate.h"
#include "ruletemplates/timedescriptortemplate.h"
#include "ruletemplates/calendaritemtemplate.h"
#include "ruletemplates/timeeventitemtemplate.h"
#include "ruletemplates/stateevaluatortemplate.h"
#include "ruletemplates/statedescriptortemplate.h"
#include "ruletemplates/ruleactiontemplate.h"
#include "ruletemplates/ruleactionparamtemplate.h"
#include "connection/awsclient.h"
#include "models/thingmodel.h"
#include "models/sortfilterproxymodel.h"
#include "system/systemcontroller.h"
#include "types/package.h"
#include "types/packages.h"
#include "types/repository.h"
#include "types/repositories.h"
#include "models/packagesfiltermodel.h"
#include "configuration/networkmanager.h"
#include "types/networkdevices.h"
#include "types/networkdevice.h"
#include "scriptsyntaxhighlighter.h"
#include "scriptmanager.h"
#include "scripting/codecompletion.h"
#include "scripting/completionmodel.h"
#include "scripting/scriptautosaver.h"
#include "types/script.h"
#include "types/scripts.h"
#include "types/types.h"
#include "usermanager.h"
#include "types/tokeninfos.h"
#include "types/tokeninfo.h"
#include "types/userinfo.h"
#include "thinggroup.h"
#include "types/statetypesproxy.h"
#include "types/ioconnection.h"
#include "types/ioconnections.h"
#include "types/ioconnectionwatcher.h"
#include "zigbee/zigbeemanager.h"
#include "zigbee/zigbeeadapter.h"
#include "zigbee/zigbeeadapters.h"
#include "zigbee/zigbeeadaptersproxy.h"
#include "zigbee/zigbeenetwork.h"
#include "zigbee/zigbeenetworks.h"
#include "modbus/modbusrtumanager.h"
#include "modbus/modbusrtumasters.h"
#include "types/serialportsproxy.h"

#include <QtQml/qqml.h>

static QObject* interfacesModel_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new Interfaces();
}

static QObject* awsClientProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return AWSClient::instance();
}

static QObject* typesProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return Types::instance();
}

void registerQmlTypes() {

    const char uri[] = "Nymea";

    qmlRegisterType<Engine>(uri, 1, 0, "Engine");

    qmlRegisterUncreatableType<ThingManager>(uri, 1, 0, "ThingManager", "Can't create this in QML. Get it from the Engine.");
    qmlRegisterUncreatableType<JsonRpcClient>(uri, 1, 0, "JsonRpcClient", "Can't create this in QML. Get it from the Engine.");
    qmlRegisterUncreatableType<NymeaConnection>(uri, 1, 0, "NymeaConnection", "Can't create this in QML. Get it from the Engine.");

    // libnymea-common
    qmlRegisterSingletonType<Types>(uri, 1, 0, "Types", typesProvider);

    qmlRegisterUncreatableType<ParamType>(uri, 1, 0, "ParamType", "Can't create this in QML. Get it from the ParamTypes.");
    qmlRegisterUncreatableType<ParamTypes>(uri, 1, 0, "ParamTypes", "Can't create this in QML. Get it from the ThingClass.");
    qmlRegisterUncreatableType<EventType>(uri, 1, 0, "EventType", "Can't create this in QML. Get it from the EventTypes.");
    qmlRegisterUncreatableType<EventTypes>(uri, 1, 0, "EventTypes", "Can't create this in QML. Get it from the ThingClass.");
    qmlRegisterUncreatableType<StateType>(uri, 1, 0, "StateType", "Can't create this in QML. Get it from the StateTypes.");
    qmlRegisterUncreatableType<StateTypes>(uri, 1, 0, "StateTypes", "Can't create this in QML. Get it from the ThingClass.");
    qmlRegisterUncreatableType<ActionType>(uri, 1, 0, "ActionType", "Can't create this in QML. Get it from the ActionTypes.");
    qmlRegisterUncreatableType<ActionTypes>(uri, 1, 0, "ActionTypes", "Can't create this in QML. Get it from the ThingClass.");
    qmlRegisterType<StateTypesProxy>(uri, 1, 0, "StateTypesProxy");

    qmlRegisterUncreatableType<State>(uri, 1, 0, "State", "Can't create this in QML. Get it from the States.");
    qmlRegisterUncreatableType<States>(uri, 1, 0, "States", "Can't create this in QML. Get it from the Thing.");

    qmlRegisterUncreatableType<BrowserItems>(uri, 1, 0, "BrowserItems", "Can't create this in QML. Get it from ThingManager.");
    qmlRegisterUncreatableType<BrowserItem>(uri, 1, 0, "BrowserItem", "Can't create this in QML. Get it from BrowserItems.");

    qmlRegisterUncreatableType<Vendor>(uri, 1, 0, "Vendor", "Can't create this in QML. Get it from the Vendors.");
    qmlRegisterUncreatableType<Vendors>(uri, 1, 0, "Vendors", "Can't create this in QML. Get it from the ThingManager.");
    qmlRegisterType<VendorsProxy>(uri, 1, 0, "VendorsProxy");

    qmlRegisterUncreatableType<Thing>(uri, 1, 0, "Thing", "Can't create this in QML. Get it from the Things.");
    qmlRegisterUncreatableType<Things>(uri, 1, 0, "Things", "Can't create this in QML. Get it from the ThingManager.");
    qmlRegisterType<ThingsProxy>(uri, 1, 0, "ThingsProxy");
    qmlRegisterType<InterfacesModel>(uri, 1, 0, "InterfacesModel");
    qmlRegisterType<InterfacesSortModel>(uri, 1, 0, "InterfacesSortModel");

    qmlRegisterUncreatableType<ThingClass>(uri, 1, 0, "ThingClass", "Can't create this in QML. Get it from the ThingClasses.");
    qmlRegisterUncreatableType<ThingClasses>(uri, 1, 0, "ThingClasses", "Can't create this in QML. Get it from the ThingManager.");
    qmlRegisterType<ThingClassesProxy>(uri, 1, 0, "ThingClassesProxy");
    qmlRegisterType<ThingDiscovery>(uri, 1, 0, "ThingDiscovery");
    qmlRegisterType<ThingDiscoveryProxy>(uri, 1, 0, "ThingDiscoveryProxy");
    qmlRegisterUncreatableType<ThingDescriptor>(uri, 1, 0, "ThingDescriptor", "Get it from ThingDiscovery");

    qmlRegisterType<ThingModel>(uri, 1, 0, "ThingModel");

    qmlRegisterUncreatableType<RuleManager>(uri, 1, 0, "RuleManager", "Get it from the Engine");
    qmlRegisterUncreatableType<Rules>(uri, 1, 0, "Rules", "Get it from RuleManager");
    qmlRegisterUncreatableType<Rule>(uri, 1, 0, "Rule", "Get it from Rules");
    qmlRegisterUncreatableType<RuleActions>(uri, 1, 0, "RuleActions", "Get them from the rule");
    qmlRegisterUncreatableType<RuleAction>(uri, 1, 0, "RuleAction", "Get it from RuleActions");
    qmlRegisterUncreatableType<RuleActionParams>(uri, 1, 0, "RuleActionParams", "Get it from RuleActions");
    qmlRegisterUncreatableType<RuleActionParam>(uri, 1, 0, "RuleActionParam", "Get it from RuleActionParams");
    qmlRegisterType<RulesFilterModel>(uri, 1, 0, "RulesFilterModel");
    qmlRegisterUncreatableType<EventDescriptors>(uri, 1, 0, "EventDescriptors", "Get them from rules");
    qmlRegisterUncreatableType<EventDescriptor>(uri, 1, 0, "EventDescriptor", "Get them from rules");
    qmlRegisterUncreatableType<ParamTypes>(uri, 1, 0, "ParamTypes", "Uncreatable");
    qmlRegisterUncreatableType<ParamType>(uri, 1, 0, "ParamType", "Uncreatable");
    qmlRegisterType<Param>(uri, 1, 0, "Param");
    qmlRegisterUncreatableType<ParamDescriptor>(uri, 1, 0, "ParamDescriptor", "Uncreatable");
    qmlRegisterUncreatableType<ParamDescriptors>(uri, 1, 0, "ParamDescriptors", "Uncreatable");
    qmlRegisterUncreatableType<StateDescriptor>(uri, 1, 0, "StateDescriptor", "Uncreatable");
    qmlRegisterUncreatableType<StateEvaluator>(uri, 1, 0, "StateEvaluator", "Uncreatable");
    qmlRegisterUncreatableType<StateEvaluators>(uri, 1, 0, "StateEvaluators", "Uncreatable");
    qmlRegisterUncreatableType<TimeDescriptor>(uri, 1, 0, "TimeDescriptor", "Uncreatable");
    qmlRegisterUncreatableType<TimeEventItems>(uri, 1, 0, "TimeEventItems", "Uncreatable");
    qmlRegisterUncreatableType<TimeEventItem>(uri, 1, 0, "TimeEventItem", "Uncreatable");
    qmlRegisterUncreatableType<RepeatingOption>(uri, 1, 0, "RepeatingOption", "Uncreatable");
    qmlRegisterUncreatableType<CalendarItems>(uri, 1, 0, "CalendarItems", "Uncreatable");
    qmlRegisterUncreatableType<CalendarItem>(uri, 1, 0, "CalendarItem", "Uncreatable");

    qmlRegisterUncreatableType<Interface>(uri, 1, 0, "Interface", "Uncreatable");
    qmlRegisterSingletonType<Interfaces>(uri, 1, 0, "Interfaces", interfacesModel_provider);
    qmlRegisterType<InterfacesProxy>(uri, 1, 0, "InterfacesProxy");

    qmlRegisterUncreatableType<ThingGroup>(uri, 1, 0, "ThingGroup", "Uncreatable");

    qmlRegisterUncreatableType<Plugin>(uri, 1, 0, "Plugin", "Can't create this in QML. Get it from the Plugins.");
    qmlRegisterUncreatableType<Plugins>(uri, 1, 0, "Plugins", "Can't create this in QML. Get it from the ThingManager.");
    qmlRegisterType<PluginsProxy>(uri, 1, 0, "PluginsProxy");

    qmlRegisterUncreatableType<NymeaConfiguration>(uri, 1, 0, "NymeaConfiguration", "Get it from Engine");
    qmlRegisterUncreatableType<ServerConfiguration>(uri, 1, 0, "ServerConfiguration", "Get it from NymeaConfiguration");
    qmlRegisterUncreatableType<ServerConfigurations>(uri, 1, 0, "ServerConfigurations", "Get it from NymeaConfiguration");
    qmlRegisterUncreatableType<WebServerConfiguration>(uri, 1, 0, "WebServerConfiguration", "Get it from NymeaConfiguration");
    qmlRegisterUncreatableType<WebServerConfigurations>(uri, 1, 0, "WebServerConfigurations", "Get it from NymeaConfiguration");
    qmlRegisterUncreatableType<MqttPolicy>(uri, 1, 0, "MqttPolicy", "Get it from NymeaConfiguration");
    qmlRegisterUncreatableType<MqttPolicies>(uri, 1, 0, "MqttPolicies", "Get it from NymeaConfiguration");

    qmlRegisterType<NymeaDiscovery>(uri, 1, 0, "NymeaDiscovery");
    qmlRegisterUncreatableType<NymeaHosts>(uri, 1, 0, "NymeaHosts", "Get it from NymeaDiscovery");
    qmlRegisterType<NymeaHostsFilterModel>(uri, 1, 0, "NymeaHostsFilterModel");
    qmlRegisterUncreatableType<NymeaHost>(uri, 1, 0, "NymeaHost", "Get it from NymeaHosts");
    qmlRegisterUncreatableType<Connection>(uri, 1, 0, "Connection", "Get it from NymeaHost");

    qmlRegisterType<LogsModel>(uri, 1, 0, "LogsModel");
    qmlRegisterType<LogsModelNg>(uri, 1, 0, "LogsModelNg");
    qmlRegisterUncreatableType<LogEntry>(uri, 1, 0, "LogEntry", "Get them from LogsModel");
    qmlRegisterType<BarSeriesAdapter>(uri, 1, 0, "BarSeriesAdapter");
    qmlRegisterType<XYSeriesAdapter>(uri, 1, 0, "XYSeriesAdapter");

    qmlRegisterUncreatableType<TagsManager>(uri, 1, 0, "TagsManager", "Get it from Engine");
    qmlRegisterUncreatableType<Tags>(uri, 1, 0, "Tags", "Get it from TagsManager");
    qmlRegisterUncreatableType<Tag>(uri, 1, 0, "Tag", "Get it from Tags");
    qmlRegisterType<TagsProxyModel>(uri, 1, 0, "TagsProxyModel");
    qmlRegisterType<TagListModel>(uri, 1, 0, "TagListModel");
    qmlRegisterType<TagListProxyModel>(uri, 1, 0, "TagListProxyModel");

    qmlRegisterType<BtWiFiSetup>(uri, 1, 0, "BtWiFiSetup");
    qmlRegisterType<BluetoothDiscovery>(uri, 1, 0, "BluetoothDiscovery");
    qmlRegisterUncreatableType<BluetoothDeviceInfo>(uri, 1, 0, "BluetoothDeviceInfo", "Can't create this in QML. Get it from the DeviceInfos.");
    qmlRegisterUncreatableType<BluetoothDeviceInfos>(uri, 1, 0, "BluetoothDeviceInfos", "Can't create this in QML. Get it from the BluetoothDiscovery.");
    qmlRegisterUncreatableType<WirelessAccessPoint>(uri, 1, 0, "WirelessAccessPoint", "Can't create this in QML. Get it from the WirelessAccessPoints.");
    qmlRegisterUncreatableType<WirelessAccessPoints>(uri, 1, 0, "WirelessAccessPoints", "Can't create this in QML. Get it from the Engine instance.");
    qmlRegisterType<WirelessAccessPointsProxy>(uri, 1, 0, "WirelessAccessPointsProxy");

    qmlRegisterSingletonType<AWSClient>(uri, 1, 0, "AWSClient", awsClientProvider);
    qmlRegisterUncreatableType<AWSDevice>(uri, 1, 0, "AWSDevice", "Can't create this in QML. Get it from AWSClient");

    qmlRegisterType<RuleTemplates>(uri, 1, 0, "RuleTemplates");
    qmlRegisterType<RuleTemplatesFilterModel>(uri, 1, 0, "RuleTemplatesFilterModel");
    qmlRegisterUncreatableType<RuleTemplate>(uri, 1, 0, "RuleTemplate", "Get them from RuleTemplates");
    qmlRegisterUncreatableType<EventDescriptorTemplates>(uri, 1, 0, "EventDescriptorTemplates", "Get it from RuleTemplate");
    qmlRegisterUncreatableType<EventDescriptorTemplate>(uri, 1, 0, "EventDescriptorTemplate", "Get it from EventDescriptorTemplates");
    qmlRegisterUncreatableType<TimeDescriptorTemplate>(uri, 1, 0, "TimeDescriptorTemplate", "Get it from RuleTemplate");
    qmlRegisterUncreatableType<CalendarItemTemplates>(uri, 1, 0, "CalendarItemTemplates", "Get it from TimeDescriptorTemplate");
    qmlRegisterUncreatableType<CalendarItemTemplate>(uri, 1, 0, "CalendarItemTemplate", "Get it from CalendarItemTemplates");
    qmlRegisterUncreatableType<TimeEventItemTemplates>(uri, 1, 0, "TimeEventItemTemplates", "Get it from TimeDescriptorTemplate");
    qmlRegisterUncreatableType<TimeEventItemTemplate>(uri, 1, 0, "TimeEventItemTemplate", "Get it from TimeEventItemTemplates");
    qmlRegisterUncreatableType<StateEvaluatorTemplate>(uri, 1, 0, "StateEvaluatorTemplate", "Get it from RuleTemplate");
    qmlRegisterUncreatableType<StateDescriptorTemplate>(uri, 1, 0, "StateDescriptorTemplate", "Get it from StateEvaluatorTemplate");
    qmlRegisterUncreatableType<RuleActionTemplates>(uri, 1, 0, "RuleActionTemplates", "Get it from RuleTemplate");
    qmlRegisterUncreatableType<RuleActionTemplate>(uri, 1, 0, "RuleActionTemplate", "Get it from RuleActionTemplates");
    qmlRegisterUncreatableType<RuleActionParamTemplates>(uri, 1, 0, "RuleActionParamTemplates", "Get it from RuleActionTemplate");
    qmlRegisterUncreatableType<RuleActionParamTemplate>(uri, 1, 0, "RuleActionParamTemplate", "Get it from RuleActionParamTemplates");

    qmlRegisterUncreatableType<SystemController>(uri, 1, 0, "SystemController", "Get it from Engine");
    qmlRegisterUncreatableType<Packages>(uri, 1, 0, "Packages", "Get it from SystemController");
    qmlRegisterUncreatableType<Package>(uri, 1, 0, "Package", "Get it from Packages");
    qmlRegisterUncreatableType<Repositories>(uri, 1, 0, "Repositories", "Get it from SystemController");
    qmlRegisterUncreatableType<Repository>(uri, 1, 0, "Repository", "Get it from Repositories");
    qmlRegisterType<PackagesFilterModel>(uri, 1, 0, "PackagesFilterModel");

    qmlRegisterType<ZigbeeManager>(uri, 1, 0, "ZigbeeManager");
    qmlRegisterUncreatableType<ZigbeeAdapter>(uri, 1, 0, "ZigbeeAdapter", "Get it from the ZigbeeAdapters");
    qmlRegisterUncreatableType<ZigbeeAdapters>(uri, 1, 0, "ZigbeeAdapters", "Get it from ZigbeeManager");
    qmlRegisterType<ZigbeeAdaptersProxy>(uri, 1, 0, "ZigbeeAdaptersProxy");
    qmlRegisterUncreatableType<ZigbeeNetwork>(uri, 1, 0, "ZigbeeNetwork", "Get it from the ZigbeeManager");
    qmlRegisterUncreatableType<ZigbeeNetworks>(uri, 1, 0, "ZigbeeNetworks", "Get it from the ZigbeeManager");

    qmlRegisterType<ModbusRtuManager>(uri, 1, 0, "ModbusRtuManager");
    qmlRegisterUncreatableType<ModbusRtuMaster>(uri, 1, 0, "ModbusRtuMaster", "Get it from the ModbusRtuMasters");
    qmlRegisterUncreatableType<ModbusRtuMasters>(uri, 1, 0, "ModbusRtuMasters", "Get it from the ModbusRtuManager");
    qmlRegisterUncreatableType<SerialPort>(uri, 1, 0, "SerialPort", "Get it from the SerialPorts");
    qmlRegisterUncreatableType<SerialPorts>(uri, 1, 0, "SerialPorts", "Get it from the apropriate manager object.");
    qmlRegisterType<SerialPortsProxy>(uri, 1, 0, "SerialPortsProxy");

    qmlRegisterType<NetworkManager>(uri, 1, 0, "NetworkManager");
    qmlRegisterUncreatableType<NetworkDevices>(uri, 1, 0, "NetworkDevices", "Get it from NetworkManager");
    qmlRegisterUncreatableType<WiredNetworkDevices>(uri, 1, 0, "WiredNetworkDevices", "Get it from NetworkManager");
    qmlRegisterUncreatableType<WirelessNetworkDevices>(uri, 1, 0, "WirelessNetworkDevices", "Get it from NetworkManager");
    qmlRegisterUncreatableType<NetworkDevice>(uri, 1, 0, "NetworkDevice", "Get it from NetworkDevices");
    qmlRegisterUncreatableType<WiredNetworkDevice>(uri, 1, 0, "WiredNetworkDevice", "Get it from NetworkDevices");
    qmlRegisterUncreatableType<WirelessNetworkDevice>(uri, 1, 0, "WirelessNetworkDevice", "Get it from NetworkDevices");

    qmlRegisterUncreatableType<ScriptManager>(uri, 1, 0, "ScriptManager", "Get it from Engine");
    qmlRegisterUncreatableType<Scripts>(uri, 1, 0, "Scripts", "Getit from ScriptManager");
    qmlRegisterUncreatableType<Script>(uri, 1, 0, "Script", "Getit from Scripts");
    qmlRegisterType<ScriptSyntaxHighlighter>(uri, 1, 0, "ScriptSyntaxHighlighter");
    qmlRegisterType<CodeCompletion>(uri, 1, 0, "CodeCompletion");
    qmlRegisterUncreatableType<CompletionProxyModel>(uri, 1, 0, "CompletionModel", "Get it from ScriptSyntaxHighlighter");
    qmlRegisterType<ScriptAutoSaver>(uri, 1, 0, "ScriptAutoSaver");

    qmlRegisterType<UserManager>(uri, 1, 0, "UserManager");
    qmlRegisterUncreatableType<UserInfo>(uri, 1, 0, "UserInfo", "Get it from UserManager");
    qmlRegisterUncreatableType<TokenInfo>(uri, 1, 0, "TokenInfo", "Get it from TokenInfos");
    qmlRegisterUncreatableType<TokenInfos>(uri, 1, 0, "TokenInfos", "Get it from UserManager");

    qmlRegisterUncreatableType<IOConnections>(uri, 1, 0, "IOConnections", "Get it from ThingManager");
    qmlRegisterUncreatableType<IOConnection>(uri, 1, 0, "IOConnection", "Get it from IOConnections");
    qmlRegisterType<IOInputConnectionWatcher>(uri, 1, 0, "IOInputConnectionWatcher");
    qmlRegisterType<IOOutputConnectionWatcher>(uri, 1, 0, "IOOutputConnectionWatcher");

    qmlRegisterType<SortFilterProxyModel>(uri, 1, 0, "SortFilterProxyModel");
}

#endif // LIBNYMEAAPPCORE_H
