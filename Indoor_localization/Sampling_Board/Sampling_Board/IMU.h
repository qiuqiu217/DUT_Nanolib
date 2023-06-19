/*
*********************************************************************************************************
*
*	文件名称 : IMU.h
*	版    本 : V1.0
*	说    明 : 十轴惯性传感器驱动头文件
*
*********************************************************************************************************
*/

#ifndef IMU_H
#define IMU_H

#include "includes.h"
#include "REG.h"
#include "wit_c_sdk.h"

/*
*********************************************************************************************************
*	                                      宏定义
*********************************************************************************************************
*/
/* LOG打印设置 */
#if 0
	#define IMU_LOG     COM_DEBUG.printf
#else
	#define IMU_LOG     
#endif
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
*                                       变量声明
*********************************************************************************************************
*/
//const uint32_t c_uiBaud[8] = {0,4800, 9600, 19200, 38400, 57600, 115200, 230400};

/*
*********************************************************************************************************
*	                                     函数声明
*********************************************************************************************************
*/
void IMU_Init(void);
void CopeCmdData(unsigned char ucData);
void CmdProcess(void);// 命令处理函数  
void AutoScanSensor(void);//自动匹配不同波特率的函数
void SensorUartSend(uint8_t *p_data, uint32_t uiSize);//
void SensorDataUpdate(uint32_t uiReg, uint32_t uiRegNum);
void Delayms(uint16_t ucMs);

void SensorDataRead(void);










































#endif