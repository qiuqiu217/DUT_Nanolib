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
*                                          标准库
*********************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
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
#include "Plantar.h"            //足底压力传感器
#include "IMU.h"                //惯性传感器
#include "SPP.h"                //蓝牙串口
#include "Command.h"            //指令处理
#include "Solution.h"           //数据处理解析
#include "Function.h"           //功能函数

/*
*********************************************************************************************************
*	                                   系统相关宏定义
*********************************************************************************************************
*/
/* LOG打印设置 */
#if 0
	#define TASK_LOG     printf
#else
	#define TASK_LOG(...)
#endif

/* 使能定义 */
#define ENABLE      1
#define DISABLE     0
/* 电平定义 */
#define SET         1
#define RESET       0
/* 返回值宏定义 */
#define RET_ERROR       -1
#define RET_INVALID     0
#define RET_OK          1

/* 串口宏定义 */
#define DEBUG_COM   COM1
#define SPP_COM     COM3
#define IMU_COM     COM2

/* Debug串口接收缓冲区大小 */
#define RX_BUFF_SIZE            256
/* 每个Buff储存多少帧数据 */
#define FRAME_IN_BUFF           10

/* 压力传感器阵列采集模式定义 */
#define ARRAY_SAMPLINGMODE              0u       //传感阵列循环采样
#define SINGLEPOINT_SAMPLINGMODE        1u       //指定单点循环采样
#define INSTRUCTIONS_ARRAYMODE          2u       //收到指令执行阵列采样
#define INSTRUCTIONS_SINGLEMODE         3u       //收到指令执行单点采样
/* Debug命令消息定义 */
#define DEBUG_UART_COMMAND              0u       //Debug串口接收到命令
#define BLUETOOTH_COMMAND               1u       //蓝牙串口接收到命令
/* 指令采样消息定义 */
#define SINGLE_INSTRUCTION              0u       //进行压力数据发送完成
#define ARRAY_INSTRUCTION               1u       //进行IMU数据发送完成
/* 数据采集事件组定义 */
#define PLANTAR_SAMPLING                (1 << 0)
#define IMU_SAMPLING                    (1 << 1)
/* 蓝牙传输事件组定义 */
#define BLUETOOTH_CONNECT               (1 << 0)
#define BLUETOOTH_TRANSFER              (1 << 1)
#define BLUETOOTH_START_TRANSFER        (BLUETOOTH_CONNECT | BLUETOOTH_TRANSFER)

/*
*********************************************************************************************************
*	                                      数据结构定义
*********************************************************************************************************
*/
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

/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
void Error_Handler(void);

/**
* @brief Get_TimeStamp 获取系统运行时间戳
* @retval uint32_t  系统运行时间(ms)
*/
inline uint32_t Get_TimeStamp(void)
{
    return HAL_GetTick();
}

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
