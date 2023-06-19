#ifndef SENSORDATA_H
#define SENSORDATA_H
#include "qobject.h"
#include "qobjectdefs.h"
#include <QMap>
#include <QVector>
#include <QDebug>

class SensorData: public QObject
{
    Q_OBJECT

public:
    explicit SensorData(QObject *parent = nullptr);
    //传感器数据结构体
    struct SingleFrame{
        QList<float> plantar;

        QMap<QString,float> acc;
        QMap<QString,float> angle;
        QMap<QString,float> gyro;
        QMap<QString,uint16_t> mag;
//        float quaternion[4];
//        int atoms;
//        int height;
        QMap<QString,float> test;
    };

    //传感器数据容器，使用qmap实现，既能快速搜寻其中一个，又能迅速插入删除
    QMap<int,SingleFrame> dataSequence;
    template<typename T>
    QVector<T> getMemberVector(T SingleFrame::*member) {
        QVector<T> memberVector;
        for (auto it = dataSequence.begin(); it != dataSequence.end(); ++it) {
            memberVector.push_back(it.value().*member);
        }
        return memberVector;
    }

    template<typename T>
    QVector<T> getSpecificValueVector(QMap<QString,T> SingleFrame::*member, const QString& key) {
        QVector<T> valueVector;
        for (auto it = dataSequence.begin(); it != dataSequence.end(); ++it) {
            if ((it.value().*member).contains(key)) {
                valueVector.push_back((it.value().*member)[key]);
            }
        }
        return valueVector;
    }

    void PrintDataSequence();
signals:
    void sensorDataChanged();






public slots:
    void on_dataSequence_changed();




};



extern SensorData *DataL;
extern SensorData *DataR;




#endif // SENSORDATA_H
