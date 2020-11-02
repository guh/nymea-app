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

#include "statedescriptor.h"

#include <QDebug>

StateDescriptor::StateDescriptor(const QUuid &thingId, const QUuid &stateTypeId, StateDescriptor::ValueOperator valueOperator, const QVariant &value, QObject *parent):
    QObject(parent),
    m_thingId(thingId),
    m_stateTypeId(stateTypeId),
    m_operator(valueOperator),
    m_value(value)
{

}

StateDescriptor::StateDescriptor(const QString &interfaceName, const QString &interfaceState, StateDescriptor::ValueOperator valueOperator, const QVariant &value, QObject *parent):
    QObject(parent),
    m_interfaceName(interfaceName),
    m_interfaceState(interfaceState),
    m_operator(valueOperator),
    m_value(value)
{

}

StateDescriptor::StateDescriptor(QObject *parent) : QObject(parent)
{

}

QUuid StateDescriptor::thingId() const
{
    return m_thingId;
}

void StateDescriptor::setThingId(const QUuid &thingId)
{
    if (m_thingId != thingId) {
        m_thingId = thingId;
        emit thingIdChanged();
    }
}

StateDescriptor::ValueOperator StateDescriptor::valueOperator() const
{
    return m_operator;
}

void StateDescriptor::setValueOperator(StateDescriptor::ValueOperator valueOperator)
{
    if (m_operator != valueOperator) {
        m_operator = valueOperator;
        emit valueOperatorChanged();
    }
}

QUuid StateDescriptor::stateTypeId() const
{
    return m_stateTypeId;
}

void StateDescriptor::setStateTypeId(const QUuid &stateTypeId)
{
    if (m_stateTypeId != stateTypeId) {
        m_stateTypeId = stateTypeId;
        emit stateTypeIdChanged();
    }
}

QString StateDescriptor::interfaceName() const
{
    return m_interfaceName;
}

void StateDescriptor::setInterfaceName(const QString &interfaceName)
{
    if (m_interfaceName != interfaceName) {
        m_interfaceName = interfaceName;
        emit interfaceNameChanged();
    }
}

QString StateDescriptor::interfaceState() const
{
    return m_interfaceState;
}

void StateDescriptor::setInterfaceState(const QString &interfaceState)
{
    if (m_interfaceState != interfaceState) {
        m_interfaceState = interfaceState;
        emit interfaceStateChanged();
    }
}

QVariant StateDescriptor::value() const
{
    return m_value;
}

void StateDescriptor::setValue(const QVariant &value)
{
    if (m_value != value) {
        m_value = value;
        emit valueChanged();
    }
}

QUuid StateDescriptor::valueThingId() const
{
    return m_valueThingId;
}

void StateDescriptor::setValueThingId(const QUuid &valueThingId)
{
    if (m_valueThingId != valueThingId) {
        m_valueThingId = valueThingId;
        emit valueThingIdChanged();
    }
}

QUuid StateDescriptor::valueStateTypeId() const
{
    return m_valueStateTypeId;
}

void StateDescriptor::setValueStateTypeId(const QUuid &valueStateTypeId)
{
    if (m_valueStateTypeId != valueStateTypeId) {
        m_valueStateTypeId = valueStateTypeId;
        emit valueStateTypeIdChanged();
    }
}

StateDescriptor *StateDescriptor::clone() const
{
    StateDescriptor *ret = new StateDescriptor(thingId(), stateTypeId(), valueOperator(), value());
    ret->setInterfaceName(interfaceName());
    ret->setInterfaceState(interfaceState());
    ret->setValueThingId(valueThingId());
    ret->setValueStateTypeId(valueStateTypeId());
    return ret;
}

#define COMPARE(a, b) if (a != b) { qDebug() << a << "!=" << b; return false; }
#define COMPARE_PTR(a, b) if (!a->operator==(b)) { qDebug() << a << "!=" << b; return false; }
bool StateDescriptor::operator==(StateDescriptor *other) const
{
    COMPARE(m_thingId, other->thingId())
    COMPARE(m_stateTypeId, other->stateTypeId())
    COMPARE(m_interfaceName, other->interfaceName())
    COMPARE(m_interfaceState, other->interfaceState())
    COMPARE(m_operator, other->valueOperator())
    COMPARE(m_value, other->value())
    COMPARE(m_valueThingId, other->valueThingId())
    COMPARE(m_valueStateTypeId, other->valueStateTypeId())
    return true;
}
