/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	版    本 : V1.0
*	说    明 : 本实验主要学习emWin的BMP图片显示，本程序代码提供了两种显示方法，用户可以宏定义进行切换
*              1. 直接加载到SDRAM进行显示选择：Method1
*              2. 边加载边显示的方式选择      :Method2
*              实际项目中强烈推荐方式一，即Method1，采用V7板子H7+32位SDRAM，LTDC颜色格式配置为RGB565.
*              绘制800*480*2的图片可以达到8ms到11ms一帧(开Cache，8ms)，基本能够胜任大部分嵌入式GUI项目。
*              
*	修改记录 :
*		版本号   日期         作者          说明
*		V1.0    2021-05-23   Eric2013  	    首版    
*                                     
*	Copyright (C), 2021-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __GUI_BMPdisplay_H
#define __GUI_BMPdisplay_H

#include "includes.h"
#include "GUI_Main.h"
#include "ff.h"


void _ShowBMPEx(const char *sFilename);
GUI_HMEM _ShowBMPAt(const char *sFilename, int x, int y);
uint8_t _DrawBMPasBackground(const char *sFilename, int x, int y, GUI_MEMDEV_Handle hMemBMP);

#endif

