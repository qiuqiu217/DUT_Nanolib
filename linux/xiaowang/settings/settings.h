#ifndef SETTINGS_H
#define SETTINGS_H

#include "app_window.h"

/*
*********************************************************************************************************
*                                               类声明
*********************************************************************************************************
*/
/**
* App_Settings_Create类
* 用于新建系统设置APP窗口的类
*/
class App_Settings_Create : public AppCreate
{
public:
    App_Settings_Create(Application_List& list, int index);
    int appWindow_Create(QWidget *parent, QSize screen_size, QString name) override;
};


#endif
