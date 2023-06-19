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

/* 足底压力传感器阵列数量配置 */
#define SENSOR_NUM_TOTAL 45 
#define SENSOR_NUM_LINE 10
#define SENSOR_NUM_COLUMN 6

#include "includes.h"

/*
*********************************************************************************************************
*	                                      宏定义
*********************************************************************************************************
*/
/* LOG打印设置 */
#if 1
	#define PLANTAR_LOG     COM_DEBUG.printf
#else
	#define PLANTAR_LOG     
#endif
/* 行列选通控制引脚定义 */
#define GATE_C0 19
#define GATE_C1 18
#define GATE_C2 32
#define GATE_R0 33
#define GATE_R1 25
#define GATE_R2 26 
#define GATE_R3 27
#define GATE_R4 14
#define GATE_R5 23
/* 行列选通使能引脚定义 */
#define EN_INH1 22
#define EN_INH2 21
#define EN_INH3 13
/* ADC引脚定义 */
#define AOUT 39  //ADC1_3 


/*
*********************************************************************************************************
*	                                      类声明
*********************************************************************************************************
*/
class Plantar 
{    
    public:
        Plantar();          //构造函数
        ~Plantar();         //析构函数           
        void Init(void);            //压力传感器阵列采集初始化函数
        int8_t Channel_Judge(uint8_t Line, uint8_t Column);           //判断压力传感器阵列选通项是否正确
        int8_t Channel_Change(uint8_t Line, uint8_t Column);          //改变压力传感器阵列选通项   
        void Channel_Selection(uint8_t Line, uint8_t Column);         //压力传感器阵列选通函数
        void Channel_Disable(void);                //压力传感器阵列禁用
        int8_t Sampling_Mode_Change(uint8_t Mode);          //修改压力传感器阵列采集模式
        int8_t Sampling_Rate_Change(uint8_t Rate);          //修改压力传感器阵列采集延时
        void ADC_Oneshot_Read(uint16_t *adc_raw, uint32_t *voltage);            //ADC单次采集,并可生成原始数据和标定校准过的电压数据
        void Single_Frame_Read(uint16_t raw_data_p[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN],uint32_t voltage_p[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN]);          //测量单帧阵列数据
};


#endif