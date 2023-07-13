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
*                                          ��׼��
*********************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
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
#include "stm32f4xx_ll_rtc.h"
/*
*********************************************************************************************************
*                                           RTOS
*********************************************************************************************************
*/
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
/*
*********************************************************************************************************
*                                           BSP
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
#include "Plantar.h"            //���ѹ��������
#include "IMU.h"                //���Դ�����
#include "SPP.h"                //��������
#include "Command.h"            //ָ���
#include "Solution.h"           //���ݴ������
#include "Function.h"           //���ܺ���

/*
*********************************************************************************************************
*	                                   ϵͳ��غ궨��
*********************************************************************************************************
*/
/* LOG��ӡ���� */
#if 0
	#define TASK_LOG     printf
#else
	#define TASK_LOG(...)
#endif

/* ʹ�ܶ��� */
#define ENABLE      1
#define DISABLE     0
/* ��ƽ���� */
#define SET         1
#define RESET       0
/* ����ֵ�궨�� */
#define RET_ERROR       -1
#define RET_INVALID     0
#define RET_OK          1

/* ���ں궨�� */
#define DEBUG_COM   COM1
#define SPP_COM     COM3
#define IMU_COM     COM2

/* Debug���ڽ��ջ�������С */
#define RX_BUFF_SIZE            256
/* ÿ��Buff�������֡���� */
#define FRAME_IN_BUFF           10

/* ѹ�����������вɼ�ģʽ���� */
#define ARRAY_SAMPLINGMODE              0u       //��������ѭ������
#define SINGLEPOINT_SAMPLINGMODE        1u       //ָ������ѭ������
#define INSTRUCTIONS_ARRAYMODE          2u       //�յ�ָ��ִ�����в���
#define INSTRUCTIONS_SINGLEMODE         3u       //�յ�ָ��ִ�е������
/* Debug������Ϣ���� */
#define DEBUG_UART_COMMAND              0u       //Debug���ڽ��յ�����
#define BLUETOOTH_COMMAND               1u       //�������ڽ��յ�����
/* ָ�������Ϣ���� */
#define SINGLE_INSTRUCTION              0u       //����ѹ�����ݷ������
#define ARRAY_INSTRUCTION               1u       //����IMU���ݷ������
/* ���ݲɼ��¼��鶨�� */
#define PLANTAR_SAMPLING                (1 << 0)
#define IMU_SAMPLING                    (1 << 1)
/* ���������¼��鶨�� */
#define BLUETOOTH_CONNECT               (1 << 0)
#define BLUETOOTH_TRANSFER              (1 << 1)
#define BLUETOOTH_START_TRANSFER        (BLUETOOTH_CONNECT | BLUETOOTH_TRANSFER)

/*
*********************************************************************************************************
*	                                      ���ݽṹ����
*********************************************************************************************************
*/
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

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
void Error_Handler(void);

/**
* @brief Get_TimeStamp ��ȡϵͳ����ʱ���
* @retval uint32_t  ϵͳ����ʱ��(ms)
*/
inline uint32_t Get_TimeStamp(void)
{
    return HAL_GetTick();
}

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
