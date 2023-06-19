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
#include "GUI_DrawingBoard_DLG.h"
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
extern WM_HWIN hWinDrawingBoardTask;       //绘画板APP窗口句柄
GUI_HMEM hMemDrawingBoard;				   //绘画板内存设备句柄
GUI_COLOR PaintBrush_Color;				   //绘画板画笔颜色
int PaintBrush_Size;					   //绘画板画笔大小
/* 画板画笔颜色数组声明 */
#if GUI_LANGUAGE > 0u
	static char* Coler_List[8] = { "黑色","灰色","蓝色","红色","绿色","黄色","橘色","青色"};
#else	
	static char* Coler_List[8] = { "BLACK","GRAY","BLUE","RED","GREEN","YELLOW","ORANGE","CYAN" };
#endif
static GUI_COLOR GUI_Coler_List[8] = { GUI_BLACK,GUI_GRAY,GUI_BLUE,GUI_RED,GUI_GREEN,GUI_YELLOW,GUI_ORANGE,GUI_CYAN};
/* 画笔、橡皮标志位 */
uint8_t Brush_Eraser_bit = Brush_bit;
/************************************************************************************************************************
*
*       静态代码
*
*************************************************************************************************************************
*/
/**
  * @brief 系统设置——应用窗口回调函数
  * @note 无
  * @param WM_MESSAGE* pMsg  GUI消息
  * @retval 无
  */
static void _cbAPP_DrawingBoard(WM_MESSAGE* pMsg)
{
	const GUI_PID_STATE* pState;
	GUI_MEMDEV_Handle Board;
	GUI_RECT rBoard = { DrawingBoard_COORDINATE_X, DrawingBoard_COORDINATE_Y - NEW_APP_BAR_HIGHT, DrawingBoard_WIDTH + DrawingBoard_COORDINATE_X, DrawingBoard_HIGHT - NEW_APP_BAR_HIGHT + DrawingBoard_COORDINATE_Y };
	WM_HWIN hItem;
	WM_HWIN hWin;
	int NCode;
	int Id; 
	uint32_t ulStart, ulEnd;
	uint8_t		Pic_Name = 0;
	char buf[20];
	uint8_t i;
	#if GUI_OPERATING > 0u
		OS_ERR  err;
	#endif
	
	(void) NCode;
	(void) Id;
	
	hWin = pMsg->hWin;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
#if APP_DrawingBoard_WINDOW_DEFAULT > 0u
		/* app窗口初始化 */
		FRAMEWIN_SetBarColor(hWin, 1, NEW_APP_BAR_COLOR_ENALBE);                //窗口标题栏颜色
		FRAMEWIN_SetBarColor(hWin, 0, NEW_APP_BAR_COLOR_UNENALBE);
#if GUI_LANGUAGE > 0u
		FRAMEWIN_SetFont(hWin, &GUI_FontHZ24);                                  //窗口文本字体
		FRAMEWIN_SetText(hWin, "绘画板");                                     //窗口文本
#else
		FRAMEWIN_SetFont(hWin, NEW_APP_BAR_FONT);                               //窗口文本字体
		FRAMEWIN_SetText(hWin, "DrawingBoard");                                      //窗口文本
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
		/* 创建绘画板内存设备 */
		hMemDrawingBoard = GUI_MEMDEV_CreateFixed(DrawingBoard_COORDINATE_X,
							DrawingBoard_COORDINATE_Y,
							DrawingBoard_WIDTH,
							DrawingBoard_HIGHT,
							GUI_MEMDEV_HASTRANS,
							GUI_MEMDEV_APILIST_16,
							GUICC_M565);
		/* 设置绘画板参数 */
		PaintBrush_Color = GUI_BLACK;
		PaintBrush_Size = 6;
		/* 初始化页面控件 */
		hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_Color_TEXT);      
		#if GUI_LANGUAGE > 0u
			TEXT_SetFont(hItem, &GUI_FontHZ24);
			TEXT_SetText(hItem, "画笔颜色");
		#else
			TEXT_SetFont(hItem, GUI_FONT_24B_1);
			TEXT_SetText(hItem, "Color");
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_Size_TEXT);
		#if GUI_LANGUAGE > 0u
			TEXT_SetFont(hItem, &GUI_FontHZ24);
			TEXT_SetText(hItem, "画笔大小");
		#else
			TEXT_SetFont(hItem, GUI_FONT_24B_1);
			TEXT_SetText(hItem, "Size");
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_Clear_BUTTON);  
		#if GUI_LANGUAGE > 0u
			BUTTON_SetFont(hItem, &GUI_FontHZ24);
			BUTTON_SetText(hItem, "清空画板");
		#else
			BUTTON_SetFont(hItem, GUI_FONT_24B_1);
			BUTTON_SetText(hItem, "Clear");
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_Save_BUTTON);
		#if GUI_LANGUAGE > 0u
			BUTTON_SetFont(hItem, &GUI_FontHZ24);
			BUTTON_SetText(hItem, "保存画板");
		#else
			BUTTON_SetFont(hItem, GUI_FONT_24B_1);
			BUTTON_SetText(hItem, "Save");
		#endif
		hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_PaintBrush_BUTTON);  
		BUTTON_SetText(hItem, "");
		BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &bmbrush);
		BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &bmbrush);
		BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &bmbrush);
		hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_Color_DROPDOWN);
		DROPDOWN_SetTextHeight(hItem, 25);
		#if GUI_LANGUAGE > 0u             
			DROPDOWN_SetFont(hItem, &GUI_FontHZ24);
		#else
			DROPDOWN_SetFont(hItem, GUI_FONT_24B_1);
		#endif
		DROPDOWN_SetAutoScroll(hItem, 1);      
		DROPDOWN_SetScrollbarWidth(hItem, 30);
		DROPDOWN_SetTextAlign(hItem, TEXT_CF_VCENTER);
		for (i = 0; i < 8; i++)
		{
			DROPDOWN_AddString(hItem, Coler_List[i]);
		}
		hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_Size_SPINBOX);
		SPINBOX_SetButtonSize(hItem, 50);
		SPINBOX_SetRange(hItem, 1, 20);
		SPINBOX_SetValue(hItem, PaintBrush_Size);
		#if GUI_LANGUAGE > 0u             
			SPINBOX_SetFont(hItem, &GUI_FontHZ24);
		#else
			SPINBOX_SetFont(hItem, GUI_FONT_24B_1);
		#endif
		break;

	case WM_TOUCH:
		pState = (const GUI_PID_STATE*)pMsg->Data.p;    //提取PID信息
		if (pState != NULL)								//判断pState非空指针
		{
			if (pState->Pressed)						//判断是否触摸
			{
				/* 向画板内存设备绘制内容 */
				Board = GUI_MEMDEV_Select(hMemDrawingBoard);
				if (Brush_Eraser_bit == Brush_bit)
				{
					GUI_SetColor(PaintBrush_Color);
					GUI_SetPenSize(PaintBrush_Size);
				}
				else
				{
					GUI_SetColor(GUI_WHITE);
					GUI_SetPenSize((PaintBrush_Size/5)*10 + 10);
				}
				GUI_DrawPoint(pState->x + DrawingBoard_COORDINATE_X, pState->y + DrawingBoard_COORDINATE_Y);
				GUI_MEMDEV_Select(Board);
				/* 执行窗口无效化，会触发执行WM_PAINT消息 */
				WM_InvalidateRect(WM_GetClientWindow(hWin), &rBoard);
			}
		}
		break;


	case WM_PAINT:
		/* 重绘窗口 */
		GUI_SetBkColor(GUI_WHITE);
		GUI_Clear();
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRect(DrawingBoard_COORDINATE_X - 1, DrawingBoard_COORDINATE_Y - NEW_APP_BAR_HIGHT - 1, DrawingBoard_WIDTH + DrawingBoard_COORDINATE_X, DrawingBoard_HIGHT - NEW_APP_BAR_HIGHT + DrawingBoard_COORDINATE_Y);
		/* 绘制画板内存设备内容 */
		GUI_MEMDEV_Select(0);
		GUI_MEMDEV_WriteAt(hMemDrawingBoard, DrawingBoard_COORDINATE_X, DrawingBoard_COORDINATE_Y);
		break;

	case WM_NOTIFY_PARENT:
		/* 获取控件 ID */
		Id = WM_GetId(pMsg->hWinSrc);
		/* 获取消息内容 */
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_APP_DrawingBoard_PaintBrush_BUTTON:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
				/* 画笔、橡皮切换 */
				Brush_Eraser_bit = ~Brush_Eraser_bit;
				hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_PaintBrush_BUTTON);
				if (Brush_Eraser_bit == Brush_bit)
				{
					BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &bmbrush);
					BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &bmbrush);
					BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &bmbrush);
				}
				else
				{
					BUTTON_SetBitmap(hItem, BUTTON_BI_DISABLED, &bmeraser);
					BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &bmeraser);
					BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &bmeraser);
				}
				break;
			}
			break;
		case ID_APP_DrawingBoard_Color_DROPDOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_Color_DROPDOWN);
				PaintBrush_Color = GUI_Coler_List[DROPDOWN_GetSel(hItem)];
				break;
			}
			break;
		case ID_APP_DrawingBoard_Size_SPINBOX:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_VALUE_CHANGED:
				hItem = WM_GetDialogItem(hWin, ID_APP_DrawingBoard_Size_SPINBOX);
				PaintBrush_Size = SPINBOX_GetValue(hItem);
				break;
			}
			break;
		case ID_APP_DrawingBoard_Clear_BUTTON:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
				Board = GUI_MEMDEV_Select(hMemDrawingBoard);
				GUI_SetBkColor(GUI_WHITE);
				GUI_Clear();
				GUI_MEMDEV_Select(Board);
				/* 执行窗口无效化，会触发执行WM_PAINT消息 */
				WM_InvalidateRect(WM_GetClientWindow(hWin), &rBoard);
				break;
			}
			break;
		case ID_APP_DrawingBoard_Save_BUTTON:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
				#if GUI_OPERATING > 0u
					/* 记录截图前起始时间 */
					ulStart = OSTimeGet(&err);					
					/* 开启调度锁 */
					OSSchedLock(&err);				
					/* 如果SD卡中没有BoardSave文件，会进行创建 */
					result = f_mkdir("0:/BoardSave");
					/* 开始储存画板 */			
					for(i = 0; i < 100; i++)
					{
						if(Pic_Name > 100)
						{
							printf("画板储存空间已满\n\r");
							break;
						}
						sprintf(buf,"0:/BoardSave/%d.bmp",Pic_Name);
						/* 创建截图 */
						result = f_open(&file,buf, FA_WRITE|FA_CREATE_NEW);	
						/* 新建截图失败 */
						if(result != FR_OK)
						{
							if(result == FR_EXIST)
							{
								Pic_Name++; 
							}
							else
							{
								printf("画板储存失败\n\r");
								break;
							}
						}
						else
						{
							/* 向SD卡绘制BMP图片 */
							GUI_BMP_SerializeEx(_WriteByte2File, DrawingBoard_COORDINATE_X, DrawingBoard_COORDINATE_Y, DrawingBoard_WIDTH, DrawingBoard_HIGHT, &file);			
							/* 创建完成后关闭file */
							result = f_close(&file);
							/* 记录截图后时间并获取截图过程耗时 */
							ulEnd = OSTimeGet(&err);
							ulEnd -= ulStart;					
							printf("画板储存完成，耗时 = %dms\r\n", ulEnd);
							printf("画板名称:%s\n\r",buf);
							break;
						}
					}						
					/* 开启调度锁 */
					OSSchedUnlock(&err);					
				#endif
				break;
			}
			break;
		}
		break;

	case WM_DELETE:
		/* 删除画板内存设备 */
		GUI_MEMDEV_Delete(hMemDrawingBoard);
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
  * @brief 绘画板APP窗口创建函数
  * @note 无
  * @param 无
  * @retval hWin：资源表中第一个控件的句柄
  */
WM_HWIN FUN_APP_DrawingBoard_Create(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(Dialog_APP_DrawingBoard_Create, GUI_COUNTOF(Dialog_APP_DrawingBoard_Create), _cbAPP_DrawingBoard, WM_HBKWIN, 0, 0);
	return hWin;
}
/*************************** End of file ****************************/
