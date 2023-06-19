#ifndef  GUI_Camera_DLG_H
#define  GUI_Camera_DLG_H

#include "GUI_Main.h"

/***********************************************************************
*                            用户自定义消息
************************************************************************
*/
#define MSG_CAMERA (GUI_ID_USER + APP_Camera_BASE_ID + 0x01)
/***********************************************************************
*       控件ID定义  (控件ID = 页面基本ID + 控件偏移ID)
************************************************************************
*/
#define ID_APP_Camera_FRAMEWIN       (GUI_ID_USER + APP_Camera_BASE_ID + 0x00)

/***********************************************************************
*                             对话框资源表
************************************************************************
*/
/* 照相机APP对话框 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Camera_Create[] =
{
  { FRAMEWIN_CreateIndirect, "Camera", ID_APP_Camera_FRAMEWIN, NEW_APP_COORDINATE_X, NEW_APP_COORDINATE_Y, NEW_APP_WIDTH, NEW_APP_HIGHT, 0, 0x64, 0 },
};

/***********************************************************************
*						       函数声明
************************************************************************
*/
static void _cbAPP_Camera(WM_MESSAGE* pMsg);                         //照相机APP窗口回调函数
WM_HWIN FUN_APP_Camera_Create(void);                                 //照相机APP窗口创建函数


#endif
