#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置默认大小
    this->resize(1800,1200);

    //初始化子界面
    blePage = new BlePage(this);
    plantarPage = new PlantarPage(this);
    imuPage = new ImuPage(this);
    trackPage = new TrackPage(this);
    //压入堆栈窗口
    ui->pageStack->addWidget(blePage);
    ui->pageStack->addWidget(plantarPage);
    ui->pageStack->addWidget(imuPage);
    ui->pageStack->addWidget(trackPage);
    //状态栏显示欢迎
    ui->statusbar->showMessage(tr("欢迎使用室内定位系统"));








    //设置信号槽
    connect(dataL->btDevice,&BluetoothHandler::display_message,this,&MainWindow::display_messageBox_callback);









}

MainWindow::~MainWindow()
{
    delete ui;
}


//工具栏的界面切换按钮
void MainWindow::on_toBleButton_triggered()
{
    ui->pageStack->setCurrentWidget(blePage);
}

void MainWindow::on_toPlantarButton_triggered()
{
    ui->pageStack->setCurrentWidget(plantarPage);
}

void MainWindow::on_toImuButton_triggered()
{
    ui->pageStack->setCurrentWidget(imuPage);
}

void MainWindow::on_toTrackButton_triggered()
{
    ui->pageStack->setCurrentWidget(trackPage);
}


//全局的信息显示

void MainWindow::on_statusChanged(const QString &text)
{
    ui->statusbar->showMessage(text);
}


void MainWindow::display_messageBox_callback(const int &level,const QString &title, const QString &text)
{
    if(level == QMessageBox::Question){
        QMessageBox::question(this,title,text);
    } else if(level == QMessageBox::Information){
        QMessageBox::information(this,title,text);
    } else if(level == QMessageBox::Warning){
        QMessageBox::warning(this,title,text);
    } else if(level == QMessageBox::Critical){
        QMessageBox::critical(this,title,text);
    } else{
        QMessageBox::critical(this,tr("错误"),"未知level数值");
    }
}





