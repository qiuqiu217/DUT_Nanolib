#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <QObject>
#include <QBluetoothDeviceInfo>
#include <qbluetoothdevicediscoveryagent.h>
#include <QBluetoothSocket>
#include <QBluetoothUuid>
#include <QBluetoothAddress>
#include <QIODevice>
#include <QTimer>

#include "data/datacoversionutils.h"
#include "bluetooth/bluetoothhandler.h"




class DataProcess : public QObject
{
    Q_OBJECT
    QThread extractThread;
public:
    //传感器数据结构体
       struct SingleFrameIMU{

           QMap<QString,float> acc;
           QMap<QString,float> angle;
           QMap<QString,float> gyro;
           QMap<QString,uint16_t> mag;
           QMap<QString,float> quaternion;
           unsigned int atoms;
           unsigned int height;
       };

       QMap<unsigned int,SingleFrameIMU> ImuRowSequence;// 收到的imu原始序列
       QMap<unsigned int,QList<float>> PlantarRowSequence;// 收到的plantar原始序列

       QMap<unsigned int,SingleFrameIMU> ImuProcessedSequence;// 处理过的imu序列
       QMap<unsigned int,QList<float>> PlantarProcessedSequence;// 处理过的plantar序列



       BluetoothHandler *btDevice;




       //解析单值
       template<typename T>
       QVector<T> extractValues(const QMap<unsigned int, SingleFrameIMU>& dataSequence, const QString& member,int length = -1) {
           QVector<T> result;
           if(length == -1){
               length = dataSequence.size();
           }
           for (const auto& imu : dataSequence) {

               if (member == "height") {
                   result.append(imu.height);
               }
               if (member == "atoms") {
                   result.append(imu.atoms);
               }else if(member == "acc") {
                   result.append(imu.acc);
               } else if (member == "gyro") {
                   result.append(imu.gyro);
               } else if (member == "angle") {
                   result.append(imu.angle);
               } else if (member == "mag") {
                   result.append(imu.mag);
               }else if (member == "quaternion") {
                   result.append(imu.quaternion);
               }else{
                   qDebug()<<"错误调用了extractValues,访问了不存在的值";
                   break;
               }

           }
           return result;
       }

       //解析Map成员的值
       template<typename T>
       QVector<T> extractValues(const QMap<unsigned int, SingleFrameIMU>& dataSequence,  const QString& member,const QString& key) {
           QVector<T> result;
           for (const auto& imu : dataSequence) {
               if (member == "acc") {
                   result.append(imu.acc.value(key));
               } else if (member == "gyro") {
                   result.append(imu.gyro.value(key));
               } else if (member == "angle") {
                   result.append(imu.angle.value(key));
               } else if (member == "mag") {
                   result.append(imu.mag.value(key));
               }else if (member == "quaternion") {
                   result.append(imu.quaternion.value(key));
               }else{
                   qDebug()<<"错误调用了extractValues,访问了不存在的值";
                   break;
               }
           }
           return result;
       }
       //获取所有的key
       template<typename T>
       QVector<T> extractKeys(const QMap<unsigned int, SingleFrameIMU>& dataSequence) {
           QList<unsigned int> allKeysList = dataSequence.keys();
           QVector<T> allKeys;
           std::transform(allKeysList.begin(), allKeysList.end(), std::back_inserter(allKeys), [](unsigned int key) {
               return static_cast<T>(key);
           });
           return allKeys;
       }



       void printDataSequence(QMap<unsigned int,SingleFrameIMU> dataSequenceMap);
       void printSingleFrame(SingleFrameIMU frame);


    explicit DataProcess(QObject *parent = nullptr);
//    void send_data(QString str);




public slots:
    void data_received_callback(int length);


signals:

    // sensor数据接收相关信号
    void sensor_data_received(QByteArray data_received);//解析到帧头帧尾中间的数据
    //sensor数据写入Map信号
    void sensor_acc_writed(unsigned int timestamp);
    void sensor_gyro_writed(unsigned int timestamp);
    void sensor_mag_writed(unsigned int timestamp);
    void sensor_angle_writed(unsigned int timestamp);
    void sensor_atoms_writed(unsigned int timestamp);
    void sensor_height_writed(unsigned int timestamp);
    void sensor_quaternion_writed(unsigned int timestamp);

    void sensor_plantar_writed(unsigned int timestamp);
    //单帧接收完成
    void imu_frame_ok(unsigned int timestamp,SingleFrameIMU frameData);//imu单帧接收完成信号
    void plantar_frame_ok(unsigned int timestamp, QList<float> frameData);//plantar单帧接收完成信号








private:
//    //蓝牙连接相关
//    QString m_serviceUuid = "00001101-0000-1000-8000-00805F9B34FB";
//    QString m_targetDeviceName;//目标设备名称
//    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;//设备查找
//    QString m_BTaddress;//地址
//    QBluetoothSocket *m_socket;//套接字
//    QTimer *m_readTimer;//读取定时器

//    //数据传输协议开始与结束字符串
//    char startChar = 0x71;
//    char endChar = 0x68;
//    QByteArray start_flag_qByteArray = QByteArray(4,startChar);
//    QByteArray end_flag_qByteArray = QByteArray(4,endChar);

    //开始与结束字符串
    QByteArray startPattern = QByteArray::fromHex("7171717171");
    QByteArray endPattern = QByteArray::fromHex("6868686868");
    int m_startIndex;
    int m_endIndex;

    QByteArray m_buffer;









    //数据接收时间戳
    unsigned  int m_timestamp ;//时间戳
    unsigned int m_timestamp_pervious_IMU;//IMU的上次更新时间戳
    unsigned int m_timestamp_pervious_Plantar;//plantar的上次更新时间戳
    //发现字符串标志位
    bool m_foundStartPattern;


private slots:

//    //蓝牙层面的回调函数
//    void device_discovered_callback(const QBluetoothDeviceInfo &info);
//    void discovery_finished_callback();
//    void device_connected_callback();
    void data_extracted_callback(QByteArray data_received);//提取到传感器数据

//    void serialBT_timer_start();//有新内容到达，开启定时读取计时器
//    void serialBT_timerTimeout_callback();



    //数据更新层回调函数
    void sensor_IMU_writed_callback(unsigned int timestamp);
    void sensor_Plantar_writed_callback(unsigned int timestamp);

    //IMU单帧数据完成回调
    void imu_frame_ok_callback(unsigned int timestamp,SingleFrameIMU frameData);
    //Plantar单帧完成回调
    void plantar_frame_ok_callback(unsigned int timestamp, QList<float> frameData);

};

extern DataProcess *dataL;
//extern BtHandler *btDeviceR;






#endif // DATAPROCESS_H
