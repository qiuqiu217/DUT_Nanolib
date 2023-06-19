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

#include "includes.h"
/*
*********************************************************************************************************************************
*	                                                命令交互格式：JSON
*   {
*       "Command":      / "Change the sampling mod" （修改采集方式）/    / "Change the selection" （修改选通的阵列点）/
*                       / "Change the sampling rate" （修改采集速率）/
*       "Sampling Mode":        / uint8_t （采样模式）/
*       "Channel":
*               {
*                   "Line":     / uint8_t （选通行）/
*                   "Column":   / "Plantar left" （选通列）/
*               }
*       "Sampling Rate":        / uint8_t  (采样率)  /
*   }
*
**********************************************************************************************************************************
*/

/*
*********************************************************************************************************
*	                                      宏定义
*********************************************************************************************************
*/
/* LOG打印设置 */
#if 1
	#define Command_LOG     COM_DEBUG.printf
#else
	#define Command_LOG     
#endif
/*
*********************************************************************************************************
*	                                      类声明
*********************************************************************************************************
*/
class Command 
{    
    public:
        Command();         //构造函数
        ~Command();        //析构函数   
        int8_t Command_Parsing(const char *json_Command);           //解析debug串口命令   
        int8_t Change_Bluetooth_Transfer_State(void);               //修改蓝牙串口传输状态
        int8_t Change_Plantar_Sampling_State(void);                 //修改压力传感器采集状态
        int8_t Change_IMU_Sampling_State(void);                     //修改IMU采集状态
        int8_t Change_Plantar_SamplingMode(void);                   //修改压力传感器采集模式
        int8_t Change_Plantar_Selection(void);                      //修改压力传感器选通点
        int8_t Change_Plantar_SamplingRate(void);                   //修改压力传感器采集模式
        int8_t Plantar_Single_Sampling(void);                       //压力传感器单点指令采集
        int8_t Plantar_Array_Sampling(void);                        //压力传感器阵列指令采集
        int8_t IMU_Calibration(void);                               //IMU校正
};

/* IMU校准函数 */
int8_t Magnetometer_calibration(void);            //初始化磁力计
int8_t Acceleration_calibration(void);            //加速度校准
int8_t Angle_calibration(void);                   //角度校准


#endif