#ifndef  GUI_DrawingBoard_DLG_H
#define  GUI_DrawingBoard_DLG_H

#include "DIALOG.h"
#include "GUI_Main.h"

/***********************************************************************
*       绘画板页面宏定义
************************************************************************
*/
/* 绘画板设置 */
#define DrawingBoard_COORDINATE_X                   20                          //绘画板于父窗口坐标
#define DrawingBoard_COORDINATE_Y                   60                          
#define DrawingBoard_WIDTH                          640                        //绘画板长宽
#define DrawingBoard_HIGHT                          384        
/***********************************************************************
*       宏定义
************************************************************************
*/
#define Brush_bit       0
#define Eraser_bit      1
/***********************************************************************
*                  位图声明
************************************************************************
*/
extern GUI_CONST_STORAGE GUI_BITMAP bmbrush;
extern GUI_CONST_STORAGE GUI_BITMAP bmeraser;
/***********************************************************************
*       控件ID定义  (控件ID = 页面基本ID + 控件偏移ID)
************************************************************************
*/
#define ID_APP_DrawingBoard_FRAMEWIN				(GUI_ID_USER + APP_DrawingBoard_BASE_ID + 0x00)
#define ID_APP_DrawingBoard_Color_TEXT				(GUI_ID_USER + APP_DrawingBoard_BASE_ID + 0x01)
#define ID_APP_DrawingBoard_Size_TEXT				(GUI_ID_USER + APP_DrawingBoard_BASE_ID + 0x02)
#define ID_APP_DrawingBoard_Color_DROPDOWN		    (GUI_ID_USER + APP_DrawingBoard_BASE_ID + 0x03)
#define ID_APP_DrawingBoard_Size_SPINBOX		    (GUI_ID_USER + APP_DrawingBoard_BASE_ID + 0x04)
#define ID_APP_DrawingBoard_PaintBrush_BUTTON	    (GUI_ID_USER + APP_DrawingBoard_BASE_ID + 0x05)
#define ID_APP_DrawingBoard_Clear_BUTTON			(GUI_ID_USER + APP_DrawingBoard_BASE_ID + 0x06)
#define ID_APP_DrawingBoard_Save_BUTTON			    (GUI_ID_USER + APP_DrawingBoard_BASE_ID + 0x07)

/***********************************************************************
*                             对话框资源表
************************************************************************
*/
/* 绘画板APP对话框 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_DrawingBoard_Create[] =
{
  { FRAMEWIN_CreateIndirect, "DrawingBoard", ID_APP_DrawingBoard_FRAMEWIN, NEW_APP_COORDINATE_X, NEW_APP_COORDINATE_Y, NEW_APP_WIDTH, NEW_APP_HIGHT, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Color",ID_APP_DrawingBoard_Color_TEXT,  660,  90,  140,  25, TEXT_CF_HCENTER },
  { TEXT_CreateIndirect, "Size",ID_APP_DrawingBoard_Size_TEXT,  660,  180,  140,  25, TEXT_CF_HCENTER },
  { DROPDOWN_CreateIndirect, "Color",ID_APP_DrawingBoard_Color_DROPDOWN,  665,  115,  120,  170, 0 },
  { SPINBOX_CreateIndirect, "Size",ID_APP_DrawingBoard_Size_SPINBOX,  665,  205,  120,  50, 0 },
  { BUTTON_CreateIndirect, "PaintBrush",ID_APP_DrawingBoard_PaintBrush_BUTTON,   700,  20,  60,  60, 0},
  { BUTTON_CreateIndirect, "Clear",ID_APP_DrawingBoard_Clear_BUTTON,   680,  310,  100,  40, 0},
  { BUTTON_CreateIndirect, "Save",ID_APP_DrawingBoard_Save_BUTTON,   680,  360,  100,  40, 0},
};

/*
************************************************************************
*						       函数声明
************************************************************************
*/
static void _cbAPP_DrawingBoard(WM_MESSAGE* pMsg);						   //绘画板APP窗口回调函数
WM_HWIN FUN_APP_DrawingBoard_Create(void);                                 //绘画板APP窗口创建函数

#endif
