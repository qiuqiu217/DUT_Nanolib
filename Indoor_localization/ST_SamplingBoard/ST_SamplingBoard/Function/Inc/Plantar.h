/*
*********************************************************************************************************
*
*	�ļ����� : Plantar.h
*	��    �� : V1.0
*	˵    �� : �������ڶ�λ��Ŀѹ�����������еĲɼ�����ͷ�ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-03-10     Qiuqiu&Meipo    �״η���
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#ifndef PLANTAR_H
#define PLANTAR_H

/* ���ѹ�������������������� */
#define SENSOR_NUM_TOTAL 45 
#define SENSOR_NUM_ROW 10
#define SENSOR_NUM_COLUMN 6

#include "main.h"


/* LOG��ӡ���� */
#if 1
	#define PLANTAR_LOG     printf
#else
	#define PLANTAR_LOG     (...)
#endif

#define ENABLE      1
#define DISABLE     0


/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/* ���ѹ�����������ݲɼ� */
static inline void Row_Select(uint8_t Row);
static inline void Column_Select(uint8_t Column);
static inline void Channel_Disable(void);
int8_t Singal_Point_Sampling(uint8_t Row, uint8_t Column, uint16_t *buf);
int8_t Array_Scanning_Sampling(void);

/* ���ѹ�����������ݴ��� */
void Plantar_Buff_Init(void);
int8_t Plantar_Buff_Full_Judge(void);
int8_t Plantar_Read_Write_Buff_Switch(void);

#endif
