#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMessageBox>
#include <QObject>

#include "app_window.h"
#include "filedialog/filedialog.h"
#include "ui_serial_port.h"


/*
*********************************************************************************************************
*                                               类声明
*********************************************************************************************************
*/

/**
* App_Serial_port_window类
* 串口调试助手APP窗口的类
*/
class App_Serial_port_window : public AppWindow
{
    Q_OBJECT
public:
    App_Serial_port_window(const QString &uiFileName, QWidget *parent, QSize screen_size, QString name);
    ~App_Serial_port_window();
    void interfacesInit();          //初始化串口调试助手界面
    void scanSerialPort();          //扫描系统可用串口
    void baudRateItemInit();        //波特率项初始化
    void dataBitsItemInit();        //数据位项初始化
    void parityItemInit();          //检验位项初始化
    void stopBitsItemInit();        //停止位项初始化

protected:

private:
    QSerialPort *serialPort; /* 串口对象 */
    QSize cWindow_size;
    QTimer* autoSendTimer;
    long int txBytesCount = 0, rxBytesCount = 0;

    void setFontAndSize(QWidget* widget, QSize cWindow_size, int div_width, int div_height);

    /* 串口配置栏控件名称 */
    const QString serial_sLabel[7] = {
        {"serial_label"},
        {"com_label"},
        {"baud_label"},
        {"databit_label"},
        {"stopbit_label"},
        {"checkbit_label"},
        {"motion_label"},
    };
    const QString serial_sComboBox[5] = {
        {"com_combobox"},
        {"baud_combobox"},
        {"databit_combobox"},
        {"stopbit_combobox"},
        {"checkbit_combobox"},
    };
    const QString serial_sButton = {"motion_button"};
    /* 接收区设置栏控件名称 */
    const QString rx_slabel = {"rx_label"};
    const QString rx_sRadio[2] = {
        {"rxascii_radio"},
        {"rxhex_radio"},
    };
    const QString rx_sButton[2] = {
        {"rxclear_button"},
        {"rxsave_button"},
    };
    /* 发送区设置栏控件名称 */
    const QString tx_sLabel[3] = {
        {"tx_label"},
        {"period_label"},
        {"ms_label"},
    };
    const QString tx_sLineEdit = {"period_lineEdit"};
    const QString tx_sRadio[2] = {
        {"txascii_radio"},
        {"txhex_radio"},
    };
    const QString tx_sCheckBox[2] = {
        {"newline_checkbox"},
        {"autosend_checkbox"},
    };
    /* 发送接收区控件名称 */
    const QString rx_textBrowser = {"rx_textbrowser"};
    const QString tx_textEdit = {"tx_textedit"};
    const QString tx_comboBox = {"txhistory_combobox"};
    const QString tx_button[2] = {
        {"txload_button"},
        {"txsend_button"},
    };
    /* 底部栏控件名称 */
    const QString bWidow_button= {"clear_button"};
    const QString bWidow_label[3]= {
        {"welcome_label"},
        {"txnum_label"},
        {"rxnum_label"},
    };
    /* 发送区底部栏控件名称 */
    const QString sbWidow_comboBox = {"txhistory_combobox"};
    const QString sbWidow_button[2]= {
        {"txload_button"},
        {"txsend_button"},
    };


private slots:
    void sendPushButtonClicked();
    void openSerialPortPushButtonClicked();
    void cntClearPushButtonClicked();
    void autoSendCheckBoxChanged();
    void rxClearPushButtonClicked();
    void txRadioToggled(bool);
    void textEdit_textChanged();
    void historyComboBoxIndexChanged(int index);
    void serialPortReadyRead();
    void rxSavePushButtonClicked();
    void txLoadPushButtonClicked();
    void handleDirectorySelected(const QString &path);
};

/**
* App_Serial_port_Create类
* 用于新建系统设置APP窗口的类
*/
class App_Serial_port_Create : public AppCreate
{

public:
    App_Serial_port_Create(Application_List& list, int index);
    int appWindow_Create(QWidget *parent, QSize screen_size, QString name) override;

private:
    AppWindow *serial_appWindow = nullptr;
};


#endif
