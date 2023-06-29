/*
*********************************************************************************************************
*
*	�ļ����� : Solution.h
*	��    �� : V1.0
*	˵    �� : �������ڶ�λ��Ŀ���ѹ������������ѹ���������ͷ�ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-06-28     Qiuqiu&Meipo    �״η���
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#ifndef Solution_H
#define Solution_H

#include "main.h"

/*
*********************************************************************************************************
*	                                      �궨��
*********************************************************************************************************
*/
/* LOG��ӡ���� */
#if 1
	#define Solution_LOG     printf
#else
	#define Solution_LOG     
#endif



float Singel_Point_Calculation(uint32_t *voltage);              //����ѹ��ֵ����
float Conductance_Calculation(uint32_t *voltage);               //����絼ֵ����
int8_t Pressure_Calculation(float *buff);            //�Ի������е����ݽ��н��㣬�õ�ѹ��ֵ

/* ���������ݴ������� */
void Plantar_Data_Frame_Handling(uint8_t frame);        //ѹ������������֡����
void IMU_Data_Frame_Handling(uint8_t frame);            //IMU����֡����
void Plantar_Data_Frame_Transmit(void);                 //����ѹ������������֡
void IMU_Data_Frame_Transmit(void);                     //����IMU����֡

#endif

