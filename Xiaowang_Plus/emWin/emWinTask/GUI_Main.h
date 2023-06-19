#ifndef  GUI_Main_H
#define  GUI_Main_H

#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"
#include "GRAPH.h"
#include "MENU.h"
#include "MULTIPAGE.h"
#include "ICONVIEW.h"
#include "TREEVIEW.h"

/***********************************************************************
*                        GUI仿真器/硬件运行标志位
************************************************************************
*/
/* GUI运行环境 */
#define GUI_OPERATING                          1                           //运行环境0为模拟器，1为硬件运行

/* GUI语言 */
#define GUI_LANGUAGE                           GUI_OPERATING                           //GUI语言0为英文，1为中文

#if GUI_OPERATING > 0u
	/* 头文件引用 */
	#include "JPEGConf.h"
	#include "decode_dma.h"
	#include "GUI_BMPdisplay.h"
	#include "includes.h"
#else
	/* 仿真器数据类型定义 */
	/* exact-width signed integer types */
	typedef   signed          char int8_t;
	typedef   signed short     int int16_t;
	typedef   signed           int int32_t;

	/* exact-width unsigned integer types */
	typedef unsigned          char uint8_t;
	typedef unsigned short     int uint16_t;
	typedef unsigned           int uint32_t;
#endif
/***********************************************************************
*                        外部函数/变量声明
************************************************************************
*/
#if GUI_OPERATING > 0u
	/* 系统相关外部函数 */
	extern void TOUCH_Calibration(uint8_t _PointCount);
	extern void MountSD(void);
	extern void _WriteByte2File(U8 Data, void * p);
#endif

/* GUI相关外部函数 */
extern void MainTask(void);                      //GUI主任务
extern void Fun_Delete_DesktopTimer(void);       //删除桌面定时器停止桌面重绘
extern void Fun_Create_DesktopTimer(void);       //创建桌面定时器,开始桌面定期重绘
/* 所有未制作APP 都使用这个界面 */
extern WM_HWIN FUN_APP_Reserved_Create(void);    //预留APP窗口创建函数

#if GUI_OPERATING > 0u
	/* 文件系统相关全局变量 */
	extern FRESULT result;
	extern FIL file;
	extern FIL FileSave;   //专门用于图片的保存
	extern DIR DirInf;
	extern UINT bw;
	extern FATFS fs;
	extern char DiskPath[4]; /* SD卡逻辑驱动路径，比盘符0，就是"0:/" */
	extern const char * FR_Table[];
#endif

/* 系统中文字库 */
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ32;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ12;

/* 桌面图标第一页位图声明 */
extern GUI_CONST_STORAGE GUI_BITMAP bmmycomputer;
extern GUI_CONST_STORAGE GUI_BITMAP bmsetting;
extern GUI_CONST_STORAGE GUI_BITMAP bmfolder;
extern GUI_CONST_STORAGE GUI_BITMAP bmpicture;
extern GUI_CONST_STORAGE GUI_BITMAP bmvideo;
extern GUI_CONST_STORAGE GUI_BITMAP bmcamera;
extern GUI_CONST_STORAGE GUI_BITMAP bmnews;
extern GUI_CONST_STORAGE GUI_BITMAP bmclock;
extern GUI_CONST_STORAGE GUI_BITMAP bmcalendar;
extern GUI_CONST_STORAGE GUI_BITMAP bmmusic;
extern GUI_CONST_STORAGE GUI_BITMAP bmrecorder;
extern GUI_CONST_STORAGE GUI_BITMAP bmradio;
/* 桌面图标第二页位图声明 */
extern GUI_CONST_STORAGE GUI_BITMAP bmcodedlock;
extern GUI_CONST_STORAGE GUI_BITMAP bmcontrol;
extern GUI_CONST_STORAGE GUI_BITMAP bmdrawingboard;
extern GUI_CONST_STORAGE GUI_BITMAP bmusb;
extern GUI_CONST_STORAGE GUI_BITMAP bmgame;
extern GUI_CONST_STORAGE GUI_BITMAP bmgps;
extern GUI_CONST_STORAGE GUI_BITMAP bmlamplight;
extern GUI_CONST_STORAGE GUI_BITMAP bmoscilloscope;
/* 桌面图标第三页位图声明 */


/***********************************************************************
*                        系统标志位定义
************************************************************************
*/
/***********************************************************************
*            控件ID定义  (控件ID = 页面基本ID + 控件偏移ID)
************************************************************************
*/
/*---------------------------------------------------------------------------*/
/*---------------------------  页面基本ID地址   -----------------------------*/      
/*---------------------------------------------------------------------------*/
#define DESKTOP_BASE_ID             0x0000                             //桌面基本ID
#define KEYPAD_BASE_ID              0x0100                             //数字软键盘基本ID
#define APP_Camera_BASE_ID          0x0200                             //照相机APP页面基本ID
#define APP_Clock_BASE_ID           0x0300                             //电子时钟APP页面基本ID
#define APP_Calendar_BASE_ID        0x0400                             //万年历APP页面基本ID
#define APP_Music_BASE_ID           0x0500                             //音乐播放APP页面基本ID
#define APP_Settings_BASE_ID        0x0600                             //系统设置APP页面基本ID
#define APP_Picture_BASE_ID         0x0700                             //图片浏览APP页面基本ID
#define APP_DrawingBoard_BASE_ID    0x0b00                             //绘画板APP页面基本ID
#define APP_Oscilloscope_BASE_ID    0x0c00                             //示波器APP页面基本ID
#define APP_Reserved_BASE_ID        0x0f00                             //预留APP页面基本ID

/*---------------------------------------------------------------------------*/
/*-----------------------------  桌面控件   ---------------------------------*/
/*---------------------------------------------------------------------------*/
#define ID_DESKTOP_ICONVIEW      (GUI_ID_USER + DESKTOP_BASE_ID + 0x00)               //桌面图标
#define ID_DESKTOP_System_TEXT   (GUI_ID_USER + DESKTOP_BASE_ID + 0x02)               //系统日期
#define ID_DESKTOP_Date_TEXT     (GUI_ID_USER + DESKTOP_BASE_ID + 0x03)               //系统日期
#define ID_DESKTOP_Time_TEXT     (GUI_ID_USER + DESKTOP_BASE_ID + 0x04)               //系统时间

/*---------------------------------------------------------------------------*/
/*------------------------------  数字软键盘  -------------------------------*/
/*---------------------------------------------------------------------------*/
/* 数字键盘界面 */
#define ID_KEYPAD_FRAMEWIN       (GUI_ID_USER + KEYPAD_BASE_ID + 0x00)
#define ID_KEYPAD_BUTTON0        (GUI_ID_USER + KEYPAD_BASE_ID + 0x01)
#define ID_KEYPAD_BUTTON1        (GUI_ID_USER + KEYPAD_BASE_ID + 0x02)
#define ID_KEYPAD_BUTTON2        (GUI_ID_USER + KEYPAD_BASE_ID + 0x03)
#define ID_KEYPAD_BUTTON3        (GUI_ID_USER + KEYPAD_BASE_ID + 0x04)
#define ID_KEYPAD_BUTTON4        (GUI_ID_USER + KEYPAD_BASE_ID + 0x05)
#define ID_KEYPAD_BUTTON5        (GUI_ID_USER + KEYPAD_BASE_ID + 0x06)
#define ID_KEYPAD_BUTTON6        (GUI_ID_USER + KEYPAD_BASE_ID + 0x07)
#define ID_KEYPAD_BUTTON7        (GUI_ID_USER + KEYPAD_BASE_ID + 0x08)
#define ID_KEYPAD_BUTTON8        (GUI_ID_USER + KEYPAD_BASE_ID + 0x09)
#define ID_KEYPAD_BUTTON9        (GUI_ID_USER + KEYPAD_BASE_ID + 0x0A)
#define ID_KEYPAD_BUTTON10       (GUI_ID_USER + KEYPAD_BASE_ID + 0x0B)
#define ID_KEYPAD_BUTTON11       (GUI_ID_USER + KEYPAD_BASE_ID + 0x0C)
#define ID_KEYPAD_BUTTON12       (GUI_ID_USER + KEYPAD_BASE_ID + 0x0D)
#define ID_KEYPAD_BUTTON13       (GUI_ID_USER + KEYPAD_BASE_ID + 0x0E)

/************************************************************************************************************************
*
*       GUI页面默认选型
*
*************************************************************************************************************************
*/
/*---------------------------------------------------------------------------*/
/*--------------------------  新建APP默认设置   ----------------------------*/
/*---------------------------------------------------------------------------*/
#define NEW_APP_COORDINATE_X                   0                          //新建窗口起始坐标
#define NEW_APP_COORDINATE_Y                   0
#define NEW_APP_WIDTH                          800                        //新建窗口长宽
#define NEW_APP_HIGHT                          480
#define NEW_APP_BAR_HIGHT                      40                         //新建窗口标题栏高度
#define NEW_APP_BAR_COLOR_ENALBE               GUI_BLUE                   //新建窗口标题栏颜色
#define NEW_APP_BAR_COLOR_UNENALBE             GUI_LIGHTGRAY              //新建窗口标题栏颜色（未激活）
#define NEW_APP_BAR_ALIGN                      GUI_TA_HCENTER|GUI_TA_VCENTER             //新建窗口标题栏对齐方式
#define NEW_APP_CLOSEBUTTON                    1                          //窗口关闭按钮
#define NEW_APP_ACTIVE                         1                          //窗口默认激活

#if GUI_LANGUAGE > 0u
	#define NEW_APP_BAR_FONT                       &GUI_FontHZ24              //新建窗口字体（中文）
#else
	#define NEW_APP_BAR_FONT                       GUI_FONT_24B_1             //新建窗口字体
#endif

/*---------------------------------------------------------------------------*/
/*------------------------  新建APP是否执行默认选项  ------------------------*/
/*---------------------------------------------------------------------------*/
#define APP_Clock_WINDOW_DEFAULT                     1                           //电子时钟app窗口是否按默认格式新建
#define APP_Camera_WINDOW_DEFAULT               	 1                           //照相机app窗口是否按默认格式新建
#define APP_Calender_WINDOW_DEFAULT                  1                           //万年历app窗口是否按默认格式新建
#define APP_Music_WINDOW_DEFAULT                     1                           //音乐播放app窗口是否按默认格式新建
#define APP_Settings_WINDOW_DEFAULT                  1                           //系统设置app窗口是否按默认格式新建
#define APP_Picture_WINDOW_DEFAULT                   1                           //图片浏览app窗口是否按默认格式新建
#define APP_DrawingBoard_WINDOW_DEFAULT              1                           //绘画板app窗口是否按默认格式新建
#define APP_Oscilloscope_WINDOW_DEFAULT              1                           //示波器app窗口是否按默认格式新建
#define APP_Reserved_WINDOW_DEFAULT               	 1                           //预留app窗口是否按默认格式新建
/*---------------------------------------------------------------------------*/
/*--------------------------  新建窗口默认设置   ----------------------------*/
/*---------------------------------------------------------------------------*/
#define NEW_WINDOW_COORDINATE_X                 200                        //新建窗口起始坐标
#define NEW_WINDOW_COORDINATE_Y                 90
#define NEW_WINDOW_WIDTH                        500                        //新建窗口长宽
#define NEW_WINDOW_HIGHT                        300
#define NEW_WINDOW_BAR_HIGHT                    40                         //新建窗口标题栏高度
#define NEW_WINDOW_BAR_COLOR_ENALBE             GUI_BLUE                   //新建窗口标题栏颜色
#define NEW_WINDOW_BAR_COLOR_UNENALBE           GUI_LIGHTGRAY              //新建窗口标题栏颜色（未激活）
#define NEW_WINDOW_BAR_ALIGN                    GUI_TA_HCENTER|GUI_TA_VCENTER             //新建窗口标题栏对齐方式
#define NEW_WINDOW_CLOSEBUTTON                  1                          //窗口关闭按钮
#define NEW_WINDOW_RESIZEABLE                   1                          //窗口可缩放
#define NEW_WINDOW_REMOVABLE                    1                          //窗口可移动
#define NEW_WINDOW_MAXMIZE                      1                          //窗口默认最大化
#define NEW_WINDOW_ACTIVE                       1                          //窗口默认激活

#if GUI_LANGUAGE > 0u
	#define NEW_WINDOW_BAR_FONT                     &GUI_FontHZ24              //新建窗口字体（中文）
#else
	#define NEW_WINDOW_BAR_FONT                     GUI_FONT_24B_1             //新建窗口字体
#endif


/***********************************************************************
*            函数声明
************************************************************************
*/
static void _cbBkWindow(WM_MESSAGE* pMsg);                          //桌面窗口回调函数


#endif
