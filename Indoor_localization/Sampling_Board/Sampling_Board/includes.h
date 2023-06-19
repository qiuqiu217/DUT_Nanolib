/*
*********************************************************************************************************
*
*	文件名称 : includes.h
*	版    本 : V1.0
*	说    明 : 这是室内定位项目总头文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-12     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#ifndef INCLUDES_H
#define INCLUDES_H

/* 串口定义 */
#define COM_DEBUG       Serial        //调试串口-向电脑输出的
#define COM_IMU         Serial1       //UART1-----与IMU通讯的串口
/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string>
/*
*********************************************************************************************************
*                                         其它库
*********************************************************************************************************
*/
#include <Arduino.h>            /* arduino官方库 */
#include <ArduinoJson.h>        /* CJSON */
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <iostream>
#include <algorithm>
#include <cmath>
/*
*********************************************************************************************************
*                                        APP / BSP
*********************************************************************************************************
*/
#include <EEPROM.h>             /* FLASH */
#include "Plantar.h"            /* 压力传感器阵列 */
#include "Command.h"            /* 串口命令 */
#include "Function.h"           /* 功能函数 */
#include "Solution.h"           /* 传感数据处理 */
#include "imu.h"                /* 惯性传感器 */
#include "BluetoothSerial.h"    /* 蓝牙 */

/*
*********************************************************************************************************
*	                                      宏定义
*********************************************************************************************************
*/
/* 任务绑定CPU定义 */
#define PRO_CPU       0           //绑定CPU0
#define APP_CPU       1           //绑定CPU1
/* 修改全局变量是否启用互斥锁 */
#define GLOBAL_VARIABLE_MUTEX_ENABLE    0 
/* 修改Plantar缓冲区是否启用互斥锁 */
#define PLANTAR_BUFF_MUTEX_ENABLE       0
/* 压力传感器阵列ADC采集方式 */
#define ARRAY_SAMPLINGMODE              0       //传感阵列循环采样
#define SINGLEPOINT_SAMPLINGMODE        1       //指定单点循环采样
#define INSTRUCTIONS_SAMPLINGMODE       2       //收到指令执行采样
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
/*
*********************************************************************************************************
*	                                     结构体定义
*********************************************************************************************************
*/
/* 压力传感器阵列数据缓冲区结构体定义 */
typedef struct     /* 缓冲区结构体定义 */
{
	uint32_t *Read_Buff;        //指向读Buff的指针
    uint32_t *Write_Buff;       //指向写Buff的指针    
	uint16_t Read;              //读Buff的读指针
	uint16_t Write;             //写Buff的写指针
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



#endif
