#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include "QCustomPlot.h"
#include <QAction>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFrame>
#include <QMenu>
#include <QPen>
#include <QWidget>


class GraphPlotter : public QWidget
{
    Q_OBJECT

public:
    explicit GraphPlotter(QWidget *parent = nullptr);
    ~GraphPlotter();

    void clear();

    void add(const QVector<double> &x, const QVector<double> &y, const QString &label,
             const QPen &pen);

    void setXLabel(const QString &label);
    void setYLabel(const QString &label);
    void setXRange(double min, double max);
    void setYRange(double min, double max);


private slots:
    void onSelectionChanged();
    void onMousePress();
    void onMouseWheel();
    void onMenuRequest(QPoint pos);
    void onMoveLegend();
    void onSavePng();


private:
    void initLayouts();
    void initPlotter();


private:
    QCustomPlot *m_plotter;

    QAction *m_actionSavePng;
};


#endif // GRAPHPLOTTER_H
