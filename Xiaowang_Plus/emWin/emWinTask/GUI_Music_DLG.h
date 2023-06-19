#ifndef  GUI_Music_DLG_H
#define  GUI_Music_DLG_H

#include "GUI_Main.h"

/***********************************************************************
*                            用户自定义消息
************************************************************************
*/
#define MSG_DispSpec      (GUI_ID_USER + APP_Music_BASE_ID + 0x30)
#define MSG_MusicStart    (GUI_ID_USER + APP_Music_BASE_ID + 0x31)
#define MSG_NextMusic     (GUI_ID_USER + APP_Music_BASE_ID + 0x32)

/***********************************************************************
*       控件ID定义  (控件ID = 页面基本ID + 控件偏移ID)
************************************************************************
*/
#define ID_APP_Music_FRAMEWIN                  (GUI_ID_USER + APP_Music_BASE_ID +0x00)              //音乐播放app窗口
#define ID_APP_Music_Previous_BUTTON           (GUI_ID_USER + APP_Music_BASE_ID +0x01)
#define ID_APP_Music_Next_BUTTON               (GUI_ID_USER + APP_Music_BASE_ID +0x02)
#define ID_APP_Music_Play_BUTTON               (GUI_ID_USER + APP_Music_BASE_ID +0x03)
#define ID_APP_Music_Music_PROGBAR             (GUI_ID_USER + APP_Music_BASE_ID +0x04)
#define ID_APP_Music_Select_DROPDOWN           (GUI_ID_USER + APP_Music_BASE_ID +0x05)
#define ID_APP_Music_Song_TEXT				   (GUI_ID_USER + APP_Music_BASE_ID +0x06)
#define ID_APP_Music_Time_TEXT			       (GUI_ID_USER + APP_Music_BASE_ID +0x07)
#define ID_APP_Music_Number_TEXT			   (GUI_ID_USER + APP_Music_BASE_ID +0x08)
#define ID_APP_Music_Volume_LEFT_SLIDER        (GUI_ID_USER + APP_Music_BASE_ID +0x09)
#define ID_APP_Music_Volume_LEFT_TEXT          (GUI_ID_USER + APP_Music_BASE_ID +0x0A)
#define ID_APP_Music_Volume_RIGHT_SLIDER       (GUI_ID_USER + APP_Music_BASE_ID +0x0B)
#define ID_APP_Music_Volume_RIGHT_TEXT         (GUI_ID_USER + APP_Music_BASE_ID +0x0C)
#define ID_APP_Music_Volume_Speaker_SLIDER     (GUI_ID_USER + APP_Music_BASE_ID +0x0D)
#define ID_APP_Music_Volume_Speaker_TEXT       (GUI_ID_USER + APP_Music_BASE_ID +0x0E)

/***********************************************************************
*                             对话框资源表
************************************************************************
*/
/* 音乐播放APP对话框 */
static const GUI_WIDGET_CREATE_INFO Dialog_APP_Music_Create[] =
{
  { FRAMEWIN_CreateIndirect, "Music", ID_APP_Music_FRAMEWIN, NEW_APP_COORDINATE_X, NEW_APP_COORDINATE_Y, NEW_APP_WIDTH, NEW_APP_HIGHT, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Previous",ID_APP_Music_Previous_BUTTON,   200 - 35,  320, 70,  70, 0},
  { BUTTON_CreateIndirect, "Play",ID_APP_Music_Play_BUTTON,   400 - 35,  320, 70,  70, 0},
  { BUTTON_CreateIndirect, "Next",ID_APP_Music_Next_BUTTON,   600 - 35,  320, 70,  70, 0},
  { PROGBAR_CreateIndirect, "Music",ID_APP_Music_Music_PROGBAR,  100,  270,  600,  20, 0 },
  { TEXT_CreateIndirect, "Name",ID_APP_Music_Song_TEXT,  100,  10,  600,  25, TEXT_CF_HCENTER },
  { TEXT_CreateIndirect, "Time",ID_APP_Music_Time_TEXT,  400,  245,  300,  25, TEXT_CF_RIGHT },
  { TEXT_CreateIndirect, "Number",ID_APP_Music_Number_TEXT,  100,  245,  300,  25, TEXT_CF_LEFT },
  { DROPDOWN_CreateIndirect, "Select",ID_APP_Music_Select_DROPDOWN,  100,  40,  300,  170, 0 },
  { SLIDER_CreateIndirect, "Volume Speaker",ID_APP_Music_Volume_Speaker_SLIDER,  450,  70,  250,  30, 0 },
  { TEXT_CreateIndirect, "Volume Speaker",ID_APP_Music_Volume_Speaker_TEXT,  450,  40,  250,  25, TEXT_CF_LEFT },
  { SLIDER_CreateIndirect, "Volume LEFT",ID_APP_Music_Volume_LEFT_SLIDER,  450,  140,  250,  30, 0 },
  { TEXT_CreateIndirect, "Volume LEFT",ID_APP_Music_Volume_LEFT_TEXT,  450,  110,  250,  25, TEXT_CF_LEFT },
  { SLIDER_CreateIndirect, "Volume RIGHT",ID_APP_Music_Volume_RIGHT_SLIDER,  450,  210,  250,  30, 0 },
  { TEXT_CreateIndirect, "Volume RIGHT",ID_APP_Music_Volume_RIGHT_TEXT,  450,  180,  250,  25, TEXT_CF_LEFT },
};

/***********************************************************************
*              函数声明
************************************************************************
*/
static void _cbAPP_Music(WM_MESSAGE* pMsg);						    //音乐播放APP窗口回调函数
WM_HWIN FUN_APP_Music_Create(void);                                 //音乐播放APP窗口创建函数



#endif
