/*
*********************************************************************************************************
*
*	文件名称 : Solution.h
*	版    本 : V1.0
*	说    明 : 这是室内定位项目足底压力传感器阵列压力结算程序头文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-15     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#ifndef Solution_H
#define Solution_H

#include "includes.h"

/*
*********************************************************************************************************
*	                                      宏定义
*********************************************************************************************************
*/
/* LOG打印设置 */
#if 1
	#define Solution_LOG     COM_DEBUG.printf
#else
	#define Solution_LOG     
#endif

/* 每个Buff储存多少帧数据 */
#define FRAME_IN_BUFF           10

/*
*********************************************************************************************************
*	                                      类声明
*********************************************************************************************************
*/
class Plantar_Solution 
{    
    public:
        Plantar_Solution();         //构造函数
        ~Plantar_Solution();        //析构函数
        void Buff_Init(void);       //压力传感器阵列数据缓冲区初始化
        int8_t Buff_Full_Judge(void);               //判断缓冲区是否写满
        int8_t Read_Write_Buff_Switch(void);        //交换读写缓冲区
        int8_t Put_into_Buff(uint32_t voltage_p[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN]);            //将采集出来的压力传感器阵列存入相应缓冲区
        void TimeStamp(void);               //记录压力传感器采集时间戳
        float Singel_Point_Calculation(uint32_t *voltage);              //单点压力值计算
        float Conductance_Calculation(uint32_t *voltage);               //单点电导值计算
        int8_t Pressure_Calculation(float *buff);            //对缓冲区中的数据进行解算，得到压力值
    private:
        float prar_V = 3.27;          /* 采集板供电电压VCC */
        /* 1/R - V 曲线拟合函数参数 */
        float para_a = 624.1;      /* 函数形式: y = a*exp(-((b*x)/c)^2) */
        float para_b = 0.6779;
        float para_c = 0.2652;
        float vref = 0.6;
        float Rf = 4700;  
};

class IMU_Solution
{
    public:
        IMU_Solution();         //构造函数
        ~IMU_Solution();        //析构函数
        void Buff_Init();       //IMU数据缓冲区初始化
        int8_t Buff_Full_Judge(void);               //判断缓冲区是否写满
        int8_t Read_Write_Buff_Switch(void);        //交换读写缓冲区
        void TimeStamp(void);               //记录IMU采集时间戳
        int8_t Put_into_Buff(float *acc, float *gyro, float *angle, int16_t *mag, int32_t atom, int32_t hight, float *quat);         //将采集出来的IMU数据存入相应缓冲区
};

/* 传感器数据处理及传输 */
void Plantar_Data_Frame_Handling(uint8_t frame);        //压力传感器数据帧处理
void IMU_Data_Frame_Handling(uint8_t frame);            //IMU数据帧处理
void Plantar_Data_Frame_Transmit(void);                 //发送压力传感器数据帧
void IMU_Data_Frame_Transmit(void);                     //发送IMU数据帧

#endif