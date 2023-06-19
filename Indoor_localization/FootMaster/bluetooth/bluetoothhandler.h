#ifndef BLUETOOTHHANDLER_H
#define BLUETOOTHHANDLER_H

#include <QObject>
#include <QThread>
#include <QObject>
#include <QBluetoothDeviceInfo>
#include <qbluetoothdevicediscoveryagent.h>
#include <QBluetoothSocket>
#include <QBluetoothUuid>
#include <QBluetoothAddress>
#include <QIODevice>
#include <QQueue>
#include <iterator>
#include <QTimer>
#include <QMessageBox>



#include <QJsonObject> // { }
#include <QJsonArray> // [ ]
#include <QJsonDocument> // 解析Json
#include <QJsonValue> // int float double bool null { } [ ]
#include <QJsonParseError>





class BluetoothHandler : public QThread
{
    Q_OBJECT

public:
   explicit  BluetoothHandler(QString name,QObject *parent = 0);





    //蓝牙接收buffer
    QByteArray m_readBuffer = QByteArray();
    //蓝牙接收队列
    QQueue<uint8_t> m_readQueue;
    uint32_t m_msTime;





signals:
    void data_appended(int length);
    void display_message(const int &level,const QString &title, const QString &text);

public slots:
    void send_data(QString str);
    void send_data(QByteArray arrayData);


protected:
    void run() override;

private:
    //蓝牙连接相关
    QString m_serviceUuid = "00001101-0000-1000-8000-00805F9B34FB";
    QString m_targetDeviceName;//目标设备名称
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;//设备查找
    QString m_BTaddress;//地址
    QBluetoothSocket *m_socket;//套接字
    QTimer *m_readTimer;//读取定时器


private slots:
    /**********蓝牙层面的回调函数*************/
    //连接控制层
    void device_discovered_callback(const QBluetoothDeviceInfo &info);
    void discovery_finished_callback();
    void device_connected_callback();
    void device_disconnected_callback();
    //数据收发层
    void data_received_callback();


    void msTimeout_callback();

};


#endif // BLUETOOTHHANDLER_H
