/*
*********************************************************************************************************
*
*	文件名称 : Solution.h
*	版    本 : V1.0
*	说    明 : 这是室内定位项目足底压力传感器阵列压力结算程序头文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-06-28     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#ifndef Solution_H
#define Solution_H

#include "main.h"

/*
*********************************************************************************************************
*	                                      宏定义
*********************************************************************************************************
*/
/* LOG打印设置 */
#if 1
	#define Solution_LOG     printf
#else
	#define Solution_LOG     
#endif



float Singel_Point_Calculation(uint32_t *voltage);              //单点压力值计算
float Conductance_Calculation(uint32_t *voltage);               //单点电导值计算
int8_t Pressure_Calculation(float *buff);            //对缓冲区中的数据进行解算，得到压力值

/* 传感器数据处理及传输 */
void Plantar_Data_Frame_Handling(uint8_t frame);        //压力传感器数据帧处理
void IMU_Data_Frame_Handling(uint8_t frame);            //IMU数据帧处理
void Plantar_Data_Frame_Transmit(void);                 //发送压力传感器数据帧
void IMU_Data_Frame_Transmit(void);                     //发送IMU数据帧

#endif

