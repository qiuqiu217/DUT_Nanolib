#ifndef  GUI_Picture_DLG_H
#define  GUI_Picture_DLG_H

#include "DIALOG.h"
#include "GUI_Main.h"

/***********************************************************************
*       控件ID定义  (控件ID = 页面基本ID + 控件偏移ID)
************************************************************************
*/
#define ID_APP_Picture_FRAMEWIN		   (GUI_ID_USER + APP_Picture_BASE_ID + 0x00)
/***********************************************************************
*                             对话框资源表
************************************************************************
*/
/* 图片浏览APP对话框 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Picture_Create[] =
{
  { FRAMEWIN_CreateIndirect, "Set Date", ID_APP_Picture_FRAMEWIN, NEW_APP_COORDINATE_X, NEW_APP_COORDINATE_Y, NEW_APP_WIDTH, NEW_APP_HIGHT, 0, 0x64, 0 },
};

/*
************************************************************************
*						       函数声明
************************************************************************
*/
static void _cbAPP_Picture(WM_MESSAGE* pMsg);						  //图片浏览APP窗口回调函数
WM_HWIN FUN_APP_Picture_Create(void);                                 //图片浏览APP窗口创建函数

#endif
