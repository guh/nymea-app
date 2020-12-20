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

#include "systemcontroller.h"

#include "types/package.h"
#include "types/repository.h"
#include "types/packages.h"
#include "types/repositories.h"

#include <QTimeZone>

SystemController::SystemController(JsonRpcClient *jsonRpcClient, QObject *parent):
    JsonHandler(parent),
    m_jsonRpcClient(jsonRpcClient)
{
    m_jsonRpcClient->registerNotificationHandler(this, "notificationReceived");
    m_packages = new Packages(this);
    m_repositories = new Repositories(this);

    startTimer(1000, Qt::VeryCoarseTimer);
}

void SystemController::init()
{
    m_packages->clear();
    m_repositories->clear();
    if (m_jsonRpcClient->ensureServerVersion("2.0")) {
        m_jsonRpcClient->sendCommand("System.GetCapabilities", this, "getCapabilitiesResponse");
    } else {
        m_powerManagementAvailable = false;
        m_updateManagementAvailable = false;
        m_timeManagementAvailable = false;
    }
}

QString SystemController::nameSpace() const
{
    return "System";
}

bool SystemController::powerManagementAvailable() const
{
    return m_powerManagementAvailable;
}

bool SystemController::updateManagementAvailable() const
{
    return m_updateManagementAvailable;
}

int SystemController::restart()
{
    return m_jsonRpcClient->sendCommand("System.Restart", this, "restartResponse");
}

int SystemController::reboot()
{
    return m_jsonRpcClient->sendCommand("System.Reboot", this, "rebootResponse");
}

int SystemController::shutdown()
{
    return m_jsonRpcClient->sendCommand("System.Shutdown", this, "shutdownResponse");
}

bool SystemController::updateManagementBusy() const
{
    return m_updateManagementBusy;
}

bool SystemController::updateRunning() const
{
    return m_updateRunning;
}

void SystemController::checkForUpdates()
{
    m_jsonRpcClient->sendCommand("System.CheckForUpdates");
}

Packages *SystemController::packages() const
{
    return m_packages;
}

void SystemController::updatePackages(const QString packageId)
{
    QVariantMap params;
    if (!packageId.isEmpty()) {
        params.insert("packageIds", QStringList() << packageId);
    }
    m_jsonRpcClient->sendCommand("System.UpdatePackages", params);
}

void SystemController::removePackages(const QString packageId)
{
    QVariantMap params;
    if (!packageId.isEmpty()) {
        params.insert("packageIds", QStringList() << packageId);
    }
    m_jsonRpcClient->sendCommand("System.RemovePackages", params, this, "removePackageResponse");
}

Repositories *SystemController::repositories() const
{
    return m_repositories;
}

int SystemController::enableRepository(const QString &id, bool enabled)
{
    QVariantMap params;
    params.insert("repositoryId", id);
    params.insert("enabled", enabled);
    return m_jsonRpcClient->sendCommand("System.EnableRepository", params, this, "enableRepositoryResponse");
}

bool SystemController::timeManagementAvailable() const
{
    return m_timeManagementAvailable;
}

QDateTime SystemController::serverTime() const
{
    return m_serverTime;
}

int SystemController::setServerTime(const QDateTime &serverTime)
{
    QVariantMap params;
    params.insert("automaticTime", false);
    params.insert("time", serverTime.toSecsSinceEpoch());
    params.insert("timeZone", serverTime.timeZone().id());
    return m_jsonRpcClient->sendCommand("System.SetTime", params, this, "setTimeResponse");
}

QStringList SystemController::timeZones() const
{
    QStringList ret;
    foreach (const QByteArray &tzId, QTimeZone::availableTimeZoneIds()) {
        ret << tzId;
    }
    return ret;
}

QString SystemController::serverTimeZone() const
{
    // NOTE: Ideally we'd just set the TimeZone of our serverTime prooperly, however, there's a bug on Android
    // Which doesn't allow to create QTimeZone objects by IANA id.... So, let's keep that separated in a string
    // https://bugreports.qt.io/browse/QTBUG-83438
//    return m_serverTime.timeZone().id();
    return m_serverTimeZone;
}

int SystemController::setServerTimeZone(const QString &serverTimeZone)
{
    QVariantMap params;
    params.insert("timeZone", serverTimeZone);
    return m_jsonRpcClient->sendCommand("System.SetTime", params, this, "setTimeResponse");
}

bool SystemController::automaticTimeAvailable() const
{
    return m_automaticTimeAvailable;
}

bool SystemController::automaticTime() const
{
    return m_automaticTime;
}

int SystemController::setAutomaticTime(bool automaticTime)
{
    QVariantMap params;
    params.insert("automaticTime", automaticTime);
    return m_jsonRpcClient->sendCommand("System.SetTime", params, this, "setTimeResponse");
}

void SystemController::getCapabilitiesResponse(int /*commandId*/, const QVariantMap &data)
{
    m_powerManagementAvailable = data.value("powerManagement").toBool();
    emit powerManagementAvailableChanged();

    m_updateManagementAvailable = data.value("updateManagement").toBool();
    emit updateManagementAvailableChanged();

    m_timeManagementAvailable = data.value("timeManagement").toBool();
    emit timeManagementAvailableChanged();

    if (m_updateManagementAvailable) {
        m_jsonRpcClient->sendCommand("System.GetUpdateStatus", this, "getUpdateStatusResponse");
        m_jsonRpcClient->sendCommand("System.GetPackages", this, "getPackagesResponse");
        m_jsonRpcClient->sendCommand("System.GetRepositories", this, "getRepositoriesResponse");
    }

    if (m_jsonRpcClient->ensureServerVersion("4.1")) {
        m_jsonRpcClient->sendCommand("System.GetTime", this, "getServerTimeResponse");
    }

    qDebug() << "nymea:core capabilities: Power management:" << m_powerManagementAvailable << "Update management:" << m_updateManagementAvailable << "Time management:" << m_timeManagementAvailable;
}

void SystemController::getUpdateStatusResponse(int /*commandId*/, const QVariantMap &data)
{
    m_updateManagementBusy = data.value("busy").toBool();
    m_updateRunning = data.value("updateRunning").toBool();
    emit updateRunningChanged();
}

void SystemController::getPackagesResponse(int commandId, const QVariantMap &data)
{
    Q_UNUSED(commandId)
    foreach (const QVariant &packageVariant, data.value("packages").toList()) {
        QString id = packageVariant.toMap().value("id").toString();
        QString displayName = packageVariant.toMap().value("displayName").toString();
        Package *p = new Package(id, displayName);
        p->setSummary(packageVariant.toMap().value("summary").toString());
        p->setInstalledVersion(packageVariant.toMap().value("installedVersion").toString());
        p->setCandidateVersion(packageVariant.toMap().value("candidateVersion").toString());
        p->setChangelog(packageVariant.toMap().value("changelog").toString());
        p->setUpdateAvailable(packageVariant.toMap().value("updateAvailable").toBool());
        p->setRollbackAvailable(packageVariant.toMap().value("rollbackAvailable").toBool());
        p->setCanRemove(packageVariant.toMap().value("canRemove").toBool());
        m_packages->addPackage(p);
    }
}

void SystemController::getRepositoriesResponse(int /*commandId*/, const QVariantMap &data)
{
    foreach (const QVariant &repoVariant, data.value("repositories").toList()) {
        QString id = repoVariant.toMap().value("id").toString();
        QString displayName = repoVariant.toMap().value("displayName").toString();
        Repository *repo = new Repository(id, displayName);
        repo->setEnabled(repoVariant.toMap().value("enabled").toBool());
        m_repositories->addRepository(repo);
    }
}

void SystemController::removePackageResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Remove result" << commandId << params;
}

void SystemController::enableRepositoryResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "Enable repo response" << params;
    emit enableRepositoryFinished(commandId, params.value("success").toBool());
}

void SystemController::getServerTimeResponse(int commandId, const QVariantMap &params)
{
    Q_UNUSED(commandId)
    m_serverTime = QDateTime::fromSecsSinceEpoch(params.value("time").toUInt());

    // NOTE: Ideally we'd just set the TimeZone of our serverTime prooperly, however, there's a bug on Android
    // Which doesn't allow to create QTimeZone objects by IANA id.... So, let's keep that separated in a string
    // https://bugreports.qt.io/browse/QTBUG-83438

//    m_serverTime.setTimeZone(QTimeZone(params.value("timeZone").toString().toUtf8()));
    m_serverTimeZone = params.value("timeZone").toString();

    emit serverTimeChanged();
    emit serverTimeZoneChanged();
    m_automaticTimeAvailable = params.value("automaticTimeAvailable").toBool();
    emit automaticTimeAvailableChanged();
    m_automaticTime = params.value("automaticTime").toBool();
    emit automaticTimeChanged();
    qDebug() << "Server time:" << m_serverTime << "Automatic Time available:" << m_automaticTimeAvailable << "Automatic time:" << m_automaticTime;
}

void SystemController::setTimeResponse(int commandId, const QVariantMap &params)
{
    qDebug() << "set time response" << commandId << params;
}

void SystemController::restartResponse(int commandId, const QVariantMap &params)
{
    bool success = params.value("success").toBool();
    emit restartReply(commandId, success);
}

void SystemController::rebootResponse(int commandId, const QVariantMap &params)
{
    bool success = params.value("success").toBool();
    emit rebootReply(commandId, success);
}

void SystemController::shutdownResponse(int commandId, const QVariantMap &params)
{
    bool success = params.value("success").toBool();
    emit shutdownReply(commandId, success);
}

void SystemController::notificationReceived(const QVariantMap &data)
{
    QString notification = data.value("notification").toString();
    if (notification == "System.UpdateStatusChanged") {
        qDebug() << "System.UpdateStatusChanged:" << data.value("params").toMap();
        if (m_updateManagementBusy != data.value("params").toMap().value("busy").toBool()) {
            m_updateManagementBusy = data.value("params").toMap().value("busy").toBool();
            emit updateManagementBusyChanged();
        }
        if (m_updateRunning != data.value("params").toMap().value("updateRunning").toBool()) {
            m_updateRunning = data.value("params").toMap().value("updateRunning").toBool();
            emit updateRunningChanged();
        }
    } else if (notification == "System.PackageAdded") {
        QVariantMap packageMap = data.value("params").toMap().value("package").toMap();
        QString id = packageMap.value("id").toString();
        QString displayName = packageMap.value("displayName").toString();
        Package *p = new Package(id, displayName);
        p->setSummary(packageMap.value("summary").toString());
        p->setInstalledVersion(packageMap.value("installedVersion").toString());
        p->setCandidateVersion(packageMap.value("candidateVersion").toString());
        p->setChangelog(packageMap.value("changelog").toString());
        p->setUpdateAvailable(packageMap.value("updateAvailable").toBool());
        p->setRollbackAvailable(packageMap.value("rollbackAvailable").toBool());
        p->setCanRemove(packageMap.value("canRemove").toBool());
        m_packages->addPackage(p);
    } else if (notification == "System.PackageChanged") {
        QVariantMap packageMap = data.value("params").toMap().value("package").toMap();
        QString id = packageMap.value("id").toString();
        Package *p = m_packages->getPackage(id);
        if (!p) {
            qWarning() << "Received a package update notification for a package we don't know";
            return;
        }
        p->setSummary(packageMap.value("summary").toString());
        p->setInstalledVersion(packageMap.value("installedVersion").toString());
        p->setCandidateVersion(packageMap.value("candidateVersion").toString());
        p->setChangelog(packageMap.value("changelog").toString());
        p->setUpdateAvailable(packageMap.value("updateAvailable").toBool());
        p->setRollbackAvailable(packageMap.value("rollbackAvailable").toBool());
        p->setCanRemove(packageMap.value("canRemove").toBool());
    } else if (notification == "System.PackageRemoved") {
        QString packageId = data.value("params").toMap().value("packageId").toString();
        m_packages->removePackage(packageId);
    } else if (notification == "System.RepositoryAdded") {
        QVariantMap repoMap = data.value("params").toMap().value("repository").toMap();
        QString id = repoMap.value("id").toString();
        QString displayName = repoMap.value("displayName").toString();
        Repository *repo = new Repository(id, displayName);
        repo->setEnabled(repoMap.value("enabled").toBool());
        m_repositories->addRepository(repo);
    } else if (notification == "System.RepositoryChanged") {
        QVariantMap repoMap = data.value("params").toMap().value("repository").toMap();
        QString id = repoMap.value("id").toString();
        Repository *repo = m_repositories->getRepository(id);
        if (!repo) {
            qWarning() << "Received a repository update notification for a repository we don't know";
            return;
        }
        repo->setEnabled(repoMap.value("enabled").toBool());
    } else if (notification == "System.RepositoryRemoved") {
        QString repositoryId = data.value("params").toMap().value("repositoryId").toString();
        m_repositories->removeRepository(repositoryId);
    } else if (notification == "System.CapabilitiesChanged") {
        m_powerManagementAvailable = data.value("params").toMap().value("powerManagement").toBool();
        m_updateManagementAvailable = data.value("params").toMap().value("updateManagement").toBool();
        qWarning() << "System capabilites changed: power management:" << m_powerManagementAvailable << "update management:" << m_updateManagementAvailable;
        emit powerManagementAvailableChanged();
        emit updateManagementAvailableChanged();
    } else if (notification == "System.TimeConfigurationChanged") {
        qDebug() << "System time configuration changed" << data.value("params").toMap().value("timeZone").toByteArray();
        m_serverTime = QDateTime::fromSecsSinceEpoch(data.value("params").toMap().value("time").toUInt());

        // NOTE: Ideally we'd just set the TimeZone of our serverTime prooperly, however, there's a bug on Android
        // Which doesn't allow to create QTimeZone objects by IANA id.... So, let's keep that separated in a string
        // https://bugreports.qt.io/browse/QTBUG-83438
        // m_serverTime.setTimeZone(QTimeZone(data.value("params").toMap().value("timeZone").toByteArray()));
        m_serverTimeZone = data.value("params").toMap().value("timeZone").toString();

        emit serverTimeChanged();
        emit serverTimeZoneChanged();
        m_automaticTimeAvailable = data.value("params").toMap().value("automaticTimeAvailable").toBool();
        emit automaticTimeAvailableChanged();
        m_automaticTime = data.value("params").toMap().value("automaticTime").toBool();
        emit automaticTimeChanged();
    } else {
        qWarning() << "Unhandled System Notification" << data.value("notification");
    }
}

void SystemController::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    m_serverTime = m_serverTime.addSecs(1);
    emit serverTimeChanged();
}
