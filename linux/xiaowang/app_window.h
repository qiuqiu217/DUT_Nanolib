#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include <QUiLoader>
#include "mainwindow.h"


#define FONT_SIZE       20
#define FONT_FAMILY     "Microsoft YaHei"
#define ICON_SIZE       25

/*
*********************************************************************************************************
*                                               类声明
*********************************************************************************************************
*/

class AppWindow : public QFrame
{
    Q_OBJECT

public:
    explicit AppWindow(const QString &uiFileName, QWidget *parent, QSize screen_size, QString name);
    explicit AppWindow(QWidget *parent, QSize screen_size, QString name);
    ~AppWindow();
    QWidget* get_cWidgetptr(void);
    template<typename Widget_Type>
    Widget_Type* getWidget(const QString &name);

protected:
    QVBoxLayout *appwindow_layout = nullptr;      //app窗口布局
    QWidget *appwindow_twidget= nullptr;      //窗口顶部栏
    QWidget *appwindow_cwidget= nullptr;      //窗口中部栏

};

template<typename Widget_Type>
Widget_Type* AppWindow::getWidget(const QString &name) {
    return this->findChild<Widget_Type*>(name);
}


#endif // APP_WINDOW_H
