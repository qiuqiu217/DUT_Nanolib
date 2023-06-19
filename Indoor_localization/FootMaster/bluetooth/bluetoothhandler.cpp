#include "bluetoothhandler.h"




BluetoothHandler::BluetoothHandler(QString name,QObject *parent)
    : QThread(parent),m_targetDeviceName(name),m_msTime(0)
{
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent;
    connect(m_discoveryAgent,&QBluetoothDeviceDiscoveryAgent::deviceDiscovered,this,&BluetoothHandler::device_discovered_callback);
    connect(m_discoveryAgent,&QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothHandler::discovery_finished_callback);
    m_discoveryAgent->start();

    QTimer *msTimer = new QTimer();//毫秒滴答计时器，用来获取时间戳
    msTimer->start(1);
    connect(msTimer,SIGNAL(timeout()),this,SLOT(msTimeout_callback()));
}



/*
*********************************************************************************************************
*	                                      蓝牙连接层相关函数
*********************************************************************************************************
*/


/**
 * @brief 新设备发现回调
 */
void BluetoothHandler::device_discovered_callback(const QBluetoothDeviceInfo &info){
    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());
    qDebug()<<label;
    if(info.name()== m_targetDeviceName)
    {
        m_BTaddress = info.address().toString();//地址
        m_discoveryAgent->stop();//结束扫描
        emit m_discoveryAgent->finished();//发送扫描结束信号
        m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);//创建管道
        m_socket->connectToService(QBluetoothAddress(m_BTaddress), QBluetoothUuid(m_serviceUuid), QIODevice::ReadWrite);//连接设备
//        m_readTimer = new QTimer();//创建读取内容的定时器

        //蓝牙连接与传输回调
        connect(m_socket,&QBluetoothSocket::connected, this, &BluetoothHandler::device_connected_callback);//连接成功
        connect(m_socket,&QBluetoothSocket::disconnected, this, &BluetoothHandler::device_disconnected_callback);//连接成功
        connect(m_socket,&QBluetoothSocket::readyRead,this,&BluetoothHandler::data_received_callback);//有新内容到达串口



    }

}



/**
 * @brief 扫描结束回调
 */
void BluetoothHandler::discovery_finished_callback(){

    qDebug()<<"Device discovered! Start to connect to "<<m_BTaddress;

}


/**
 * @brief 设备连接成功回调
 */
void BluetoothHandler::device_connected_callback(){
    qDebug()<<m_targetDeviceName<<"is connected";
//    send_data(QString("hello bluetooth"));

    //发送开始指令
    // 定义 { } 对象
//    QJsonObject TransferJsonObj;
//    QJsonObject PlantarJsonObj;
//    QJsonObject ImuJsonObj;
//    // 插入元素，对应键值对
//    TransferJsonObj.insert("Command", "Change the Bluetooth transfer state");
//    TransferJsonObj.insert("Transfer State", 1);
//    ImuJsonObj.insert("Command","Change the IMU sampling state");
//    ImuJsonObj.insert("Sampling State",1);
//    PlantarJsonObj.insert("Command","Change the plantar sampling state");
//    PlantarJsonObj.insert("Sampling State",1);

//    uint8_t time_delay_ms = 2;

//    //json文档对象
//    QJsonDocument doc;




//    doc.setObject(QJsonObject());
//    doc.setObject(ImuJsonObj);
//    send_data(doc.toJson(QJsonDocument::Compact));
//    qDebug()<<doc.toJson(QJsonDocument::Compact);
//    QThread::msleep(time_delay_ms);

//    doc.setObject(QJsonObject());
//    doc.setObject(PlantarJsonObj);
//    send_data(doc.toJson(QJsonDocument::Compact));
//    qDebug()<<doc.toJson(QJsonDocument::Compact);
//    QThread::msleep(time_delay_ms);



//    doc.setObject(QJsonObject());
//    doc.setObject(TransferJsonObj);
//    send_data(doc.toJson(QJsonDocument::Compact));
//    qDebug()<<doc.toJson(QJsonDocument::Compact);
//    QThread::msleep(time_delay_ms);
}



/**
 * @brief 断开连接回调
 */
void BluetoothHandler::device_disconnected_callback(){
    qDebug()<<"设备断开连接，重新开始扫描";
    emit display_message(QMessageBox::Critical,"错误","蓝牙连接断开，请检查设备，正在尝试重连");
    m_discoveryAgent->start();
}


/**
 * @brief 发送数据
 */
void BluetoothHandler::send_data(QString str){

    QByteArray arrayData = str.toUtf8();
    arrayData.append('\n');
    m_socket->write(arrayData);

}
/**
 * @brief 发送数据-字节数组
 */
void BluetoothHandler::send_data(QByteArray arrayData){
    arrayData.append('\n');
    m_socket->write(arrayData);
}



/**
 * @brief 收到数据回调
 */
void BluetoothHandler::data_received_callback(){

    m_readBuffer = m_socket->read(1000);//读取这段时间内接收的所有的字符
    std::copy( m_readBuffer.begin(),  m_readBuffer.end(), std::back_inserter(m_readQueue));
//    qDebug()<<"received data! length is:"<<m_readBuffer.length()<<" m_usTime is: "<<m_msTime;

    emit data_appended(m_readBuffer.length());
}


void BluetoothHandler::run()
{
    // 启动事件循环
     qDebug() << "void TestThread::run(): tid = " << currentThreadId();
}

void BluetoothHandler::msTimeout_callback(){
    m_msTime++;
}


