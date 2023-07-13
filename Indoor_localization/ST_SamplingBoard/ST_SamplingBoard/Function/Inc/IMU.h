/*
*********************************************************************************************************
*
*	�ļ����� : IMU.h
*	��    �� : V1.0
*	˵    �� : ���ڶ�λ��ĿIMU�Ĳɼ������ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-06-26         Qiuqiu      �״η���
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
*	                                      �궨��
*********************************************************************************************************
*/
/* LOG��ӡ���� */
#if 0
	#define IMU_LOG     printf
#else
	#define IMU_LOG(...)
#endif

/* IMU�������� */
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
*	                                      ��������
*********************************************************************************************************
*/
/* IMU��ʼ������ȡ */
void IMU_Init(void);
void CopeCmdData(unsigned char ucData);
void AutoScanSensor(void);                              //�Զ�ƥ�䲻ͬ�����ʵĺ���
void SensorUartSend(uint8_t *p_data, uint32_t uiSize);
void SensorDataUpdate(uint32_t uiReg, uint32_t uiRegNum);
void Delayms(uint16_t ucMs);
/* IMU���ݴ��洦�� */
void IMU_Buff_Init(void);                       //IMU���ݻ�������ʼ��
_RET_TYPE IMU_Buff_Full_Judge(void);               //�жϻ������Ƿ�д��
_RET_TYPE IMU_Read_Write_Buff_Switch(void);        //������д������
void IMU_TimeStamp(void);                       //��¼IMU�ɼ�ʱ���
_RET_TYPE IMU_Put_into_Buff(float *acc, float *gyro, float *angle, int16_t *mag, int32_t atom, int32_t hight, float *quat);         //���ɼ�������IMU���ݴ�����Ӧ������
void IMU_TimeStamp(void);                       //����IMU�ɼ�ʱ���



#endif

