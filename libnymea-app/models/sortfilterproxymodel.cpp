#include "sortfilterproxymodel.h"

#include <QtDebug>

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    connect(this, &QSortFilterProxyModel::sourceModelChanged, this, [=](){
        connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &SortFilterProxyModel::countChanged);
        connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &SortFilterProxyModel::countChanged);
        connect(sourceModel(), &QAbstractItemModel::modelReset, this, &SortFilterProxyModel::countChanged);
        emit countChanged();
    });
}

QString SortFilterProxyModel::filterRoleName() const
{
    return m_filterRoleName;
}

void SortFilterProxyModel::setFilterRoleName(const QString &filterRoleName)
{
    if (m_filterRoleName != filterRoleName) {
        m_filterRoleName = filterRoleName;
        emit filterRoleNameChanged();
        invalidateFilter();
        emit countChanged();
    }
}

QStringList SortFilterProxyModel::filterList() const
{
    return m_filterList;
}

void SortFilterProxyModel::setFilterList(const QStringList &filterList)
{
    if (m_filterList != filterList) {
        m_filterList = filterList;
        emit filterListChanged();
        invalidateFilter();
        emit countChanged();
    }
}

QVariant SortFilterProxyModel::get(int row, const QString &role) const
{
    int roleId = roleNames().key(role.toUtf8());
    return QSortFilterProxyModel::data(index(row, 0), roleId);
}

bool SortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!m_filterList.isEmpty() && !m_filterRoleName.isEmpty()) {
        QModelIndex idx = sourceModel()->index(source_row, 0, source_parent);
        int filterRole = sourceModel()->roleNames().key(m_filterRoleName.toUtf8());
        QVariant data = sourceModel()->data(idx, filterRole);
        if (!m_filterList.contains(data.toString())) {
            return false;
        }
    }
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
