/*
*********************************************************************************************************
*
*	文件名称 : IMU.h
*	版    本 : V1.0
*	说    明 : 室内定位项目IMU的采集程序文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-06-26         Qiuqiu      首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/
#ifndef IMU_H
#define IMU_H

#include "main.h"
#include "REG.h"
#include "wit_c_sdk.h"

typedef int8_t _RET_TYPE;
/*
*********************************************************************************************************
*	                                      宏定义
*********************************************************************************************************
*/
/* LOG打印设置 */
#if 0
	#define IMU_LOG     printf
#else
	#define IMU_LOG(...)
#endif

/* IMU数据类型 */
#define ACC_UPDATE		0x01
#define GYRO_UPDATE		0x02
#define ANGLE_UPDATE	0x04
#define MAG_UPDATE		0x08
#define ATMO_UPDATE     0x10
#define HEIGHT_UPDATE   0x20
#define QUAT_UPDATE     0x40
#define READ_UPDATE		0x80

/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
/* IMU初始化及读取 */
void IMU_Init(void);
void CopeCmdData(unsigned char ucData);
void AutoScanSensor(void);                              //自动匹配不同波特率的函数
void SensorUartSend(uint8_t *p_data, uint32_t uiSize);
void SensorDataUpdate(uint32_t uiReg, uint32_t uiRegNum);
void Delayms(uint16_t ucMs);
/* IMU数据储存处理 */
void IMU_Buff_Init(void);                       //IMU数据缓冲区初始化
_RET_TYPE IMU_Buff_Full_Judge(void);               //判断缓冲区是否写满
_RET_TYPE IMU_Read_Write_Buff_Switch(void);        //交换读写缓冲区
void IMU_TimeStamp(void);                       //记录IMU采集时间戳
_RET_TYPE IMU_Put_into_Buff(float *acc, float *gyro, float *angle, int16_t *mag, int32_t atom, int32_t hight, float *quat);         //将采集出来的IMU数据存入相应缓冲区
void IMU_TimeStamp(void);                       //存入IMU采集时间戳



#endif

