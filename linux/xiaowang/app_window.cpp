#include "mainwindow.h"

/**
* @brief MainWindow构造函数
* @param QWidget *parent 父窗口
* @retval None
*/
AppWindow::AppWindow(const QString &uiFileName, QWidget *parent, QSize screen_size, QString name)
    : QFrame(parent)
{
    qDebug()<<"AppWindow creation"<<endl;
    setFixedSize(screen_size.width(), screen_size.height());

    /* 桌面分为上中下三部分布局 */
    appwindow_layout = new QVBoxLayout(this);     //创建桌面布局，分为上中下三部分
    appwindow_layout->setContentsMargins(0, 0, 0, 0);
    appwindow_layout->setSpacing(0);
    /* 设置窗口顶部栏 */
    appwindow_twidget = new QWidget;
    appwindow_layout->addWidget(appwindow_twidget);
    appwindow_layout->setStretchFactor(appwindow_twidget, 1);       //设置每个布局所占的高度
    QString style = "QWidget {"
                    "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "stop:0 #373636, stop:1 #373636);"
                    "}";
    appwindow_twidget->setStyleSheet(style);
    /* 设置app名称 */
    QLabel* app_name = new QLabel(appwindow_twidget);     //创建一个新的QLabel对象来显示欢迎语
    app_name->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    QFont font = app_name->font();
    font.setFamily(FONT_FAMILY);
    font.setPixelSize(FONT_SIZE);
    app_name->setFont(font);
    QPalette palette = app_name->palette();
    palette.setColor(QPalette::WindowText, QColor("#dadada"));
    app_name->setPalette(palette);
    app_name->setText(name);
    /* 绘制最小化按钮 */
    QToolButton *minimizeButton = new QToolButton(this);
    minimizeButton->setIcon(QPixmap(":/image/chartlet/minimize.png").scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio));
    minimizeButton->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    minimizeButton->setFixedSize(ICON_SIZE, ICON_SIZE);
    minimizeButton->setAutoRaise(true); // 去掉按钮的边框
    minimizeButton->setFocusPolicy(Qt::NoFocus);
    //connect(minimizeButton, SIGNAL(clicked()), this, SLOT(showMinimized()));
    /* 创建关闭按钮 */
    QToolButton *closeButton = new QToolButton(this);
    closeButton->setIcon(QPixmap(":/image/chartlet/close.png").scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio));
    closeButton->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    closeButton->setFixedSize(ICON_SIZE, ICON_SIZE);
    closeButton->setAutoRaise(true);    //去掉按钮的边框
    closeButton->setFocusPolicy(Qt::NoFocus);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    /* 添加窗口名称 */
    QHBoxLayout *twidgetLayout = new QHBoxLayout;
    twidgetLayout->setAlignment(Qt::AlignVCenter);             //设置布局的对齐方式为右对齐
    twidgetLayout->addStretch(100);
    twidgetLayout->addWidget(app_name);
    twidgetLayout->addStretch(80);
    twidgetLayout->setContentsMargins(0, 0, 0, 0);
    /* 将按钮添加到布局中 */
    QHBoxLayout *right_leftLayout = new QHBoxLayout;
    right_leftLayout->setDirection(QBoxLayout::RightToLeft);    //设置布局方向为从右到左
    right_leftLayout->setAlignment(Qt::AlignRight);             //设置布局的对齐方式为右对齐
    right_leftLayout->setSpacing(20);                           //设置部件之间的间距
    right_leftLayout->setContentsMargins(10, 0, 10, 0);         //设置布局的边距
    right_leftLayout->addWidget(closeButton);
    right_leftLayout->addWidget(minimizeButton);
    twidgetLayout->addLayout(right_leftLayout);
    appwindow_twidget->setLayout(twidgetLayout);
    /* 创建桌面中间窗口 */
    appwindow_cwidget = new QWidget;
    QFile file(uiFileName);
    file.open(QFile::ReadOnly);
    QUiLoader loader;
    appwindow_cwidget = loader.load(&file, appwindow_cwidget);
    file.close();
    appwindow_layout->addWidget(appwindow_cwidget);
    appwindow_layout->setStretchFactor(appwindow_cwidget, 24);       //设置每个布局所占的高度

}


/**
* @brief MainWindow构造函数
* @param QWidget *parent 父窗口
* @retval None
*/
AppWindow::AppWindow(QWidget *parent, QSize screen_size, QString name)
    : QFrame(parent)
{
    qDebug()<<"AppWindow creation"<<endl;
    setFixedSize(screen_size.width(), screen_size.height());

    /* 桌面分为上中下三部分布局 */
    appwindow_layout = new QVBoxLayout(this);     //创建桌面布局，分为上中下三部分
    appwindow_layout->setContentsMargins(0, 0, 0, 0);
    appwindow_layout->setSpacing(0);
    /* 设置窗口顶部栏 */
    appwindow_twidget = new QWidget;
    appwindow_layout->addWidget(appwindow_twidget);
    appwindow_layout->setStretchFactor(appwindow_twidget, 1);       //设置每个布局所占的高度
    QString style = "QWidget {"
                    "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "stop:0 #373636, stop:1 #373636);"
                    "}";
    appwindow_twidget->setStyleSheet(style);
    /* 设置app名称 */
    QLabel* app_name = new QLabel(appwindow_twidget);     //创建一个新的QLabel对象来显示欢迎语
    app_name->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    QFont font = app_name->font();
    font.setFamily(FONT_FAMILY);
    font.setPixelSize(FONT_SIZE);
    app_name->setFont(font);
    QPalette palette = app_name->palette();
    palette.setColor(QPalette::WindowText, QColor("#dadada"));
    app_name->setPalette(palette);
    app_name->setText(name);
    /* 绘制最小化按钮 */
    QToolButton *minimizeButton = new QToolButton(this);
    minimizeButton->setIcon(QPixmap(":/image/chartlet/minimize.png").scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio));
    minimizeButton->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    minimizeButton->setFixedSize(ICON_SIZE, ICON_SIZE);
    minimizeButton->setAutoRaise(true); // 去掉按钮的边框
    minimizeButton->setFocusPolicy(Qt::NoFocus);
    //connect(minimizeButton, SIGNAL(clicked()), this, SLOT(showMinimized()));
    /* 创建关闭按钮 */
    QToolButton *closeButton = new QToolButton(this);
    closeButton->setIcon(QPixmap(":/image/chartlet/close.png").scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio));
    closeButton->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    closeButton->setFixedSize(ICON_SIZE, ICON_SIZE);
    closeButton->setAutoRaise(true);    //去掉按钮的边框
    closeButton->setFocusPolicy(Qt::NoFocus);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    /* 添加窗口名称 */
    QHBoxLayout *twidgetLayout = new QHBoxLayout;
    twidgetLayout->setAlignment(Qt::AlignVCenter);             //设置布局的对齐方式为右对齐
    twidgetLayout->addStretch(100);
    twidgetLayout->addWidget(app_name);
    twidgetLayout->addStretch(80);
    twidgetLayout->setContentsMargins(0, 0, 0, 0);
    /* 将按钮添加到布局中 */
    QHBoxLayout *right_leftLayout = new QHBoxLayout;
    right_leftLayout->setDirection(QBoxLayout::RightToLeft);    //设置布局方向为从右到左
    right_leftLayout->setAlignment(Qt::AlignRight);             //设置布局的对齐方式为右对齐
    right_leftLayout->setSpacing(20);                           //设置部件之间的间距
    right_leftLayout->setContentsMargins(10, 0, 10, 0);           //设置布局的边距
    right_leftLayout->addWidget(closeButton);
    right_leftLayout->addWidget(minimizeButton);
    twidgetLayout->addLayout(right_leftLayout);
    appwindow_twidget->setLayout(twidgetLayout);
    /* 创建桌面中间窗口 */
    appwindow_cwidget = new QWidget;
    appwindow_layout->addWidget(appwindow_cwidget);
    appwindow_layout->setStretchFactor(appwindow_cwidget, 28);       //设置每个布局所占的高度
}

/**
* @brief AppWindow析构函数
* @param None
* @retval None
*/
AppWindow::~AppWindow()
{
    qDebug()<<"AppWindow destruction"<<endl;
}


/**
* @brief AppWindow::get_cWidgetptr 获取中部窗口指针
* @param None
* @retval QWidget*  底部栏指针
*/
QWidget* AppWindow::get_cWidgetptr(){
    return appwindow_cwidget;
}
