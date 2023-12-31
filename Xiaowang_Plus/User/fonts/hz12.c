/*
*********************************************************************************************************
*
*	模块名称 : 汉字点阵字库。有效显示区 高11x宽11, 最右一列留白
*	文件名称 : hz12.c
*	版    本 : V1.0
*	说    明 : 只包含本程序用到汉字字库
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2011-09-08 armfly  ST固件库V3.5.0版本。
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "fonts.h"

#ifdef USE_SMALL_FONT
/*
	FLASH中内嵌小字库，只包括本程序用到的汉字点阵
	每行点阵数据，头2字节是汉子的内码，后面32字节是16点阵汉子的字模数据。
*/

unsigned char const g_Hz12[] = {
/* 最后一行必须用0xFF,0xFF结束，这是字库数组结束标志 */
0xFF,0xFF

};

#else
	unsigned char const g_Hz12[] = {0xFF, 0xFF};
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
