#include "mainwindow.h"
#include "app_list.h"
#include "computer/computer.h"
#include "settings/settings.h"
#include "serial_port/serial_port.h"


/**
* @brief MainWindow构造函数
* @param QWidget *parent 父窗口
* @retval None
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug()<<"MainWindow creation"<<endl;
    Set_DesktopSize();      //设置桌面分辨率
    Set_DesktopBackground();        //设置桌面背景

    /* 桌面分为上中下三部分布局 */
    desktop_cwidget = new QWidget(this);       //创建桌面中心小部件
    setCentralWidget(desktop_cwidget);
    desktop_layout = new QVBoxLayout(desktop_cwidget);     //创建桌面布局，分为上中下三部分
    desktop_layout->setContentsMargins(0, 0, 0, 0);

    if(Init_AppList() == RET_ERROR) qDebug()<<"Failed to initialize the applist"<<endl;     //初始化应用列表
    if(Init_DesktopTopbar() == RET_ERROR) qDebug()<<"Failed to initialize the desktop topbar"<<endl;        //初始化桌面顶部信息栏
    if(Init_DesktopSlidebar() == RET_ERROR) qDebug()<<"Failed to initialize the desktop slidebar"<<endl;    //初始化桌面中部滑动窗口
    if(Init_DesktopButtombar() == RET_ERROR) qDebug()<<"Failed to initialize the desktop buttombar"<<endl;  //初始化桌面底部指示栏
}

/**
* @brief MainWindow析构函数
* @param None
* @retval None
*/
MainWindow::~MainWindow()
{
    qDebug()<<"MainWindow destruction"<<endl;
}

/**
* @brief MainWindow::Set_DesktopSize    设置桌面分辨率
* @param None
* @retval int   RET_ERROR 失败    RET_OK 成功
*/
int MainWindow::Set_DesktopSize()
{
    /* 获取屏幕的分辨率，Qt官方建议使用这
     * 种方法获取屏幕分辨率，防上多屏设备导致对应不上
     * 注意，这是获取整个桌面系统的分辨率
     */
    QList <QScreen *> list_screen =  QGuiApplication::screens();

        /* 如果是ARM平台，直接设置大小为屏幕的大小 */
    #if __arm__
        /* 重设大小 */
        this->resize(list_screen.at(0)->geometry().width(),
                     list_screen.at(0)->geometry().height());
        screen_size.setWidth(list_screen.at(0)->geometry().width());
        screen_size.setHeight(list_screen.at(0)->geometry().height());
    #else
        /* 否则则设置主窗体大小为1024x600 */
        this->resize(1024, 600);
        //this->resize(2048, 1200);
        screen_size.setWidth(this->width());
        screen_size.setHeight(this->height());
    #endif
        setFixedSize(screen_size.width(), screen_size.height());

    return RET_OK;
}

/**
* @brief MainWindow::Set_DesktopBackground    设置桌面背景
* @param None
* @retval int   RET_ERROR 失败    RET_OK 成功
*/
int MainWindow::Set_DesktopBackground()
{
    /* 设置背景图片路径 */
    path_background = ":/image/background/xiaowang.jpg";
    /* 加载背景图片 */
    pixmap_background.load(path_background);
    if (pixmap_background.isNull()) {
        qDebug()<<"Cannot found image"<<endl;       //图像加载失败，处理错误情况
        return RET_ERROR;
    }
    else {
        /* 图像加载成功，可以进行后续操作 */
        qDebug()<<"Found image"<<endl;
        pixmap_background = pixmap_background.scaled(this->size(), Qt::IgnoreAspectRatio);
        this->show();
        return RET_OK;
    }
}

/**
* @brief MainWindow::Set_DesktopTopbar    初始化桌面顶部信息栏
* @param None
* @retval int   RET_ERROR 失败    RET_OK 成功
*/
int MainWindow::Init_DesktopTopbar()
{
    /* 设置顶部信息栏控件 */
    desktop_twidget = new QWidget;
    //desktop_twidget->setStyleSheet("border: 1px solid red");
    desktop_layout->addWidget(desktop_twidget);
    desktop_layout->setStretchFactor(desktop_twidget, 2);       //设置每个布局所占的高度

    int fontSize = this->height() / 25;         //根据窗口宽度计算字体大小
    /* 设置系统日期及时间显示 */
    sys_time = new QLabel(desktop_twidget);     //创建一个新的QLabel对象来显示时间
    sys_time->setAlignment(Qt::AlignRight);
    sys_date = new QLabel(desktop_twidget);     //创建一个新的QLabel对象来显示日期
    sys_date->setAlignment(Qt::AlignRight);
    sys_time->setStyleSheet(QString("font: %1px Times New Roman").arg(fontSize));
    sys_date->setStyleSheet(QString("font: %1px Times New Roman").arg(fontSize));
    QTime time = QTime::currentTime();          //添加当前系统时间
    QString text = time.toString("hh:mm:ss");
    sys_time->setText(text);
    QDate date = QDate::currentDate();          //添加当前系统日期
    QString text1 = date.toString("yyyy-MM-dd");
    sys_date->setText(text1);

    /* 设置系统欢迎语 */
    sys_welcome = new QLabel(desktop_twidget);     //创建一个新的QLabel对象来显示欢迎语
    sys_welcome->setAlignment(Qt::AlignLeft);
    sys_welcome->setStyleSheet(QString("font: %1px Microsoft YaHei").arg(fontSize));
    sys_welcome->setText("欢迎使用智能小汪");

    /* 设置CPU温度 */
    sys_cputemp = new QLabel(desktop_twidget);     //创建一个新的QLabel对象来显示CPU温度
    sys_cputemp->setAlignment(Qt::AlignLeft);
    sys_cputemp->setStyleSheet(QString("font: %1px Microsoft YaHei").arg(fontSize));
    sys_cputemp->setText("CPU:" + getcpuTemp() + "℃");

    /* 将系统日期和时间添加到右上角布局 */
    QHBoxLayout *right_leftLayout = new QHBoxLayout;
    right_leftLayout->setDirection(QBoxLayout::RightToLeft);    //设置布局方向为从右到左
    right_leftLayout->setAlignment(Qt::AlignRight);             //设置布局的对齐方式为右对齐
    right_leftLayout->setSpacing(10);                           //设置部件之间的间距
    right_leftLayout->setContentsMargins(0, 0, 0, 0);           //设置布局的边距
    right_leftLayout->addWidget(sys_time);
    right_leftLayout->addWidget(sys_date);

    /* 将欢迎词语和CPU温度添加到左上角布局 */
    QHBoxLayout *left_rightLayout = new QHBoxLayout;
    left_rightLayout->setAlignment(Qt::AlignLeft);              //设置布局的对齐方式为左对齐
    left_rightLayout->setSpacing(10);                           //设置部件之间的间距
    left_rightLayout->setContentsMargins(0, 0, 0, 0);           //设置布局的边距
    left_rightLayout->addWidget(sys_welcome);
    left_rightLayout->addWidget(sys_cputemp);

    /* 设置顶部信息栏整体布局 */
    QHBoxLayout *sectionLayout = new QHBoxLayout(desktop_twidget);
    sectionLayout->addLayout(left_rightLayout);
    sectionLayout->addLayout(right_leftLayout);
    desktop_twidget->setLayout(sectionLayout);

    /* 开启系统时间更新定时器 */
    timer = new QTimer(this);           //创建一个新的 QTimer 对象，每秒更新一次时钟
    connect(timer, &QTimer::timeout, [this] {       //定时更新系统日期
        QTime time = QTime::currentTime();
        QString text = time.toString("hh:mm:ss");
        this->sys_time->setText(text);
    });
    connect(timer, &QTimer::timeout, [this] {       //定时更新系统时间
        QDate date = QDate::currentDate();
        QString text = date.toString("yyyy-MM-dd");
        this->sys_date->setText(text);
    });
    connect(timer, SIGNAL(timeout()), this, SLOT(updateCpuTemp()));     //定时更新Cpu温度
    timer->start(1000);

    return RET_OK;
}

/**
* @brief MainWindow::Init_DesktopSlidebar    初始化桌面中部滑动窗口
* @param None
* @retval int   RET_ERROR 失败    RET_OK 成功
*/
int MainWindow::Init_DesktopSlidebar()
{
    /* 创建桌面中间滑动窗口 */
    desktop_swidget = new QStackedWidget;       //创建一个 QStackedWidget 并将其添加到中间的布局中
    desktop_layout->addWidget(desktop_swidget);
    desktop_layout->setStretchFactor(desktop_swidget, 30);       //设置每个布局所占的高度
    for (int i = 0; i < 3; i++) {       //创建三个透明的Widget并将它们添加到 QStackedWidget 中
        desktop_pwidget[i] = new QWidget;
        desktop_pwidget[i]->setAttribute(Qt::WA_TranslucentBackground);
        desktop_swidget->addWidget(desktop_pwidget[i]);
        desktop_playout[i] = new QGridLayout(desktop_pwidget[i]);        //为desktop_pwidget设置一个布局
        desktop_playout[i]->setContentsMargins(0, this->height()/15, 0, 0);
        desktop_playout[i]->setVerticalSpacing(this->height()/40);
        desktop_playout[i]->setAlignment(Qt::AlignCenter);      //设置应用名称居中
    }
    /* 实现桌面中间滑动窗口动作 */
    slide_animation = new QPropertyAnimation(desktop_swidget, "currentIndex");
    slide_animation->setDuration(200);
    slide_animation->setEasingCurve(QEasingCurve::InOutQuad);
    Init_DesktopApp();

    return RET_OK;
}

/**
* @brief MainWindow::Init_DesktopButtombar    初始化桌面底部指示栏
* @param None
* @retval int   RET_ERROR 失败    RET_OK 成功
*/
int MainWindow::Init_DesktopButtombar()
{
    /* 添加底部页面指示部件 */
    buttom_cwidget = new B_indicationWidget(desktop_swidget);
    desktop_layout->addWidget(buttom_cwidget);
    desktop_layout->setStretchFactor(buttom_cwidget, 1);       //设置每个布局所占的高度
    connect(desktop_swidget, &QStackedWidget::currentChanged, buttom_cwidget, static_cast<void (B_indicationWidget::*)()>(&B_indicationWidget::update));
    return RET_OK;
}


/**
* @brief MainWindow::Init_Init_AppList    初始化应用列表
* @param None
* @retval int   RET_ERROR 失败    RET_OK 成功
*/
int MainWindow::Init_AppList() {

     //应用图标路径
    QString app_icon_paths[8] = {
        ":/image/icon/平台.png",
        ":/image/icon/配置.png",
        ":/image/icon/时间.png",
        ":/image/icon/日历.png",
        ":/image/icon/文件夹.png",
        ":/image/icon/图片.png",
        ":/image/icon/视频.png",
        ":/image/icon/串口.png",
    };

    // 应用名称
    QString app_names[8] = {
        "我的电脑",
        "设置",
        "时间",
        "日历",
        "文件夹",
        "图片",
        "视频",
        "串口"
    };

    AppInfo app;
    for (int i = 0; i < 8; i++) {
        app.name = app_names[i];
        app.icon_path = app_icon_paths[i];
        app_list.Append_inList(app);
    }
    App_Computer_Create* computerapp = nullptr;
    App_Settings_Create* settingsapp = nullptr;
    App_Serial_port_Create* serial_portsapp = nullptr;

    computerapp = new App_Computer_Create(app_list, app_list.Seek_inList(app_names[0]));
    settingsapp = new App_Settings_Create(app_list, app_list.Seek_inList(app_names[1]));
    serial_portsapp = new App_Serial_port_Create(app_list, app_list.Seek_inList(app_names[7]));

    return RET_OK;
}

/**
* @brief MainWindow::Init_DesktopApp    初始化桌面中应用图标
* @param None
* @retval int   RET_ERROR 失败    RET_OK 成功
*/
int MainWindow::Init_DesktopApp()
{

    /* 将应用列表中所有应用添加到桌面 */
    for (int i = 0; i < app_list.Return_Listsize(); i++) {
        int page = i / 18;      //计算应用图标应该排布的页、行、列
        int row = (i % 18) / 6;
        int col = (i % 18) % 6;
        QToolButton *app_icon = new QToolButton;        //设置按钮图标、大小及文本
        app_icon->setIcon(QPixmap(app_list.Return_Iconpath(i)).scaled(this->height()/8, this->height()/8, Qt::KeepAspectRatio));
        app_icon->setIconSize(QSize(this->height()/5, this->height()/5));
        app_icon->setFixedSize(this->height()/5, this->height()/5 + this->height()/40);
        app_icon->setText(app_list.Return_Name(i));
        QFont font = app_icon->font();
        font.setPixelSize(this->height()/25);
        font.setFamily("Microsoft YaHei");
        app_icon->setFont(font);
        app_icon->setAutoRaise(true);   //设置按钮透明
        //app_icon->setAutoFillBackground(true);
        app_icon->setFocusPolicy(Qt::NoFocus);
        app_icon->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  //设置文本位于图标正下方居中对
        app_icon->setStyleSheet("QToolButton:hover { background-color: blue; }");
        connect(app_icon, &QToolButton::clicked, this, &MainWindow::onAppIconClicked);
        desktop_playout[page]->addWidget(app_icon, row, col);
        app_list.Set_ToolButtonptr(i, app_icon);
    }

    for (int i = 0; i < 3; i++) {       // 在每一页添加垂直分隔符
        QSpacerItem *vertical_spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        desktop_playout[i]->addItem(vertical_spacer, 3, 0, 1, 6);
    }

    return RET_OK;
}


void MainWindow::onAppIconClicked() {
    //获取发送信号的按钮对象
    QToolButton *app_icon = qobject_cast<QToolButton *>(sender());

    //获取应用名称
    QString app_name = app_icon->text();
    qDebug()<<app_name<<endl;

    AppCreate* object = app_list.Return_Objectptr(app_list.Seek_inList(app_name));
    if(object != nullptr) {
        object->appWindow_Create(this, screen_size, app_name);
    }
    else {
        qDebug()<<"object is null"<<endl;
    }
}

/**
* @brief MainWindow::readCpuTemp    从系统中读取Cpu温度
* @param None
* @retval QString   从系统中读取的Cpu温度字符串
*/
QString MainWindow::readCpuTemp()
{
    char const *filename = "/sys/class/hwmon/hwmon0/temp1_input";

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        timer->stop();
        return "open file error!";
    }

    QTextStream in(&file);
        QString tempValue = in.readAll();
        QStringList list = tempValue.split("\n");
        return list[0];

}

/**
* @brief MainWindow::cpuTemp    获取Cpu温度
* @param None
* @retval QString   从系统中读取的Cpu温度字符串
*/
QString MainWindow::getcpuTemp()
{
    return cputemp;
}

/**
* @brief MainWindow::updateCpuTemp    更新Cpu温度
* @param None
* @retval None
*/
void MainWindow::updateCpuTemp()
{
    #if __arm__
        cputemp = readCpuTemp();
        double temp_data = cputemp.toDouble() / 1000;
        cputemp = QString::number(temp_data,'f',2);
        emit cpuTempChanged();
    #else
        int randNum = QRandomGenerator::global()->bounded(50, 81);
        cputemp = QString::number(randNum);
    #endif

    if(sys_cputemp != nullptr){
        sys_cputemp->setText("CPU:" + getcpuTemp() + "℃");
    }
}

/**
* @brief paintEvent  重绘事件
* @param QPaintEvent *event
* @retval None
*/
void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);

    if (pixmap_background.isNull()) {
        qDebug()<<"Cannot found image"<<endl;       //图像加载失败，处理错误情况
    }
    else {      // 在窗口中央绘制背景图片
        QPainter painter(this);
        int x = (this->width() - pixmap_background.width()) / 2;
        int y = (this->height() - pixmap_background.height()) / 2;
        painter.drawPixmap(x, y, pixmap_background);
    }
}

/**
* @brief mousePressEvent  鼠标点击事件
* @param QMouseEvent *event
* @retval None
*/
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    /* 记录鼠标按下的位置 */
    mouse_pressPos = event->pos();
}

/**
* @brief mouseMoveEvent  鼠标移动事件
* @param QMouseEvent *event
* @retval None
*/
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
}

/**
* @brief mouseReleaseEvent  鼠标释放事件
* @param QMouseEvent *event
* @retval None
*/
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    /* 计算鼠标滑动的距离和方向*/
    int deltaX = event->pos().x() - mouse_pressPos.x();
    int deltaY = event->pos().y() - mouse_pressPos.y();

    if (qAbs(deltaX) > qAbs(deltaY)) {  // 水平滑动
        if (deltaX > width() / 3) {     // 向右滑动，切换到上一页
            int currentIndex = desktop_swidget->currentIndex();
            int prevIndex = (currentIndex - 1 + desktop_swidget->count()) % desktop_swidget->count();
            slide_animation->setStartValue(currentIndex);
            slide_animation->setEndValue(prevIndex);
            slide_animation->start();
        }
        else if (deltaX < -width() / 3) {       // 向左滑动，切换到下一页
            int currentIndex = desktop_swidget->currentIndex();
            int nextIndex = (currentIndex + 1) % desktop_swidget->count();
            slide_animation->setStartValue(currentIndex);
            slide_animation->setEndValue(nextIndex);
            slide_animation->start();
        }
    }
}

/**
* @brief CircleWidget::paintEvent  底部页面指示部件重绘事件
* @param QPaintEvent *event
* @retval None
*/
void B_indicationWidget::paintEvent(QPaintEvent *event) {

    /* m_desktop_swidget不为空指针 */
    if (m_desktop_swidget == nullptr) {
        qDebug()<<"m_desktop_swidget is nullptr"<<endl;
        return;
    }
    /* 绘制底部页面指示图形 */
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int radius = qMin(width(), height()) / 3;
    for (int i = 0; i < 3; i++) {
        if (i == m_desktop_swidget->currentIndex()) {
            painter.setBrush(Qt::black);
        }
        else{
            painter.setBrush(Qt::lightGray);
        }
        painter.drawEllipse(QPoint(width()/2 + (i-1)*5*radius, height()/2), radius, radius);
    }
}
