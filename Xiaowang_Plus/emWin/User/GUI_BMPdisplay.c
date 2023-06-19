/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	版    本 : V1.0
*	说    明 : 本实验主要学习emWin的BMP图片显示，本程序代码提供了两种显示方法，用户可以宏定义进行切换
*              1. 直接加载到SDRAM进行显示选择：Method1
*              2. 边加载边显示的方式选择      :Method2
*              实际项目中强烈推荐方式一，即Method1，采用V7板子H7+32位SDRAM，LTDC颜色格式配置为RGB565.
*              绘制800*480*2的图片可以达到8ms到11ms一帧(开Cache，8ms)，基本能够胜任大部分嵌入式GUI项目。
*              
*	修改记录 :
*		版本号   日期         作者          说明
*		V1.0    2021-05-23   Eric2013  	    首版    
*                                     
*	Copyright (C), 2021-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "GUI_BMPdisplay.h"

/*
*********************************************************************************************************
*	                                	变量
*********************************************************************************************************
*/
/* 采用函数GUI_BMP_DrawEx显示BMP图片时，此数组的大小一定要能够存储图片的一行像素数据，切记！！！
   比如：BMP图片是32位颜色深度，最大支持长度是1024个像素，那么存储一行像素数据就需要1024*4字节。
*/
static char acBuffer[1024*4];

/*
*********************************************************************************************************
*	函 数 名: _GetData
*	功能说明: 被函数GUI_BMP_DrawEx调用
*	形    参：p             FIL类型数据
*             NumBytesReq   请求读取的字节数
*             ppData        数据指针
*             Off           如果Off = 1，那么将重新从起始位置读取                 
*	返 回 值: 返回读取的字节数
*********************************************************************************************************
*/
int _GetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) 
{
	static int FileAddress = 0;
	UINT NumBytesRead;
	FIL *PicFile;

	PicFile = (FIL *)p;

	/*
	* 检测缓存大小
	*/
	if (NumBytesReq > sizeof(acBuffer)) {
	NumBytesReq = sizeof(acBuffer);
	}

	/*
	* 设置读取位置
	*/
	if(Off == 1) FileAddress = 0;
	else FileAddress = Off;
	result =f_lseek(PicFile, FileAddress);

	/*
	* 读取数据到缓存
	*/
	result = f_read(PicFile, acBuffer, NumBytesReq, &NumBytesRead);

	/*
	* 让指针ppData指向读取的数据
	*/
	*ppData = (const U8 *)acBuffer;

	/*
	* 返回读取的字节数
	*/
	return NumBytesRead;
}

/*
*********************************************************************************************************
*	函 数 名: _ShowBMPEx
*	功能说明: 显示BMP图片
*	形    参: sFilename  要读取的文件名
*	返 回 值: 无
*********************************************************************************************************
*/
void _ShowBMPEx(const char *sFilename) 
{
	/* 打开文件 */		
	result = f_open(&file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (result != FR_OK)
	{
		return;
	}
	
	/* 绘制BMP图片 */
	GUI_BMP_DrawEx(_GetData, &file, 0, 0);
	
	/* 关闭文件 */
	f_close(&file);
}

/*
*********************************************************************************************************
*	函 数 名: _ShowBMPAt
*	功能说明: 显示BMP图片
*	形    参: sFilename  要读取的文件名
*                     x  要显示的x轴坐标位置
*                     y  要显示的y轴坐标位置
*	返 回 值: 返回绘制了BMP图片的内存设备句柄。
*********************************************************************************************************
*/
GUI_HMEM _ShowBMPAt(const char *sFilename, int x, int y) 
{
	char *_acBuffer;
	int XSize, YSize;
	GUI_HMEM hMem;
	GUI_MEMDEV_Handle hMemBMP;

	/* 打开文件 */		
	result = f_open(&file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (result != FR_OK)
	{
		return 0;
	}
	 
	/* 申请一块内存空间 并且将其清零 */
	hMem = GUI_ALLOC_AllocZero(file.obj.objsize);
	
	/* 将申请到内存的句柄转换成指针类型 */
	_acBuffer = GUI_ALLOC_h2p(hMem);

	/* 读取文件到动态内存 */
	result = f_read(&file, _acBuffer, file.obj.objsize, &bw);
	if (result != FR_OK)
	{
		return 0;
	}
	
	XSize = GUI_BMP_GetXSize(_acBuffer);
	YSize = GUI_BMP_GetYSize(_acBuffer);
	
	/* 创建内存设备，并将BMP图片绘制到此内存设备里面，此内存设备要在主程序中用到
	   所以退出此函数前，不要释放。
	*/
	hMemBMP = GUI_MEMDEV_CreateEx(0, 0, XSize, YSize, GUI_MEMDEV_HASTRANS);
	GUI_MEMDEV_Select(hMemBMP);
	GUI_BMP_Draw(_acBuffer, x, y);
	GUI_MEMDEV_Select(0);

	/* 释放动态内存hMem */
	GUI_ALLOC_Free(hMem);
	
	/* 关闭文件 */
	f_close(&file);
	
	return hMemBMP;
}

/*
*********************************************************************************************************
*	函 数 名: _DrawBMPasBackground
*	功能说明: 显示BMP图片作为背景图片
*	形    参: sFilename  要读取的文件名
*                     x  要显示的x轴坐标位置
*                     y  要显示的y轴坐标位置
*               hMemBMP  背景图片存储设备
*	返 回 值: 0为绘制失败 1为绘制成功
*********************************************************************************************************
*/
uint8_t _DrawBMPasBackground(const char *sFilename, int x, int y, GUI_HMEM hMemBMP) 
{
	char *_acBuffer;
	GUI_HMEM hMem;

	/* 打开文件 */		
	result = f_open(&file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (result != FR_OK)
	{
		return 0;
	}
	 
	/* 申请一块内存空间 并且将其清零 */
	hMem = GUI_ALLOC_AllocZero(file.obj.objsize);
	
	/* 将申请到内存的句柄转换成指针类型 */
	_acBuffer = GUI_ALLOC_h2p(hMem);

	/* 读取文件到动态内存 */
	result = f_read(&file, _acBuffer, file.obj.objsize, &bw);
	if (result != FR_OK)
	{
		return 0;
	}

	/* 创建内存设备，并将BMP图片绘制到此内存设备里面，此内存设备要在主程序中用到
	   所以退出此函数前，不要释放。
	*/
	GUI_MEMDEV_Select(hMemBMP);
	GUI_BMP_Draw(_acBuffer, x, y);
	GUI_MEMDEV_Select(0);

	/* 释放动态内存hMem */
	GUI_ALLOC_Free(hMem);
	
	/* 关闭文件 */
	f_close(&file);
	return 1;
}


