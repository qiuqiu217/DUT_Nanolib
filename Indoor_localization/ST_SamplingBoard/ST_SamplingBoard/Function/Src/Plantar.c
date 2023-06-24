/*
*********************************************************************************************************
*
*	文件名称 : Plantar.cpp
*	版    本 : V1.0
*	说    明 : 这是室内定位项目压力传感器阵列的采集程序文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-10     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "Plantar.h"

/* 压力传感器阵列行列数定义 */
const uint8_t Sensor_Num_Row = SENSOR_NUM_ROW;       //压力传感器阵列行数
const uint8_t Sensor_Num_Column[SENSOR_NUM_ROW][2] = { {0,2}, {0,3}, {0,3}, {0,3}, {0,3}, {0,4}, {0,5}, {0,5}, {1,5}, {2,5} };      //压力传感器阵列列数

Plantar_Buff_TypeDef Plantar_Buff;              //压力传感器阵列Buff结构体声明
IMU_Buff_TypeDef IMU_Buff;                      //惯性传感器Buff结构体声明

/* 压力传感器阵列数据缓冲区变量声明 */
uint16_t Plantar_Voltage_Buff_1[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];          //压力传感器Buff1
uint16_t Plantar_Voltage_Buff_2[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];          //压力传感器Buff2
/* 时间戳缓冲区变量声明 */
uint32_t Plantar_Time_Stamp_1[FRAME_IN_BUFF];
uint32_t Plantar_Time_Stamp_2[FRAME_IN_BUFF];

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

/**
 * @brief Row_Select 进行行选通
 * @param uint8_t Row  选通行
 */
static inline void Row_Select(uint8_t Row)
{
    if (Row < 5)            //选通项在前五行中
    {
        /* 进行行选通（前五行） */
        MUX_SetPin(GATE_R0_GPIO, GATE_R0_PIN, Row & 1 >> 0);
        MUX_SetPin(GATE_R1_GPIO, GATE_R1_PIN, Row & 2 >> 1);
        MUX_SetPin(GATE_R2_GPIO, GATE_R2_PIN, Row & 4 >> 2);
        MUX_SetPin(EN_INH2_GPIO, EN_INH2_PIN, ENABLE);      //高电平-使能选通器
        MUX_SetPin(EN_INH3_GPIO, EN_INH3_PIN, DISABLE);     //低电平-禁用选通器
    } 
    else                    //选通项在后五行中
    {
        Row -= 5;
        /* 进行行选通（后五行） */
        MUX_SetPin(GATE_R3_GPIO, GATE_R3_PIN, (Row & 1) >> 0);
        MUX_SetPin(GATE_R4_GPIO, GATE_R4_PIN, (Row & 2) >> 1);
        MUX_SetPin(GATE_R5_GPIO, GATE_R5_PIN, (Row & 4) >> 2);
        MUX_SetPin(EN_INH2_GPIO, EN_INH2_PIN, DISABLE);     //低电平-禁用选通器
        MUX_SetPin(EN_INH3_GPIO, EN_INH3_PIN, ENABLE);      //高电平-使能选通器

    }
}

/**
 * @brief Column_Select 进行列选通
 * @param uint8_t Column  选通列
 */
static inline void Column_Select(uint8_t Column)
{
    /* 进行列选通 */
    MUX_SetPin(GATE_C0_GPIO, GATE_C0_PIN, (Column & 1) >> 0);
    MUX_SetPin(GATE_C1_GPIO, GATE_C1_PIN, (Column & 2) >> 1);
    MUX_SetPin(GATE_C2_GPIO, GATE_C2_PIN, (Column & 4) >> 2);
    MUX_SetPin(EN_INH1_GPIO, EN_INH1_PIN, ENABLE);      //高电平-使能选通器    
}

/**
 * @brief Channel_Disable 禁用行列选通
 */
static inline void Channel_Disable(void)
{
    MUX_SetPin(EN_INH1_GPIO, EN_INH1_PIN, DISABLE);     //低电平-禁用选通器
    MUX_SetPin(EN_INH2_GPIO, EN_INH2_PIN, DISABLE);     //低电平-禁用选通器
    MUX_SetPin(EN_INH3_GPIO, EN_INH3_PIN, DISABLE);     //低电平-禁用选通器
}
    
/**
 * @brief Singal_Point_Sampling 单点采样
 * @param uint8_t Row  选通行
 * @param uint8_t Column  选通列
 * @param uint16_t *buf  数据存储区域
 * @return int8_t -1 采集失败      1 采集成功
 */
int8_t Singal_Point_Sampling(uint8_t Row, uint8_t Column, uint16_t *buf)
{
    if(buf == NULL)
    {
        PLANTAR_LOG("Invalid memory address");
        return -1;
    }
    if((Row >Sensor_Num_Row) || (Column > Sensor_Num_Column[Row][1]))
    {
        PLANTAR_LOG("Invalid row and column value");
        return -1;
    }
    Row_Select(Row);
    Column_Select(Column);
    *buf = ADC_OneShot_Read()* (3.3/4096);
    Channel_Disable();
    return 1;
}


/**
 * @brief Array_Scanning_Sampling 阵列扫描采集
 * @return int8_t -1 采集失败      1 采集成功
 */
int8_t Array_Scanning_Sampling(void)
{
    uint8_t i = 0, j = 0, num = 0;
    while(i < Sensor_Num_Row)          //从第一行开始按顺序采集
    {
        j = Sensor_Num_Column[i][0];
        while(j <= Sensor_Num_Column[i][1])      //采集一行时从第一列开始顺序采集
        {
            Row_Select(i);          //选通对应行列
            Column_Select(j);
            nop_delay(5);           //模拟信号建立时间
            *(Plantar_Buff.Write_Buff + Plantar_Buff.Write_Frame*FRAME_IN_BUFF + num) = ADC_OneShot_Read()* (3.3/4096);
            Channel_Disable();
            j++;
        }
        i++;
    }
    return 1;
}

/**
 * @brief Plantar_Buff_Init 压力传感器阵列采集数据缓存区
 */
void Plantar_Buff_Init(void)
{
    /* 初始化Buff */
    Plantar_Buff.Write_Buff = Plantar_Voltage_Buff_1;
    Plantar_Buff.Read_Buff = Plantar_Voltage_Buff_2;
    Plantar_Buff.Write_Time_Buff = Plantar_Time_Stamp_1;
    Plantar_Buff.Read_Time_Buff = Plantar_Time_Stamp_2;
    Plantar_Buff.Write_Frame = 0;
}

/**
 * @brief Plantar_Buff_Init 压力传感器阵列采集数据缓存区
 * @return int8_t  -1缓冲区未写满    1缓冲区已经写满
 */
int8_t Plantar_Buff_Full_Judge(void)
{
    PLANTAR_LOG("Amount:%d \n\r",Plantar_Buff.Write_Frame); 
    if(Plantar_Buff.Write_Frame == FRAME_IN_BUFF)
    {
        PLANTAR_LOG("Plantar buffer overflow");
        return 1;
    }
    else
    {
        PLANTAR_LOG("Plantar buffer not overflow");
        return -1;
    }
}

/**
 * @brief Read_Write_Buff_Switch 交换读写缓冲区
 *
 * @return int8_t -1表示读写缓冲区异常  1表示读写缓冲区交换成功
 */
int8_t Plantar_Read_Write_Buff_Switch(void)
{
    uint16_t *read_buff;        
    uint16_t *write_buff;
    uint32_t *read_time_buff;        
    uint32_t *write_time_buff;
    /* 判断读写缓冲区状态是否异常 */
    if(Plantar_Buff.Write_Buff == NULL || Plantar_Buff.Read_Buff == NULL)         //读写缓冲区不指向NULL
    {
        PLANTAR_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_buff = Plantar_Buff.Read_Buff;
    write_buff = Plantar_Buff.Write_Buff;
    read_time_buff = Plantar_Buff.Read_Time_Buff;
    write_time_buff = Plantar_Buff.Write_Time_Buff;   
    /* 设置读写缓冲区 */
    Plantar_Buff.Read_Buff = write_buff;
    Plantar_Buff.Write_Buff = read_buff;
    /* 设置时间戳读写缓冲区 */
    Plantar_Buff.Read_Time_Buff = write_time_buff;
    Plantar_Buff.Write_Time_Buff = read_time_buff;
    /* 设置数据计数值 */
    Plantar_Buff.Write_Frame = 0;
    return 1;                     
}

