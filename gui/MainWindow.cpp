#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "core/GroupActor.h"
#include "core/Utils.h"

#include <QVector>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::initStatusBar()
{
    if (!statusBar()) {
        setStatusBar(new QStatusBar);
    }

    int h = QFontMetrics(statusBar()->font()).height();

    m_statusProgressBar = new QProgressBar;
    m_statusProgressBar->setMinimumHeight(h);
    m_statusProgressBar->setMaximumHeight(h);
    m_statusProgressBar->setEnabled(false);
    m_statusProgressBar->setValue(0);
    m_statusProgressBar->setMinimum(0);
    m_statusProgressBar->setMaximum(100);
    m_statusProgressBar->setTextVisible(true);

    statusBar()->addPermanentWidget(m_statusProgressBar);
    statusBar()->layout()->setMargin(centralWidget()->layout()->margin());
    statusBar()->layout()->setSpacing(centralWidget()->layout()->spacing());
    statusBar()->showMessage(tr("Состояние: ничего не выполняется"));
}

void MainWindow::on_btnLoadFromFile_clicked()
{
    QString fileName =
        QFileDialog::getOpenFileName(this, tr("Выберете файл с результатами тестирования акторов"), QDir::homePath());

    if (fileName.isNull() || fileName.isEmpty()) {
        return;
    }

    statusBar()->showMessage(tr("Состояние: извлечение данных из файла ") + fileName);

    GetActorsBaseFromFile(fileName, m_actorsBase, m_names);
    m_actors = m_actorsBase.getActors();

    ui->tableActorsBase->clear();
    for (size_t i = 0; i < m_actors.size(); ++i) {
        ui->tableActorsBase->add(m_actors[i], m_names[int(i)]);
    }

    ui->tabWidget->setCurrentIndex(0);

    statusBar()->showMessage(tr("Состояние: ничего не выполняется"));
}

void MainWindow::on_btnGenerate_clicked()
{
    QString fileName = "generated.txt";
    GenerateFile(fileName, 100, 200);

    GetActorsBaseFromFile(fileName, m_actorsBase, m_names);
    m_actors = m_actorsBase.getActors();

    ui->tableActorsBase->clear();
    for (size_t i = 0; i < m_actors.size(); ++i) {
        ui->tableActorsBase->add(m_actors[i], m_names[int(i)]);
    }

    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_btnComputeSingleIP_clicked()
{
    ui->tabWidget->setCurrentIndex(0);

    statusBar()->showMessage(tr("Состояние: вычисление характеристик акторов"));
    m_statusProgressBar->setEnabled(true);
    m_statusProgressBar->setValue(0);

    double beta = ui->dsbBeta->value();
    size_t it   = size_t(ui->sbSingleCountIt->value());
    size_t size = ui->tableActorsBase->actors().size();

    Array<double>  iq(size), ip(size), gr(size), gn(size);
    Array<QString> labels(size);

    for (size_t i = 0; i < size; ++i) {
        Actor  actor = ui->tableActorsBase->actors()[i];
        size_t count;
        double qr, qn;

        gr[i] = actor.gr(beta);
        gn[i] = actor.gn(beta);
        iq[i] = actor.iq(beta);
        if (ui->cbSingleIp->isChecked()) {
            ip[i]     = actor.ip(beta, count, qr, qn, 101, it);
            labels[i] = "(m = " + QString::number(count) + ")";
        }

        m_statusProgressBar->setValue(int(100.0 * (i + 1) / size));
        m_statusProgressBar->update();
    }

    QString strBeta = " (β = " + QString::number(beta, 'g', 3) + ")";

    ui->tableActorsBase->sortByColumn(3, Qt::AscendingOrder);
    ui->tableActorsBase->addColumn(tr("Gr") + strBeta, gr);
    ui->tableActorsBase->addColumn(tr("Gn") + strBeta, gn);
    ui->tableActorsBase->addColumn(tr("IQ") + strBeta, iq);
    if (ui->cbSingleIp->isChecked()) {
        ui->tableActorsBase->addColumn(tr("IP") + strBeta, ip, labels);
    }
    ui->tableActorsBase->sortByColumn(3, Qt::DescendingOrder);

    m_statusProgressBar->setEnabled(false);
    m_statusProgressBar->setValue(0);
    statusBar()->showMessage(tr("Состояние: ничего не выполняется"));
}

void MainWindow::on_rbGroupActorAuto_toggled(bool checked)
{
    ui->sbGroupSize->setEnabled(checked);
}

void MainWindow::on_btnComputeGroupIP_clicked()
{
    statusBar()->showMessage(tr("Состояние: вычисление характеристик группы"));

    double beta = ui->dsbGroupBeta->value();
    size_t it   = size_t(ui->sbGroupCountIt->value());
    double qr(0), qn(0), ip(0);

    Array<Actor>   group;
    Array<QString> names;

    if (ui->rbGroupActorChecked->isChecked()) {
        if (ui->tableActorsBase->getCheckedActors().size() < 3) {
            statusBar()->showMessage(tr("Состояние: ничего не выполняется"));
            return;
        }
        group = ui->tableActorsBase->getCheckedActors();
        names = ui->tableActorsBase->getCheckedNames();
    } else {
        size_t count = size_t(ui->sbGroupSize->value());
        group        = GroupActor::searchOptimalGroup(m_actors, beta, count);

        names.resize(group.size());
        for (size_t i = 0; i < group.size(); ++i) {
            for (size_t j = 0; j < m_actors.size(); ++j) {
                if (m_actors[j].id() == group[i].id()) {
                    names[i] = m_names[j];
                }
            }
        }

        if (group.size() == 0) {
            QMessageBox msgBox;
            msgBox.setText(tr("Сообщение."));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setInformativeText(tr("Не удалось найти группу для заданных параметров."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        }
    }

    if (group.size() > 0) {
        ip = GroupActor::groupIP(group, beta, qr, qn, it);
        ui->dsbGroupQr->setValue(qr);
        ui->dsbGroupQn->setValue(qn);
        ui->dsbGroupIP->setValue(ip);
    } else {
        ui->dsbGroupQr->setValue(0);
        ui->dsbGroupQn->setValue(0);
        ui->dsbGroupIP->setValue(0);
    }

    statusBar()->showMessage(tr("Состояние: ничего не выполняется"));

    ui->tabWidget->setCurrentIndex(1);

    draw(group, names);
}

void MainWindow::draw(const Array<Actor> &group, const Array<QString> &names)
{
    ui->plotter->clear();
    ui->tableGroup->clear();

    if (group.size() == 0) {
        return;
    }

    ui->tableGroup->add(group, names);

    statusBar()->showMessage(tr("Состояние: построение графиков"));
    m_statusProgressBar->setEnabled(true);
    m_statusProgressBar->setValue(0);

    QVector<double> beta, qr, qn, gr, gn, pr, pn;

    double min  = -12;
    double max  = 12;
    double step = 0.1;

    ui->plotter->setXRange(min, max);
    ui->plotter->setYRange(-0.025, 1.025);
    for (double b = min; b <= max + 0.5 * step; b += step) {
        beta.append(b);
    }

    qr.resize(beta.size());
    qn.resize(beta.size());
    gr.resize(beta.size());
    gn.resize(beta.size());
    pr.resize(beta.size());
    pn.resize(beta.size());

    size_t m = group.size();
    size_t k = m / 2;

    for (int i = 0; i < beta.size(); ++i) {
        double b = beta[i];

        gr[i] = 0;
        gn[i] = 0;
        for (auto act : group) {
            gr[i] += act.gr(b);
            gn[i] += act.gn(b);
        }
        gr[i] /= m;
        gn[i] /= m;

        Array<double> gpr, gpn;

        GroupActor::compute(group, b, qr[i], qn[i], gpr, gpn, size_t(ui->sbGroupCountIt->value()));
        pr[i] = gpr[k];
        pn[i] = gpn[k];

        m_statusProgressBar->setValue(int(double(100 * i) / beta.size()));
    }

    ui->plotter->add(beta, gn, "Gn(β)", QPen(Qt::yellow, 1.6));
    ui->plotter->add(beta, gr, "Gr(β)", QPen(Qt::magenta, 1.6));
    ui->plotter->add(beta, pn, "Pn(β)", QPen(Qt::darkGreen, 1.6));
    ui->plotter->add(beta, pr, "Pr(β)", QPen(Qt::blue, 1.6));
    ui->plotter->add(beta, qn, "Qn(β)", QPen(Qt::darkGray, 1.6));
    ui->plotter->add(beta, qr, "Qr(β)", QPen(Qt::red, 1.6));

    m_statusProgressBar->setEnabled(false);
    m_statusProgressBar->setValue(0);
    statusBar()->showMessage(tr("Состояние: ничего не выполняется"));
}

void MainWindow::on_btnUncheck_clicked()
{
    ui->tableActorsBase->uncheckAll();
    ui->tableGroup->uncheckAll();
}

void MainWindow::on_rbGroupActorChecked_toggled(bool checked)
{
    if (checked) {
        ui->tabWidget->setCurrentIndex(0);
    }
}
