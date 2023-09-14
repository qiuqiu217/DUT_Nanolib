#ifndef APP_LIST_H
#define APP_LIST_H

#include <QMainWindow>
#include <QToolButton>
#include <QObject>
#include <QWidget>

class AppCreate;


struct AppInfo {
    QString name;       //应用名称
    QString icon_path;      //应用图标路径
    QToolButton *buttonptr = nullptr;       //应用图标按钮指针
    AppCreate *objectptr = nullptr;     //应用类指针
};


/**
* AppCreate类
* 用于新建APP窗口的类，作为所有新建APP窗口的类的基类
*/
class AppCreate : public QObject
{
public:
    virtual int appWindow_Create(QWidget *parent, QSize screen_size, QString name) = 0;
};


/**
* Application_List类
* QList<AppInfo> app_list 用于存放应用的列表
*/
class Application_List
{
public:
    Application_List();
    void Append_inList(AppInfo app);
    void Add_inList(AppInfo app, int index);
    void Remove_fromList(int index);
    int Seek_inList(QString app_name);
    AppInfo Return_Listnode(int index);
    int Copy_Listnode(int index, AppInfo &newInfo);
    void Update_Listnode(int index, const AppInfo &newInfo);
    int Return_Listsize(void);
    QString Return_Name(int index);
    QString Return_Iconpath(int index);
    AppCreate * Return_Objectptr(int index);
    void Set_ToolButtonptr(int index, QToolButton *ptr);
    QToolButton* Get_ToolButtonptr(int index);


private:
    QList<AppInfo> app_list;        //存放应用列表的链表结构
};

#endif // APP_LIST_H
