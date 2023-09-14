#include "serial_port.h"

/**
* @brief App_Serial_port_Create 构造函数
* @param const QString &uiFileName ui文件名, QWidget *parent 父窗口, QSize screen_size 屏幕大小, QString name 应用名称
* @retval None
*/
App_Serial_port_window::App_Serial_port_window(const QString &uiFileName, QWidget *parent, QSize screen_size, QString name)
    : AppWindow(uiFileName, parent, screen_size, name) {

    serialPort = new QSerialPort(this);     //新建串口
    autoSendTimer = new QTimer(this);       //新建自动发送定时器
    /* 初始化串口调试助手界面 */
    interfacesInit();
    /* 扫描系统的串口 */
    scanSerialPort();
    /* 波特率项初始化 */
    baudRateItemInit();
    /* 数据位项初始化 */
    dataBitsItemInit();
    /* 检验位项初始化 */
    parityItemInit();
    /* 停止位项初始化 */
    stopBitsItemInit();
    /* 信号槽连接 */
    QPushButton *dataSendButton = this->getWidget<QPushButton>(tx_button[1]);       //发送按钮信号槽连接
    connect(dataSendButton, &QPushButton::clicked,
            this, &App_Serial_port_window::sendPushButtonClicked);

    QPushButton *serialOpenButton = this->getWidget<QPushButton>(serial_sButton);   //打开/关闭串口信号槽连接
    connect(serialOpenButton, &QPushButton::clicked,
            this, &App_Serial_port_window::openSerialPortPushButtonClicked);

    QPushButton *cntClearCutton = this->getWidget<QPushButton>(bWidow_button);      //收发计数清空信号槽连接
    connect(cntClearCutton, &QPushButton::clicked,
            this, &App_Serial_port_window::cntClearPushButtonClicked);

    QCheckBox *autoSendCheckBox = this->getWidget<QCheckBox>(tx_sCheckBox[1]);      //自动发送信号槽连接
    connect(autoSendCheckBox, &QCheckBox::stateChanged,
            this, &App_Serial_port_window::autoSendCheckBoxChanged);

    QComboBox *historyComboBox = this->getWidget<QComboBox>(sbWidow_comboBox);      //历史消息信号槽连接
    connect(historyComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &App_Serial_port_window::historyComboBoxIndexChanged);

    QPushButton *rxClearButton = this->getWidget<QPushButton>(rx_sButton[0]);       //清空接收区信号槽连接
    connect(rxClearButton, &QPushButton::clicked,
            this, &App_Serial_port_window::rxClearPushButtonClicked);

    QPushButton *rxSaveButton = this->getWidget<QPushButton>(rx_sButton[1]);        //保存接收区消息信号槽连接
    connect(rxSaveButton, &QPushButton::clicked,
            this, &App_Serial_port_window::rxSavePushButtonClicked);

    QPushButton *txLoadButton = this->getWidget<QPushButton>(sbWidow_button[0]);        //加载接收区消息信号槽连接
    connect(txLoadButton, &QPushButton::clicked,
            this, &App_Serial_port_window::txLoadPushButtonClicked);

    QRadioButton *txHexRadio = this->getWidget<QRadioButton>(tx_sRadio[1]);     //发送区格式信号槽连接
    connect(txHexRadio, &QRadioButton::toggled,
            this, &App_Serial_port_window::txRadioToggled);

    QTextEdit *TextEdit = this->getWidget<QTextEdit>(tx_textEdit);      //发送区文本改变信号槽连接
    connect(TextEdit, &QTextEdit::textChanged,
            this, &App_Serial_port_window::textEdit_textChanged);

    connect(autoSendTimer, &QTimer::timeout,       //自动发送定时器信号槽连接
            this, &App_Serial_port_window::sendPushButtonClicked);

    connect(serialPort, &QSerialPort::readyRead,        //串口数据接收信号槽连接
            this, &App_Serial_port_window::serialPortReadyRead);
}

/**
* @brief App_Serial_port_Create 析构函数
* @param None
* @retval None
*/
App_Serial_port_window::~App_Serial_port_window() {
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    delete serialPort;
}

/**
* @brief App_Serial_port_window::setFontAndSize 初始化应用界面
* @param QWidget* widget 控件指针   QSize cWindow_size 父窗口大小  int div_width 宽度除数   int div_height 高度除数
* @retval None
*/
void App_Serial_port_window::setFontAndSize(QWidget* widget, QSize cWindow_size, int div_width, int div_height) {
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(cWindow_size.height()/30);
    widget->setFont(font);
    widget->setFixedSize(cWindow_size.width()/div_width, cWindow_size.height()/div_height);
}

/**
* @brief App_Serial_port_window::interfacesInit 初始化应用界面
* @param None
* @retval None
*/
void App_Serial_port_window::interfacesInit() {

    cWindow_size = get_cWidgetptr()->size();
    /* 串口配置栏初始化 */
    QWidget* serial_swidow = this->getWidget<QWidget>("serial_swidget");
    if(serial_swidow != nullptr) {
        for(int i = 0; i < 7; i++) {
            QLabel *label = this->getWidget<QLabel>(serial_sLabel[i]);
            if(label != nullptr) {
                setFontAndSize(label, cWindow_size, (i == 0) ? 8 : 15, 28);
            }
        }
        for(int i = 0; i < 5;i++) {
            QComboBox *combobox = this->getWidget<QComboBox>(serial_sComboBox[i]);
            if(combobox != nullptr) {
                setFontAndSize(combobox, cWindow_size, 8, 25);
            }
        }
        QPushButton *button = this->getWidget<QPushButton>(serial_sButton);
        if(button != nullptr) {
            setFontAndSize(button, cWindow_size, 8, 25);
        }
        QFormLayout *layout = qobject_cast<QFormLayout *>(serial_swidow->layout());
        if(layout != nullptr) {
            layout->setVerticalSpacing(cWindow_size.height()/90);
            layout->setHorizontalSpacing(cWindow_size.width()/50);
            serial_swidow->setLayout(layout);
        }
    }
    /* 接收区配置栏初始化 */
    QWidget* rx_swidow = this->getWidget<QWidget>("rx_swidget");
    if(rx_swidow != nullptr) {
        QLabel *label = this->getWidget<QLabel>(rx_slabel);
        if(label != nullptr) {
            setFontAndSize(label, cWindow_size, 8, 28);
        }
        for(int i = 0; i < 2;i++) {
            QRadioButton *radio = this->getWidget<QRadioButton>(rx_sRadio[i]);
            if(radio != nullptr) {
                setFontAndSize(radio, cWindow_size, 8, 25);
                if(i == 0) radio->setChecked(true);
            }
        }
        for(int i = 0; i < 2;i++) {
            QPushButton *button = this->getWidget<QPushButton>(rx_sButton[i]);
            if(button != nullptr) {
                setFontAndSize(button, cWindow_size, 8, 25);
            }
        }
        QFormLayout *layout = qobject_cast<QFormLayout *>(rx_swidow->layout());
        if(layout != nullptr) {
            layout->setVerticalSpacing(cWindow_size.height()/90);
            layout->setHorizontalSpacing(cWindow_size.width()/50);
            rx_swidow->setLayout(layout);
        }
    }
    /* 发送区配置栏初始化 */
    QWidget* tx_swidow = this->getWidget<QWidget>("tx_swidget");
    if(tx_swidow != nullptr) {
        for(int i = 0; i < 3;i++) {
            QLabel *label = this->getWidget<QLabel>(tx_sLabel[i]);
            if(label != nullptr) {
                if(i == 0) setFontAndSize(label, cWindow_size, 8, 28);
                else if(i == 1) setFontAndSize(label, cWindow_size, 7, 28);
                else if(i == 2) setFontAndSize(label, cWindow_size, 30, 28);
            }
        }
        QLineEdit *lineEdit = this->getWidget<QLineEdit>(tx_sLineEdit);
        if(lineEdit != nullptr) {
            lineEdit->setValidator(new QIntValidator(lineEdit));
            lineEdit->setText("1000");
            setFontAndSize(lineEdit, cWindow_size, 12, 25);
        }
        for(int i = 0; i < 2;i++) {
            QRadioButton *radio = this->getWidget<QRadioButton>(tx_sRadio[i]);
            if(radio != nullptr) {
                setFontAndSize(radio, cWindow_size, 10, 25);
                if(i == 0) radio->setChecked(true);
            }
        }
        for(int i = 0; i < 2;i++) {
            QCheckBox *checkBox = this->getWidget<QCheckBox>(tx_sCheckBox[i]);
            if(checkBox != nullptr) {
               setFontAndSize(checkBox, cWindow_size, 8, 25);
            }
        }
        QFormLayout *layout = qobject_cast<QFormLayout *>(tx_swidow->layout());
        if(layout != nullptr) {
            layout->setVerticalSpacing(cWindow_size.height()/90);
            layout->setHorizontalSpacing(cWindow_size.width()/50);
            tx_swidow->setLayout(layout);
        }
    }
    /* 底部栏初始化 */
    QWidget* bwindow = this->getWidget<QWidget>("bottom_widget");
    if(bwindow != nullptr) {
        QFont font;        //设置底部栏字体
        font.setFamily("Microsoft YaHei");
        font.setPixelSize(cWindow_size.height()/35);
        for(int i = 0; i < 3;i++) {
            QLabel *label = this->getWidget<QLabel>(bWidow_label[i]);
            if(label != nullptr) {
                label->setFont(font);
                if(i == 1)  label->setText(QString("发送字节数: %1").arg(txBytesCount));
                if(i == 2)  label->setText(QString("接收字节数: %1").arg(rxBytesCount));
            }
        }
        QPushButton *button = this->getWidget<QPushButton>("clear_button");
        if(button != nullptr) {
            button->setFont(font);
            button->setFixedSize(cWindow_size.width()/10, cWindow_size.height()/30);
        }
    }
    /* 发送区设置栏初始化 */
    QWidget* txbwindow = this->getWidget<QWidget>("tx_sbwidget");
    if(txbwindow != nullptr) {
        QFont font;        //设置底部栏字体
        font.setFamily("Microsoft YaHei");
        font.setPixelSize(cWindow_size.height()/30);
        QComboBox *combobox = this->getWidget<QComboBox>(sbWidow_comboBox);
        if(combobox != nullptr) {
            combobox->setFont(font);
            combobox->setFixedSize(cWindow_size.width()*5/12, cWindow_size.height()/25);
        }
        for(int i = 0; i < 2;i++) {
            QPushButton *button = this->getWidget<QPushButton>(sbWidow_button[i]);
            if(button != nullptr) {
                button->setFont(font);
                button->setFixedSize(cWindow_size.width()/9, cWindow_size.height()/25);
                if(i == 1) button->setEnabled(false);
            }
        }
    }
    /* 串口数据接收显示区初始化 */
    QTextBrowser *textbrowser = this->getWidget<QTextBrowser>(rx_textBrowser);
    if(textbrowser != nullptr) {
        QFont font;        //设置底部栏字体
        font.setFamily("Microsoft YaHei");
        font.setPixelSize(cWindow_size.height()/30);
        textbrowser->setFont(font);
        textbrowser->setPlaceholderText("数据接收区");
    }
    /* 串口数据发送显示区初始化 */
    QTextEdit *textedit = this->getWidget<QTextEdit>(tx_textEdit);
    if(textedit != nullptr) {
        QFont font;        //设置底部栏字体
        font.setFamily("Microsoft YaHei");
        font.setPixelSize(cWindow_size.height()/30);
        textedit->setFont(font);
    }
}

/**
* @brief App_Serial_port_window::scanSerialPort 扫描串口
* @param None
* @retval None
*/
void App_Serial_port_window::scanSerialPort()
{
    /* 查找可用串口 */
    foreach (const QSerialPortInfo &info,
            QSerialPortInfo::availablePorts()) {
        QComboBox *combobox = this->getWidget<QComboBox>(serial_sComboBox[0]);
        combobox->addItem(info.portName());
    }
}

/**
* @brief App_Serial_port_window::baudRateItemInit 波特率选项设置
* @param None
* @retval None
*/
void App_Serial_port_window::baudRateItemInit()
{
    /* QList链表，字符串类型 */
    QList <QString> list;
    list<<"1200"<<"2400"<<"4800"<<"9600"
       <<"19200"<<"38400"<<"57600"
      <<"115200"<<"230400"<<"460800"
     <<"921600";
    QComboBox *combobox = this->getWidget<QComboBox>(serial_sComboBox[1]);
    for (int i = 0; i < 11; i++) {
        combobox->addItem(list[i]);
    }
    combobox->setCurrentIndex(7);
}

/**
* @brief App_Serial_port_window::dataBitsItemInit 数据位选项设置
* @param None
* @retval None
*/
void App_Serial_port_window::dataBitsItemInit()
{
    /* QList链表，字符串类型 */
    QList <QString> list;
    list<<"5"<<"6"<<"7"<<"8";
    QComboBox *combobox = this->getWidget<QComboBox>(serial_sComboBox[2]);
    for (int i = 0; i < 4; i++) {
        combobox->addItem(list[i]);
    }
    combobox->setCurrentIndex(3);
}

/**
* @brief App_Serial_port_window::parityItemInit 检验位选项设置
* @param None
* @retval None
*/
void App_Serial_port_window::parityItemInit()
{
    /* QList链表，字符串类型 */
    QList <QString> list;
    list<<"None"<<"Even"<<"Odd"<<"Space"<<"Mark";
    QComboBox *combobox = this->getWidget<QComboBox>(serial_sComboBox[3]);
    for (int i = 0; i < 5; i++) {
        combobox->addItem(list[i]);
    }
    combobox->setCurrentIndex(0);
}

/**
* @brief App_Serial_port_window::stopBitsItemInit 停止位选项设置
* @param None
* @retval None
*/
void App_Serial_port_window::stopBitsItemInit()
{
    /* QList链表，字符串类型 */
    QList <QString> list;
    list<<"1"<<"2";
    QComboBox *combobox = this->getWidget<QComboBox>(serial_sComboBox[4]);
    for (int i = 0; i < 2; i++) {
        combobox->addItem(list[i]);
    }
    combobox->setCurrentIndex(0);
}

/**
* @brief App_Serial_port_window::sendPushButtonClicked 数据发送槽函数
* @param None
* @retval None
*/
void App_Serial_port_window::sendPushButtonClicked()
{
    /* 获取textEdit数据,转换成GB18030格式的字节流 */
    QTextEdit *textedit = this->getWidget<QTextEdit>(tx_textEdit);
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    QRadioButton *txHexRadio = this->getWidget<QRadioButton>(tx_sRadio[1]);
    QByteArray data;
    if (txHexRadio->isChecked()) {
        //如果是十六进制模式，先将文本转化为hex
        QString text = textedit->toPlainText();
        data = QByteArray::fromHex(text.toLatin1());
    }
    else {
        //如果是ASCII模式，将文本转化为GB18030编码的字节流
        data = codec->fromUnicode(textedit->toPlainText());
    }
    QCheckBox *checkbox = this->getWidget<QCheckBox>(tx_sCheckBox[0]);
    if (checkbox->isChecked())  data = data + "\n";
    serialPort->write(data);
    /* 更新发送字节数统计 */
    txBytesCount += data.size();
    QLabel *label = this->getWidget<QLabel>(bWidow_label[1]);
    label->setText(QString("发送字节数: %1").arg(txBytesCount));
    /* 添加发送历史记录 */
    QComboBox *comboBox = this->getWidget<QComboBox>(sbWidow_comboBox);
    comboBox->blockSignals(true);       //阻止QComboBox发出信号
    if (data.size() <= 100) {
        comboBox->insertItem(0, data);
        if (comboBox->count() > 10) {
            comboBox->removeItem(comboBox->count() - 1);
        }
    }
    comboBox->blockSignals(false);      //恢复QComboBox的信号
}

/**
* @brief App_Serial_port_window::openSerialPortPushButtonClicked 打开/关闭串口槽函数
* @param None
* @retval None
*/
void App_Serial_port_window::openSerialPortPushButtonClicked()
{
    QPushButton *pushbutton = this->getWidget<QPushButton>(serial_sButton);
    QPushButton *pushbutton1 = this->getWidget<QPushButton>(sbWidow_button[1]);
    QComboBox *combobox[5];
    for (int i = 0; i < 5; i++) {
        combobox[i] = this->getWidget<QComboBox>(serial_sComboBox[i]);
    }
    if (pushbutton->text() == "打开串口") {     //打开串口操作
        /* 设置串口名 */
        serialPort->setPortName(combobox[0]->currentText());
        /* 设置波特率 */
        serialPort->setBaudRate(combobox[1]->currentText().toInt());
        /* 设置数据位数 */
        switch (combobox[2]->currentText().toInt()) {
        case 5:
            serialPort->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            serialPort->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            serialPort->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            serialPort->setDataBits(QSerialPort::Data8);
            break;
        default: break;
        }
        /* 设置停止位 */
        switch (combobox[3]->currentText().toInt()) {
        case 1:
            serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            serialPort->setStopBits(QSerialPort::TwoStop);
            break;
        default: break;
        }
        /* 设置奇偶校验 */
        switch (combobox[4]->currentIndex()) {
        case 0:
            serialPort->setParity(QSerialPort::NoParity);
            break;
        case 1:
            serialPort->setParity(QSerialPort::EvenParity);
            break;
        case 2:
            serialPort->setParity(QSerialPort::OddParity);
            break;
        case 3:
            serialPort->setParity(QSerialPort::SpaceParity);
            break;
        case 4:
            serialPort->setParity(QSerialPort::MarkParity);
            break;
        default: break;
        }
        /* 设置流控制 */
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        if (!serialPort->open(QIODevice::ReadWrite))
            QMessageBox::about(NULL, "错误",
                               "串口无法打开！可能串口已经被占用！");
        else {
            for (int i = 0; i < 5; i++) {
                combobox[i]->setEnabled(false);
            }
            pushbutton->setText("关闭串口");
            pushbutton1->setEnabled(true);
        }
   }
   else {       //关闭串口操作
        serialPort->close();
        for (int i = 0; i < 5; i++)
            combobox[i]->setEnabled(true);
        pushbutton->setText("打开串口");
        pushbutton1->setEnabled(false);
    }
}

/**
* @brief App_Serial_port_window::serialPortReadyRead 串口读取完毕槽函数
* @param None
* @retval None
*/
void App_Serial_port_window::serialPortReadyRead()
{
    /* 接收缓冲区中读取数据 */
    QTextBrowser *textbrowser = this->getWidget<QTextBrowser>(rx_textBrowser);
    QByteArray data = serialPort->readAll();        //新接收到的数据
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString str = codec->toUnicode(data);  // 使用QTextCodec将QByteArray转换为QString
    QRadioButton *radio = this->getWidget<QRadioButton>(rx_sRadio[0]);
    if(radio->isChecked()) {
        textbrowser->insertPlainText(str);  // 使用转换后的字符串
    }
    else {
        QByteArray encodedData = codec->fromUnicode(str);  // 使用QTextCodec将QString转换为QByteArray
        QString hexData = encodedData.toHex(' ');  // 将QByteArray转换为十六进制字符串
        textbrowser->insertPlainText(QString(hexData));
    }
    /* 更新接收字节数统计 */
    rxBytesCount += data.size();
    QLabel *label = this->getWidget<QLabel>(bWidow_label[2]);
    label->setText(QString("接收字节数: %1").arg(rxBytesCount));
}


/**
* @brief App_Serial_port_window::historyComboBoxCurrentIndexChanged 历史发送下拉框选中项改变槽函数
* @param int index 选中项索引
* @retval None
*/
void App_Serial_port_window::historyComboBoxIndexChanged(int index)
{
    if (index == -1) return;
    QComboBox *comboBox = this->getWidget<QComboBox>(sbWidow_comboBox);
    QString text = comboBox->itemText(index);
    QTextEdit *textedit = this->getWidget<QTextEdit>(tx_textEdit);
    textedit->setPlainText(text);
}

/**
* @brief App_Serial_port_window::cntClearPushButtonClicked 清空收发字节计数槽函数
* @param None
* @retval None
*/
void App_Serial_port_window::cntClearPushButtonClicked() {
    txBytesCount = 0;
    rxBytesCount = 0;
    QLabel *label = this->getWidget<QLabel>(bWidow_label[1]);
    label->setText(QString("发送字节数: %1").arg(txBytesCount));
    QLabel *label1 = this->getWidget<QLabel>(bWidow_label[2]);
    label1->setText(QString("接收字节数: %1").arg(rxBytesCount));
}

/**
* @brief App_Serial_port_window::rxClearPushButtonClicked 清空收发区槽函数
* @param None
* @retval None
*/
void App_Serial_port_window::rxClearPushButtonClicked() {
    QTextBrowser *textbrowser = this->getWidget<QTextBrowser>(rx_textBrowser);
    textbrowser->setText("");
}

/**
* @brief App_Serial_port_window::autoSendCheckBoxChanged 自动发送设置槽函数
* @param None
* @retval None
*/
void App_Serial_port_window::autoSendCheckBoxChanged() {
    QCheckBox *checkBox = this->getWidget<QCheckBox>(tx_sCheckBox[1]);
    if (checkBox->isChecked()) {
        QLineEdit *lineEdit = this->getWidget<QLineEdit>(tx_sLineEdit);
        if(autoSendTimer != nullptr) {
            autoSendTimer->start(lineEdit->text().toInt());
        }
    }
    else {
        autoSendTimer->stop();
    }
}

/**
* @brief App_Serial_port_window::txRadioToggled 发送区格式切换槽函数
* @param bool
* @retval None
*/
void App_Serial_port_window::txRadioToggled(bool)
{
    QRadioButton *txHexRadio = this->getWidget<QRadioButton>(tx_sRadio[1]);
    QTextEdit *textedit = this->getWidget<QTextEdit>(tx_textEdit);
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    //获取文本编辑器中的内容
    QString text = textedit->toPlainText();
    QString convertedText;
    if (txHexRadio->isChecked()) {
        //如果是十六进制模式，先将文本转化为hex
        QByteArray gbData = codec->fromUnicode(text);
        convertedText = gbData.toHex();
    }
    else {
        //如果是ASCII模式，将hex文本转化回ASCII
        QByteArray hexData = QByteArray::fromHex(text.toLatin1());
        convertedText = codec->toUnicode(hexData);
    }
    textedit->blockSignals(true);   //阻止发出textChanged()信号
    textedit->setText(convertedText);
    textedit->blockSignals(false);  //恢复发出textChanged()信号
}

/**
* @brief App_Serial_port_window::textEdit_textChanged 发送区内容变更槽函数
* @param None
* @retval None
*/
void App_Serial_port_window::textEdit_textChanged() {
    QRadioButton *txHexRadio = this->getWidget<QRadioButton>(tx_sRadio[1]);
    if (txHexRadio->isChecked()) {
        //如果是十六进制模式，检查并修改文本编辑器中的内容
        QTextEdit *textedit = this->getWidget<QTextEdit>(tx_textEdit);
        QString text = textedit->toPlainText();
        int cursorPosition = textedit->textCursor().position();  //保存光标位置
        QRegExp regExp("[0-9A-Fa-f\\s]*");  //匹配十六进制数和空格
        if (!regExp.exactMatch(text)) {
            //如果字符不是十六进制数或空格，删除它
            text = text.remove(QRegExp("[^0-9A-Fa-f\\s]"));
        }
        textedit->blockSignals(true);  //阻止发出textChanged()信号
        textedit->setText(text);
        QTextCursor cursor = textedit->textCursor();
        cursor.setPosition(cursorPosition);  //恢复光标位置
        textedit->setTextCursor(cursor);
        textedit->blockSignals(false);  //恢复发出textChanged()信号
    }
}

/**
* @brief App_Serial_port_window::rxSavePushButtonClicked 保存接收区数据槽函数
* @param None
* @retval None
*/
void App_Serial_port_window::rxSavePushButtonClicked()
{
    Save_FileDialog *selector = new Save_FileDialog(this->getWidget<QTextBrowser>(rx_textBrowser),
                                   this, QSize(600, 400), "文件夹");
    connect(selector, &Save_FileDialog::directorySelected, this, &App_Serial_port_window::handleDirectorySelected);
    selector->show();
    selector->move((this->frameGeometry().width() - selector->rect().width())/2,
                   (this->frameGeometry().height() - selector->rect().height())/2);
}

/**
* @brief App_Serial_port_window::txLoadPushButtonClicked 加载发送区数据槽函数
* @param None
* @retval None
*/
void App_Serial_port_window::txLoadPushButtonClicked() {
    Load_FileDialog *selector = new Load_FileDialog(this->getWidget<QTextEdit>(tx_textEdit),
                                   this, QSize(600, 400), "文件夹");
    connect(selector, &Load_FileDialog::fileSelected, this, &App_Serial_port_window::handleDirectorySelected);
    selector->show();
    selector->move((this->frameGeometry().width() - selector->rect().width())/2,
                   (this->frameGeometry().height() - selector->rect().height())/2);
}

/**
* @brief App_Serial_port_window::handleDirectorySelected 文件路径选择槽函数
* @param const QString &path 文件路径
* @retval None
*/
void App_Serial_port_window::handleDirectorySelected(const QString &path)
{
    qDebug() << "User selected directory:" << path;
}


/**
* @brief App_Serial_port_Create构造函数
* @param Application_List& list 应用列表索引, int index 插入的索引
* @retval None
*/
App_Serial_port_Create::App_Serial_port_Create(Application_List& list, int index) {

    qDebug()<<"App_Serial_port_Create"<<endl;
    AppInfo settingsInfo;
    list.Copy_Listnode(index, settingsInfo);
    settingsInfo.objectptr = this;
    list.Update_Listnode(index, settingsInfo);
}


/**
* @brief App_Serial_port_Create构造函数
* @param QWidget *parent 父窗口, QSize screen_size 窗口大小, QString name 应用名称
* @retval int   RET_ERROR 失败    RET_OK 成功
*/
int App_Serial_port_Create::appWindow_Create(QWidget *parent, QSize screen_size, QString name) {
    /* 创建应用窗口 */
    serial_appWindow = new App_Serial_port_window(":/serial_port/serial_port.ui", parent, screen_size, name);       //创建一个新的窗口
    serial_appWindow->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    serial_appWindow->show();     //显示窗口

    return RET_OK;
}



