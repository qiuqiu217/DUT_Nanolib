/*
*********************************************************************************************************
*
*	�ļ����� : Function.cpp
*	��    �� : V1.0
*	˵    �� : ������Ҫʹ�õĸ��ֹ��ܺ�������
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-03-29     Qiuqiu&Meipo    �״η���
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/
#include "Function.h"

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/**
 * @brief u8Arry_float  uint8_t����ת��Ϊfloat
 * @param uint8_t *data  uint8_t����
 * @param bool key  ture��˴洢 falseС�˴洢
 * @return float  ת����float��ֵ
 */
float u8Arry_float(uint8_t *data, bool key)
{
    float fa = 0;
    uint8_t uc[4];
    if (key == true)
    {
        uc[3] = data[0];
        uc[2] = data[1];
        uc[1] = data[2];
        uc[0] = data[3];
    }
    else
    {
        uc[0] = data[0];
        uc[1] = data[1];
        uc[2] = data[2];
        uc[3] = data[3];
    }

    memcpy(&fa, uc, 4);
    return fa;
}

/**
 * @brief float_u8Arry  floatת��Ϊuint8_t����
 * @param uint8_t *u8Arry  uint8_t����
 * @param float *floatdata  ��Ҫת����float��ֵ
 * @param bool key  ture��˴洢 falseС�˴洢
 */
void float_u8Arry(uint8_t *u8Arry, float *floatdata, bool key)
{
    uint8_t farray[4];
    *(float *)farray = *floatdata;
    if (key == true)
    {
        u8Arry[3] = farray[0];
        u8Arry[2] = farray[1];
        u8Arry[1] = farray[2];
        u8Arry[0] = farray[3];
    }
    else
    {
        u8Arry[0] = farray[0];
        u8Arry[1] = farray[1];
        u8Arry[2] = farray[2];
        u8Arry[3] = farray[3];
    }
}

/**
 * @brief u32_u8Arry  uint32_tת��Ϊuint8_t����
 * @param uint8_t *u8Arry  uint8_t����
 * @param uint32_t u32data  ��Ҫת����uint32_t��ֵ
 */
void u32_u8Arry(uint32_t u32data, uint8_t *u8Arry) 
{
    u8Arry[0] = (u32data >> 24) & 0xFF;
    u8Arry[1] = (u32data >> 16) & 0xFF;
    u8Arry[2] = (u32data >> 8) & 0xFF;
    u8Arry[3] = u32data & 0xFF;
}

/**
 * @brief u8Arry_u32  uint8_t����ת��Ϊuint32_t
 * @param uint8_t *u8Arry  uint8_t����
 * @return uint32_t  ת�����uint32_t��ֵ
 */
uint32_t u8Arry_u32(uint8_t *u8Arry) 
{
    uint32_t u32data = 0;
    u32data |= ((uint32_t)(u8Arry[0]) << 24);
    u32data |= ((uint32_t)(u8Arry[1]) << 16);
    u32data |= ((uint32_t)(u8Arry[2]) << 8);
    u32data |= u8Arry[3];
    return u32data;
}

/**
 * @brief u16_u8Arry  uint16_tת��Ϊuint8_t����
 * @param uint8_t *u8Arry  uint8_t����
 * @param uint16_t u16data  ��Ҫת����uint16_t��ֵ
 */
void u16_u8Arry(uint16_t u16data, uint8_t *u8Arry) 
{
    u8Arry[0] = (u16data >> 8) & 0xFF;
    u8Arry[1] = u16data & 0xFF;
}

/**
 * @brief u8Arry_u16  uint8_t����ת��Ϊuint16_t
 * @param uint8_t *u8Arry  uint8_t����
 * @return uint16_t  ת�����uint16_t��ֵ
 */
uint16_t u8Arry_u16(uint8_t *u8Arry) 
{
    uint16_t u16data = 0;
    u16data |= ((uint32_t)(u8Arry[0]) << 8);
    u16data |= u8Arry[1];
    return u16data;
}

