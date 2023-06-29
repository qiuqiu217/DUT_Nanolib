/*
*********************************************************************************************************
*
*	�ļ����� : Function.h
*	��    �� : V1.0
*	˵    �� : ������Ҫʹ�õĸ��ֹ��ܺ�������ͷ�ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-03-29     Qiuqiu&Meipo    �״η���
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#ifndef Function_H
#define Function_H

#include "main.h"

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
float u8Arry_float(uint8_t *data, bool key);        //uint8_t����ת��Ϊfloat
void float_u8Arry(uint8_t *u8Arry, float *floatdata, bool key);     //floatת��Ϊuint8_t����
void u32_u8Arry(uint32_t u32data, uint8_t *u8Arry);         //uint32_tת��Ϊuint8_t����
uint32_t u8Arry_u32(uint8_t *u8Arry);           //uint8_t����ת��Ϊuint32_t
void u16_u8Arry(uint16_t u16data, uint8_t *u8Arry);         //uint16_tת��Ϊuint8_t����
uint16_t u8Arry_u16(uint8_t *u8Arry);       //uint8_t����ת��Ϊuint16_t


#endif

