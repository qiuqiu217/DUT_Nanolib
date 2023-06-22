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

#define NOP do { __asm__ __volatile__ ("nop"); } while (0)

/**
 * @brief nop_delay
 */
void inline nop_delay(uint8_t num)
{
    while(num)
    {
        NOP;
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
        MUX_SetPin(GATE_R0_GPIO, GATE_R0_PIN, (Row & 0b001) >> 0);
        MUX_SetPin(GATE_R1_GPIO, GATE_R1_PIN, (Row & 0b010) >> 1);
        MUX_SetPin(GATE_R2_GPIO, GATE_R2_PIN, (Row & 0b100) >> 2);
        MUX_SetPin(EN_INH2_GPIO, EN_INH2_PIN, ENABLE);      //高电平-使能选通器
        MUX_SetPin(EN_INH3_GPIO, EN_INH3_PIN, DISABLE);     //低电平-禁用选通器
    } 
    else                    //选通项在后五行中
    {
        Row -= 5;
        /* 进行行选通（后五行） */
        MUX_SetPin(GATE_R3_GPIO, GATE_R3_PIN, (Row & 0b001) >> 0);
        MUX_SetPin(GATE_R4_GPIO, GATE_R4_PIN, (Row & 0b010) >> 1);
        MUX_SetPin(GATE_R5_GPIO, GATE_R5_PIN, (Row & 0b100) >> 2);
        MUX_SetPin(EN_INH2_GPIO, EN_INH2_PIN, DISABLE);     //高电平-使能选通器
        MUX_SetPin(EN_INH3_GPIO, EN_INH3_PIN, ENABLE);      //低电平-禁用选通器

    }
}

/**
 * @brief Column_Select 进行列选通
 * @param uint8_t Column  选通列
 */
static inline void Column_Select(uint8_t Column)
{
    /* 进行列选通 */
    MUX_SetPin(GATE_C0_GPIO, GATE_C0_PIN, (Column & 0b001) >> 0);
    MUX_SetPin(GATE_C1_GPIO, GATE_C1_PIN, (Column & 0b010) >> 1);
    MUX_SetPin(GATE_C2_GPIO, GATE_C2_PIN, (Column & 0b100) >> 2);
    MUX_SetPin(EN_INH1_GPIO, EN_INH1_PIN, ENABLE);      //高电平-使能选通器    
}
