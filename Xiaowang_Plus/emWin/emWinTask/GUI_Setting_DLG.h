#ifndef  GUI_Setting_DLG_H
#define  GUI_Setting_DLG_H

#include "DIALOG.h"
#include "GUI_Main.h"

/***********************************************************************
*       宏定义
************************************************************************
*/
/* 桌面背景使能 */
#define Background_Enable           1
#define Background_Gradient         0
/* 系统时间修改使能 */
#define System_Time_Modify_Enable   1
#define System_Time_Modify_Disable  0
/* 系统语音播报使能 */
#define Voice_Broadcast_Modify_Enable   1
#define Voice_Broadcast_Modify_Disable  0

/***********************************************************************
*       系统设置页面宏定义
************************************************************************
*/
/* 系统设置APP窗口多页控件设置 */
#define APP_Settings_LISTBOX_COORDINATE_X                   0                          //列表控件起始位置
#define APP_Settings_LISTBOX_COORDINATE_Y                   0                          
#define APP_Settings_LISTBOX_WIDTH                          200                        //列表控件长宽
#define APP_Settings_LISTBOX_HIGHT                          440        
/* 系统设置APP_Settings窗口多页控件页面设置 */
#define APP_Settings_MULTIPAGE_WINDOW_COORDINATE_X            200                          //多页控件页面起始位置
#define APP_Settings_MULTIPAGE_WINDOW_COORDINATE_Y            0                          
#define APP_Settings_MULTIPAGE_WINDOW_WIDTH                   590                          //多页控件页面长宽
#define APP_Settings_MULTIPAGE_WINDOW_HIGHT                   435 


/***********************************************************************
*       控件ID定义  (控件ID = 页面基本ID + 控件偏移ID)
************************************************************************
*/
/* 系统设置主界面 */
#define ID_APP_Settings_FRAMEWIN         (GUI_ID_USER + APP_Settings_BASE_ID + 0x00)              //系统设置app窗口
#define ID_APP_Settings_LISTBOX          (GUI_ID_USER + APP_Settings_BASE_ID + 0x01)
#define ID_APP_Settings_Window1          (GUI_ID_USER + APP_Settings_BASE_ID + 0x02)
#define ID_APP_Settings_Window2          (GUI_ID_USER + APP_Settings_BASE_ID + 0x03)
#define ID_APP_Settings_Window3          (GUI_ID_USER + APP_Settings_BASE_ID + 0x04)
#define ID_APP_Settings_Window4          (GUI_ID_USER + APP_Settings_BASE_ID + 0x05)
#define ID_APP_Settings_Window5          (GUI_ID_USER + APP_Settings_BASE_ID + 0x06)
#define ID_APP_Settings_Window6          (GUI_ID_USER + APP_Settings_BASE_ID + 0x07)
#define ID_APP_Settings_Window7          (GUI_ID_USER + APP_Settings_BASE_ID + 0x08)
#define ID_APP_Settings_Window8          (GUI_ID_USER + APP_Settings_BASE_ID + 0x09)
#define ID_APP_Settings_MULTIPAGE        (GUI_ID_USER + APP_Settings_BASE_ID + 0x0A)
#define ID_APP_Settings_Window1_TEXT     (GUI_ID_USER + APP_Settings_BASE_ID + 0x0B)
#define ID_APP_Settings_Window2_TEXT     (GUI_ID_USER + APP_Settings_BASE_ID + 0x0C)
#define ID_APP_Settings_Window3_TEXT     (GUI_ID_USER + APP_Settings_BASE_ID + 0x0D)
#define ID_APP_Settings_Window4_TEXT     (GUI_ID_USER + APP_Settings_BASE_ID + 0x0E)
#define ID_APP_Settings_Window5_TEXT     (GUI_ID_USER + APP_Settings_BASE_ID + 0x0F)
#define ID_APP_Settings_Window6_TEXT     (GUI_ID_USER + APP_Settings_BASE_ID + 0x10)
#define ID_APP_Settings_Window7_TEXT     (GUI_ID_USER + APP_Settings_BASE_ID + 0x11)
#define ID_APP_Settings_Window8_TEXT     (GUI_ID_USER + APP_Settings_BASE_ID + 0x12)
/* 系统设置第一页 */
#define ID_APP_Settings_Window1_Background_TEXT         (GUI_ID_USER + APP_Settings_BASE_ID + 0x20)
#define ID_APP_Settings_Window1_Background_CHECKBOX     (GUI_ID_USER + APP_Settings_BASE_ID + 0x21)
#define ID_APP_Settings_Window1_Timemodify_TEXT         (GUI_ID_USER + APP_Settings_BASE_ID + 0x22)
#define ID_APP_Settings_Window1_Timemodify_CHECKBOX     (GUI_ID_USER + APP_Settings_BASE_ID + 0x23)
#define ID_APP_Settings_Window1_Broadcast_TEXT          (GUI_ID_USER + APP_Settings_BASE_ID + 0x24)
#define ID_APP_Settings_Window1_Broadcast_CHECKBOX      (GUI_ID_USER + APP_Settings_BASE_ID + 0x25)
#define ID_APP_Settings_Window1_System_description_BUTTON      (GUI_ID_USER + APP_Settings_BASE_ID + 0x26)
/* 系统设置第二页 */
#define ID_APP_Settings_CALENDAR                  (GUI_ID_USER + APP_Settings_BASE_ID + 0x30)
#define ID_APP_Settings_SetHour_LISTWHEEL         (GUI_ID_USER + APP_Settings_BASE_ID + 0x31)
#define ID_APP_Settings_SetMinute_LISTWHEEL       (GUI_ID_USER + APP_Settings_BASE_ID + 0x32)
#define ID_APP_Settings_SetSecond_LISTWHEEL       (GUI_ID_USER + APP_Settings_BASE_ID + 0x33)
#define ID_APP_Settings_Window2_SetDate_TEXT      (GUI_ID_USER + APP_Settings_BASE_ID + 0x34)
#define ID_APP_Settings_Window2_SetTime_TEXT      (GUI_ID_USER + APP_Settings_BASE_ID + 0x35)
#define ID_APP_Settings_Window2_Date_TEXT         (GUI_ID_USER + APP_Settings_BASE_ID + 0x36)
#define ID_APP_Settings_Window2_Time_TEXT         (GUI_ID_USER + APP_Settings_BASE_ID + 0x37)
#define ID_APP_Settings_Window2_Week_TEXT         (GUI_ID_USER + APP_Settings_BASE_ID + 0x38)
#define ID_APP_Settings_Window2_AMPM_TEXT         (GUI_ID_USER + APP_Settings_BASE_ID + 0x39)
#define ID_APP_Settings_Window2_SetDate_BUTTON    (GUI_ID_USER + APP_Settings_BASE_ID + 0x3A)
#define ID_APP_Settings_Window2_SetTime_BUTTON    (GUI_ID_USER + APP_Settings_BASE_ID + 0x3B)
/* 系统设置第五页 */
#define ID_APP_Settings_Window5_Next_BUTTON              (GUI_ID_USER + APP_Settings_BASE_ID + 0x40)
#define ID_APP_Settings_Window5_Previous_BUTTON          (GUI_ID_USER + APP_Settings_BASE_ID +0x41)
#define ID_APP_Settings_Window5_SetBackground_BUTTON     (GUI_ID_USER + APP_Settings_BASE_ID +0x42)
/* 系统设置第七页 */
#define ID_APP_Settings_Window7_SRAMIN_TEXT        (GUI_ID_USER + APP_Settings_BASE_ID + 0x50)
#define ID_APP_Settings_Window7_SRAMEX_TEXT        (GUI_ID_USER + APP_Settings_BASE_ID + 0x51)
#define ID_APP_Settings_Window7_SRAMCCM_TEXT       (GUI_ID_USER + APP_Settings_BASE_ID + 0x52)
#define ID_APP_Settings_Window7_SDIO_TEXT          (GUI_ID_USER + APP_Settings_BASE_ID + 0x53)
#define ID_APP_Settings_Window7_SRAMIN_PROGBAR     (GUI_ID_USER + APP_Settings_BASE_ID + 0x54)
#define ID_APP_Settings_Window7_SRAMEX_PROGBAR     (GUI_ID_USER + APP_Settings_BASE_ID + 0x55)
#define ID_APP_Settings_Window7_SRAMCCM_PROGBAR    (GUI_ID_USER + APP_Settings_BASE_ID + 0x56)
#define ID_APP_Settings_Window7_SDIO_PROGBAR       (GUI_ID_USER + APP_Settings_BASE_ID + 0x57)
#define ID_APP_Settings_Window7_SRAMIN_TOT_TEXT        (GUI_ID_USER + APP_Settings_BASE_ID + 0x58)
#define ID_APP_Settings_Window7_SRAMEX_TOT_TEXT        (GUI_ID_USER + APP_Settings_BASE_ID + 0x59)
#define ID_APP_Settings_Window7_SRAMCCM_TOT_TEXT       (GUI_ID_USER + APP_Settings_BASE_ID + 0x5A)
#define ID_APP_Settings_Window7_SDIO_TOT_TEXT          (GUI_ID_USER + APP_Settings_BASE_ID + 0x5B)
#define ID_APP_Settings_Window7_SRAMIN_FRE_TEXT        (GUI_ID_USER + APP_Settings_BASE_ID + 0x5C)
#define ID_APP_Settings_Window7_SRAMEX_FRE_TEXT        (GUI_ID_USER + APP_Settings_BASE_ID + 0x5D)
#define ID_APP_Settings_Window7_SRAMCCM_FRE_TEXT       (GUI_ID_USER + APP_Settings_BASE_ID + 0x5E)
#define ID_APP_Settings_Window7_SDIO_FRE_TEXT          (GUI_ID_USER + APP_Settings_BASE_ID + 0x5F)

/***********************************************************************
*                             对话框资源表
************************************************************************
*/
/* 系统设置APP对话框 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Settings_Create[] =
{
  { FRAMEWIN_CreateIndirect, "Setting", ID_APP_Settings_FRAMEWIN, NEW_APP_COORDINATE_X, NEW_APP_COORDINATE_Y, NEW_APP_WIDTH, NEW_APP_HIGHT, 0, 0x64, 0 },
  { LISTBOX_CreateIndirect, "Listbox", ID_APP_Settings_LISTBOX, APP_Settings_LISTBOX_COORDINATE_X, APP_Settings_LISTBOX_COORDINATE_Y, APP_Settings_LISTBOX_WIDTH, APP_Settings_LISTBOX_HIGHT, 0 },
  //{ MULTIPAGE_CreateIndirect, "MultiPage", ID_APP_Settings_MULTIPAGE, APP_Settings_MULTIPAGE_COORDINATE_X, APP_Settings_MULTIPAGE_COORDINATE_Y, APP_Settings_MULTIPAGE_WIDTH, APP_Settings_MULTIPAGE_HIGHT, 0 },
};
/* 系统设置APP第一页 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Settings_Page1_Create[] =
{
  { WINDOW_CreateIndirect, "Window1", ID_APP_Settings_Window1,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_X,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_Y, APP_Settings_MULTIPAGE_WINDOW_WIDTH, APP_Settings_MULTIPAGE_WINDOW_HIGHT, 0 },
  { TEXT_CreateIndirect, "Window1",ID_APP_Settings_Window1_TEXT,  100,  10,  400,  25, TEXT_CF_HCENTER },
  { TEXT_CreateIndirect, "background",ID_APP_Settings_Window1_Background_TEXT,  10,  40,  400,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "background",ID_APP_Settings_Window1_Background_CHECKBOX,  30,  65,  500,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "time",ID_APP_Settings_Window1_Timemodify_TEXT,  10,  95,  400,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "time",ID_APP_Settings_Window1_Timemodify_CHECKBOX,  30,  120,  500,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "broadcast",ID_APP_Settings_Window1_Broadcast_TEXT,  10,  150,  400,  25, TEXT_CF_LEFT },
  { CHECKBOX_CreateIndirect, "broadcast",ID_APP_Settings_Window1_Broadcast_CHECKBOX,  30,  175,  500,  25, TEXT_CF_LEFT },
};
/* 系统设置APP第二页 */
static char* _ap_sethour[] = { "0","1", "2", "3", "4", "5", "6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23" };
static char* _ap_setminute[] = { "0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20",
"21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","45","46",
"47","48","49","50","51","52","53","54","55","56","57","58","59" };
static char* _ap_setsecond[] = { "0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20",
"21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","45","46",
"47","48","49","50","51","52","53","54","55","56","57","58","59" };
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Settings_Page2_Create[] =
{
  { WINDOW_CreateIndirect, "Window2", ID_APP_Settings_Window2,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_X,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_Y, APP_Settings_MULTIPAGE_WINDOW_WIDTH, APP_Settings_MULTIPAGE_WINDOW_HIGHT, 0 },
  { TEXT_CreateIndirect, "Window2",ID_APP_Settings_Window2_TEXT,  100,  10,  400,  25, TEXT_CF_HCENTER },
  { TEXT_CreateIndirect, "Window2",ID_APP_Settings_Window2_SetDate_TEXT,  10,  44,  280,  25, TEXT_CF_HCENTER },
  { TEXT_CreateIndirect, "Window2",ID_APP_Settings_Window2_SetTime_TEXT,  310,  44,  280,  25, TEXT_CF_HCENTER },
  { LISTWHEEL_CreateIndirect, "Hour_Wheel", ID_APP_Settings_SetHour_LISTWHEEL, 310, 70, 80, 240, WM_CF_MEMDEV, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "Minute_Wheel", ID_APP_Settings_SetMinute_LISTWHEEL, 400 , 70, 80, 240, WM_CF_MEMDEV, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "Second_Wheel", ID_APP_Settings_SetSecond_LISTWHEEL, 490 , 70, 80, 240, WM_CF_MEMDEV, 0x0, 0 },
  { TEXT_CreateIndirect, "Date",ID_APP_Settings_Window2_Date_TEXT,  20,  340,  280,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Time",ID_APP_Settings_Window2_Time_TEXT,  400,  340,  100,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "Week",ID_APP_Settings_Window2_Week_TEXT,  200,  340,  200,  25, TEXT_CF_LEFT },
  //{ TEXT_CreateIndirect, "AMPM",ID_APP_Settings_Window2_AMPM_TEXT,  480,  340,  200,  25, TEXT_CF_LEFT },
  { BUTTON_CreateIndirect, "Set Date",ID_APP_Settings_Window2_SetDate_BUTTON,   30,  380,  240,  40, 0},
  { BUTTON_CreateIndirect, "Set Time",ID_APP_Settings_Window2_SetTime_BUTTON,   320,  380,  240,  40, 0},
};
/* 系统设置APP第三页 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Settings_Page3_Create[] =
{
  { WINDOW_CreateIndirect, "Window3", ID_APP_Settings_Window3,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_X,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_Y, APP_Settings_MULTIPAGE_WINDOW_WIDTH, APP_Settings_MULTIPAGE_WINDOW_HIGHT, 0 },
  { TEXT_CreateIndirect, "Window3",ID_APP_Settings_Window3_TEXT,  100,  10,  400,  25, TEXT_CF_HCENTER },
};
/* 系统设置APP第四页 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Settings_Page4_Create[] =
{
  { WINDOW_CreateIndirect, "Window4", ID_APP_Settings_Window4,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_X,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_Y, APP_Settings_MULTIPAGE_WINDOW_WIDTH, APP_Settings_MULTIPAGE_WINDOW_HIGHT, 0 },
  { TEXT_CreateIndirect, "Window4",ID_APP_Settings_Window4_TEXT,  100,  10,  400,  25, TEXT_CF_HCENTER },
};
/* 系统设置APP第五页 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Settings_Page5_Create[] =
{
  { WINDOW_CreateIndirect, "Window5", ID_APP_Settings_Window5,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_X,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_Y, APP_Settings_MULTIPAGE_WINDOW_WIDTH, APP_Settings_MULTIPAGE_WINDOW_HIGHT, 0 },
  { TEXT_CreateIndirect, "Window5",ID_APP_Settings_Window5_TEXT,  100,  10,  400,  25, TEXT_CF_HCENTER },
  { BUTTON_CreateIndirect, "next",ID_APP_Settings_Window5_Next_BUTTON,   460,  350,  100,  40, 0},
  { BUTTON_CreateIndirect, "previous",ID_APP_Settings_Window5_Previous_BUTTON,   40,  350,  100,  40, 0},
  { BUTTON_CreateIndirect, "Set Background",ID_APP_Settings_Window5_SetBackground_BUTTON,   200,  350,  200,  40, 0},
};
/* 系统设置APP第六页 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Settings_Page6_Create[] =
{
  { WINDOW_CreateIndirect, "Window6", ID_APP_Settings_Window6,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_X,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_Y, APP_Settings_MULTIPAGE_WINDOW_WIDTH, APP_Settings_MULTIPAGE_WINDOW_HIGHT, 0 },
  { TEXT_CreateIndirect, "Window6",ID_APP_Settings_Window6_TEXT,  100,  10,  400,  25, TEXT_CF_HCENTER },
};
/* 系统设置APP第七页 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Settings_Page7_Create[] =
{
  { WINDOW_CreateIndirect, "Window7", ID_APP_Settings_Window7,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_X,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_Y, APP_Settings_MULTIPAGE_WINDOW_WIDTH, APP_Settings_MULTIPAGE_WINDOW_HIGHT, 0 },
  { TEXT_CreateIndirect, "Window7",ID_APP_Settings_Window7_TEXT,  100,  10,  400,  25, TEXT_CF_HCENTER },
  { TEXT_CreateIndirect, "SRAMIN",ID_APP_Settings_Window7_SRAMIN_TEXT,  30,  40,  120,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SRAMEX",ID_APP_Settings_Window7_SRAMEX_TEXT,  30,  140,  120,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SRAMCCM",ID_APP_Settings_Window7_SRAMCCM_TEXT,  30,  240,  120,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SDIO",ID_APP_Settings_Window7_SDIO_TEXT,  30,  340,  120,  25, TEXT_CF_LEFT },
  { PROGBAR_CreateIndirect, "SRAMIN",ID_APP_Settings_Window7_SRAMIN_PROGBAR,  60,  85,  200,  30, TEXT_CF_LEFT },
  { PROGBAR_CreateIndirect, "SRAMEX",ID_APP_Settings_Window7_SRAMEX_PROGBAR,  60,  185,  200,  30, TEXT_CF_LEFT },
  { PROGBAR_CreateIndirect, "SRAMCCM",ID_APP_Settings_Window7_SRAMCCM_PROGBAR,  60,  285,  200,  30, TEXT_CF_LEFT },
  { PROGBAR_CreateIndirect, "SDIO",ID_APP_Settings_Window7_SDIO_PROGBAR,  60,  385,  200,  30, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SRAMIN_TOT",ID_APP_Settings_Window7_SRAMIN_TOT_TEXT,  420,  85,  200,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SRAMEX_TOT",ID_APP_Settings_Window7_SRAMEX_TOT_TEXT,  420,  185,  200,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SRAMCCM_TOT",ID_APP_Settings_Window7_SRAMCCM_TOT_TEXT,  420,  285,  200,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SDIO_TOT",ID_APP_Settings_Window7_SDIO_TOT_TEXT,  420,  385,  200,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SRAMIN_FRE",ID_APP_Settings_Window7_SRAMIN_FRE_TEXT,  270,  85,  160,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SRAMEX_FRE",ID_APP_Settings_Window7_SRAMEX_FRE_TEXT,  270,  185,  160,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SRAMCCM_FRE",ID_APP_Settings_Window7_SRAMCCM_FRE_TEXT,  270,  285,  160,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect, "SDIO_FRE",ID_APP_Settings_Window7_SDIO_FRE_TEXT,  270,  385,  160,  25, TEXT_CF_LEFT },
};
/* 系统设置APP第八页 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Settings_Page8_Create[] =
{
  { WINDOW_CreateIndirect, "Window8", ID_APP_Settings_Window8,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_X,  APP_Settings_MULTIPAGE_WINDOW_COORDINATE_Y, APP_Settings_MULTIPAGE_WINDOW_WIDTH, APP_Settings_MULTIPAGE_WINDOW_HIGHT, 0 },
  { TEXT_CreateIndirect, "Window8",ID_APP_Settings_Window8_TEXT,  100,  10,  400,  25, TEXT_CF_HCENTER },
};
/***********************************************************************
*              函数声明
************************************************************************
*/
static void _cbAPP_Settings(WM_MESSAGE* pMsg);						   //系统设置APP窗口回调函数
static void _cbAPP_Settings_Window1(WM_MESSAGE* pMsg);				   //系统设置APP第一页回调函数
static void _cbAPP_Settings_Window2(WM_MESSAGE* pMsg);				   //系统设置APP第二页回调函数
static void _cbAPP_Settings_Window3(WM_MESSAGE* pMsg);				   //系统设置APP第三页回调函数
static void _cbAPP_Settings_Window4(WM_MESSAGE* pMsg);				   //系统设置APP第四页回调函数
static void _cbAPP_Settings_Window5(WM_MESSAGE* pMsg);				   //系统设置APP第五页回调函数
static void _cbAPP_Settings_Window6(WM_MESSAGE* pMsg);				   //系统设置APP第六页回调函数
static void _cbAPP_Settings_Window7(WM_MESSAGE* pMsg);				   //系统设置APP第七页回调函数
static void _cbAPP_Settings_Window8(WM_MESSAGE* pMsg);				   //系统设置APP第八页回调函数
WM_HWIN FUN_APP_Settings_Create(void);                                 //系统设置APP窗口创建函数
WM_HWIN FUN_APP_Settings_Window1_Create(WM_HWIN hParents);			   //系统设置APP第一页创建函数
WM_HWIN FUN_APP_Settings_Window2_Create(WM_HWIN hParents);			   //系统设置APP第二页创建函数
WM_HWIN FUN_APP_Settings_Window3_Create(WM_HWIN hParents);			   //系统设置APP第三页创建函数
WM_HWIN FUN_APP_Settings_Window4_Create(WM_HWIN hParents);			   //系统设置APP第四页创建函数
WM_HWIN FUN_APP_Settings_Window5_Create(WM_HWIN hParents);			   //系统设置APP第五页创建函数
WM_HWIN FUN_APP_Settings_Window6_Create(WM_HWIN hParents);			   //系统设置APP第六页创建函数
WM_HWIN FUN_APP_Settings_Window7_Create(WM_HWIN hParents);			   //系统设置APP第七页创建函数
WM_HWIN FUN_APP_Settings_Window8_Create(WM_HWIN hParents);			   //系统设置APP第八页创建函数


#endif
