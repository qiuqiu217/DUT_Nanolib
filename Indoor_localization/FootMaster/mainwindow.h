#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "blepage.h"
#include "imupage.h"
#include "trackpage.h"
#include "plantarpage.h"
#include <QMessageBox>





QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    BlePage *blePage;
    PlantarPage *plantarPage;
    ImuPage *imuPage;
    TrackPage *trackPage;


private slots:
    //界面切换按钮
    void on_toBleButton_triggered();
    void on_toPlantarButton_triggered();
    void on_toImuButton_triggered();
    void on_toTrackButton_triggered();

    //全局信息显示
    void on_statusChanged(const QString &text);
    void display_messageBox_callback(const int &level,const QString &title, const QString &text);
};



#endif // MAINWINDOW_H
