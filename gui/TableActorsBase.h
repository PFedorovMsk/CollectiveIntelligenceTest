#ifndef TABLEACTORSBASE_H
#define TABLEACTORSBASE_H

#include "core/Actor.h"

#include <QCheckBox>
#include <QString>
#include <QTableWidget>


class TableItemInt : public QTableWidgetItem
{
public:
    TableItemInt(int value);
    bool operator<(const QTableWidgetItem &other) const override;
};


class TableItemDbl : public QTableWidgetItem
{
public:
    TableItemDbl(double value);
    bool operator<(const QTableWidgetItem &other) const override;
};

class TableItemDblWithLabel : public QTableWidgetItem
{
public:
    TableItemDblWithLabel(double value, const QString &label);
    bool operator<(const TableItemDblWithLabel &other) const;

private:
    double value;
};


class TableActorsBase : public QTableWidget
{
    Q_OBJECT

public:
    explicit TableActorsBase(QWidget *parent = nullptr);

    void add(const Actor &actor, const QString &name);
    void add(const Array<Actor> &actors, const Array<QString> &names);

    void addColumn(const QString &head, const Array<double> &values);
    void addColumn(const QString &head, const Array<double> &values, const Array<QString> &labels);

    const Array<Actor> &actors() const;

    Array<Actor>   getCheckedActors() const;
    Array<QString> getCheckedNames() const;

public slots:
    void clear();
    void uncheckAll();

private:
    void optimizeColumnWidth(int index);


private:
    Array<Actor>       m_actors;
    Array<QCheckBox *> m_checkBoxes;
    Array<QString>     m_names;
};


#endif // TABLEACTORSBASE_H
