#include "GraphPlotter.h"


GraphPlotter::GraphPlotter(QWidget *parent)
    : QWidget(parent)
{
    initPlotter();
    initLayouts();

    m_actionSavePng = new QAction(tr("Сохранить изображение (.png)"), this);
    connect(m_actionSavePng, SIGNAL(triggered()), this, SLOT(onSavePng()));

    clear();
}

GraphPlotter::~GraphPlotter()
{
    delete m_plotter;
}

void GraphPlotter::initLayouts()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);

    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Plain);
    frame->setLineWidth(6);

    QHBoxLayout *frameLayout = new QHBoxLayout;
    frameLayout->setMargin(1);
    frameLayout->addWidget(m_plotter);

    frame->setLayout(frameLayout);
    mainLayout->addWidget(frame);

    this->setLayout(mainLayout);
}

void GraphPlotter::initPlotter()
{
    m_plotter = new QCustomPlot;
    m_plotter->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                               QCP::iSelectLegend | QCP::iSelectPlottables);

    m_plotter->xAxis->grid()->setSubGridVisible(true);
    m_plotter->yAxis->grid()->setSubGridVisible(true);
    m_plotter->yAxis->setLabelPadding(m_plotter->xAxis->labelPadding() + 2);
    m_plotter->axisRect()->setupFullAxesBox();

    m_plotter->setFont(this->font());
    m_plotter->legend->setVisible(true);
    m_plotter->legend->setSelectableParts(QCPLegend::spItems);
    QBrush brush = m_plotter->legend->brush();
    brush.setColor(QColor::fromRgb(255, 255, 255, 215));
    m_plotter->legend->setBrush(brush);
    m_plotter->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_plotter, SIGNAL(selectionChangedByUser()), this, SLOT(onSelectionChanged()));
    connect(m_plotter, SIGNAL(mousePress(QMouseEvent *)), this, SLOT(onMousePress()));
    connect(m_plotter, SIGNAL(mouseWheel(QWheelEvent *)), this, SLOT(onMouseWheel()));
    connect(m_plotter, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(onMenuRequest(QPoint)));
    connect(m_plotter->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plotter->xAxis2,
            SLOT(setRange(QCPRange)));
    connect(m_plotter->yAxis, SIGNAL(rangeChanged(QCPRange)), m_plotter->yAxis2,
            SLOT(setRange(QCPRange)));
}

// slots:

void GraphPlotter::onSelectionChanged()
{
    bool test = m_plotter->xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
                m_plotter->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
                m_plotter->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
                m_plotter->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels);
    if (test) {
        m_plotter->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
        m_plotter->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    }

    test = m_plotter->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
           m_plotter->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
           m_plotter->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
           m_plotter->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels);
    if (test) {
        m_plotter->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
        m_plotter->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    }

    for (int i = 0; i < m_plotter->graphCount(); ++i) {
        QCPGraph *              graph = m_plotter->graph(i);
        QCPPlottableLegendItem *item  = m_plotter->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected()) {
            item->setSelected(true);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
}

void GraphPlotter::onMousePress()
{
    if (m_plotter->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_plotter->axisRect()->setRangeDrag(m_plotter->xAxis->orientation());
    } else if (m_plotter->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_plotter->axisRect()->setRangeDrag(m_plotter->yAxis->orientation());
    } else {
        m_plotter->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    }
}

void GraphPlotter::onMouseWheel()
{
    if (m_plotter->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_plotter->axisRect()->setRangeZoom(m_plotter->xAxis->orientation());
    } else if (m_plotter->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_plotter->axisRect()->setRangeZoom(m_plotter->yAxis->orientation());
    } else {
        m_plotter->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
}

void GraphPlotter::onMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (m_plotter->legend->selectTest(pos, false) >= 0) {
        menu->addAction(tr("В левый верхний угол"), this, SLOT(onMoveLegend()))
            ->setData((int(Qt::AlignTop | Qt::AlignLeft)));
        menu->addAction(tr("В правый верхний угол"), this, SLOT(onMoveLegend()))
            ->setData((int(Qt::AlignTop | Qt::AlignRight)));
        menu->addAction(tr("В правый нижний угол"), this, SLOT(onMoveLegend()))
            ->setData((int(Qt::AlignBottom | Qt::AlignRight)));
        menu->addAction(tr("В левый нижний угол"), this, SLOT(onMoveLegend()))
            ->setData((int(Qt::AlignBottom | Qt::AlignLeft)));
    } else {
        menu->addAction(tr("Сохранить"), this, SLOT(onSavePng()));
    }
    menu->popup(m_plotter->mapToGlobal(pos));
}

void GraphPlotter::onMoveLegend()
{
    if (QAction *contextAction = qobject_cast<QAction *>(sender())) {
        bool ok;
        int  dataInt = contextAction->data().toInt(&ok);
        if (ok) {
            m_plotter->axisRect()->insetLayout()->setInsetAlignment(0, Qt::Alignment(dataInt));
            m_plotter->replot();
        }
    }
}

void GraphPlotter::clear()
{
    m_plotter->clearGraphs();
    m_plotter->xAxis->setRange(-1, 1);
    m_plotter->yAxis->setRange(0, 1);
    m_plotter->replot();
}

void GraphPlotter::onSavePng()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить изображение"),
                                                    QDir::homePath(), tr("Изображение (*.png)"));
    m_plotter->savePng(fileName);
}

void GraphPlotter::add(const QVector<double> &x, const QVector<double> &y, const QString &label,
                       const QPen &pen)
{
    int k = m_plotter->graphCount();
    m_plotter->addGraph();
    m_plotter->graph(k)->setData(x, y);
    m_plotter->graph(k)->setName(label);
    m_plotter->graph(k)->setPen(pen);
    m_plotter->replot();
}

void GraphPlotter::setXLabel(const QString &label)
{
    m_plotter->xAxis->setLabel(label);
    m_plotter->replot();
}

void GraphPlotter::setYLabel(const QString &label)
{
    m_plotter->yAxis->setLabel(label);
    m_plotter->replot();
}

void GraphPlotter::setXRange(double min, double max)
{
    if (min >= max) {
        return;
    }
    m_plotter->xAxis->setRange(min, max);
    m_plotter->replot();
}

void GraphPlotter::setYRange(double min, double max)
{
    if (min >= max) {
        return;
    }
    m_plotter->yAxis->setRange(min, max);
    m_plotter->replot();
}
