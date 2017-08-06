#include "TableActorsBase.h"

#include <QFontMetrics>
#include <QHBoxLayout>
#include <QHeaderView>


TableItemInt::TableItemInt(int value)
    : QTableWidgetItem(QString::number(value))
{
}

bool TableItemInt::operator<(const QTableWidgetItem &other) const
{
    return (this->text().toInt() < other.text().toInt());
}


TableItemDbl::TableItemDbl(double value)
    : QTableWidgetItem(QString::number(value, 'g', 6))
{
}

bool TableItemDbl::operator<(const QTableWidgetItem &other) const
{
    return (this->text().toDouble() < other.text().toDouble());
}


TableItemDblWithLabel::TableItemDblWithLabel(double value, const QString &label)
    : QTableWidgetItem(QString::number(value, 'g', 6) + " " + label)
    , value(value)
{
}

bool TableItemDblWithLabel::operator<(const TableItemDblWithLabel &other) const
{
    return (value < other.value);
}


// ---------------------------------------------------------------------------//


TableActorsBase::TableActorsBase(QWidget *parent)
    : QTableWidget(parent)
{
    clear();
}

void TableActorsBase::add(const Actor &actor, const QString &name)
{
    int row = this->rowCount();
    this->setRowCount(row + 1);

    m_checkBoxes.push_back(new QCheckBox(this));
    m_actors.push_back(actor);
    m_names.push_back(name);

    this->setCellWidget(row, 0, new QWidget);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(5);
    layout->addWidget(m_checkBoxes[m_checkBoxes.size() - 1]);
    this->cellWidget(row, 0)->setLayout(layout);

    this->setItem(row, 1, new TableItemInt(actor.id()));
    this->setItem(row, 2, new QTableWidgetItem(name));
    this->setItem(row, 3, new TableItemDbl(actor.thetaG()));
    this->setItem(row, 4, new TableItemDbl(actor.thetaE()));

    for (int i = 0; i < this->columnCount(); ++i) {
        optimizeColumnWidth(i);
    }

    this->sortByColumn(3);
}

void TableActorsBase::add(const Array<Actor> &actors, const Array<QString> &names)
{
    assert(actors.size() == names.size());

    for (size_t i = 0; i < actors.size(); ++i) {
        add(actors[i], names[i]);
    }
}

void TableActorsBase::addColumn(const QString &head, const Array<double> &values)
{
    assert(int(values.size()) == this->rowCount());

    int col = this->columnCount();
    this->setColumnCount(col + 1);
    this->setHorizontalHeaderItem(col, new QTableWidgetItem(head));

    for (int i = 0; i < this->rowCount(); ++i) {
        this->setItem(i, col, new TableItemDbl(values[i]));
    }

    optimizeColumnWidth(col);
}

void TableActorsBase::addColumn(const QString &head, const Array<double> &values,
                                const Array<QString> &labels)
{
    assert(int(values.size()) == this->rowCount());

    int col = this->columnCount();
    this->setColumnCount(col + 1);
    this->setHorizontalHeaderItem(col, new QTableWidgetItem(head));

    for (int i = 0; i < this->rowCount(); ++i) {
        this->setItem(i, col, new TableItemDblWithLabel(values[i], labels[i]));
    }

    optimizeColumnWidth(col);
}

const Array<Actor> &TableActorsBase::actors() const
{
    return m_actors;
}

Array<Actor> TableActorsBase::getCheckedActors() const
{
    Array<Actor> actors;
    for (size_t i = 0; i < m_checkBoxes.size(); ++i) {
        if (m_checkBoxes[i]->isChecked()) {
            actors.push_back(m_actors[i]);
        }
    }
    return actors;
}

Array<QString> TableActorsBase::getCheckedNames() const
{
    Array<QString> names;
    for (size_t i = 0; i < m_checkBoxes.size(); ++i) {
        if (m_checkBoxes[i]->isChecked()) {
            names.push_back(m_names[i]);
        }
    }
    return names;
}


// public slots:

void TableActorsBase::clear()
{
    QTableWidget::clear();

    m_actors.clear();
    m_names.clear();
    m_checkBoxes.clear();

    this->setSortingEnabled(true);
    this->verticalHeader()->setVisible(false);
    this->setRowCount(0);
    this->setColumnCount(5);
    this->setHorizontalHeaderLabels({tr(""), tr("ID"), tr("Ф.И.О."), tr("θg"), tr("θe")});

    for (int i = 0; i < this->columnCount(); ++i) {
        optimizeColumnWidth(i);
    }
}

void TableActorsBase::uncheckAll()
{
    for (size_t i = 0; i < m_checkBoxes.size(); ++i) {
        m_checkBoxes[i]->setChecked(false);
    }
}


// private:

void TableActorsBase::optimizeColumnWidth(int index)
{
    if (index < 0 || index > this->columnCount() - 1) {
        return;
    }

    if (index == 0) {
        this->setColumnWidth(0, this->rowHeight(0));
        return;
    }

    QFontMetrics fontMetrics(this->font());

    int width = fontMetrics.width(this->horizontalHeaderItem(index)->text() + "     ");

    for (int i = 0; i < this->rowCount(); ++i) {
        int wi = fontMetrics.width(this->item(i, index)->text() + "     ");
        if (width < wi) {
            width = wi;
        }
    }

    this->setColumnWidth(index, width);
}
