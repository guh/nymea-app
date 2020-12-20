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

#include "rule.h"

#include "eventdescriptor.h"
#include "eventdescriptors.h"
#include "stateevaluator.h"
#include "stateevaluators.h"
#include "statedescriptor.h"
#include "ruleaction.h"
#include "ruleactions.h"
#include "timedescriptor.h"
#include "timeeventitems.h"
#include "timeeventitem.h"
#include "calendaritems.h"
#include "calendaritem.h"
#include "ruleactionparams.h"
#include "ruleactionparam.h"

#include <QDebug>

Rule::Rule(const QUuid &id, QObject *parent) :
    QObject(parent),
    m_id(id),
    m_eventDescriptors(new EventDescriptors(this)),
//    m_stateEvaluator(new StateEvaluator(this)),
    m_actions(new RuleActions(this)),
    m_exitActions(new RuleActions(this)),
    m_timeDescriptor(new TimeDescriptor(this))
{
//    qDebug() << "### Creating rule" << this;
}

Rule::~Rule()
{
}

QUuid Rule::id() const
{
    return m_id;
}

QString Rule::name() const
{
    return m_name;
}

void Rule::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

bool Rule::enabled() const
{
    return m_enabled;
}

void Rule::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged();
    }
}

bool Rule::active() const
{
    return m_active;
}

void Rule::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        emit activeChanged();
    }
}

bool Rule::executable() const
{
    return m_executable;
}

void Rule::setExecutable(bool executable)
{
    if (m_executable != executable) {
        m_executable = executable;
        emit executableChanged();
    }
}

EventDescriptors *Rule::eventDescriptors() const
{
    return m_eventDescriptors;
}

StateEvaluator *Rule::stateEvaluator() const
{
    return m_stateEvaluator;
}

RuleActions *Rule::actions() const
{
    return m_actions;
}

RuleActions *Rule::exitActions() const
{
    return m_exitActions;
}

TimeDescriptor *Rule::timeDescriptor() const
{
    return m_timeDescriptor;
}

void Rule::setStateEvaluator(StateEvaluator *stateEvaluator)
{
    if (m_stateEvaluator) {
        m_stateEvaluator->deleteLater();
    }
    m_stateEvaluator = stateEvaluator;
    if (m_stateEvaluator) { // Might be a nullptr now if cleared
        m_stateEvaluator->setParent(this);
    }
    emit stateEvaluatorChanged();
}

StateEvaluator* Rule::createStateEvaluator() const
{
    return new StateEvaluator();
}

Rule *Rule::clone() const
{
    Rule *ret = new Rule(this->id());
    ret->setName(this->name());
    ret->setEnabled(this->enabled());
    ret->setExecutable(this->executable());
    for (int i = 0; i < this->eventDescriptors()->rowCount(); i++) {
        ret->eventDescriptors()->addEventDescriptor(this->eventDescriptors()->get(i)->clone());
    }
    for (int i = 0; i < this->timeDescriptor()->timeEventItems()->rowCount(); i++) {
        ret->timeDescriptor()->timeEventItems()->addTimeEventItem(this->timeDescriptor()->timeEventItems()->get(i)->clone());
    }
    for (int i = 0; i < this->timeDescriptor()->calendarItems()->rowCount(); i++) {
        ret->timeDescriptor()->calendarItems()->addCalendarItem(this->timeDescriptor()->calendarItems()->get(i)->clone());
    }
    if (this->stateEvaluator()) {
        ret->setStateEvaluator(this->stateEvaluator()->clone());
    }
    for (int i = 0; i < this->actions()->rowCount(); i++) {
        ret->actions()->addRuleAction(this->actions()->get(i)->clone());
    }
    for (int i = 0; i < this->exitActions()->rowCount(); i++) {
        ret->exitActions()->addRuleAction(this->exitActions()->get(i)->clone());
    }
    return ret;
}

bool Rule::compare(Rule *other) const
{
    qDebug() << "comparing rule" << this << "to" << other;
    return this->operator==(other);
}

#define COMPARE(a, b) if (a != b) { qDebug() << a << "!=" << b; return false; }
#define COMPARE_PTR(a, b) if (!a && !b) return true; if (!a || !b) return false; if (!a->operator==(b)) { qDebug() << a << "!=" << b; return false; }
bool Rule::operator==(Rule *other) const
{
    COMPARE(m_id, other->id());
    COMPARE(m_name, other->name());
    COMPARE(m_enabled, other->enabled());
    COMPARE(m_executable, other->executable());
    COMPARE_PTR(m_eventDescriptors, other->eventDescriptors());
    COMPARE_PTR(m_stateEvaluator, other->stateEvaluator());
    COMPARE_PTR(m_actions, other->actions());
    COMPARE_PTR(m_exitActions, other->exitActions());
    COMPARE_PTR(m_timeDescriptor, other->timeDescriptor());
    return true;
}

QDebug operator <<(QDebug &dbg, Rule *rule)
{
    dbg << rule->name() << " (Enabled:" << rule->enabled() << "Active:" << rule->active() << ")" << Qt::endl;
    if (rule->eventDescriptors()->rowCount() > 0) {
        dbg << "Event descriptors:" << Qt::endl;
    }
    for (int i = 0; i < rule->eventDescriptors()->rowCount(); i++) {
        EventDescriptor *ed = rule->eventDescriptors()->get(i);
        dbg << " " << i << ":";
        if (!ed->thingId().isNull() && !ed->eventTypeId().isNull()) {
            dbg << "Thing ID:" << ed->thingId() << "Event Type ID:" << ed->eventTypeId() << Qt::endl;
        } else {
            dbg << "Interface Name:" << ed->interfaceName() << "Event Name:" << ed->interfaceEvent() << Qt::endl;
        }
        for (int j = 0; j < ed->paramDescriptors()->rowCount(); j++) {
            ParamDescriptor *epd = ed->paramDescriptors()->get(j);
            QString operatorString;
            switch (epd->operatorType()) {
            case ParamDescriptor::ValueOperatorLess:
                operatorString = "<";
                break;
            case ParamDescriptor::ValueOperatorEquals:
                operatorString = "=";
                break;
            case ParamDescriptor::ValueOperatorGreater:
                operatorString = ">";
                break;
            case ParamDescriptor::ValueOperatorNotEquals:
                operatorString = "!=";
                break;
            case ParamDescriptor::ValueOperatorLessOrEqual:
                operatorString = "<=";
                break;
            case ParamDescriptor::ValueOperatorGreaterOrEqual:
                operatorString = ">=";
                break;
            }
            dbg << "    Param" << j << ": ID:" << epd->paramTypeId() << operatorString << " Value:" << epd->value() << Qt::endl;
        }
    }
    if (rule->stateEvaluator()) {
        dbg << "State Evaluator:" << Qt::endl;
        printStateEvaluator(dbg, rule->stateEvaluator());
    }

    if (rule->actions()->rowCount() > 0) {
        dbg << "Actions:" << Qt::endl;
    }
    for (int i = 0; i < rule->actions()->rowCount(); i++) {
        RuleAction *ra = rule->actions()->get(i);
        dbg << " " << i << ":";
        if (!ra->deviceId().isNull() && !ra->actionTypeId().isNull()) {
            dbg << "Device ID:" << ra->deviceId() << "Action Type ID:" << ra->actionTypeId() << Qt::endl;
        } else {
            dbg << "Interface Name:" << ra->interfaceName() << "Action Name:" << ra->interfaceAction() << Qt::endl;
        }
        for (int j = 0; j < ra->ruleActionParams()->rowCount(); j++) {
            RuleActionParam *rap = ra->ruleActionParams()->get(j);
            if (rap->eventTypeId().isNull()) {
                dbg << "    Param" << j << ": ID:" << rap->paramTypeId() << " Value:" << rap->value() << Qt::endl;
            } else {
                dbg << "    Param" << j << ": ID:" << rap->paramTypeId() << " Source Event Type ID:" << rap->eventTypeId() << "Source Event Param ID:" << rap->eventParamTypeId() << Qt::endl;
            }
        }
    }

    if (rule->exitActions()->rowCount() > 0) {
        dbg << "Exit Actions:" << Qt::endl;
    }
    for (int i = 0; i < rule->exitActions()->rowCount(); i++) {
        RuleAction *ra = rule->exitActions()->get(i);
        dbg << " " << i << ":";
        if (!ra->deviceId().isNull() && !ra->actionTypeId().isNull()) {
            dbg << "Device ID:" << ra->deviceId() << "Action Type ID:" << ra->actionTypeId() << Qt::endl;
        } else {
            dbg << "Interface Name:" << ra->interfaceName() << "Action Name:" << ra->interfaceAction() << Qt::endl;
        }
        for (int j = 0; j < ra->ruleActionParams()->rowCount(); j++) {
            RuleActionParam *rap = ra->ruleActionParams()->get(j);
            if (rap->eventTypeId().isNull()) {
                dbg << "    Param" << j << ": ID:" << rap->paramTypeId() << " Value:" << rap->value() << Qt::endl;
            } else {
                dbg << "    Param" << j << ": ID:" << rap->paramTypeId() << " Source Event Type ID:" << rap->eventTypeId() << "Source Event Param ID:" << rap->eventParamTypeId() << Qt::endl;
            }
        }
    }
    return dbg;
}

QDebug printStateEvaluator(QDebug &dbg, StateEvaluator *stateEvaluator, int indentLevel)
{
    if (stateEvaluator->stateDescriptor()) {
        for (int i = 0; i < indentLevel; i++) { dbg << " "; }
        dbg << "State Descriptor:";
        if (!stateEvaluator->stateDescriptor()->deviceId().isNull() && !stateEvaluator->stateDescriptor()->stateTypeId().isNull()) {
            dbg << "Device ID:" << stateEvaluator->stateDescriptor()->deviceId().toString() << "State Type ID:" << stateEvaluator->stateDescriptor()->stateTypeId().toString();
        } else {
            dbg << "Interface name:" << stateEvaluator->stateDescriptor()->interfaceName() << "State Name:" << stateEvaluator->stateDescriptor()->interfaceState();
        }
        switch (stateEvaluator->stateDescriptor()->valueOperator()) {
        case StateDescriptor::ValueOperatorLess:
            dbg << "<";
            break;
        case StateDescriptor::ValueOperatorEquals:
            dbg << "=";
            break;
        case StateDescriptor::ValueOperatorGreater:
            dbg << ">";
            break;
        case StateDescriptor::ValueOperatorNotEquals:
            dbg << "!=";
            break;
        case StateDescriptor::ValueOperatorLessOrEqual:
            dbg << "<=";
            break;
        case StateDescriptor::ValueOperatorGreaterOrEqual:
            dbg << ">=";
            break;
        }
        dbg << stateEvaluator->stateDescriptor()->value() << Qt::endl;
    }
    if (stateEvaluator->childEvaluators()->rowCount() > 0) {
        for (int i = 0; i < indentLevel; i++) { dbg << " "; }
        dbg << (stateEvaluator->stateOperator() == StateEvaluator::StateOperatorAnd ? "AND" : "OR") << Qt::endl;
    }
    for (int i = 0; i < stateEvaluator->childEvaluators()->rowCount(); i++) {
        printStateEvaluator(dbg, stateEvaluator->childEvaluators()->get(i), indentLevel+1);
    }
    return dbg;
}
