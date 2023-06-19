#ifndef  GUI_Calendar_DLG_H
#define  GUI_Calendar_DLG_H

#include "DIALOG.h"
#include "GUI_Main.h"

/***********************************************************************
*       控件ID定义  (控件ID = 页面基本ID + 控件偏移ID)
************************************************************************
*/
#define ID_APP_Calendar_FRAMEWIN		   (GUI_ID_USER + APP_Calendar_BASE_ID + 0x00)
#define ID_APP_Calendar_CALENDAR           (GUI_ID_USER + APP_Calendar_BASE_ID + 0x01)
#define ID_APP_Calendar_StrDate_TEXT       (GUI_ID_USER + APP_Calendar_BASE_ID + 0x02)
#define ID_APP_Calendar_Date_TEXT          (GUI_ID_USER + APP_Calendar_BASE_ID + 0x03)
#define ID_APP_Calendar_Week_TEXT          (GUI_ID_USER + APP_Calendar_BASE_ID + 0x04)
#define ID_APP_Calendar_StrTime_TEXT       (GUI_ID_USER + APP_Calendar_BASE_ID + 0x05)
#define ID_APP_Calendar_Time_TEXT          (GUI_ID_USER + APP_Calendar_BASE_ID + 0x06)
/***********************************************************************
*                             对话框资源表
************************************************************************
*/
/* 万年历APP对话框 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Calender_Create[] =
{
  { FRAMEWIN_CreateIndirect, "Set Date", ID_APP_Calendar_FRAMEWIN, NEW_APP_COORDINATE_X, NEW_APP_COORDINATE_Y, NEW_APP_WIDTH, NEW_APP_HIGHT, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Date:", ID_APP_Calendar_StrDate_TEXT,  100,  380,  60,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Date", ID_APP_Calendar_Date_TEXT, 160,  380,  190,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Week", ID_APP_Calendar_Week_TEXT,  350,  380,  100,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Time:", ID_APP_Calendar_StrTime_TEXT,  520,  380,  125,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Time", ID_APP_Calendar_Time_TEXT,  580,  380,  100,  25, TEXT_CF_LEFT },
};

/*
************************************************************************
*						       函数声明
************************************************************************
*/
static void _cbAPP_Calender(WM_MESSAGE* pMsg);						   //万年历APP窗口回调函数
WM_HWIN FUN_APP_Calender_Create(void);                                 //万年历APP窗口创建函数

#endif
