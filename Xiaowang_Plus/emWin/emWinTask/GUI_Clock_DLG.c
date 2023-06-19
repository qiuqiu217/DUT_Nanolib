/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.28                          *
*        Compiled Jan 30 2015, 16:41:06                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

#include "GUI_Main.h"
#include "GUI_Clock_DLG.h"
#if GUI_OPERATING > 0u
	#include "includes.h"
#endif

/************************************************************************************************************************
*
*       变量声明
*
*************************************************************************************************************************
*/
/* 电子时钟位图声明 */
extern GUI_CONST_STORAGE GUI_BITMAP bmclock_classic_background;
extern GUI_CONST_STORAGE GUI_BITMAP bmclock_standard_hour_hand;
extern GUI_CONST_STORAGE GUI_BITMAP bmclock_standard_minute_hand;
extern GUI_CONST_STORAGE GUI_BITMAP bmclock_standard_second_hand;
/* 时间显示相关数组 */
extern char System_Date_Display[15];         //xxxx年xx月xx日
extern char System_Time_Display[9];          //xx:xx:xx
extern const char* apWeek[7];
/* APP窗口句柄 */
extern WM_HWIN hWinClockTask;          //电子时钟APP窗口句柄
/* 系统时间结构体 */
#if GUI_OPERATING == 0u
	RTC_T g_tRTC;
#endif
/* 存储设备相关变量 */
GUI_HMEM hMemSec;
GUI_HMEM hMemMin;
GUI_HMEM hMemHour;
GUI_HMEM hMemDST;
GUI_HMEM hMemBK;
uint32_t xSize;
uint32_t ySize;
WM_HWIN hClock;


/************************************************************************************************************************
*
*       静态代码
*
*************************************************************************************************************************
*/
/**
  * @brief 电子时钟刷新回调函数
  * @note 无
  * @param WM_MESSAGE* pMsg  GUI消息
  * @retval 无
  */
static void _cbClock(WM_MESSAGE* pMsg) 
{
    GUI_MEMDEV_Handle ret;
    int      t0;   /* 用于三个指针的计数 */
    int      t1;
    int      t2;
	
    switch (pMsg->MsgId) 
    {
    case WM_TIMER:
		/* 定时1秒更新一次时间 */
        WM_RestartTimer(pMsg->Data.v, 1000);
        #if GUI_OPERATING > 0u
            RTC_ReadClock();
        #endif

        #if GUI_OPERATING > 0u
            /* 第一个指针计数，用于旋转秒针 */
            t0 = 360 - g_tRTC.Sec * 6;
            /* 第二个指针计数，用于旋转分针 */
            t1 = 360 - g_tRTC.Min * 6;
            /* 第三个指针计数，用于旋转时针 */
            t2 = 360 - g_tRTC.Hour * 30;
        #else
            /* 第一个指针计数，用于旋转秒针 */
            t0 = 360 - 24 * 6;
            /* 第二个指针计数，用于旋转分针 */
            t1 = 360 - 35 * 6;
            /* 第三个指针计数，用于旋转时针 */
            t2 = 360 - 6  * 30;
        #endif

        /* 将相关内容绘制到存储设备hMemDST里面 */
        ret = GUI_MEMDEV_Select(hMemDST);
        GUI_SetBkColor(GUI_WHITE);
        GUI_Clear();

        /* 绘制时钟表盘 */
        GUI_MEMDEV_WriteAt(hMemBK, 0, 0);
        /* 绘制时针 */
        GUI_MEMDEV_RotateHQ(hMemHour, hMemDST, 0, 0, t2 * 1000, 1000);
        /* 绘制分针 */
        GUI_MEMDEV_RotateHQ(hMemMin, hMemDST, 0, 0, t1 * 1000, 1000);
        /* 绘制秒针 */
        GUI_MEMDEV_RotateHQ(hMemSec, hMemDST, 0, 0, t0 * 1000, 1000);

        GUI_MEMDEV_Select(ret);

        /* 执行窗口无效化，会触发执行WM_PAINT消息 */
        WM_InvalidateWindow(pMsg->hWin);
		WM_Paint(WM_GetClientWindow(hWinClockTask));
        break;

    case WM_PAINT:
		/* 选择操作窗口 */
		WM_SelectWindow(WM_GetClientWindow(hClock));
		/* 绘制到多缓冲里面 */
		GUI_MULTIBUF_Begin();
		GUI_MEMDEV_WriteAt(hMemDST, (xSize - bmclock_classic_background.XSize) / 2, (ySize - bmclock_classic_background.YSize) / 2);
		GUI_MULTIBUF_End();
		WM_SelectWindow(WM_GetClientWindow(hWinClockTask));
        break;

	case WM_DELETE:
		/* 删除电子时钟储存设备 */
		GUI_MEMDEV_Delete(hMemBK);
	    GUI_MEMDEV_Delete(hMemSec);
	    GUI_MEMDEV_Delete(hMemMin);
		GUI_MEMDEV_Delete(hMemHour);
		GUI_MEMDEV_Delete(hMemDST);
	    break;
	
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}
/* function end */

/**
  * @brief 电子时钟APP窗口回调函数
  * @note 无
  * @param WM_MESSAGE* pMsg  GUI消息
  * @retval 无
  */
static void _cbAPP_Clock(WM_MESSAGE* pMsg)
{
	WM_HWIN hWin;
	WM_HWIN hItem;

	int NCode;
	int Id;

	(void)NCode;        //空指针
	(void)Id;           //空指针

	hWin = pMsg->hWin;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		#if APP_Clock_WINDOW_DEFAULT > 0u
			/* app窗口初始化 */
			FRAMEWIN_SetBarColor(hWin, 1, NEW_APP_BAR_COLOR_ENALBE);                //窗口标题栏颜色
			FRAMEWIN_SetBarColor(hWin, 0, NEW_APP_BAR_COLOR_UNENALBE);
			#if GUI_LANGUAGE > 0u
				FRAMEWIN_SetFont(hWin, NEW_APP_BAR_FONT);                              //窗口文本字体
				FRAMEWIN_SetText(hWin, "电子时钟");                                     //窗口文本
			#else
				FRAMEWIN_SetFont(hWin, NEW_APP_BAR_FONT);                               //窗口文本字体
				FRAMEWIN_SetText(hWin, "Clock");                                        //窗口文本
			#endif
			FRAMEWIN_SetTextAlign(hWin, NEW_APP_BAR_ALIGN);                         //标题栏文本对齐方式
			FRAMEWIN_SetTitleHeight(hWin, NEW_APP_BAR_HIGHT);

			/* 初始化窗口性质 */
			if (NEW_APP_CLOSEBUTTON > 0u)
			{
				FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);    //关闭按钮
			}
			if (NEW_APP_ACTIVE > 0u)
			{
				FRAMEWIN_SetActive(hWin, 1);                                //窗口激活
			}
		#else
			/* 用户定义区 */
			/* 用户定义区结束 */
		#endif
		/* 获取系统时间 */
		#if GUI_OPERATING > 0u
			RTC_ReadClock();
		#endif
		/* 初始化文本控件 */
		hItem = WM_GetDialogItem(hWin, ID_APP_Clock_StrDate_TEXT);
		#if GUI_LANGUAGE > 0u
			TEXT_SetText(hItem, "日期：");
			TEXT_SetFont(hItem, &GUI_FontHZ24);
		#else
			TEXT_SetText(hItem, "Date:");
			TEXT_SetFont(hItem, GUI_FONT_24B_1);
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_Clock_StrTime_TEXT);
		#if GUI_LANGUAGE > 0u
			TEXT_SetText(hItem, "时间：");
			TEXT_SetFont(hItem, &GUI_FontHZ24);
		#else
			TEXT_SetText(hItem, "Time:");
			TEXT_SetFont(hItem, GUI_FONT_24B_1);
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_Clock_Date_TEXT);
		#if GUI_LANGUAGE > 0u
			sprintf(System_Date_Display, "%0.4d/%0.2d/%0.2d",
					g_tRTC.Year,
					g_tRTC.Mon,
					g_tRTC.Day);
			TEXT_SetText(hItem, System_Date_Display);
			TEXT_SetFont(hItem, &GUI_FontHZ24);
		#else
			sprintf(System_Date_Display, "20%0.2d/%0.2d/%0.2d",
					g_tRTC.Year,
					g_tRTC.Mon,
					g_tRTC.Day);
			TEXT_SetText(hItem, System_Date_Display);
			TEXT_SetFont(hItem, GUI_FONT_24B_1);
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_Clock_Week_TEXT);
		#if GUI_LANGUAGE > 0u
			TEXT_SetText(hItem, apWeek[g_tRTC.Week - 1]);
			TEXT_SetFont(hItem, &GUI_FontHZ24);
		#else
			TEXT_SetText(hItem, apWeek[g_tRTC.Week - 1]);
			TEXT_SetFont(hItem, GUI_FONT_24B_1);
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_Clock_Time_TEXT);
		sprintf(System_Time_Display, "%0.2d:%0.2d:%0.2d",
				g_tRTC.Hour,
				g_tRTC.Min,
				g_tRTC.Sec);
		TEXT_SetText(hItem, System_Time_Display);
		#if GUI_LANGUAGE > 0u
			TEXT_SetFont(hItem, &GUI_FontHZ24);
		#else
			TEXT_SetFont(hItem, GUI_FONT_24B_1);
		#endif
        /* 获取*/
        xSize = LCD_GetXSize();
        ySize = LCD_GetYSize();

        /* 时钟表盘所需存储设备 */
        hMemBK = GUI_MEMDEV_CreateFixed(0, 0, bmclock_classic_background.XSize, bmclock_classic_background.YSize, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
        /* 时钟秒针所需存储设备 */
        hMemSec = GUI_MEMDEV_CreateFixed(0, 0, bmclock_classic_background.XSize, bmclock_classic_background.YSize, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
        /* 时钟分针所需存储设备 */
        hMemMin = GUI_MEMDEV_CreateFixed(0, 0, bmclock_classic_background.XSize, bmclock_classic_background.YSize, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
        /* 时钟小时所需存储设备 */
        hMemHour = GUI_MEMDEV_CreateFixed(0, 0, bmclock_classic_background.XSize, bmclock_classic_background.YSize, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
        /* 时钟目的存储所需存储设备 */
        hMemDST = GUI_MEMDEV_CreateFixed(0, 0, bmclock_classic_background.XSize, bmclock_classic_background.YSize, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);

        /* 将时钟表盘绘制到存储设备 */
        GUI_MEMDEV_Select(hMemBK);
        GUI_SetBkColor(GUI_TRANSPARENT);
        GUI_Clear();
        GUI_DrawBitmap(&bmclock_classic_background, 0, 0);
        GUI_MEMDEV_Select(0);

        /* 将秒针绘制到存储设备 */
        GUI_MEMDEV_Select(hMemSec);
        GUI_SetBkColor(GUI_TRANSPARENT);
        GUI_Clear();
        GUI_DrawBitmap(&bmclock_standard_second_hand,
            0,
            0);
        GUI_MEMDEV_Select(0);

        /* 将分针绘制到存储设备 */
        GUI_MEMDEV_Select(hMemMin);
        GUI_SetBkColor(GUI_TRANSPARENT);
        GUI_Clear();
        GUI_DrawBitmap(&bmclock_standard_minute_hand, 0, 0);
        GUI_MEMDEV_Select(0);

        /* 将时针绘制到存储设备 */
        GUI_MEMDEV_Select(hMemHour);
        GUI_SetBkColor(GUI_TRANSPARENT);
        GUI_Clear();
        GUI_DrawBitmap(&bmclock_standard_hour_hand,
            0,
            0);

        GUI_MEMDEV_Select(0);

		/* 创建一个窗口，用于绘制时钟 */
	    hClock = WINDOW_CreateEx((xSize - bmclock_classic_background.XSize) / 2, 
						(ySize - bmclock_classic_background.YSize) / 2,
						bmclock_classic_background.XSize,
						bmclock_classic_background.YSize,
						hWin,
						WM_CF_SHOW,
						0,
						ID_APP_Clock_WINDOW,
						_cbClock);
						/* 创建定时器 */
						
        WM_CreateTimer(WM_GetClientWindow(hClock), /* 接受信息的窗口的句柄 */
					  0, 	       /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
					  10,          /* 周期，此周期过后指定窗口应收到消息*/
                      0);		   /* 留待将来使用，应为0 */
		break;

    case WM_PAINT:
        /* 清屏 */
        GUI_SetBkColor(GUI_WHITE);
        GUI_Clear();
		/* 获取系统时间 */
		#if GUI_OPERATING > 0u
			RTC_ReadClock();
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_Clock_Date_TEXT);
		#if GUI_LANGUAGE > 0u
			sprintf(System_Date_Display, "%0.4d/%0.2d/%0.2d",
					g_tRTC.Year,
					g_tRTC.Mon,
					g_tRTC.Day);
			TEXT_SetText(hItem, System_Date_Display);
		#else
			sprintf(System_Date_Display, "20%0.2d/%0.2d/%0.2d",
					g_tRTC.Year,
					g_tRTC.Mon,
					g_tRTC.Day);
			TEXT_SetText(hItem, System_Date_Display);
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_Clock_Week_TEXT);
		TEXT_SetText(hItem, apWeek[g_tRTC.Week - 1]);
		hItem = WM_GetDialogItem(hWin, ID_APP_Clock_Time_TEXT);
		sprintf(System_Time_Display, "%0.2d:%0.2d:%0.2d",
				g_tRTC.Hour,
				g_tRTC.Min,
				g_tRTC.Sec);
		TEXT_SetText(hItem, System_Time_Display);
        break;

	case WM_DELETE:
		/* 创建桌面定时器 */
		Fun_Create_DesktopTimer();
		break;

	default:
		WM_DefaultProc(pMsg);
		break;
	}
}
/* function end */


/************************************************************************************************************************
*
*       公共代码
*
*************************************************************************************************************************
*/
/**
  * @brief 电子时钟APP窗口创建函数
  * @note 无
  * @param 无
  * @retval hWin：资源表中第一个控件的句柄
  */
WM_HWIN FUN_APP_Clock_Create(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(Dialog_APP_Clock_Create, GUI_COUNTOF(Dialog_APP_Clock_Create), _cbAPP_Clock, WM_HBKWIN, 0, 0);
	return hWin;
}
/*************************** End of file ****************************/
