#ifndef COMPUTER_H
#define COMPUTER_H

#include "app_window.h"

/*
*********************************************************************************************************
*                                               类声明
*********************************************************************************************************
*/
/**
* App_Computer_Create类
* 用于新建我的电脑APP窗口的类
*/
class App_Computer_Create : public AppCreate
{
public:
    App_Computer_Create(Application_List& list, int index);
    int appWindow_Create(QWidget *parent, QSize screen_size, QString name) override;

private:
    const QString path_cpu = ":/image/chartlet/cpu.png";             //素材图片路径
    const QString path_memory = ":/image/chartlet/memory.png";
    const QString path_emmc = ":/image/chartlet/emmc.png";
    const QString path_ratio = ":/image/chartlet/ratio.png";
    const QString path_array[4] = {path_cpu, path_memory, path_emmc, path_ratio};
    QPixmap pixmap_cpu;      //素材图片位图
    QPixmap pixmap_memory;
    QPixmap pixmap_emmc;
    QPixmap pixmap_ratio;
    QPixmap pixmap_array[4] = {pixmap_cpu, pixmap_memory, pixmap_emmc, pixmap_ratio};
    const QString module_name[4] = {
            {"CPU"},
            {"MEMORY"},
            {"FLASH EMMC"},
            {"RESOLUTION"}
        };
    QString module_detail[4] = {
            {"STM32MP157D"},
            {"1.00GB"},
            {"8.00GB"},
            {}
        };
};


#endif
