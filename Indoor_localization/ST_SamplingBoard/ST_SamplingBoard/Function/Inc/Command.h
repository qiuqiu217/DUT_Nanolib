/*
*********************************************************************************************************
*
*	文件名称 : Command.h
*	版    本 : V1.0
*	说    明 : 这是室内定位项目串口调试指令集程序头文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-12     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/
#ifndef COMMAND_H
#define COMMAND_H

#include  "cJSON.h"
#include "main.h"

typedef uint8_t _COMM_TYPE;
typedef int8_t _RET_TYPE;
/*
*********************************************************************************************************
*	                                      宏定义
*********************************************************************************************************
*/
/* LOG打印设置 */
#if 1
	#define Command_LOG     printf
#else
	#define Command_LOG(...)
#endif

/* 控制指令类型定义 */
#define SYS_COMM        0x10
#define PLANTAR_COMM    0x20
#define IMU_COMM        0x30

/* 系统控制指令定义 */
#define SPP_TRAN        0x10
#define SYS_TASK        0x11

/* 压力传感器控制指令定义 */
#define PLA_SAMP        0x10
#define PLA_MODE        0x11
#define PLA_SELC        0x12
#define PLA_RATE        0x13
#define PLA_INSA        0x14
#define PLA_INSS        0x15

/* IMU控制指令定义 */
#define IMU_SAMP        0x10
#define IMU_OUT         0x11
#define IMU_RATE        0x12
#define IMU_BAUD        0x13
#define IMU_BW          0x14
#define IMU_ACC         0x15
#define IMU_MAG         0x16

/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
_RET_TYPE Command_Parsing(uint8_t *RX_Buf);

_RET_TYPE Change_Bluetooth_Transfer_State(_COMM_TYPE set);
_RET_TYPE SYS_Task_RunTimeStats(_COMM_TYPE set);

_RET_TYPE Change_Plantar_Sampling_State(_COMM_TYPE set);
_RET_TYPE Change_Plantar_SamplingMode(_COMM_TYPE set);
_RET_TYPE Change_Plantar_Selection(_COMM_TYPE set);
_RET_TYPE Change_Plantar_SamplingRate(_COMM_TYPE set);
_RET_TYPE Plantar_Array_Sampling(_COMM_TYPE set); 
_RET_TYPE Plantar_Single_Sampling(_COMM_TYPE set);

_RET_TYPE Change_IMU_Sampling_State(_COMM_TYPE set);
_RET_TYPE Change_IMU_Content(_COMM_TYPE set);
_RET_TYPE Change_IMU_OutputRate(_COMM_TYPE set);
_RET_TYPE Change_IMU_Baud(_COMM_TYPE set);
_RET_TYPE Change_IMU_Bandwidth(_COMM_TYPE set);
_RET_TYPE Change_IMU_AccCali(_COMM_TYPE set);
_RET_TYPE Change_IMU_MagCali(_COMM_TYPE set);

#endif
