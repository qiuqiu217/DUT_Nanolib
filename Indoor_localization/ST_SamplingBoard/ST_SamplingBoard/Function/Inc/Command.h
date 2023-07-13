/*
*********************************************************************************************************
*
*	�ļ����� : Command.h
*	��    �� : V1.0
*	˵    �� : �������ڶ�λ��Ŀ���ڵ���ָ�����ͷ�ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-03-12     Qiuqiu&Meipo    �״η���
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
*	                                      �궨��
*********************************************************************************************************
*/
/* LOG��ӡ���� */
#if 1
	#define Command_LOG     printf
#else
	#define Command_LOG(...)
#endif

/* ����ָ�����Ͷ��� */
#define SYS_COMM        0x10
#define PLANTAR_COMM    0x20
#define IMU_COMM        0x30

/* ϵͳ����ָ��� */
#define SPP_TRAN        0x10
#define SYS_TASK        0x11

/* ѹ������������ָ��� */
#define PLA_SAMP        0x10
#define PLA_MODE        0x11
#define PLA_SELC        0x12
#define PLA_RATE        0x13
#define PLA_INSA        0x14
#define PLA_INSS        0x15

/* IMU����ָ��� */
#define IMU_SAMP        0x10
#define IMU_OUT         0x11
#define IMU_RATE        0x12
#define IMU_BAUD        0x13
#define IMU_BW          0x14
#define IMU_ACC         0x15
#define IMU_MAG         0x16

/*
*********************************************************************************************************
*	                                      ��������
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
