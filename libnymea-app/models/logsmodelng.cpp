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

#include "logsmodelng.h"
#include <QDateTime>
#include <QDebug>
#include <QMetaEnum>
#include <QJsonDocument>

#include "engine.h"
#include "types/logentry.h"
#include "logmanager.h"

LogsModelNg::LogsModelNg(QObject *parent) : QAbstractListModel(parent)
{

}

Engine *LogsModelNg::engine() const
{
    return m_engine;
}

void LogsModelNg::setEngine(Engine *engine)
{
    if (m_engine != engine) {
        m_engine = engine;
        connect(engine->logManager(), &LogManager::logEntryReceived, this, &LogsModelNg::newLogEntryReceived);
        emit engineChanged();
    }
}

int LogsModelNg::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.count();
}

QVariant LogsModelNg::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case RoleTimestamp:
        return m_list.at(index.row())->timestamp();
    case RoleValue:
        return m_list.at(index.row())->value();
    case RoleThingId:
    case RoleDeviceId:
        return m_list.at(index.row())->thingId();
    case RoleTypeId:
        return m_list.at(index.row())->typeId();
    case RoleSource:
        return m_list.at(index.row())->source();
    case RoleLoggingEventType:
        return m_list.at(index.row())->loggingEventType();
    }
    return QVariant();
}

QHash<int, QByteArray> LogsModelNg::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(RoleTimestamp, "timestamp");
    roles.insert(RoleValue, "value");
    roles.insert(RoleThingId, "thingId");
    roles.insert(RoleDeviceId, "deviceId");
    roles.insert(RoleTypeId, "typeId");
    roles.insert(RoleSource, "source");
    roles.insert(RoleLoggingEventType, "loggingEventType");
    return roles;
}

void LogsModelNg::classBegin()
{

}

void LogsModelNg::componentComplete()
{
    m_ready = true;
    fetchMore();
}

bool LogsModelNg::busy() const
{
    return m_busy;
}

bool LogsModelNg::live() const
{
    return m_live;
}

void LogsModelNg::setLive(bool live)
{
    if (m_live != live) {
        m_live = live;
        emit liveChanged();
    }
}

QUuid LogsModelNg::thingId() const
{
    return m_thingId;
}

void LogsModelNg::setThingId(const QUuid &thingId)
{
    if (m_thingId != thingId) {
        m_thingId = thingId;
        emit thingIdChanged();
    }
}

QStringList LogsModelNg::typeIds() const
{
    QStringList strings;
    foreach (const QUuid &id, m_typeIds) {
        strings.append(id.toString());
    }
    return strings;
}

void LogsModelNg::setTypeIds(const QStringList &typeIds)
{
    QList<QUuid> fixedTypeIds;
    foreach (const QString &id, typeIds) {
        fixedTypeIds.append(QUuid(id));
    }
    if (m_typeIds != fixedTypeIds) {
        m_typeIds = fixedTypeIds;
        emit typeIdsChanged();
        beginResetModel();
        qDeleteAll(m_list);
        m_list.clear();
        endResetModel();
        fetchMore();
    }
}

QDateTime LogsModelNg::startTime() const
{
    return m_startTime;
}

void LogsModelNg::setStartTime(const QDateTime &startTime)
{
    if (m_startTime != startTime) {
        m_startTime = startTime;
        emit startTimeChanged();
    }
}

QDateTime LogsModelNg::endTime() const
{
    return m_endTime;
}

void LogsModelNg::setEndTime(const QDateTime &endTime)
{
    if (m_endTime != endTime) {
        m_endTime = endTime;
        emit endTimeChanged();
    }
}

QtCharts::QXYSeries *LogsModelNg::graphSeries() const
{
    return m_graphSeries;
}

void LogsModelNg::setGraphSeries(QtCharts::QXYSeries *graphSeries)
{
    m_graphSeries = graphSeries;
}

QDateTime LogsModelNg::viewStartTime() const
{
    return m_viewStartTime;
}

void LogsModelNg::setViewStartTime(const QDateTime &viewStartTime)
{
    if (m_viewStartTime != viewStartTime) {
        m_viewStartTime = viewStartTime;
        emit viewStartTimeChanged();
        if (m_list.count() == 0 || m_list.last()->timestamp() > m_viewStartTime) {
            if (canFetchMore()) {
                fetchMore();
            }
        }
    }
}

QVariant LogsModelNg::minValue() const
{

    return m_minValue;
}

QVariant LogsModelNg::maxValue() const
{
    return m_maxValue;
}

LogEntry *LogsModelNg::get(int index) const
{
    if (index >= 0 && index < m_list.count()) {
        return m_list.at(index);
    }
    return nullptr;
}

void LogsModelNg::logsReply(int commandId, const QVariantMap &data)
{
    Q_UNUSED(commandId)
    int offset = data.value("offset").toInt();
    int count = data.value("count").toInt();

//    qDebug() << qUtf8Printable(QJsonDocument::fromVariant(data).toJson());

    QList<LogEntry*> newBlock;
    QList<QVariant> logEntries = data.value("logEntries").toList();
    foreach (const QVariant &logEntryVariant, logEntries) {
        QVariantMap entryMap = logEntryVariant.toMap();
        QDateTime timeStamp = QDateTime::fromMSecsSinceEpoch(entryMap.value("timestamp").toLongLong());
        QString thingId;
        if (m_engine->jsonRpcClient()->ensureServerVersion("5.0")) {
            thingId = entryMap.value("thingId").toString();
        } else {
            thingId = entryMap.value("deviceId").toString();
        }
        QString typeId = entryMap.value("typeId").toString();
        QMetaEnum sourceEnum = QMetaEnum::fromType<LogEntry::LoggingSource>();
        LogEntry::LoggingSource loggingSource = static_cast<LogEntry::LoggingSource>(sourceEnum.keyToValue(entryMap.value("source").toByteArray()));
        QMetaEnum loggingEventTypeEnum = QMetaEnum::fromType<LogEntry::LoggingEventType>();
        LogEntry::LoggingEventType loggingEventType = static_cast<LogEntry::LoggingEventType>(loggingEventTypeEnum.keyToValue(entryMap.value("eventType").toByteArray()));
        QVariant value = loggingEventType == LogEntry::LoggingEventTypeActiveChange ? entryMap.value("active").toBool() : entryMap.value("value");
        LogEntry *entry = new LogEntry(timeStamp, value, thingId, typeId, loggingSource, loggingEventType, this);
        newBlock.append(entry);
    }

    qDebug() << "Received logs from" << offset << "to" << offset + count << "Actual count:" << newBlock.count();

    if (count < m_blockSize) {
        m_canFetchMore = false;
    }

    if (newBlock.isEmpty()) {
        m_busy = false;
        emit busyChanged();
        return;
    }

    beginInsertRows(QModelIndex(), offset, offset + newBlock.count() - 1);
    QVariant newMin = m_minValue;
    QVariant newMax = m_maxValue;
    for (int i = 0; i < newBlock.count(); i++) {
        LogEntry *entry = newBlock.at(i);
        m_list.insert(offset + i, entry);
        Device *dev = m_engine->deviceManager()->devices()->getDevice(entry->thingId());
        if (!dev) {
            qWarning() << "Device not found in system. Cannot add item to graph series.";
            continue;
        }

        StateType *entryStateType = dev->thingClass()->stateTypes()->getStateType(entry->typeId());

        if (m_graphSeries) {
            if (entryStateType->type().toLower() == "bool") {

                // We don't want bools painting triangles, add a toggle point to keep lines straight
                if (i > 0) {
                    LogEntry *newerEntry = newBlock.at(i - 1);
                    if (newerEntry->value().toBool() != entry->value().toBool()) {
                        m_graphSeries->append(QPointF(newerEntry->timestamp().addMSecs(-1).toMSecsSinceEpoch(), entry->value().toBool() ? 1 : 0));
                    }
                }

                if (m_graphSeries->count() == 0) {
                    // If it's the first one, make sure we add an ending point at 1
                    m_graphSeries->append(QPointF(QDateTime::currentDateTime().addDays(1).toMSecsSinceEpoch(), 1));
                    m_graphSeries->append(QPointF(QDateTime::currentDateTime().addDays(1).toMSecsSinceEpoch(), entry->value().toBool() ? 1 : 0));
                } else if (i == 0) {
                    // Adding a new batch...  remove the last appended 1 from the previous batch
                    m_graphSeries->remove(m_graphSeries->count() - 1);
                }
                m_graphSeries->append(QPointF(entry->timestamp().toMSecsSinceEpoch(), entry->value().toBool() ? 1 : 0));
                if (i == newBlock.count() - 1) {
                    // End the batch at 1 again
                    m_graphSeries->append(QPointF(entry->timestamp().addSecs(60).toMSecsSinceEpoch(), 1));
                }

                // Adjust min/max
                if (!newMin.isValid() || newMin.toDouble() > entry->value().toDouble()) {
                    newMin = 0;
                }
                if (!newMax.isValid() || newMax.toDouble() < entry->value().toDouble()) {
                    newMax = 1;
                }

            } else {

                // Add a point in the future to extend the graph (so it can scroll with time and the graph wouldn't end at the last known value)
                if (m_graphSeries->count() == 0) {
                    m_graphSeries->append(QPointF(QDateTime::currentDateTime().addDays(1).toMSecsSinceEpoch(), Types::instance()->toUiValue(entry->value(), entryStateType->unit()).toReal()));
                }

                // Add the actual value
                QVariant value = Types::instance()->toUiValue(entry->value(), entryStateType->unit());
                m_graphSeries->append(QPointF(entry->timestamp().toMSecsSinceEpoch(), value.toReal()));

                // Adjust min/max
                if (!newMin.isValid() || newMin.toDouble() > value.toDouble()) {
                    newMin = value.toReal();
                }
                if (!newMax.isValid() || newMax.toDouble() < value.toDouble()) {
                    newMax = value.toReal();
                }
            }
        }
    }
    endInsertRows();
    emit countChanged();

    qDebug() << "min" << m_minValue << "max" << m_maxValue << "newMin" << newMin << "newMax" << newMax;
    if (m_minValue != newMin) {
        m_minValue = newMin;
        emit minValueChanged();
    }
    if (m_maxValue != newMax) {
        m_maxValue = newMax;
        emit maxValueChanged();
    }

    m_busy = false;
    emit busyChanged();

    if (m_viewStartTime.isValid() && m_list.count() > 0 && m_list.last()->timestamp() > m_viewStartTime && canFetchMore()) {
        fetchMore();
    }
}

void LogsModelNg::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if (!m_ready) {
        return;
    }
    if (!m_engine) {
        qWarning() << "Cannot update. Engine not set";
        return;
    }
    if (m_busy) {
        return;
    }

    if ((!m_startTime.isNull() && m_endTime.isNull()) || (m_startTime.isNull() && !m_endTime.isNull())) {
        // Need neither or both, startTime and endTime set
        return;
    }

    m_busy = true;
    emit busyChanged();

    QVariantMap params;
    if (!m_thingId.isNull()) {
        QVariantList thingIds;
        thingIds.append(m_thingId);
        if (m_engine->jsonRpcClient()->ensureServerVersion("5.0")) {
            params.insert("thingIds", thingIds);
        } else {
            params.insert("deviceIds", thingIds);
        }
    }
    if (!m_typeIds.isEmpty()) {
        QVariantList typeIds;
        foreach (const QUuid &typeId, m_typeIds) {
            typeIds.append(typeId);
        }
        params.insert("typeIds", typeIds);
    }
    if (!m_startTime.isNull() && !m_endTime.isNull()) {
        QVariantList timeFilters;
        QVariantMap timeFilter;
        timeFilter.insert("startDate", m_startTime.toSecsSinceEpoch());
        timeFilter.insert("endDate", m_endTime.toSecsSinceEpoch());
        timeFilters.append(timeFilter);
        params.insert("timeFilters", timeFilters);
    }

    params.insert("limit", m_blockSize);
    params.insert("offset", m_list.count());

    qDebug() << "Fetching logs:" << qUtf8Printable(QJsonDocument::fromVariant(params).toJson());

    m_engine->jsonRpcClient()->sendCommand("Logging.GetLogEntries", params, this, "logsReply");
//    qDebug() << "GetLogEntries called";
}

bool LogsModelNg::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
//    qDebug() << "canFetchMore" << (m_engine && m_canFetchMore);
    return m_engine && m_canFetchMore;
}

void LogsModelNg::newLogEntryReceived(const QVariantMap &data)
{
//    qDebug() << "***** model NG" << data << m_live;
    if (!m_live) {
        return;
    }

    QVariantMap entryMap = data;
    QUuid thingId;
    if (m_engine->jsonRpcClient()->ensureServerVersion("5.0")) {
        thingId = entryMap.value("deviceId").toUuid();
    } else {
        thingId = entryMap.value("thingId").toUuid();
    }
    if (!m_thingId.isNull() && thingId != m_thingId) {
        return;
    }

    QUuid typeId = entryMap.value("typeId").toUuid();
    if (!m_typeIds.isEmpty() && !m_typeIds.contains(typeId)) {
        return;
    }

    beginInsertRows(QModelIndex(), 0, 0);
    QDateTime timeStamp = QDateTime::fromMSecsSinceEpoch(entryMap.value("timestamp").toLongLong());
    QMetaEnum sourceEnum = QMetaEnum::fromType<LogEntry::LoggingSource>();
    LogEntry::LoggingSource loggingSource = static_cast<LogEntry::LoggingSource>(sourceEnum.keyToValue(entryMap.value("source").toByteArray()));
    QMetaEnum loggingEventTypeEnum = QMetaEnum::fromType<LogEntry::LoggingEventType>();
    LogEntry::LoggingEventType loggingEventType = static_cast<LogEntry::LoggingEventType>(loggingEventTypeEnum.keyToValue(entryMap.value("eventType").toByteArray()));
    QVariant value = loggingEventType == LogEntry::LoggingEventTypeActiveChange ? entryMap.value("active").toBool() : entryMap.value("value");
    LogEntry *entry = new LogEntry(timeStamp, value, thingId, typeId, loggingSource, loggingEventType, this);
    m_list.prepend(entry);
    if (m_graphSeries) {

        Device *dev = m_engine->thingManager()->devices()->getDevice(entry->thingId());

        StateType *entryStateType = dev->thingClass()->stateTypes()->getStateType(entry->typeId());

        if (dev && dev->thingClass()->stateTypes()->getStateType(entry->typeId())->type().toLower() == "bool") {
            // First, remove the 2 rightmost (newest on the timeline) values. They're the ones in the future we added to extend the graph and making it end at 1
            if (m_graphSeries->count() > 1) {
                m_graphSeries->removePoints(0, 2);
            }

            // Prevent triangles, add a point right before the new one which reflects the old value (if there is one)
            if (m_graphSeries->points().count() > 0) {
                qreal previousValue = m_graphSeries->points().at(0).y();
                m_graphSeries->insert(0, QPointF(entry->timestamp().addMSecs(-1).toMSecsSinceEpoch(), previousValue));
            }

            // Add the actual value
            m_graphSeries->insert(0, QPointF(entry->timestamp().toMSecsSinceEpoch(), entry->value().toBool() ? 1 : 0));

            // And add the 2 "future" points again
            m_graphSeries->insert(0, QPointF(entry->timestamp().addDays(1).toMSecsSinceEpoch(), entry->value().toBool() ? 1 : 0));
            m_graphSeries->insert(0, QPointF(entry->timestamp().addDays(1).toMSecsSinceEpoch(), 1));

        } else {

            // First, remove the rightmost (newest on the timeline) value. It's the one in the future we added to extend the graph
            if (m_graphSeries->count() > 1) {
                m_graphSeries->removePoints(0, 1);
            }

            // Add the actual value
            QVariant value = Types::instance()->toUiValue(entry->value(), entryStateType->unit());
            m_graphSeries->insert(0, QPointF(entry->timestamp().toMSecsSinceEpoch(), value.toReal()));

            // And add the "future" point again
            m_graphSeries->insert(0, QPointF(entry->timestamp().addDays(1).toMSecsSinceEpoch(), value.toReal()));
        }


        if (m_minValue.toDouble() > entry->value().toDouble()) {
            m_minValue = entry->value().toDouble();
            emit minValueChanged();
        }
        if (m_maxValue.toDouble() < entry->value().toDouble()) {
            m_maxValue = entry->value().toDouble();
            emit maxValueChanged();
        }
    }
    endInsertRows();
    emit countChanged();

}


