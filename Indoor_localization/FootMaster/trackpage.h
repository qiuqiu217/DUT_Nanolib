#ifndef TRACKPAGE_H
#define TRACKPAGE_H

#include <QWidget>
#include "data/algorithm.h"
#include "bluetooth/dataprocess.h"
#include <QMessageBox>
#include <QProgressDialog>
#include <QList>
#include <QtMath>

namespace Ui {
class TrackPage;
}

class TrackPage : public QWidget
{
    Q_OBJECT

public:
    explicit TrackPage(QWidget *parent = nullptr);
    ~TrackPage();

    QString vehiclename;//轨迹名称







public slots:
    void refreshData(unsigned int timestamp,DataProcess::SingleFrameIMU frameData);

private slots:
    void on_collectDataButton_clicked();
    void on_stopCollectiontButton_clicked();
    void caliTimerTick();
    void collectCaliData(unsigned int timestamp ,DataProcess::SingleFrameIMU frameData);

private:

    //校准定时器相关
    const int caliTotalTime = 1000;//校准消耗的总时间
    const int caliTimerInterval = 20; //校准定时器间隔
    const int caliProgressFinishFlag = caliTotalTime/caliTimerInterval ;
    int m_caliProgressCounter;//进度条计数器
    //校准数据
    struct CaliFrame
    {
       double accX;
       double accY;
       double accZ;
    };
    QList<CaliFrame> ImuCaliSequence;// 校准期间获取的IMU数据
    //传感器数据结构体
    DataProcess::SingleFrameIMU CaliData_sum;
    DataProcess::SingleFrameIMU CaliData_average;
    DataProcess::SingleFrameIMU CaliData_variance;





    //全局组件
    Ui::TrackPage *ui;
//    Algorithm *algorithm;
    QTimer *collectTimer;
    QProgressDialog *caliProgressDialog;
    QTimer *calibrateTimer;


    //绘图变量
    int lastPlotSize;
    int currentPlotSize;
    unsigned int m_lastCalculateTimestamp;//上次计算时的时间戳

    //获取初始分布参数的方法
    void getOriginalDistribution();
    void startPlot();
    void progressDialogInit();
    void calculateDistributionParam();






};

#endif // TRACKPAGE_H
