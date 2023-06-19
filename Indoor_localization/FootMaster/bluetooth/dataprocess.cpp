#include "dataprocess.h"

//DataProcess *btDeviceL = new DataProcess("ESP32_Slave_Debug");
//BtHandler *btDeviceL = new BtHandler("ESP32test");
//BtHandler *btDeviceR = new BtHandler("ESP32jkjk");


#define IMU_DATA_PRINT 0
#define PLANTAR_DATA_PRINT 0

#if IMU_DATA_PRINT == 1u

#endif
#if PLANTAR_DATA_PRINT == 1u

#endif





DataProcess::DataProcess(QObject *parent)
    : QObject{parent},
      m_timestamp(0),
      m_timestamp_pervious_IMU(0),
      m_timestamp_pervious_Plantar(0),
      m_foundStartPattern(0),
      m_startIndex(0),
      m_endIndex(0)

{

    btDevice = new BluetoothHandler("ESP32_Slave_R");
    btDevice->start();


    connect(btDevice,&BluetoothHandler::data_appended,this,&DataProcess::data_received_callback);//接收到数据
    connect(this,&DataProcess::sensor_data_received, this,&DataProcess::data_extracted_callback);//解析到数据

    //将IMU每项数据回调汇总
    connect(this,&DataProcess::sensor_acc_writed,this,&DataProcess::sensor_IMU_writed_callback);
    connect(this,&DataProcess::sensor_gyro_writed,this,&DataProcess::sensor_IMU_writed_callback);
    connect(this,&DataProcess::sensor_angle_writed,this,&DataProcess::sensor_IMU_writed_callback);
    connect(this,&DataProcess::sensor_mag_writed,this,&DataProcess::sensor_IMU_writed_callback);
    connect(this,&DataProcess::sensor_atoms_writed,this,&DataProcess::sensor_IMU_writed_callback);
    connect(this,&DataProcess::sensor_height_writed,this,&DataProcess::sensor_IMU_writed_callback);
    connect(this,&DataProcess::sensor_quaternion_writed,this,&DataProcess::sensor_IMU_writed_callback);
    //IMU单帧数据完成
    connect(this,&DataProcess::imu_frame_ok,this,&DataProcess::imu_frame_ok_callback);

    //将Plantar每项数据更新回调汇总
    connect(this,&DataProcess::sensor_plantar_writed,this,&DataProcess::sensor_Plantar_writed_callback);
    //Plantar单帧完成回调
    connect(this,&DataProcess::plantar_frame_ok,this,&DataProcess::plantar_frame_ok_callback);
}






/*
*********************************************************************************************************
*	                                      蓝牙连接层相关函数
*********************************************************************************************************
*/


///**
// * @brief 新设备发现回调
// */
//void DataProcess::device_discovered_callback(const QBluetoothDeviceInfo &info){
//    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());
//    qDebug()<<label;
//    if(info.name()== m_targetDeviceName)
//    {
//        m_BTaddress = info.address().toString();//地址
//        m_discoveryAgent->stop();//结束扫描
//        emit m_discoveryAgent->finished();//发送扫描结束信号
//        m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);//创建管道
//        m_socket->connectToService(QBluetoothAddress(m_BTaddress), QBluetoothUuid(m_serviceUuid), QIODevice::ReadWrite);//连接设备


//        //蓝牙连接与传输回调
//        connect(m_socket,SIGNAL(connected()), this, SLOT(device_connected_callback()));//连接成功
//        connect(m_socket,SIGNAL(readyRead()),this,SLOT(serialBT_timerTimeout_callback()));//有新内容到达串口，开启定时器






//    }

//}




/**
 * @brief 读取完毕
 */
void DataProcess::data_received_callback(int length){

    int extractCounter = 0;//解析字符计数器
    int buffer_length = 0;//解析出的数据计数器
//    qDebug()<<"extract start,"<<",m_readQueue is:"<<btDevice->m_readQueue.size();
    unsigned int extractStartTime = btDevice->m_msTime;
    while (!btDevice->m_readQueue.isEmpty() && extractCounter < length) {
        extractCounter++;

        uint8_t c = btDevice->m_readQueue.dequeue();
        m_buffer.append(c);

        if (!m_foundStartPattern) {
            if (m_buffer.endsWith(startPattern)) {
                m_foundStartPattern = true;
                m_buffer.clear();
            }
        } else {
            if (m_buffer.endsWith(endPattern)) {
                m_buffer.chop(endPattern.size());
                buffer_length += m_buffer.length();//统计接收
                emit sensor_data_received(m_buffer);//发送数据接收信号
                m_buffer.clear();
                m_foundStartPattern = false;


            }
        }

    }
    unsigned int extractEndTime = btDevice->m_msTime;
//    qDebug()<<"extract succeed! length is:"<<buffer_length<<"count is:"<<extractCounter<<"start-end m_msTime is: "<<extractStartTime<<extractEndTime;
//    qDebug()<<"count is:"<<count<<"extract succeed! length is:"<<buffer_length<<" m_msTime is: "<<btDevice->m_msTime;


//    while ((m_startIndex = btDevice->m_readBuffer.indexOf(startPattern) )!= -1 &&
//           (m_endIndex = btDevice->m_readBuffer.indexOf(endPattern,m_startIndex + startPattern.size())) != -1)
//    {

//        QByteArray middleData = btDevice->m_readBuffer.mid(m_startIndex + startPattern.size(), m_endIndex - m_startIndex - startPattern.size());
//        unsigned int timestamp = DataCoversionUtils::u8Arry_u32(reinterpret_cast<uint8_t *>(middleData.mid(1,4).data()));//获取时间戳
//        qDebug()<<"start-end is:"<<m_startIndex<<m_endIndex<<"timestamp is:"<<timestamp;
//        //发送接收信号
//        emit sensor_data_received(middleData);//发送数据接收信号

//        btDevice->m_readBuffer.remove(m_startIndex, m_endIndex + endPattern.size() - m_startIndex);


//    }

}





   /*******【测试模块】**********/
   //    char data[10000];
   //       qint64 len = m_socket->read((char *)data, 1000);

   //       QByteArray qa2((char*)data,len);
   //       qDebug()<<qa2.toStdString().c_str();

//}


/**
 * @brief 收到到传感器数据的回调
 */
void DataProcess::data_extracted_callback(QByteArray data_received){

    /*获取指令类型*/
    uint8_t dataType  = data_received.left(1).at(0);
    /*获取时间戳*/
    m_timestamp = DataCoversionUtils::u8Arry_u32(reinterpret_cast<uint8_t *>(data_received.mid(1,4).data()));//获取时间戳
    /*根据数据类型解析数据*/
    //足底数据
    if(dataType == 0x40){
        for(int count = 0;count<45;count++){
            float plantarPoint = DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(5+count*4,4).data()),false);
            this->PlantarRowSequence[m_timestamp].append(plantarPoint);
        }
        emit sensor_plantar_writed( m_timestamp );// 发送足底数据写入信号
#if PLANTAR_DATA_PRINT == 1u
     qDebug()<<"timestamp is:"<<m_timestamp<<", plantar data is:"<<this->PlantarRowSequence[m_timestamp];
#endif



    }
    else if(dataType == 0x50){
        //加速度数据
        float accX =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(5,4).data()),false);
        float accY =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(9,4).data()),false);
        float accZ =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(13,4).data()),false);
        this->ImuRowSequence[m_timestamp].acc["X"] = accX;
        this->ImuRowSequence[m_timestamp].acc["Y"] = accY;
        this->ImuRowSequence[m_timestamp].acc["Z"] = accZ;
        emit sensor_acc_writed(m_timestamp);// 发送加速度写入信号


#if IMU_DATA_PRINT == 1u
        qDebug("timestamp is: %u, acc-X-Y-Z is: %f,%f,%f",
                        m_timestamp,
                        this->ImuRowSequence[m_timestamp].acc["X"],
                        this->ImuRowSequence[m_timestamp].acc["Y"],
                        this->ImuRowSequence[m_timestamp].acc["Z"]
                      );
#endif



    }
    else if(dataType == 0x51){
        //陀螺仪数据
        float gyroX =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(5,4).data()),false);
        float gyroY =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(9,4).data()),false);
        float gyroZ =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(13,4).data()),false);
        this->ImuRowSequence[m_timestamp].gyro["X"] = gyroX;
        this->ImuRowSequence[m_timestamp].gyro["Y"] = gyroY;
        this->ImuRowSequence[m_timestamp].gyro["Z"] = gyroZ;
        emit sensor_gyro_writed(m_timestamp);// 发送角速度写入信号
#if IMU_DATA_PRINT == 1u
        qDebug("timestamp is: %u, gyro-X-Y-Z is: %f,%f,%f",
                m_timestamp,
                this->ImuRowSequence[m_timestamp].gyro["X"],
                this->ImuRowSequence[m_timestamp].gyro["Y"],
                this->ImuRowSequence[m_timestamp].gyro["Z"]
              );
#endif

    }
    else if(dataType == 0x52){
        //角度数据
        float angleX =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(5,4).data()),false);
        float angleY =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(9,4).data()),false);
        float angleZ =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(13,4).data()),false);
        this->ImuRowSequence[m_timestamp].angle["X"] = angleX;
        this->ImuRowSequence[m_timestamp].angle["Y"] = angleY;
        this->ImuRowSequence[m_timestamp].angle["Z"] = angleZ;

        emit sensor_angle_writed(m_timestamp);// 发送角度写入信号
#if IMU_DATA_PRINT == 1u
        qDebug("timestamp is: %u, angle-X-Y-Z is: %f,%f,%f",
                m_timestamp,
                this->ImuRowSequence[m_timestamp].angle["X"],
                this->ImuRowSequence[m_timestamp].angle["Y"],
                this->ImuRowSequence[m_timestamp].angle["Z"]
              );
#endif

    }
    else if(dataType == 0x53){
        //磁力计数据
        float magX =  DataCoversionUtils::u8Arry_u16(reinterpret_cast<uint8_t *>(data_received.mid(5,2).data()));
        float magY =  DataCoversionUtils::u8Arry_u16(reinterpret_cast<uint8_t *>(data_received.mid(7,2).data()));
        float magZ =  DataCoversionUtils::u8Arry_u16(reinterpret_cast<uint8_t *>(data_received.mid(9,2).data()));
        this->ImuRowSequence[m_timestamp].mag["X"] = magX;
        this->ImuRowSequence[m_timestamp].mag["Y"] = magY;
        this->ImuRowSequence[m_timestamp].mag["Z"] = magZ;

        emit sensor_mag_writed(m_timestamp);// 发送磁力计写入信号
#if IMU_DATA_PRINT == 1u
        qDebug("timestamp is: %u, mag-X-Y-Z is: %d,%d,%d",
                m_timestamp,
                this->ImuRowSequence[m_timestamp].mag["X"],
                this->ImuRowSequence[m_timestamp].mag["Y"],
                this->ImuRowSequence[m_timestamp].mag["Z"]
              );
#endif

    }
    else if(dataType == 0x54){
        //气压计数据
        unsigned int atoms =  DataCoversionUtils::u8Arry_u32(reinterpret_cast<uint8_t *>(data_received.mid(5,4).data()));
        this->ImuRowSequence[m_timestamp].atoms = atoms;

        emit sensor_atoms_writed(m_timestamp);// 发送气压写入信号

#if IMU_DATA_PRINT == 1u
        qDebug("timestamp is: %u, atoms pressure is: %u",m_timestamp,this->ImuRowSequence[m_timestamp].atoms);
#endif
//
    }
    else if(dataType == 0x55){
        //高度数据
        unsigned int height =  DataCoversionUtils::u8Arry_u32(reinterpret_cast<uint8_t *>(data_received.mid(5,4).data()));
        this->ImuRowSequence[m_timestamp].height = height;

        emit sensor_height_writed(m_timestamp);// 发送高度写入信号
#if IMU_DATA_PRINT == 1u
    qDebug("timestamp is: %u, height is: %u",m_timestamp,this->ImuRowSequence[m_timestamp].height);
#endif
//
    }
    else if(dataType == 0x56){
        //四元数数据
        float quaternion_0 =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(5,4).data()),false);
        float quaternion_1 =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(9,4).data()),false);
        float quaternion_2 =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(13,4).data()),false);
        float quaternion_3 =  DataCoversionUtils::u8Arry_float(reinterpret_cast<uint8_t *>(data_received.mid(17,4).data()),false);
        this->ImuRowSequence[m_timestamp].quaternion["q0"] = quaternion_0;
        this->ImuRowSequence[m_timestamp].quaternion["q1"] = quaternion_1;
        this->ImuRowSequence[m_timestamp].quaternion["q2"] = quaternion_2;
        this->ImuRowSequence[m_timestamp].quaternion["q3"] = quaternion_3;

        emit sensor_quaternion_writed(m_timestamp);// 发送四元数写入信号

#if IMU_DATA_PRINT == 1u
        qDebug("timestamp is: %u, q0-q1-q2-q3 is: %f,%f,%f,%f",
                m_timestamp,
                this->ImuRowSequence[m_timestamp].quaternion["q0"],
                this->ImuRowSequence[m_timestamp].quaternion["q1"],
                this->ImuRowSequence[m_timestamp].quaternion["q2"],
                this->ImuRowSequence[m_timestamp].quaternion["q3"]
              );
#endif


    }

    else{
        qDebug()<<"Unkowned dataType is:"<<QString::number(dataType,16)<<"data extracted is: "<<data_received;
    }

}

///**
// * @brief 发送数据
// */
//void DataProcess::send_data(QString str){

//    QByteArray arrayData = str.toUtf8();
//    m_socket->write(arrayData);

//}

/*
*********************************************************************************************************
*	                                      数据相关操作函数
*********************************************************************************************************
*/

/**
 * @brief 有IMU传感器数据写入时的回调
 */
void DataProcess::sensor_IMU_writed_callback(unsigned int timestamp){
    if(timestamp == m_timestamp_pervious_IMU){
       //两次的时间戳相等，还在传输同一个frame
    }
    else
    {
        //时间戳发生更新，获取更新前的frame并判断其是否完整，发送信号
        SingleFrameIMU frameCurrent = ImuRowSequence[m_timestamp_pervious_IMU];
        if(frameCurrent.gyro.isEmpty()
            ||frameCurrent.acc.isEmpty()
            ||frameCurrent.mag.isEmpty()
            ||frameCurrent.angle.isEmpty()
            ||frameCurrent.quaternion.isEmpty()
            ||!frameCurrent.height
            ||!frameCurrent.atoms  )
        {//有数据为空

        }
        else
        {//所有数据都不为空，一帧IMU数据完成，开始进行处理

//           printSingleFrame(frameCurrent);
//             qDebug()<<"timestamp is:"<<timestamp<<",m_msTime is:"<<btDevice->m_msTime<<"，IMU单帧数据收集完成,两帧之间的时间差为："<<\
//                       timestamp-m_timestamp_pervious_IMU;
//                       (timestamp>m_timestamp_pervious_IMU?(timestamp-m_timestamp_pervious_IMU):m_timestamp_pervious_IMU-timestamp);
            //检测时间戳有没有问题，有问题纠正。
            if(timestamp - m_timestamp_pervious_IMU>30000){
                //错数异常，直接将上一时间戳加8添加进去。
                unsigned int processed_timestamp = m_timestamp_pervious_IMU+8;
                ImuProcessedSequence.insert(processed_timestamp,ImuRowSequence[m_timestamp_pervious_IMU]);

                emit imu_frame_ok(m_timestamp_pervious_IMU,ImuRowSequence[m_timestamp_pervious_IMU]);
            }
            else{
                //正常的数，直接添加进processedSequence
                ImuProcessedSequence.insert(m_timestamp_pervious_IMU,ImuRowSequence[m_timestamp_pervious_IMU]);
                emit imu_frame_ok(m_timestamp_pervious_IMU,ImuRowSequence[m_timestamp_pervious_IMU]);
            }




        }

        m_timestamp_pervious_IMU = timestamp;

    }
}

/**
 * @brief IMU单帧数据完成时的回调
 */
void DataProcess::imu_frame_ok_callback(unsigned int timestamp,SingleFrameIMU frameData)
{
//    qDebug()<<"Imu_frame_ok!timestamp is:"<<timestamp;
}


/**
 * @brief 有plantar数据写入时的回调
 */
void DataProcess::sensor_Plantar_writed_callback(unsigned int timestamp){
    if(timestamp == m_timestamp_pervious_Plantar){
       //两次的时间戳相等，还在传输同一个frame
    }
    else
    {
        //plantar 时间戳发生变化，一帧传输完成
        PlantarProcessedSequence.insert(timestamp,PlantarRowSequence[timestamp]);
//        qDebug()<<"timestamp is:"<<timestamp<<"，Plantar单帧数据收集完成,两帧之间的时间差为："<<(timestamp-m_timestamp_pervious_Plantar);
        emit plantar_frame_ok( m_timestamp_pervious_Plantar,PlantarRowSequence[timestamp]);
        m_timestamp_pervious_Plantar = timestamp;

    }
}

/**
 * @brief Plantar单帧完成的回调
 */
void DataProcess::plantar_frame_ok_callback(unsigned int timestamp, QList<float> frameData){
//     qDebug()<<"Plantar_frame_ok!,timestamp is:"<<timestamp;
}




/*
*********************************************************************************************************
*	                                      数据序列相关操作函数
*********************************************************************************************************
*/
void DataProcess::printDataSequence(QMap<unsigned int,SingleFrameIMU> dataSequenceMap){
    QMap<unsigned int,SingleFrameIMU>::iterator i;
    for (i = ImuRowSequence.begin(); i != ImuRowSequence.end(); ++i) {
        qDebug() << "Key:" << i.key();
        QMap<QString,float>::iterator j;
        for (j = i.value().acc.begin(); j != i.value().acc.end(); ++j) {
            qDebug() << "Acc Key:" << j.key() << "Value:" << j.value();
        }
        QMap<QString,float>::iterator k;
        for (k = i.value().angle.begin(); k != i.value().angle.end(); ++k) {
            qDebug() << "Angle Key:" << k.key() << "Value:" << k.value();
        }
        QMap<QString,float>::iterator l;
        for (l = i.value().gyro.begin(); l != i.value().gyro.end(); ++l) {
            qDebug() << "Gyro Key:" << l.key() << "Value:" << l.value();
        }
        QMap<QString,uint16_t>::iterator m;
        for (m = i.value().mag.begin(); m != i.value().mag.end(); ++m) {
            qDebug() << "Mag Key:" << m.key() << "Value:" << m.value();
        }
        QMap<QString,float>::iterator n;
        for (n = i.value().quaternion.begin(); n != i.value().quaternion.end(); ++n) {
            qDebug() << "Quaternion Key:" << n.key() << "Value:" << n.value();
        }

    }
    qDebug() << "Height Key:" << i.value().height;
     qDebug() << "Atoms Key:" << i.value().atoms;
}


void DataProcess::printSingleFrame(SingleFrameIMU frame){


        qDebug() << "atoms:" << frame.atoms;
        qDebug() << "height:" << frame.height;

        QMap<QString,float>::iterator j;
        for (j = frame.acc.begin(); j != frame.acc.end(); ++j) {
            qDebug() << j.key() << ":" << j.value();
        }
        QMap<QString,float>::iterator k;
        for (k = frame.angle.begin(); k != frame.angle.end(); ++k) {
            qDebug() << k.key() << ":" << k.value();
        }
        QMap<QString,float>::iterator l;
        for (l = frame.gyro.begin(); l != frame.gyro.end(); ++l) {
            qDebug() << l.key() << ":" << l.value();
        }
        QMap<QString,uint16_t>::iterator m;
        for (m = frame.mag.begin(); m != frame.mag.end(); ++m) {
            qDebug() << m.key() << ":" << m.value();
        }
        qDebug() << "quaternion:" << frame.quaternion;

}













