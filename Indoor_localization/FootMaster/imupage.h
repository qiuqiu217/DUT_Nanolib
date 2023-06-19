#ifndef IMUPAGE_H
#define IMUPAGE_H

#include <QWidget>
#include "library/qcustomplot.h"
#include "library/mywaveform.h"
#include "library/plothandler.h"
#include "blepage.h"
#include "bluetooth/dataprocess.h"
#include "data/algorithm.h"

#include <QButtonGroup>
#include <data/algorithm.h>


namespace Ui {
class ImuPage;
}

class ImuPage : public QWidget
{
    Q_OBJECT

public:
    explicit ImuPage(QWidget *parent = nullptr);
    ~ImuPage();

private slots:
    void refreshData(unsigned int timestamp_current,DataProcess::SingleFrameIMU frameData);
    void dataType_changed_callback(QString currentText);
    void axisCheckBoxsChanged_callback(int currentIndex);

    void on_startCollectionButton_clicked();
    void on_stopCollectionButton_clicked();
    void on_exportDataButton_clicked();
    bool dataDisplayChanged_callback();

    void on_KalmanFilteredCheckBox_stateChanged(int arg1);

    //坐标系显示改变回调
    //void coordinate_display_changed_callback(int btn, bool checked);
    void ImuPage::coordinate_display_changed_callback(QAbstractButton *button, bool checked);

signals:
    void dataDisplayChanged();



private:
    Ui::ImuPage *ui;
    int lastPlotSize;
    int currentPlotSize;
    QButtonGroup *m_axisCheckboxGroup;
    MyWaveform *waveformPlot;
    //定义一个Qmap来实现显示名字和调用名字的解耦，后期可以更改显示名字。
    QMap<QString, QString> curveNames = {
                                         {"accX", "acc-X"}, {"accY", "acc-Y"}, {"accZ", "acc-Z"},
                                         {"magX", "mag-X"}, {"magY", "mag-Y"}, {"magZ", "mag-Z"},
                                         {"gyroX", "gyro-X"}, {"gyroY", "gyro-Y"}, {"gyroZ", "gyro-Z"},
                                         {"angleX", "angle-X"}, {"angleY", "angle-Y"}, {"angleZ", "angle-Z"},
                                         {"q0", "q0"}, {"q1", "q1"}, {"q2", "q2"}, {"q3", "q3"},
                                         {"height", "height"}, {"atoms", "atoms"},

                                        //卡尔曼滤波曲线
                                        {"accX_KF", "acc-X_KF"}, {"accY_KF", "acc-Y_KF"}, {"accZ_KF", "acc-Z_KF"},
                                        {"magX_KF", "mag-X_KF"}, {"magY_KF", "mag-Y_KF"}, {"magZ_KF", "mag-Z_KF"},
                                        {"gyroX_KF", "gyro-X_KF"}, {"gyroY_KF", "gyro-Y_KF"}, {"gyroZ_KF", "gyro-Z_KF"},
                                        {"angleX_KF", "angle-X_KF"}, {"angleY_KF", "angle-Y_KF"}, {"angleZ_KF", "angle-Z_KF"},
                                        {"q0_KF", "q0_KF"}, {"q1_KF", "q1_KF"}, {"q2_KF", "q2_KF"}, {"q3_KF", "q3_KF"},
                                        {"height_KF", "height_KF"}, {"atoms_KF", "atoms_KF"},

                                        //东北天坐标系下的
                                        {"accX_ENU", "acc-X_ENU"}, {"accY_ENU", "acc-Y_ENU"}, {"accZ_ENU", "acc-Z_ENU"},

                                        //速度与位移曲线
                                        {"velX","velX"},{"velY","velY"},{"velZ","velZ"},
                                        {"velX_KF","velX_KF"},{"velY_KF","velY_KF"},{"velZ_KF","velZ_KF"},
                                        {"posX","posX"},{"posY","posY"},{"posZ","posZ"},
                                        {"posX_KF","posX_KF"},{"posY_KF","posY_KF"},{"posZ_KF","posZ_KF"},


                                        };
    //checkBox列表
    QList<QCheckBox *> m_curveToShowCheckBoxGroup;
    QList<bool> m_curvesSelectCheckBoxList = {0,0,0,0};
    //当前绘图的dataType
    QString m_currentText;
    //当前需要显示的曲线名称列表
    QList<QString> m_curvesToShow;
    //算法
//    Algorithm *algorithm;

    //时间戳
    unsigned int m_lastCalculateTimestamp;

    //坐标系当前的坐标系
    enum {IMU,ENU} m_currentCoordinate;

    //设置复选框状态
    void setAxisCheckBoxDisplay(const QList<bool> &visibilitys,const QStringList &texts );
    //根据列表，和dataType，选择相应的数据类型进行添加
    void addTargetedDataSingleFrame(DataProcess::SingleFrameIMU frameData);
    void curvesInit();
    void setCurvesToShow();
    //数据选择框初始化
    void dataSelectionInit();
    //坐标系选择初始化
    void coordinateSelectionInit();




};

#endif // IMUPAGE_H
