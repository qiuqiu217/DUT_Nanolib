/*
*********************************************************************************************************
*
*	文件名称 : Plantar.h
*	版    本 : V1.0
*	说    明 : 这是室内定位项目压力传感器阵列的采集程序头文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-10     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#ifndef PLANTAR_H
#define PLANTAR_H

#include "main.h"

typedef int8_t _RET_TYPE;
/*
*********************************************************************************************************
*	                                   宏定义
*********************************************************************************************************
*/
/* LOG打印设置 */
#if 0
	#define PLANTAR_LOG     printf
#else
	#define PLANTAR_LOG(...)
#endif

/* 足底压力传感器阵列数量配置 */
#define SENSOR_NUM_TOTAL 45 
#define SENSOR_NUM_ROW 10
#define SENSOR_NUM_COLUMN 6

/*
*********************************************************************************************************
*	                                      数据结构定义
*********************************************************************************************************
*/
/* 压力传感器采集设置结构体 */
typedef struct
{
    uint8_t Sampling_Mode;      //压力阵列传感器采样模式
    uint8_t Selection_Row;      //单点采样选通行
    uint8_t Selection_Column;   //单点采样选通列
    uint16_t Sampling_Delay;    //压力传感器采样延时
    
}PLANTAR_S;

/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
/**
 * @brief nop_delay
 */
void inline nop_delay(uint8_t num)
{
    while(num)
    {
        __nop();
        num--;
    }
}

/* 足底压力传感器采集设置 */
void Plantar_SettingsInit(void);
int8_t Plantar_ChannelJudge(uint8_t Row, uint8_t Column);
int8_t Plantar_ChannelChange(uint8_t Row, uint8_t Column);
int8_t Plantar_Mode_Change(uint8_t Mode);
int8_t Plantar_Rate_Change(uint8_t Rate);

/* 足底压力传感器数据采集 */
static inline void Row_Select(uint8_t Row);
static inline void Column_Select(uint8_t Column);
static inline void Channel_Disable(void);
int8_t Singal_Point_Sampling(uint8_t Row, uint8_t Column, uint16_t *buf);
int8_t Array_Scanning_Sampling(void);

/* 足底压力传感器数据处理 */
void Plantar_Buff_Init(void);
int8_t Plantar_Buff_Full_Judge(void);
int8_t Plantar_Read_Write_Buff_Switch(void);
void Plantar_TimeStamp(void);       //存入压力传感器采集时间戳

#endif
