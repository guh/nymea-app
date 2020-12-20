#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterRoleName READ filterRoleName WRITE setFilterRoleName NOTIFY filterRoleNameChanged)
    Q_PROPERTY(QStringList filterList READ filterList WRITE setFilterList NOTIFY filterListChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit SortFilterProxyModel(QObject *parent = nullptr);

    QString filterRoleName() const;
    void setFilterRoleName(const QString &filterRoleName);

    QStringList filterList() const;
    void setFilterList(const QStringList &filterList);

    Q_INVOKABLE QVariant get(int row, const QString &role) const;

signals:
    void filterRoleNameChanged();
    void filterListChanged();
    void countChanged();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString m_filterRoleName;
    QStringList m_filterList;
};

#endif // SORTFILTERPROXYMODEL_H
