/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                         ��׼��
*********************************************************************************************************
*/
#include <stdio.h>
#include <stdbool.h>
/*
*********************************************************************************************************
*                                        HAL�⼰LL��
*********************************************************************************************************
*/
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
/*
*********************************************************************************************************
*                                         RTOS
*********************************************************************************************************
*/
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"
/*
*********************************************************************************************************
*                                          BSP
*********************************************************************************************************
*/
#include "gpio.h"
#include "usart.h"
#include "adc.h"
#include "tim.h"
/*
*********************************************************************************************************
*                                        FUNCTION
*********************************************************************************************************
*/
#include "Plantar.h"


/* LOG��ӡ���� */
#if 1
	#define TASK_LOG     printf
#else
	#define TASK_LOG     (...)
#endif

/* USER CODE BEGIN EFP */

/* ÿ��Buff�������֡���� */
#define FRAME_IN_BUFF           10

/* Debug������Ϣ���� */
#define DEBUG_UART_COMMAND              0       //Debug���ڽ��յ�����
#define BLUETOOTH_COMMAND               1       //�������ڽ��յ�����
/* ���ݴ�����Ϣ���� */
#define PRESSURE_SOLUTION               0       //����ѹ�����ݴ���
#define IMU_SOLUTION                    1       //����IMU���ݴ���
/* ���ݷ�����Ϣ���� */
#define PRESSURE_TRANSFER               0       //����ѹ�����ݷ���
#define IMU_TRANSFER                    1       //����IMU���ݷ���
/* ���ݷ��������Ϣ���� */
#define PRESSURE_DONE                   0       //����ѹ�����ݷ������
#define IMU_DONE                        1       //����IMU���ݷ������
/* ָ�������Ϣ���� */
#define SINGLE_INSTRUCTION              0       //����ѹ�����ݷ������
#define ARRAY_INSTRUCTION               1       //����IMU���ݷ������
/* ���ݲɼ��¼��鶨�� */
#define PLANTAR_SAMPLING                (1 << 0)
#define IMU_SAMPLING                    (1 << 1)
/* ���������¼��鶨�� */
#define BLUETOOTH_CONNECT               (1 << 0)
#define BLUETOOTH_TRANSFER              (1 << 1)
#define BLUETOOTH_START_TRANSFER        (BLUETOOTH_CONNECT | BLUETOOTH_TRANSFER)
/* ѹ�����������вɼ�ģʽ�¼��鶨�� */
#define ARRAY_SAMPLINGMODE              (1 << 0)       //��������ѭ������
#define SINGLEPOINT_SAMPLINGMODE        (1 << 1)       //ָ������ѭ������
#define INSTRUCTIONS_ARRAYMODE          (1 << 2)       //�յ�ָ��ִ�����в���
#define INSTRUCTIONS_SINGLEMODE         (1 << 3)       //�յ�ָ��ִ�е������
#define PLANTAR_SAMPLING_MODE           (ARRAY_SAMPLINGMODE | SINGLEPOINT_SAMPLINGMODE | INSTRUCTIONS_ARRAYMODE | INSTRUCTIONS_SINGLEMODE)

/* ѹ���������������ݻ������ṹ�嶨�� */
typedef struct     /* �������ṹ�嶨�� */
{
	uint16_t *Read_Buff;        //ָ���Buff��ָ��
    uint16_t *Write_Buff;       //ָ��дBuff��ָ��    
    uint32_t *Read_Time_Buff;   //ָ���ʱ�����ָ��
    uint32_t *Write_Time_Buff;  //ָ��дʱ�����ָ��
    uint8_t Write_Frame;        //д��֡��
} Plantar_Buff_TypeDef;

/* IMU���ݻ������ṹ�嶨�� */
typedef struct     /* �������ṹ�嶨�� */
{
    float *Read_AccBuff;        //ָ����ٶȶ�Buff��ָ��
    float *Write_AccBuff;       //ָ����ٶ�дBuff��ָ��
    float *Read_GyroBuff;       //ָ�������Ƕ�Buff��ָ��
    float *Write_GyroBuff;      //ָ��������дBuff��ָ��
    float *Read_AngleBuff;      //ָ����Ƕ�Buff��ָ��
    float *Write_AngleBuff;     //ָ�����дBuff��ָ��
    int16_t *Read_MagBuff;      //ָ������ƶ�Buff��ָ��
    int16_t *Write_MagBuff;     //ָ�������дBuff��ָ��
    int32_t *Read_AtmoBuff;     //ָ����ѹ�ƶ�Buff��ָ��
    int32_t *Write_AtmoBuff;    //ָ����ѹ��дBuff��ָ��
    int32_t *Read_HightBuff;    //ָ��߶ȶ�Buff��ָ��
    int32_t *Write_HightBuff;   //ָ��߶�дBuff��ָ��
    float *Read_QuatBuff;       //ָ����Ԫ����Buff��ָ��
    float *Write_QuatBuff;      //ָ����Ԫ��дBuff��ָ��
    uint32_t *Read_Time_Buff;   //ָ���ʱ�����ָ��
    uint32_t *Write_Time_Buff;  //ָ��дʱ�����ָ��
    uint8_t Write_Frame;        //д��֡��
} IMU_Buff_TypeDef;

/* ѹ���������ɼ��ṹ�嶨�� */
typedef struct
{
    uint16_t Plantar_Sampling_Delay;        //ѹ��������������ʱ
    uint8_t Selection_Row;                  //ѹ������������ѡͨ��
    uint8_t Selection_Column;               //ѹ������������ѡͨ��
}Plantar_Sampling_TypeDef;

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
void Error_Handler(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
