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
#include "GUI_Camera_DLG.h"
#if GUI_OPERATING > 0u
	#include "includes.h"
#endif

/************************************************************************************************************************
*
*       变量声明
*
*************************************************************************************************************************
*/
/* APP窗口句柄 */
extern WM_HWIN hWinCameraTask;         //照相机APP窗口句柄

/* 相机数据传输相关数据 */
#if GUI_OPERATING > 0u
	uint32_t uiDispMemAddr;         //相机数据传输地址
	uint8_t usCameraStatus = 1;     //相机数据传输标志位
#endif

GUI_HMEM hMemCamera;    //照相机存储设备


/************************************************************************************************************************
*
*       静态代码
*
*************************************************************************************************************************
*/
/*------------------------------------------------------------------------------------------*/
/*-------------------------------------  回调函数   ----------------------------------------*/
/*------------------------------------------------------------------------------------------*/
/**
  * @brief 照相机APP窗口回调函数
  * @note 无
  * @param WM_MESSAGE* pMsg  GUI消息
  * @retval 无
  */
static void _cbAPP_Camera(WM_MESSAGE* pMsg)
{
	WM_HWIN hWin;
	static WM_HWIN hTimerCAMERA;

	int NCode;
	int Id;

	(void)NCode;        //空指针
	(void)Id;           //空指针
	
	hWin = pMsg->hWin;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
#if APP_Camera_WINDOW_DEFAULT > 0u
		/* app窗口初始化 */
		FRAMEWIN_SetBarColor(hWin, 1, NEW_APP_BAR_COLOR_ENALBE);                //窗口标题栏颜色
		FRAMEWIN_SetBarColor(hWin, 0, NEW_APP_BAR_COLOR_UNENALBE);
#if GUI_LANGUAGE > 0u
		FRAMEWIN_SetFont(hWin, NEW_APP_BAR_FONT);                                  //窗口文本字体
		FRAMEWIN_SetText(hWin, "照相机");                                     //窗口文本
#else
		FRAMEWIN_SetFont(hWin, NEW_APP_BAR_FONT);                               //窗口文本字体
		FRAMEWIN_SetText(hWin, "Camera");                            //窗口文本
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
		/* 创建存储设备，用于加载摄像头数据 */
		hMemCamera = GUI_MEMDEV_CreateEx(0, 0, 640, 480, GUI_MEMDEV_NOTRANS);
		#if GUI_OPERATING > 0u
			uiDispMemAddr = (uint32_t)GUI_MEMDEV_GetDataPtr(hMemCamera);
			/* 初始摄像头 */
			bsp_InitCamera();
			GUI_Delay(200);                     //不延迟一段时间，摄像头传输数据会卡死
			CAM_Start(uiDispMemAddr);
		#endif
		/* 临时加载个红色背景 */
		GUI_MEMDEV_Select(hMemCamera);
		GUI_SetBkColor(GUI_RED);
		GUI_Clear();
		GUI_MEMDEV_Select(0);
		break;
		
	/* 接收到摄像头数据 */
	case MSG_CAMERA:
		hTimerCAMERA = WM_CreateTimer(WM_GetClientWindow(hWinCameraTask), 0, 2, 0);
		break;
	
	case WM_TIMER:
		/* 删除定时器 */
		WM_DeleteTimer(hTimerCAMERA);
	
		/* 选择操作窗口 */
		WM_SelectWindow(WM_GetClientWindow(hWinCameraTask));
	
		/* Cache Clean和无效化 */
		#if GUI_OPERATING > 0u
			SCB_CleanInvalidateDCache();
		#endif
	
		/* 绘制到多缓冲里面 */
		GUI_MULTIBUF_Begin();
		//g_tCam.CaptureOk = 0;
		GUI_MEMDEV_WriteExAt(hMemCamera,144,70,800,800,255);
		GUI_MULTIBUF_End();
	
		/* 开始下次绘制 */
        #if GUI_OPERATING > 0u
			CAM_Start1(uiDispMemAddr);
        #endif	
	
		break;
		
	case WM_DELETE:
		/* 停止接收到摄像头数据 */
		#if GUI_OPERATING > 0u
			CAM_Stop();
		#endif
		/* 创建桌面定时器 */
		Fun_Create_DesktopTimer();
		GUI_MEMDEV_Delete(hMemCamera);
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
/*------------------------------------------------------------------------------------------*/
/*-----------------------------------  窗口创建函数   --------------------------------------*/
/*------------------------------------------------------------------------------------------*/

/**
  * @brief 照相机APP窗口创建函数
  * @note 无
  * @param 无
  * @retval hWin：资源表中第一个控件的句柄
  */
WM_HWIN FUN_APP_Camera_Create(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(Dialog_APP_Camera_Create, GUI_COUNTOF(Dialog_APP_Camera_Create), _cbAPP_Camera, WM_HBKWIN, 0, 0);
	return hWin;
}
/*************************** End of file ****************************/
