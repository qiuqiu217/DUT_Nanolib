/*
*********************************************************************************************************
*
*	文件名称 : Plantar.h
*	版    本 : V1.0
*	说    明 : 这是室内定位项目压力传感器阵列的采集程序头文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-10     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#ifndef PLANTAR_H
#define PLANTAR_H

/* 足底压力传感器阵列数量配置 */
#define SENSOR_NUM_TOTAL 45 
#define SENSOR_NUM_LINE 10
#define SENSOR_NUM_COLUMN 6

#include "main.h"


/* LOG打印设置 */
#if 1
	#define PLANTAR_LOG     printf
#else
	#define PLANTAR_LOG     (...)
#endif

#define ENABLE      1
#define DISABLE     0


/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
static inline void Row_Select(uint8_t Row);
static inline void Column_Select(uint8_t Column);
static void Channel_Sampling(uint8_t Row, uint8_t Column);
void Array_Scanning_Sampling();



#endif