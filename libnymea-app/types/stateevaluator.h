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

#ifndef STATEEVALUATOR_H
#define STATEEVALUATOR_H

#include <QObject>

class StateEvaluators;
class StateDescriptor;

class StateEvaluator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(StateOperator stateOperator READ stateOperator WRITE setStateOperator NOTIFY stateOperatorChanged)
    Q_PROPERTY(StateEvaluators* childEvaluators READ childEvaluators CONSTANT)
    Q_PROPERTY(StateDescriptor* stateDescriptor READ stateDescriptor CONSTANT)

public:
    enum StateOperator {
        StateOperatorAnd,
        StateOperatorOr
    };
    Q_ENUM(StateOperator)
    explicit StateEvaluator(QObject *parent = nullptr);

    StateOperator stateOperator() const;
    void setStateOperator(StateOperator stateOperator);

    StateEvaluators* childEvaluators() const;

    StateDescriptor* stateDescriptor() const;
    void setStateDescriptor(StateDescriptor *stateDescriptor);

    bool containsDevice(const QUuid &thingId) const;

    Q_INVOKABLE StateEvaluator* addChildEvaluator();

    StateEvaluator* clone() const;
    bool operator==(StateEvaluator *other) const;

signals:
    void stateOperatorChanged();

private:
    StateOperator m_operator = StateOperatorAnd;
    StateEvaluators *m_childEvaluators = nullptr;
    StateDescriptor *m_stateDescriptor = nullptr;

};

#endif // STATEEVALUATOR_H
