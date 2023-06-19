/*
*********************************************************************************************************
*
*	文件名称 : Function.h
*	版    本 : V1.0
*	说    明 : 这是需要使用的各种功能函数程序头文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-29     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#ifndef Function_H
#define Function_H

#include "includes.h"

/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
float u8Arry_float(uint8_t *data, bool key);        //uint8_t数组转化为float
void float_u8Arry(uint8_t *u8Arry, float *floatdata, bool key);     //float转化为uint8_t数组
void u32_u8Arry(uint32_t u32data, uint8_t *u8Arry);         //uint32_t转化为uint8_t数组
uint32_t u8Arry_u32(uint8_t *u8Arry);           //uint8_t数组转化为uint32_t
void u16_u8Arry(uint16_t u16data, uint8_t *u8Arry);         //uint16_t转化为uint8_t数组
uint16_t u8Arry_u16(uint8_t *u8Arry);       //uint8_t数组转化为uint16_t


#endif
