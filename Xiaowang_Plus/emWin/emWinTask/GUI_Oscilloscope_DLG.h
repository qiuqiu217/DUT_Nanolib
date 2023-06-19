#ifndef  GUI_Oscilloscope_DLG_H
#define  GUI_Oscilloscope_DLG_H

#include "DIALOG.h"
#include "GUI_Main.h"

/***********************************************************************
*       宏定义
************************************************************************
*/
/* ADC通道选通 */
#define Channel_Enable           1
#define Channel_Disable          0
/* 显示区网格可见 */
#define Grid_Visible			 1
#define Grid_Invisible			 0
/* 显示区网格类型 */
#define Grid_Dash				 0
#define Grid_DashDot			 1
#define Grid_DASHDOTDOT			 2
#define Grid_DOT				 3
#define Grid_Solid				 4
/* 示波器状态 */
#define Oscilloscope_Running		0
#define Oscilloscope_Keep			1
/* 示波器量程 */
#define Range_5V				 0
#define Range_10V				 1
/* ADC工作模式 */
#define Normal_Mode				 0
#define FIFO_Mode				 1
/* ADC过采样率 */
#define _NonOversampled				 0
#define _2XOversampled				 1
#define _4XOversampled				 2
#define _8XOversampled				 3
#define _16XOversampled				 4
#define _32XOversampled				 5
#define _64XOversampled				 6
/***********************************************************************
*       示波器图形页面宏定义
************************************************************************
*/
/* 示波器图形页面坐标 */
#define Oscilloscope_COORDINATE_X                   260                          //示波器图形于父窗口坐标
#define Oscilloscope_COORDINATE_Y                   85                          
#define Oscilloscope_WIDTH                          520							 //示波器图形长宽
#define Oscilloscope_HIGHT                          380   
/* 示波器图形页面边框尺寸 */
#define Oscilloscope_BorderL						30							//示波器图形左边框尺寸
#define Oscilloscope_BorderT						5							//示波器图形上边框尺寸
#define Oscilloscope_BorderR						5							//示波器图形右边框尺寸
#define Oscilloscope_BorderB						5							//示波器图形下边框尺寸
#define Oscilloscope_ScrollbarWdith					20							//示波器图形滚轮宽度
/* 示波器图形页面网格尺寸 */
#define Oscilloscope_GridDistX						80							//示波器图形网格宽度
#define Oscilloscope_GridDistY						40							//示波器图形网格高度
/* 示波器图形页面坐标轴尺寸 */
#define Oscilloscope_ScaleSizeX						4096						//示波器图形X坐标尺寸
#define Oscilloscope_ScaleSizeY						500							//示波器图形Y坐标尺寸
/* 示波器图形页面坐标轴设置 */
#define Oscilloscope_ScalePos						25							//示波器图形Y轴刻度位置
#define Oscilloscope_ScaleTickDist					40							//示波器图形Y轴刻度间距
#define Oscilloscope_ScaleFactor					0.05					    //示波器图形Y轴比例因子
/* 示波器数据项的最大数 */
#define Oscilloscope_MaxNumItems					Oscilloscope_ScaleSizeY		//示波器数据项的最大数
/* 示波器设置页面 */
#define Oscilloscope_Settings_COORDINATE_X                   100                        //示波器设置窗口起始坐标
#define Oscilloscope_Settings_COORDINATE_Y                   20
#define Oscilloscope_Settings_WIDTH                          600                        //示波器设置窗口长宽
#define Oscilloscope_Settings_HIGHT                          400
#define Oscilloscope_Settings_BAR_HIGHT						 40                         //示波器设置窗口标题栏高度
/***********************************************************************
*       控件ID定义  (控件ID = 页面基本ID + 控件偏移ID)
************************************************************************
*/
/* 示波器APP窗口控件 */
#define ID_APP_Oscilloscope_FRAMEWIN				(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x00)
#define ID_APP_Oscilloscope_GRAPH					(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x01)
#define ID_APP_Oscilloscope_Channel_1_CHECKBOX      (GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x02)
#define ID_APP_Oscilloscope_Channel_2_CHECKBOX      (GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x03)
#define ID_APP_Oscilloscope_Channel_3_CHECKBOX      (GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x04)
#define ID_APP_Oscilloscope_Channel_4_CHECKBOX      (GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x05)
#define ID_APP_Oscilloscope_Channel_5_CHECKBOX      (GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x06)
#define ID_APP_Oscilloscope_Channel_6_CHECKBOX      (GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x07)
#define ID_APP_Oscilloscope_Channel_7_CHECKBOX      (GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x08)
#define ID_APP_Oscilloscope_Channel_8_CHECKBOX      (GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x09)
#define ID_APP_Oscilloscope_Channel_1_TEXT			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x0A)
#define ID_APP_Oscilloscope_Channel_2_TEXT			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x0B)
#define ID_APP_Oscilloscope_Channel_3_TEXT			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x0C)
#define ID_APP_Oscilloscope_Channel_4_TEXT			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x0D)
#define ID_APP_Oscilloscope_Channel_5_TEXT			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x0E)
#define ID_APP_Oscilloscope_Channel_6_TEXT			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x0F)
#define ID_APP_Oscilloscope_Channel_7_TEXT			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x10)
#define ID_APP_Oscilloscope_Channel_8_TEXT			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x11)
#define ID_APP_Oscilloscope_Settings_BUTTON			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x12)
#define ID_APP_Oscilloscope_Pause_BUTTON			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x13)
#define ID_APP_Oscilloscope_Switch_BUTTON			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x14)
#define ID_APP_Oscilloscope_Save_BUTTON				(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x15)
#define ID_APP_Oscilloscope_Clear_BUTTON			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x16)
#define ID_APP_Oscilloscope_Title_TEXT				(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x17)
/* 示波器设置窗口控件 */
#define ID_Oscilloscope_Settings_FRAMEWIN						(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x18)
#define ID_Oscilloscope_Settings_Appearance_TEXT				(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x19)
#define ID_Oscilloscope_Settings_GridVis_CHECKBOX				(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x20)
#define ID_Oscilloscope_Settings_Grid_DROPDOWN					(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x21)
#define ID_Oscilloscope_Settings_Range_TEXT						(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x22)
#define ID_Oscilloscope_Settings_Range_DROPDOWN					(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x23)
#define ID_Oscilloscope_Settings_Mode_TEXT						(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x24)
#define ID_Oscilloscope_Settings_Mode_DROPDOWN					(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x25)
#define ID_Oscilloscope_Settings_Oversampling_TEXT				(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x26)
#define ID_Oscilloscope_Settings_Oversampling_SPINBOX			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x27)
#define ID_Oscilloscope_Settings_OversamplingRate_TEXT			(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x28)
#define ID_Oscilloscope_Settings_SamplingRate_TEXT				(GUI_ID_USER + APP_Oscilloscope_BASE_ID + 0x29)
/***********************************************************************
*                             对话框资源表
************************************************************************
*/
/* 示波器APP对话框 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Oscilloscope_Create[] =
{
  { FRAMEWIN_CreateIndirect, "Oscilloscope", ID_APP_Oscilloscope_FRAMEWIN, NEW_APP_COORDINATE_X, NEW_APP_COORDINATE_Y, NEW_APP_WIDTH, NEW_APP_HIGHT, 0, 0x64, 0 },
  { CHECKBOX_CreateIndirect, "Channel_1",ID_APP_Oscilloscope_Channel_1_CHECKBOX,  10,  10,  130,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "Channel_2",ID_APP_Oscilloscope_Channel_2_CHECKBOX,  10,  45,  130,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "Channel_3",ID_APP_Oscilloscope_Channel_3_CHECKBOX,  10,  80,  130,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "Channel_4",ID_APP_Oscilloscope_Channel_4_CHECKBOX,  10,  115,  130,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "Channel_5",ID_APP_Oscilloscope_Channel_5_CHECKBOX,  10,  150,  130,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "Channel_6",ID_APP_Oscilloscope_Channel_6_CHECKBOX,  10,  185,  130,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "Channel_7",ID_APP_Oscilloscope_Channel_7_CHECKBOX,  10,  220,  130,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "Channel_8",ID_APP_Oscilloscope_Channel_8_CHECKBOX,  10,  255,  130,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Channel_1",ID_APP_Oscilloscope_Channel_1_TEXT,  140,  10,  130,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Channel_2",ID_APP_Oscilloscope_Channel_2_TEXT,  140,  45,  130,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Channel_3",ID_APP_Oscilloscope_Channel_3_TEXT,  140,  80,  130,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Channel_4",ID_APP_Oscilloscope_Channel_4_TEXT,  140,  115,  130,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Channel_5",ID_APP_Oscilloscope_Channel_5_TEXT,  140,  150,  130,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Channel_6",ID_APP_Oscilloscope_Channel_6_TEXT,  140,  185,  130,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Channel_7",ID_APP_Oscilloscope_Channel_7_TEXT,  140,  220,  130,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Channel_8",ID_APP_Oscilloscope_Channel_8_TEXT,  140,  255,  130,  25, TEXT_CF_LEFT },
  { BUTTON_CreateIndirect, "Settings",ID_APP_Oscilloscope_Settings_BUTTON,   10,  355,  70,  70, 0},
  { BUTTON_CreateIndirect, "Switch",ID_APP_Oscilloscope_Switch_BUTTON,   90,  355,  70,  70, 0},
  { BUTTON_CreateIndirect, "Pause",ID_APP_Oscilloscope_Pause_BUTTON,   170,  355,  70,  70, 0},
  { BUTTON_CreateIndirect, "Save",ID_APP_Oscilloscope_Save_BUTTON,   130,  300,  110,  40, 0},
  { BUTTON_CreateIndirect, "Clear",ID_APP_Oscilloscope_Clear_BUTTON,   10,  300,  110,  40, 0},
  { TEXT_CreateIndirect, "Title",ID_APP_Oscilloscope_Title_TEXT,  Oscilloscope_COORDINATE_X,  10,  Oscilloscope_WIDTH,  25, TEXT_CF_HCENTER },
};
/* 示波器设置窗口对话框 */
static const GUI_WIDGET_CREATE_INFO Dialog_Oscilloscope_Settings_Create[] =
{
  { FRAMEWIN_CreateIndirect, "Settings", ID_Oscilloscope_Settings_FRAMEWIN, Oscilloscope_Settings_COORDINATE_X, Oscilloscope_Settings_COORDINATE_Y, Oscilloscope_Settings_WIDTH, Oscilloscope_Settings_HIGHT, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Appearance",ID_Oscilloscope_Settings_Appearance_TEXT,  10,  20,  250,  25, TEXT_CF_LEFT },
  { DROPDOWN_CreateIndirect, "Select",ID_Oscilloscope_Settings_Grid_DROPDOWN,  20,  50,  300,  150, 0 },
  { CHECKBOX_CreateIndirect, "GridVis",ID_Oscilloscope_Settings_GridVis_CHECKBOX,  380,  50,  250,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Range",ID_Oscilloscope_Settings_Range_TEXT,  10,  95,  450,  25, TEXT_CF_LEFT },
  { DROPDOWN_CreateIndirect, "Select",ID_Oscilloscope_Settings_Range_DROPDOWN,  20,  125,  300,  60, 0 },
  { TEXT_CreateIndirect, "Mode",ID_Oscilloscope_Settings_Mode_TEXT,  10,  170,  450,  25, TEXT_CF_LEFT },
  { DROPDOWN_CreateIndirect, "Mode",ID_Oscilloscope_Settings_Mode_DROPDOWN,  20,  200,  300,  60, 0 },
  { TEXT_CreateIndirect, "Oversampling",ID_Oscilloscope_Settings_Oversampling_TEXT,  10,  245,  450,  25, TEXT_CF_LEFT },
  { SPINBOX_CreateIndirect, "Oversampling",ID_Oscilloscope_Settings_Oversampling_SPINBOX,  20,  275,  100,  50, 0 },
  { TEXT_CreateIndirect, "SamplingRate",ID_Oscilloscope_Settings_OversamplingRate_TEXT,  140,  290,  250,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SamplingRate",ID_Oscilloscope_Settings_SamplingRate_TEXT,  340,  290,  250,  25, TEXT_CF_LEFT },
};
/*
************************************************************************
*						       函数声明
************************************************************************
*/
static void _cbAPP_Oscilloscope(WM_MESSAGE* pMsg);							//示波器APP窗口回调函数
WM_HWIN FUN_APP_Oscilloscope_Create(void);									//示波器APP窗口创建函数
WM_HWIN FUN_Oscilloscope_Settings_Create(WM_HWIN hparents);					//示波器设置窗口创建函数

#endif
