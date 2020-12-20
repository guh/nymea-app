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

#include "nymeaconfiguration.h"

#include "serverconfiguration.h"
#include "serverconfigurations.h"
#include "mqttpolicy.h"
#include "mqttpolicies.h"

#include "jsonrpc/jsonrpcclient.h"

#include <QUuid>

NymeaConfiguration::NymeaConfiguration(JsonRpcClient *client, QObject *parent):
    JsonHandler(parent),
    m_client(client),
    m_tcpServerConfigurations(new ServerConfigurations(this)),
    m_webSocketServerConfigurations(new ServerConfigurations(this)),
    m_webServerConfigurations(new WebServerConfigurations(this)),
    m_mqttServerConfigurations(new ServerConfigurations(this)),
    m_mqttPolicies(new MqttPolicies(this))
{
    client->registerNotificationHandler(this, "notificationReceived");
}

QString NymeaConfiguration::nameSpace() const
{
    return "Configuration";
}

void NymeaConfiguration::init()
{
    m_tcpServerConfigurations->clear();
    m_webSocketServerConfigurations->clear();
    m_mqttServerConfigurations->clear();
    m_client->sendCommand("Configuration.GetConfigurations", this, "getConfigurationsResponse");
    m_client->sendCommand("Configuration.GetMqttServerConfigurations", this, "getMqttServerConfigsReply");
    m_client->sendCommand("Configuration.GetMqttPolicies", this, "getMqttPoliciesReply");
}

QString NymeaConfiguration::serverName() const
{
    return m_serverName;
}

void NymeaConfiguration::setServerName(const QString &serverName)
{
    QVariantMap params;
    params.insert("serverName", serverName);
    m_client->sendCommand("Configuration.SetServerName", params, this, "setServerNameResponse");
}

void NymeaConfiguration::setTimezone(const QString &timezone)
{
    QVariantMap params;
    params.insert("timeZone", timezone);
    m_client->sendCommand("System.SetTimeZone", params, this, "setTimezoneResponse");
}

bool NymeaConfiguration::debugServerEnabled() const
{
    return m_debugServerEnabled;
}

void NymeaConfiguration::setDebugServerEnabled(bool debugServerEnabled)
{
    QVariantMap params;
    params.insert("enabled", debugServerEnabled);
    m_client->sendCommand("Configuration.SetDebugServerEnabled", params, this, "setDebugServerEnabledResponse");
}

bool NymeaConfiguration::cloudEnabled() const
{
    return m_cloudEnabled;
}

void NymeaConfiguration::setCloudEnabled(bool cloudEnabled)
{
    QVariantMap params;
    params.insert("enabled", cloudEnabled);
    m_client->sendCommand("Configuration.SetCloudEnabled", params, this, "setCloudEnabledResponse");
}

ServerConfigurations *NymeaConfiguration::tcpServerConfigurations() const
{
    return m_tcpServerConfigurations;
}

ServerConfigurations *NymeaConfiguration::webSocketServerConfigurations() const
{
    return m_webSocketServerConfigurations;
}

WebServerConfigurations *NymeaConfiguration::webServerConfigurations() const
{
    return m_webServerConfigurations;
}

ServerConfigurations *NymeaConfiguration::mqttServerConfigurations() const
{
    return m_mqttServerConfigurations;
}

MqttPolicies *NymeaConfiguration::mqttPolicies() const
{
    return m_mqttPolicies;
}

ServerConfiguration *NymeaConfiguration::createServerConfiguration(const QString &address, int port, bool authEnabled, bool sslEnabled)
{
    return new ServerConfiguration(QUuid::createUuid().toString(), QHostAddress(address), port, authEnabled, sslEnabled);
}

WebServerConfiguration *NymeaConfiguration::createWebServerConfiguration(const QString &address, int port, bool authEnabled, bool sslEnabled, const QString &publicFolder)
{
    auto ret = new WebServerConfiguration(QUuid::createUuid().toString(), QHostAddress(address), port, authEnabled, sslEnabled);
    ret->setPublicFolder(publicFolder);
    return ret;
}

MqttPolicy *NymeaConfiguration::createMqttPolicy() const
{
    return new MqttPolicy(QString(), QString(), QString(), {"#"}, {"#"});
}

void NymeaConfiguration::setTcpServerConfiguration(ServerConfiguration *configuration)
{
    QVariantMap params;
    QVariantMap configurationMap;
    configurationMap.insert("id", configuration->id());
    configurationMap.insert("address", configuration->address());
    configurationMap.insert("port", configuration->port());
    configurationMap.insert("authenticationEnabled", configuration->authenticationEnabled());
    configurationMap.insert("sslEnabled", configuration->sslEnabled());
    params.insert("configuration", configurationMap);
    m_client->sendCommand("Configuration.SetTcpServerConfiguration", params, this, "setTcpConfigReply");
}

void NymeaConfiguration::setWebSocketServerConfiguration(ServerConfiguration *configuration)
{
    QVariantMap params;
    QVariantMap configurationMap;
    configurationMap.insert("id", configuration->id());
    configurationMap.insert("address", configuration->address());
    configurationMap.insert("port", configuration->port());
    configurationMap.insert("authenticationEnabled", configuration->authenticationEnabled());
    configurationMap.insert("sslEnabled", configuration->sslEnabled());
    params.insert("configuration", configurationMap);
    m_client->sendCommand("Configuration.SetWebSocketServerConfiguration", params, this, "setWebSocketConfigReply");
}

void NymeaConfiguration::setWebServerConfiguration(WebServerConfiguration *configuration)
{
    QVariantMap params;
    QVariantMap configurationMap;
    configurationMap.insert("id", configuration->id());
    configurationMap.insert("address", configuration->address());
    configurationMap.insert("port", configuration->port());
    configurationMap.insert("authenticationEnabled", configuration->authenticationEnabled());
    configurationMap.insert("sslEnabled", configuration->sslEnabled());
    configurationMap.insert("publicFolder", configuration->publicFolder());
    params.insert("configuration", configurationMap);
    m_client->sendCommand("Configuration.SetWebServerConfiguration", params, this, "setWebConfigReply");
}

void NymeaConfiguration::setMqttServerConfiguration(ServerConfiguration *configuration)
{
    QVariantMap params;
    QVariantMap configurationMap;
    configurationMap.insert("id", configuration->id());
    configurationMap.insert("address", configuration->address());
    configurationMap.insert("port", configuration->port());
    configurationMap.insert("authenticationEnabled", configuration->authenticationEnabled());
    configurationMap.insert("sslEnabled", configuration->sslEnabled());
    params.insert("configuration", configurationMap);
    m_client->sendCommand("Configuration.SetMqttServerConfiguration", params, this, "setMqttConfigReply");
}

void NymeaConfiguration::deleteTcpServerConfiguration(const QString &id)
{
    QVariantMap params;
    params.insert("id", id);
    m_client->sendCommand("Configuration.DeleteTcpServerConfiguration", params, this, "deleteTcpConfigReply");
}

void NymeaConfiguration::deleteWebSocketServerConfiguration(const QString &id)
{
    QVariantMap params;
    params.insert("id", id);
    m_client->sendCommand("Configuration.DeleteWebSocketServerConfiguration", params, this, "deleteWebSocketConfigReply");
}

void NymeaConfiguration::deleteWebServerConfiguration(const QString &id)
{
    QVariantMap params;
    params.insert("id", id);
    m_client->sendCommand("Configuration.DeleteWebServerConfiguration", params, this, "deleteWebConfigReply");
}

void NymeaConfiguration::deleteMqttServerConfiguration(const QString &id)
{
    QVariantMap params;
    params.insert("id", id);
    m_client->sendCommand("Configuration.DeleteMqttServerConfiguration", params, this, "deleteMqttConfigReply");
}

void NymeaConfiguration::updateMqttPolicy(MqttPolicy *policy)
{
    QVariantMap params;
    QVariantMap policyMap;
    policyMap.insert("clientId", policy->clientId());
    policyMap.insert("username", policy->username());
    policyMap.insert("password", policy->password());
    policyMap.insert("allowedPublishTopicFilters", policy->allowedPublishTopicFilters());
    policyMap.insert("allowedSubscribeTopicFilters", policy->allowedSubscribeTopicFilters());
    params.insert("policy", policyMap);
    m_client->sendCommand("Configuration.SetMqttPolicy", params, this, "setMqttPolicyReply");
}

void NymeaConfiguration::deleteMqttPolicy(const QString &clientId)
{
    QVariantMap params;
    params.insert("clientId", clientId);
    m_client->sendCommand("Configuration.DeleteMqttPolicy", params, this, "deleteMqttPolicyReply");
}

void NymeaConfiguration::getConfigurationsResponse(int commandId, const QVariantMap &params)
{
    Q_UNUSED(commandId)
//    qDebug() << "have config reply" << params;
    QVariantMap basicConfig = params.value("basicConfiguration").toMap();
    m_debugServerEnabled = basicConfig.value("debugServerEnabled").toBool();
    emit debugServerEnabledChanged();
    m_serverName = basicConfig.value("serverName").toString();
    emit serverNameChanged();
    QVariantMap cloudConfig = params.value("cloud").toMap();
    m_cloudEnabled = cloudConfig.value("enabled").toBool();
    emit cloudEnabledChanged();

    tcpServerConfigurations()->clear();
    foreach (const QVariant &tcpServerVariant, params.value("tcpServerConfigurations").toList()) {
//        qDebug() << "tcp server config:" << tcpServerVariant;
        QVariantMap tcpConfigMap = tcpServerVariant.toMap();
        ServerConfiguration *config = new ServerConfiguration(tcpConfigMap.value("id").toString(), QHostAddress(tcpConfigMap.value("address").toString()), tcpConfigMap.value("port").toInt(), tcpConfigMap.value("authenticationEnabled").toBool(), tcpConfigMap.value("sslEnabled").toBool());
        m_tcpServerConfigurations->addConfiguration(config);
    }
    webSocketServerConfigurations()->clear();
    foreach (const QVariant &websocketServerVariant, params.value("webSocketServerConfigurations").toList()) {
        QVariantMap websocketConfigMap = websocketServerVariant.toMap();
        ServerConfiguration *config = new ServerConfiguration(websocketConfigMap.value("id").toString(), QHostAddress(websocketConfigMap.value("address").toString()), websocketConfigMap.value("port").toInt(), websocketConfigMap.value("authenticationEnabled").toBool(), websocketConfigMap.value("sslEnabled").toBool());
        m_webSocketServerConfigurations->addConfiguration(config);
    }

    webServerConfigurations()->clear();
    foreach (const QVariant &webServerVariant, params.value("webServerConfigurations").toList()) {
        QVariantMap webServerConfigMap = webServerVariant.toMap();
        WebServerConfiguration* config = new WebServerConfiguration(webServerConfigMap.value("id").toString(), QHostAddress(webServerConfigMap.value("address").toString()), webServerConfigMap.value("port").toInt(), webServerConfigMap.value("authenticationEnabled").toBool(), webServerConfigMap.value("sslEnabled").toBool());
        config->setPublicFolder(webServerConfigMap.value("publicFolder").toString());
        m_webServerConfigurations->addConfiguration(config);
    }
}

void NymeaConfiguration::setServerNameResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Server name set:" << commandId << params;
}

void NymeaConfiguration::setTimezoneResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Set timezone response" << commandId << params;
}

void NymeaConfiguration::getCloudConfigurationResponse(const QVariantMap &params)
{
    qDebug() << "Cloud config reply" << params;
}

void NymeaConfiguration::setCloudEnabledResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Set cloud enabled:" << commandId << params;
}

void NymeaConfiguration::setDebugServerEnabledResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Debug server set:" << commandId << params;
}

void NymeaConfiguration::setTcpConfigReply(int commandId, const QVariantMap &params)
{
    qDebug() << "Set TCP server config reply" << commandId << params;
}

void NymeaConfiguration::deleteTcpConfigReply(int commandId, const QVariantMap &params)
{
    qDebug() << "Deöete  TCP server config reply" << commandId << params;
}

void NymeaConfiguration::setWebSocketConfigReply(int commandId, const QVariantMap &params)
{
    qDebug() << "set websocket config reply" << commandId << params;
}

void NymeaConfiguration::setWebConfigReply(int commandId, const QVariantMap &params)
{
    qDebug() << "set web server config reply" << commandId << params;
}

void NymeaConfiguration::deleteWebConfigReply(int commandId, const QVariantMap &params)
{
    qDebug() << "Delete web server config reply" << commandId << params;
}

void NymeaConfiguration::deleteWebSocketConfigReply(int commandId, const QVariantMap &params)
{
    qDebug() << "Delete web socket server config reply" << commandId << params;
}

void NymeaConfiguration::getMqttServerConfigsReply(int commandId, const QVariantMap &params)
{
    Q_UNUSED(commandId)
    m_mqttServerConfigurations->clear();
    foreach (const QVariant &mqttServerVariant, params.value("mqttServerConfigurations").toList()) {
        QVariantMap mqttConfigMap = mqttServerVariant.toMap();
        ServerConfiguration *config = new ServerConfiguration(mqttConfigMap.value("id").toString(), QHostAddress(mqttConfigMap.value("address").toString()), mqttConfigMap.value("port").toInt(), mqttConfigMap.value("authenticationEnabled").toBool(), mqttConfigMap.value("sslEnabled").toBool());
        m_mqttServerConfigurations->addConfiguration(config);
    }
}

void NymeaConfiguration::setMqttConfigReply(int commandId, const QVariantMap &params)
{
    qDebug() << "Set mqtt config reply" << commandId << params;
}

void NymeaConfiguration::deleteMqttConfigReply(int commandId, const QVariantMap &params)
{
    qDebug() << "Delete Mqtt Broker config reply:" << commandId << params;
}

void NymeaConfiguration::getMqttPoliciesReply(int commandId, const QVariantMap &params)
{
    Q_UNUSED(commandId)
//    qDebug() << "Mqtt polices:" << params;
    m_mqttPolicies->clear();
    foreach (const QVariant &policyVariant, params.value("mqttPolicies").toList()) {
        QVariantMap policyMap = policyVariant.toMap();
        MqttPolicy *policy = new MqttPolicy(
                    policyMap.value("clientId").toString(),
                    policyMap.value("username").toString(),
                    policyMap.value("password").toString(),
                    policyMap.value("allowedPublishTopicFilters").toStringList(),
                    policyMap.value("allowedSubscribeTopicFilters").toStringList());
        m_mqttPolicies->addPolicy(policy);
    }
}

void NymeaConfiguration::setMqttPolicyReply(int commandId, const QVariantMap &params)
{
    qDebug() << "Set MQTT policy reply" << commandId << params;
}

void NymeaConfiguration::deleteMqttPolicyReply(int commandId, const QVariantMap &params)
{
    qDebug() << "Delete MQTT policy reply" << commandId << params;
}

void NymeaConfiguration::notificationReceived(const QVariantMap &notification)
{
    QString notif = notification.value("notification").toString();
    if (notif == "Configuration.BasicConfigurationChanged") {
        QVariantMap params = notification.value("params").toMap().value("basicConfiguration").toMap();
        qDebug() << "notif" << params;
        m_debugServerEnabled = params.value("debugServerEnabled").toBool();
        emit debugServerEnabledChanged();
        m_serverName = params.value("serverName").toString();
        emit serverNameChanged();
        return;
    }
    if (notif == "Configuration.CloudConfigurationChanged") {
        QVariantMap params = notification.value("params").toMap().value("cloudConfiguration").toMap();
        qDebug() << "notif" << params;
        m_cloudEnabled = params.value("enabled").toBool();
        emit cloudEnabledChanged();
        return;
    }
    if (notif.endsWith("ServerConfigurationChanged")) {
        ServerConfigurations *configModel = nullptr;
        ServerConfiguration *serverConfig = nullptr;

        QVariantMap params;
        if (notif == "Configuration.TcpServerConfigurationChanged") {
            configModel = m_tcpServerConfigurations;
            params = notification.value("params").toMap().value("tcpServerConfiguration").toMap();
        }
        if (notif == "Configuration.WebSocketServerConfigurationChanged") {
            configModel = m_webSocketServerConfigurations;
            params = notification.value("params").toMap().value("webSocketServerConfiguration").toMap();
        }
        if (notif == "Configuration.WebServerConfigurationChanged") {
            configModel = m_webServerConfigurations;
            params = notification.value("params").toMap().value("webServerConfiguration").toMap();
        }
        if (notif == "Configuration.MqttServerConfigurationChanged") {
            configModel = m_mqttServerConfigurations;
            params = notification.value("params").toMap().value("mqttServerConfiguration").toMap();
        }
        if (!configModel) {
            return;
        }

        for (int i = 0; i < configModel->rowCount(); i++) {
            ServerConfiguration* config = configModel->get(i);
            if (config->id() == params.value("id").toString()) {
                serverConfig = config;
            }
        }

        if (!serverConfig) {
            if (notif == "Configuration.WebServerConfigurationChanged") {
                serverConfig = new WebServerConfiguration(params.value("id").toString());
            } else {
                serverConfig = new ServerConfiguration(params.value("id").toString());
            }
            configModel->addConfiguration(serverConfig);
        }
        serverConfig->setAddress(params.value("address").toString());
        serverConfig->setPort(params.value("port").toInt());
        serverConfig->setAuthenticationEnabled(params.value("authenticationEnabled").toBool());
        serverConfig->setSslEnabled(params.value("sslEnabled").toBool());
        if (notif == "Configuration.WebServerConfigurationChanged") {
            qobject_cast<WebServerConfiguration*>(serverConfig)->setPublicFolder(params.value("publicFolder").toString());
        }

        return;
    }
    if (notif == "Configuration.TcpServerConfigurationRemoved") {
        m_tcpServerConfigurations->removeConfiguration(notification.value("params").toMap().value("id").toString());
        return;
    }
    if (notif == "Configuration.WebSocketServerConfigurationRemoved") {
        m_webSocketServerConfigurations->removeConfiguration(notification.value("params").toMap().value("id").toString());
        return;
    }
    if (notif == "Configuration.WebServerConfigurationRemoved") {
        m_webServerConfigurations->removeConfiguration(notification.value("params").toMap().value("id").toString());
        return;
    }
    if (notif == "Configuration.MqttServerConfigurationRemoved") {
        m_mqttServerConfigurations->removeConfiguration(notification.value("params").toMap().value("id").toString());
        return;
    }
    if (notif == "Configuration.MqttPolicyChanged") {
        MqttPolicy *policy = nullptr;
        QVariantMap policyMap = notification.value("params").toMap().value("policy").toMap();
        for (int i = 0; i < m_mqttPolicies->rowCount(); i++) {
            if (m_mqttPolicies->get(i)->clientId() == policyMap.value("clientId").toString()) {
                policy = m_mqttPolicies->get(i);
                break;
            }
        }
        if (!policy) {
            policy = new MqttPolicy(policyMap.value("clientId").toString());
            m_mqttPolicies->addPolicy(policy);
        }
        policy->setUsername(policyMap.value("username").toString());
        policy->setPassword(policyMap.value("password").toString());
        policy->setAllowedPublishTopicFilters(policyMap.value("allowedPublishTopicFilters").toStringList());
        policy->setAllowedSubscribeTopicFilters(policyMap.value("allowedSubscribeTopicFilters").toStringList());
        qDebug() << "MQTT policy added" << policy->clientId() << policy->username() << policy->password();
        return;
    }
    if (notif == "Configuration.MqttPolicyRemoved") {
        MqttPolicy* policy = m_mqttPolicies->getPolicy(notification.value("params").toMap().value("clientId").toString());
        if (!policy) {
            qWarning() << "Reveived a policy removed notification for apolicy we don't know";
            return;
        }
        m_mqttPolicies->removePolicy(policy);
    }

    qDebug() << "Unhandled Configuration notification" << notif << notification;
}
