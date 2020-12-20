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

#include "device.h"
#include "deviceclass.h"
#include "devicemanager.h"

#include <QDebug>

Device::Device(DeviceManager *thingManager, DeviceClass *thingClass, const QUuid &parentId, QObject *parent) :
    QObject(parent),
    m_thingManager(thingManager),
    m_parentId(parentId),
    m_thingClass(thingClass)
{
}

QString Device::name() const
{
    return m_name;
}

void Device::setName(const QString &name)
{
    m_name = name;
    emit nameChanged();
}

QUuid Device::id() const
{
    return m_id;
}

void Device::setId(const QUuid &id)
{
    m_id = id;
}

QUuid Device::deviceClassId() const
{
    return m_thingClass->id();
}

QUuid Device::thingClassId() const
{
    return m_thingClass->id();
}

QUuid Device::parentDeviceId() const
{
    return m_parentId;
}

bool Device::isChild() const
{
    return !m_parentId.isNull();
}

Device::ThingSetupStatus Device::setupStatus() const
{
    return m_setupStatus;
}

QString Device::setupDisplayMessage() const
{
    return m_setupDisplayMessage;
}

void Device::setSetupStatus(Device::ThingSetupStatus setupStatus, const QString &displayMessage)
{
    if (m_setupStatus != setupStatus || m_setupDisplayMessage != displayMessage) {
        m_setupStatus = setupStatus;
        m_setupDisplayMessage = displayMessage;
        emit setupStatusChanged();
    }
}

Params *Device::params() const
{
    return m_params;
}

void Device::setParams(Params *params)
{
    if (m_params != params) {
        if (m_params) {
            m_params->deleteLater();
        }
        params->setParent(this);
        m_params = params;
        emit paramsChanged();
    }
}

Params *Device::settings() const
{
    return m_settings;
}

void Device::setSettings(Params *settings)
{
    if (m_settings != settings) {
        if (m_settings) {
            m_settings->deleteLater();
        }
        settings->setParent(this);
        m_settings = settings;
        emit settingsChanged();
    }
}

States *Device::states() const
{
    return m_states;
}

void Device::setStates(States *states)
{
    if (m_states != states) {
        if (m_states) {
            m_states->deleteLater();
        }
        states->setParent(this);
        m_states = states;
        emit statesChanged();
    }
}

State *Device::state(const QUuid &stateTypeId) const
{
    return m_states->getState(stateTypeId);
}

State *Device::stateByName(const QString &stateName) const
{
    StateType *st = m_thingClass->stateTypes()->findByName(stateName);
    if (!st) {
        return nullptr;
    }
    return m_states->getState(st->id());
}

DeviceClass *Device::thingClass() const
{
    return m_thingClass;
}

bool Device::hasState(const QUuid &stateTypeId)
{
    foreach (State *state, states()->states()) {
        if (state->stateTypeId() == stateTypeId) {
            return true;
        }
    }
    return false;
}

QVariant Device::stateValue(const QUuid &stateTypeId)
{
    foreach (State *state, states()->states()) {
        if (state->stateTypeId() == stateTypeId) {
            return state->value();
        }
    }
    return QVariant();
}

void Device::setStateValue(const QUuid &stateTypeId, const QVariant &value)
{
    foreach (State *state, states()->states()) {
        if (state->stateTypeId() == stateTypeId) {
            state->setValue(value);
            return;
        }
    }
}

int Device::executeAction(const QString &actionName, const QVariantList &params)
{
    ActionType *actionType = m_thingClass->actionTypes()->findByName(actionName);

    QVariantList finalParams;
    foreach (const QVariant &paramVariant, params) {
        QVariantMap param = paramVariant.toMap();
        if (!param.contains("paramTypeId") && param.contains("paramName")) {
            ParamType *paramType = actionType->paramTypes()->findByName(param.take("paramName").toString());
            param.insert("paramTypeId", paramType->id());
        }
        finalParams.append(param);
    }
    return m_thingManager->executeAction(m_id, actionType->id(), finalParams);
}

QDebug operator<<(QDebug &dbg, Device *thing)
{
    dbg.nospace() << "Thing: " << thing->name() << " (" << thing->id().toString() << ") Class:" << thing->thingClass()->name() << " (" << thing->thingClassId().toString() << ")" << Qt::endl;
    for (int i = 0; i < thing->thingClass()->paramTypes()->rowCount(); i++) {
        ParamType *pt = thing->thingClass()->paramTypes()->get(i);
        Param *p = thing->params()->getParam(pt->id().toString());
        if (p) {
            dbg << "  Param " << i << ": " << pt->id().toString() << ": " << pt->name() << " = " << p->value() << Qt::endl;
        } else {
            dbg << "  Param " << i << ": " << pt->id().toString() << ": " << pt->name() << " = " << "*** Unknown value ***" << Qt::endl;
        }
    }
    for (int i = 0; i < thing->thingClass()->settingsTypes()->rowCount(); i++) {
        ParamType *pt = thing->thingClass()->settingsTypes()->get(i);
        Param *p = thing->settings()->getParam(pt->id().toString());
        if (p) {
            dbg << "  Setting " << i << ": " << pt->id().toString() << ": " << pt->name() << " = " << p->value() << Qt::endl;
        } else {
            dbg << "  Setting " << i << ": " << pt->id().toString() << ": " << pt->name() << " = " << "*** Unknown value ***" << Qt::endl;
        }
    }
    for (int i = 0; i < thing->thingClass()->stateTypes()->rowCount(); i++) {
        StateType *st = thing->thingClass()->stateTypes()->get(i);
        State *s = thing->states()->getState(st->id());
        dbg << "  State " << i << ": " << st->id() << ": " << st->name() << " = " << s->value() << Qt::endl;
    }
    return dbg;
}
