#include "sensordata.h"
#include "blepage.h"


SensorData* DataL = new SensorData();
SensorData* DataR = new SensorData();

SensorData::SensorData(QObject *parent)
    : QObject{parent}
{

}




/**
 * @brief 数据改变回调
 */
void SensorData::on_dataSequence_changed(){
    qDebug()<<"onData changed!";
}


/**
 * @brief 打印所有数据
 */
void SensorData::PrintDataSequence(){
    for (auto singleFrame : dataSequence.toStdMap()) {
            qDebug()<<"timestamp sequence is:"<<singleFrame.first;
        }
}








