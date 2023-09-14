#include "app_list.h"
#include "computer/computer.h"

/**
* @brief Application_List构造函数
* @param None
* @retval None
*/
Application_List::Application_List()
{

}


/**
* @brief Application_List::App_AppendinList    将该应用添加到应用列表末尾
* @param AppInfo app 应用信息
* @retval None
*/
void Application_List::Append_inList(AppInfo app) {
    app_list.append(app);
}

/**
* @brief Application_List::App_AddinList    将该应用添加到应用列表指定位置
* @param AppInfo app 应用信息     int index 需要添加的位置索引
* @retval None
*/
void Application_List::Add_inList(AppInfo app, int index) {
    app_list.insert(index, app);
}

/**
* @brief Application_List::App_RemovefromList    将指定的应用从应用列表移除
* @param int index 需要移除的应用位置索引
* @retval None
*/
void Application_List::Remove_fromList(int index) {
    app_list.removeAt(index);
}

/**
* @brief Application_List::App_RemovefromList    将指定的应用从应用列表移除
* @param QString app_name 应用名称
* @retval -1 应用不在列表中    int 应用在列表中的索引（不为负）
*/
int Application_List::Seek_inList(QString app_name) {
    int index = -1;
    for (int i = 0; i < app_list.size(); i++) {
        if (app_list[i].name == app_name) {
            index = i;
            break;
        }
    }
    return index;
}


/**
* @brief Application_List::Return_Listnode    返回应用列表指定结点
* @param int index 需要获取的应用位置索引
* @retval AppInfo 返回该索引处结点
*/
AppInfo Application_List::Return_Listnode(int index) {
    return app_list[index];
}


/**
* @brief Application_List::Copy_Listnode    复制应用列表指定结点
* @param int index 需要获取的应用位置索引
* @retval int RET_ERROR 失败    RET_OK 成功
*/
int Application_List::Copy_Listnode(int index, AppInfo &newInfo) {
    newInfo.name = app_list[index].name;
    newInfo.buttonptr = app_list[index].buttonptr;
    newInfo.icon_path = app_list[index].icon_path;
    newInfo.objectptr = app_list[index].objectptr;
    return RET_OK;
}

/**
* @brief Application_List::Update_Listnode   更新应用列表指定结点
* @param int index 需要获取的应用位置索引
* @retval None
*/
void Application_List::Update_Listnode(int index, const AppInfo &newInfo) {
    app_list[index] = newInfo;
}


/**
* @brief Application_List::App_ReturnListsize    返回应用列表大小
* @param int index 需要获取的应用位置索引
* @retval QString 应用名称
*/
int Application_List::Return_Listsize(void) {
    return app_list.size();
}

/**
* @brief Application_List::App_ReturnName    返回列表中指定索引应用名称
* @param int index 需要获取的应用位置索引
* @retval QString 应用名称
*/
QString Application_List::Return_Name(int index) {
    return app_list[index].name;
}

/**
* @brief Application_List::App_ReturnIconpath    返回列表中指定索引应用图标路径
* @param int index 需要获取的应用位置索引
* @retval QString 应用图标路径
*/
QString Application_List::Return_Iconpath(int index) {
    return app_list[index].icon_path;
}

/**
* @brief Application_List::Return_Objectptr    返回列表中指定索引应用窗口实例指针
* @param int index 需要获取的应用位置索引
* @retval AppCreate*    应用窗口实例指针
*/
AppCreate* Application_List::Return_Objectptr(int index) {
    return app_list[index].objectptr;
}

/**
* @brief Application_List::Set_ToolButtonptr    返回列表中指定索引应用图标控件指针
* @param int index 位置索引         QToolButton *ptr 图标控件指针
* @retval None
*/
void Application_List::Set_ToolButtonptr(int index, QToolButton *ptr) {
    app_list[index].buttonptr = ptr;
}

/**
* @brief Application_List::App_ReturnIconpath    返回列表中指定索引应用图标路径
* @param int index 需要获取的应用位置索引
* @retval QString 应用图标路径
*/
QToolButton* Application_List::Get_ToolButtonptr(int index) {
    return app_list[index].buttonptr;
}
