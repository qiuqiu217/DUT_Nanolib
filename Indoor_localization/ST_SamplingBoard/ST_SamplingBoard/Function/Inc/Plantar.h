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

#include "main.h"

typedef int8_t _RET_TYPE;
/*
*********************************************************************************************************
*	                                   �궨��
*********************************************************************************************************
*/
/* LOG��ӡ���� */
#if 0
	#define PLANTAR_LOG     printf
#else
	#define PLANTAR_LOG(...)
#endif

/* ���ѹ�������������������� */
#define SENSOR_NUM_TOTAL 45 
#define SENSOR_NUM_ROW 10
#define SENSOR_NUM_COLUMN 6

/*
*********************************************************************************************************
*	                                      ���ݽṹ����
*********************************************************************************************************
*/
/* ѹ���������ɼ����ýṹ�� */
typedef struct
{
    uint8_t Sampling_Mode;      //ѹ�����д���������ģʽ
    uint8_t Selection_Row;      //�������ѡͨ��
    uint8_t Selection_Column;   //�������ѡͨ��
    uint16_t Sampling_Delay;    //ѹ��������������ʱ
    
}PLANTAR_S;

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/**
 * @brief nop_delay
 */
void inline nop_delay(uint8_t num)
{
    while(num)
    {
        __nop();
        num--;
    }
}

/* ���ѹ���������ɼ����� */
void Plantar_SettingsInit(void);
int8_t Plantar_ChannelJudge(uint8_t Row, uint8_t Column);
int8_t Plantar_ChannelChange(uint8_t Row, uint8_t Column);
int8_t Plantar_Mode_Change(uint8_t Mode);
int8_t Plantar_Rate_Change(uint8_t Rate);

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
void Plantar_TimeStamp(void);       //����ѹ���������ɼ�ʱ���

#endif
