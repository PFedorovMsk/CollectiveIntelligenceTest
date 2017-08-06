#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QStatusBar>

#include "core/Actor.h"
#include "core/ActorsBase.h"


namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_btnLoadFromFile_clicked();
    void on_btnGenerate_clicked();
    void on_btnComputeSingleIP_clicked();
    void on_rbGroupActorAuto_toggled(bool checked);
    void on_btnComputeGroupIP_clicked();
    void on_btnUncheck_clicked();

    void initStatusBar();
    void draw(const Array<Actor> &group, const Array<QString> &names);

    void on_rbGroupActorChecked_toggled(bool checked);

private:
    Ui::MainWindow *ui;

    QProgressBar *m_statusProgressBar;

    ActorsBase     m_actorsBase;
    Array<Actor>   m_actors;
    Array<QString> m_names;
};


#endif // MAINWINDOW_H
