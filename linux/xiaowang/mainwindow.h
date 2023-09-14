#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
*********************************************************************************************************
*                                           Qt库文件
*********************************************************************************************************
*/
/* 基本头文件 */
#include <QMainWindow>      //MainWindow类
#include <QGuiApplication>      //GUI应用程序的控制流和主要设置
/* 控件相关 */
#include <QStackedWidget>
#include <QToolButton>
#include <QPushButton>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QLabel>
/* 对话框相关 */
#include <QInputDialog>
#include <QMessageBox>
/* 绘图功能相关 */
#include <QPixmap>      //图片处理
#include <QPainter>     //绘制形状、文本和图像
#include <QRect>        //矩形绘制
/* 布局视图相关 */
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QTreeView>
#include <QListView>
/* 动画相关 */
#include <QPropertyAnimation>
/* 事件相关 */
#include <QResizeEvent>
#include <QMouseEvent>
/* 文件夹相关 */
#include <QFileSystemModel>     //本地文件系统提数据模型
#include <QFileDialog>          //本地文件系统对话框
/* 定时器相关 */
#include <QTimer>
/* 其他功能相关 */
#include <QProcess>     //外部程序通信
#include <QScreen>      //屏幕配置
#include <QTime>        //时钟时间函数
#include <QRandomGenerator>     //随机数
#include <QTextCodec>           //文本编码转换
/* 调试打印 */
#include <QDebug>
/*
*********************************************************************************************************
*                                               头文件
*********************************************************************************************************
*/
#include "app_list.h"
#include "app_window.h"

/*
*********************************************************************************************************
*                                               宏定义
*********************************************************************************************************
*/
#define RET_OK      1
#define RET_ERROR   -1

/*
*********************************************************************************************************
*                                               类声明
*********************************************************************************************************
*/

/**
*  底部页面指示Widget部件类
*  当桌面的页面变更时，调用该类重绘函数绘制底部指示图标
*/
class B_indicationWidget : public QWidget {
    Q_OBJECT

public:
    B_indicationWidget(QStackedWidget *desktop_swidget, QWidget *parent = nullptr)
        : QWidget(parent), m_desktop_swidget(desktop_swidget) {
    }
    using QWidget::update;

protected:
    /*------------- 事件函数 -------------*/
    void paintEvent(QPaintEvent *event) override;       //重绘事件

private:
    /*------------- 变量声明 -------------*/
    QStackedWidget *m_desktop_swidget;
};



/**
* MainWindow类
* MainWindow作为桌面
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getcpuTemp(void);
    Q_INVOKABLE void updateCpuTemp(void);
    Q_PROPERTY(QString getcpuTemp READ getcpuTemp NOTIFY cpuTempChanged)

public slots:
    void onAppIconClicked();

protected:
    /*------------- 事件函数 -------------*/
    void paintEvent(QPaintEvent *event) override;       //重绘事件
    //void resizeEvent(QResizeEvent *event)  override;        //窗口大小变更事件
    void mousePressEvent(QMouseEvent *event) override;      //鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event) override;       //鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event) override;    //鼠标释放事件

private:
    /*------------- 变量声明 -------------*/
    /* 应用列表及应用实例指针 */
    Application_List app_list;      //应用列表
//    App_Computer_Create* computerapp = nullptr;
//    App_Settings_Create* settingsapp = nullptr;
    /* 桌面基本信息 */
    QSize screen_size;              //屏幕分辨率
    QPoint mouse_pressPos;          //鼠标点击坐标
    /* 桌面背景相关 */
    QPixmap pixmap_background;      //背景图片位图
    QString path_background;        //背景图片路径
    /* 桌面整体布局相关 */
    QWidget *desktop_cwidget = nullptr;         //桌面中心小部件
    QVBoxLayout *desktop_layout = nullptr;      //桌面布局
    /* 桌面顶部信息栏相关 */
    QWidget *desktop_twidget= nullptr;      //桌面顶部信息栏
    QLabel *sys_time = nullptr;             //系统时间显示
    QLabel *sys_date = nullptr;             //系统日期显示
    QLabel *sys_welcome = nullptr;          //系统欢迎语
    QLabel *sys_cputemp = nullptr;          //系统cpu温度
    QString cputemp;
    QTimer *timer = nullptr;
    /* 桌面滑动栏相关 */
    QStackedWidget *desktop_swidget = nullptr;          //滑动翻页Widget控件
    QWidget *desktop_pwidget[3];
    QGridLayout *desktop_playout[3];
    QPropertyAnimation *slide_animation = nullptr;      //滑动翻页动作
    /* 桌面底部指示栏相关 */
    B_indicationWidget *buttom_cwidget = nullptr;       //底部页面指示B_indicationWidget控件

    /*------------- 函数声明 -------------*/
    int Set_DesktopSize(void);
    int Set_DesktopBackground(void);
    int Init_DesktopTopbar(void);
    int Init_DesktopSlidebar(void);
    int Init_DesktopButtombar(void);
    int Init_AppList(void);
    int Init_DesktopApp(void);
    QString readCpuTemp(void);

signals:
    void cpuTempChanged();

};

#endif // MAINWINDOW_H
