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
*                                         标准库
*********************************************************************************************************
*/
#include <stdio.h>
#include <stdbool.h>
/*
*********************************************************************************************************
*                                        HAL库及LL库
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


/* LOG打印设置 */
#if 1
	#define TASK_LOG     printf
#else
	#define TASK_LOG     (...)
#endif

/* USER CODE BEGIN EFP */

/* 每个Buff储存多少帧数据 */
#define FRAME_IN_BUFF           10

/* Debug命令消息定义 */
#define DEBUG_UART_COMMAND              0       //Debug串口接收到命令
#define BLUETOOTH_COMMAND               1       //蓝牙串口接收到命令
/* 数据处理消息定义 */
#define PRESSURE_SOLUTION               0       //进行压力数据处理
#define IMU_SOLUTION                    1       //进行IMU数据处理
/* 数据发送消息定义 */
#define PRESSURE_TRANSFER               0       //进行压力数据发送
#define IMU_TRANSFER                    1       //进行IMU数据发送
/* 数据发送完成消息定义 */
#define PRESSURE_DONE                   0       //进行压力数据发送完成
#define IMU_DONE                        1       //进行IMU数据发送完成
/* 指令采样消息定义 */
#define SINGLE_INSTRUCTION              0       //进行压力数据发送完成
#define ARRAY_INSTRUCTION               1       //进行IMU数据发送完成
/* 数据采集事件组定义 */
#define PLANTAR_SAMPLING                (1 << 0)
#define IMU_SAMPLING                    (1 << 1)
/* 蓝牙传输事件组定义 */
#define BLUETOOTH_CONNECT               (1 << 0)
#define BLUETOOTH_TRANSFER              (1 << 1)
#define BLUETOOTH_START_TRANSFER        (BLUETOOTH_CONNECT | BLUETOOTH_TRANSFER)
/* 压力传感器阵列采集模式事件组定义 */
#define ARRAY_SAMPLINGMODE              (1 << 0)       //传感阵列循环采样
#define SINGLEPOINT_SAMPLINGMODE        (1 << 1)       //指定单点循环采样
#define INSTRUCTIONS_ARRAYMODE          (1 << 2)       //收到指令执行阵列采样
#define INSTRUCTIONS_SINGLEMODE         (1 << 3)       //收到指令执行单点采样
#define PLANTAR_SAMPLING_MODE           (ARRAY_SAMPLINGMODE | SINGLEPOINT_SAMPLINGMODE | INSTRUCTIONS_ARRAYMODE | INSTRUCTIONS_SINGLEMODE)

/* 压力传感器阵列数据缓冲区结构体定义 */
typedef struct     /* 缓冲区结构体定义 */
{
	uint16_t *Read_Buff;        //指向读Buff的指针
    uint16_t *Write_Buff;       //指向写Buff的指针    
    uint32_t *Read_Time_Buff;   //指向读时间戳的指针
    uint32_t *Write_Time_Buff;  //指向写时间戳的指针
    uint8_t Write_Frame;        //写入帧数
} Plantar_Buff_TypeDef;

/* IMU数据缓冲区结构体定义 */
typedef struct     /* 缓冲区结构体定义 */
{
    float *Read_AccBuff;        //指向加速度读Buff的指针
    float *Write_AccBuff;       //指向加速度写Buff的指针
    float *Read_GyroBuff;       //指向陀螺仪读Buff的指针
    float *Write_GyroBuff;      //指向陀螺仪写Buff的指针
    float *Read_AngleBuff;      //指向倾角读Buff的指针
    float *Write_AngleBuff;     //指向倾角写Buff的指针
    int16_t *Read_MagBuff;      //指向磁力计读Buff的指针
    int16_t *Write_MagBuff;     //指向磁力计写Buff的指针
    int32_t *Read_AtmoBuff;     //指向气压计读Buff的指针
    int32_t *Write_AtmoBuff;    //指向气压计写Buff的指针
    int32_t *Read_HightBuff;    //指向高度读Buff的指针
    int32_t *Write_HightBuff;   //指向高度写Buff的指针
    float *Read_QuatBuff;       //指向四元数读Buff的指针
    float *Write_QuatBuff;      //指向四元数写Buff的指针
    uint32_t *Read_Time_Buff;   //指向读时间戳的指针
    uint32_t *Write_Time_Buff;  //指向写时间戳的指针
    uint8_t Write_Frame;        //写入帧数
} IMU_Buff_TypeDef;

/* 压力传感器采集结构体定义 */
typedef struct
{
    uint16_t Plantar_Sampling_Delay;        //压力传感器采样延时
    uint8_t Selection_Row;                  //压力传感器单点选通行
    uint8_t Selection_Column;               //压力传感器单点选通列
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
