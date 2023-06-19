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
#include "GUI_Picture_DLG.h"
#if GUI_OPERATING > 0u
	#include "includes.h"
#endif

#define _MAX_PATH 256
#define _MAX_EXT  256

//extern FRESULT result;
//extern FIL file;
//FILINFO finfo;
//extern DIR DirInf;
//extern UINT bw;
//extern FATFS fs;
//extern FATFS fs_nand;
//extern FATFS fs_usb;
/*
********************************************************************
*
*       Static data
*
**********************************************************************
*/
//static struct {
//	U32 Mask;
//	char c;
//} _aAttrib[] = {
//{AM_RDO,      'R'},
//{AM_HID,	  'H'},
//{AM_SYS,	  'S'},
//{AM_DIR,	  'D'},
//{AM_ARC,	  'A'} };


/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetExt
*/
//static void _GetExt(char* pFile, char* pExt) {
//	int Len;
//	int i;
//	int j;
//
//	//
//	// Search beginning of extension
//	//
//	Len = strlen(pFile);
//	for (i = Len; i > 0; i--) {
//		if (*(pFile + i) == '.') {
//			*(pFile + i) = '\0';     // Cut extension from filename
//			break;
//		}
//	}
//	//
//	// Copy extension
//	//
//	j = 0;
//	while (*(pFile + ++i) != '\0') {
//		*(pExt + j) = *(pFile + i);
//		j++;
//	}
//	*(pExt + j) = '\0';          // Set end of string
//}

/*********************************************************************
*
*       _GetData
*/
//void FilePath_process1(char* source)
//{
//	while (*source != '\0')
//	{
//		source++;
//	}
//	source--;
//	*source = '\0';
//}
//
//void FilePath_process2(char* source)
//{
//	while (*source != '\0')
//	{
//		source++;
//	}
//	*source = '\\';
//	source++;
//	*source = '\0';
//}
//
//static int _GetData2(CHOOSEFILE_INFO* pInfo) {
//
//	static char         acAttrib[20] = { 0 };
//	static char         acExt[_MAX_EXT];
//	char                c;
//	int                 i;
//	int                 r;
//	char* fn;
//	OS_ERR      	err;
//
//	OSSchedLock(&err);
//	switch (pInfo->Cmd) {
//	case CHOOSEFILE_FINDFIRST:
//		FilePath_process1(pInfo->pRoot);
//		if (f_opendir(&DirInf, pInfo->pRoot) == FR_OK)
//		{
//			if (f_readdir(&DirInf, &finfo) == FR_OK)
//			{						   
//				fn = finfo.fname;
//
//				r = 0;
//			}
//			else
//			{
//				r = 1;
//			}
//		}
//		else
//		{
//			r = 1;
//		}
//		FilePath_process2(pInfo->pRoot);
//		break;
//	case CHOOSEFILE_FINDNEXT:
//		if (f_readdir(&DirInf, &finfo) == FR_OK)
//		{
//			if (!finfo.fname[0])
//			{
//				r = 1;
//				break;
//			}	
//			
//			fn = finfo.fname;
//
//			r = 0;
//		}
//		else
//		{
//			r = 1;
//		}
//		break;
//	}
//	if (r == 0) {
//		pInfo->Flags = (finfo.fattrib & _aAttrib[4].Mask) ? CHOOSEFILE_FLAG_DIRECTORY : 0;
//		for (i = 0; i < GUI_COUNTOF(_aAttrib); i++) {
//			if (finfo.fattrib & _aAttrib[i].Mask) {
//				c = _aAttrib[i].c;
//			}
//			else {
//				c = '-';
//			}
//			acAttrib[i] = c;
//		}
//		if (finfo.fattrib & _aAttrib[4].Mask)
//		{
//			pInfo->pExt = "Directory";
//		}
//		else
//		{
//			_GetExt(fn, acExt);
//			pInfo->pExt = acExt;
//		}
//
//		pInfo->pAttrib = acAttrib;
//		pInfo->pName = fn;
//		pInfo->SizeL = finfo.fsize << 10;
//		pInfo->SizeH = finfo.fsize >> 22;
//	}
//	OSSchedUnlock(&err);
//	return r;
//}
/************************************************************************************************************************
*
*       变量声明
*
*************************************************************************************************************************
*/
/* APP窗口句柄 */
extern WM_HWIN hWinPictureTask;       //图片预览APP窗口句柄

/************************************************************************************************************************
*
*       静态代码
*
*************************************************************************************************************************
*/
/**
  * @brief 图片浏览——应用窗口回调函数
  * @note 无
  * @param WM_MESSAGE* pMsg  GUI消息
  * @retval 无
  */
static void _cbAPP_Picture(WM_MESSAGE* pMsg)
{
	WM_HWIN hItem;
	WM_HWIN hWin;
	int NCode;
	int Id;
	const char        acMask[] = "*.*";
	CHOOSEFILE_INFO   Info = { 0 };
	int               r;
	char             acDir[_MAX_PATH] = "1:";
	char const* apDrives[1] = { 0 };
	
	(void) NCode;
	(void) Id;

	/*
		FS_SD		0
		FS_NAND		1
		FS_USB		2
	*/
	/*acDir[0] = 0x30 - 1;
	apDrives[0] = acDir;
	Info.pfGetData = _GetData2;
	Info.pMask = acMask;*/
	
	hWin = pMsg->hWin;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		#if APP_Picture_WINDOW_DEFAULT > 0u
			/* app窗口初始化 */
			FRAMEWIN_SetBarColor(hWin, 1, NEW_APP_BAR_COLOR_ENALBE);                //窗口标题栏颜色
			FRAMEWIN_SetBarColor(hWin, 0, NEW_APP_BAR_COLOR_UNENALBE);
			#if GUI_LANGUAGE > 0u
				FRAMEWIN_SetFont(hWin, &GUI_FontHZ24);                                  //窗口文本字体
				FRAMEWIN_SetText(hWin, "图片浏览");                                     //窗口文本
			#else
				FRAMEWIN_SetFont(hWin, NEW_APP_BAR_FONT);                               //窗口文本字体
				FRAMEWIN_SetText(hWin, "Picture");                                      //窗口文本
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
		//hItem = CHOOSEFILE_Create(WM_GetClientWindow(hWin), 	       //Handle of parent window.
		//		0, 			       //X position in pixels of the  dialog in clie nt coordinates.
		//		0, 			       //Y position in pixels of the  dialog in client coordinates.
		//		800, 	   //X-size of the dialog in pixels.
		//		440,       //Y-size of the dialog in pixels
		//		apDrives, 		       //Pointer to an array of strings containing the root directories to be used.
		//		GUI_COUNTOF(apDrives),//Number of root directories. 
		//		0, 				   //Initial index of the root directory to be used.
		//		"File Dialog", 	   //Title to be shown in the title bar.
		//		0, 				   //Additional flags for the FRAMEWIN widget.
		//		&Info);			   //Pointer to a CHOOSEFILE_INFO structure.
		break;

	case WM_PAINT:	
		GUI_SetBkColor(GUI_WHITE);
		GUI_Clear();					//清屏
		break;

	case WM_NOTIFY_PARENT:
		break;

	case WM_DELETE:
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
  * @brief 图片浏览APP窗口创建函数
  * @note 无
  * @param 无
  * @retval hWin：资源表中第一个控件的句柄
  */
WM_HWIN FUN_APP_Picture_Create(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(Dialog_APP_Picture_Create, GUI_COUNTOF(Dialog_APP_Picture_Create), _cbAPP_Picture, WM_HBKWIN, 0, 0);
	return hWin;
}
/*************************** End of file ****************************/
