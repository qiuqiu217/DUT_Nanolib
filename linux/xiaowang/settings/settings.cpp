#include "settings.h"

/**
* @brief App_Settings_Create构造函数
* @param None
* @retval None
*/
App_Settings_Create::App_Settings_Create(Application_List& list, int index) {
    qDebug()<<"App_Settings_Create"<<endl;
    AppInfo settingsInfo;
    list.Copy_Listnode(index, settingsInfo);
    settingsInfo.objectptr = this;
    list.Update_Listnode(index, settingsInfo);
}

/**
* @brief App_Settings_Create构造函数
* @param None
* @retval None
*/
int App_Settings_Create::appWindow_Create(QWidget *parent, QSize screen_size, QString name) {
    /* 创建应用窗口 */
    AppWindow *window = new AppWindow(parent, screen_size, name);       //创建一个新的窗口
    window->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    window->show();     //显示窗口

    return RET_OK;

}
