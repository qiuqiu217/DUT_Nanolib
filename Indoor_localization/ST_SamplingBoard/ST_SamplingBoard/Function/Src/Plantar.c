/*
*********************************************************************************************************
*
*	�ļ����� : Plantar.cpp
*	��    �� : V1.0
*	˵    �� : �������ڶ�λ��Ŀѹ�����������еĲɼ������ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-03-10     Qiuqiu&Meipo    �״η���
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "Plantar.h"

/* ѹ���������������������� */
const uint8_t Sensor_Num_Row = SENSOR_NUM_ROW;       //ѹ����������������
const uint8_t Sensor_Num_Column[SENSOR_NUM_ROW][2] = { {0,2}, {0,3}, {0,3}, {0,3}, {0,3}, {0,4}, {0,5}, {0,5}, {1,5}, {2,5} };      //ѹ����������������
const uint16_t Plantar_Rate_Settings[10] = {5,10,15,20,50,100,200,500,1000,2000};      //�����ò�����ʱ

extern osMutexId_t GV_MutexHandle;      //ȫ�ֱ���������
extern PLANTAR_S Plantar;                       //ѹ���������ɼ��ṹ��

Plantar_Buff_TypeDef Plantar_Buff;              //ѹ������������Buff�ṹ������

/* ѹ���������������ݻ������������� */
uint16_t Plantar_Voltage_Buff_1[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];          //ѹ��������Buff1
uint16_t Plantar_Voltage_Buff_2[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];          //ѹ��������Buff2
/* ʱ����������������� */
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
 * @brief Plantar_SettingsInit ���ѹ���������ɼ����ó�ʼ��
 */
void Plantar_SettingsInit(void)
{
    Plantar.Sampling_Mode  = ARRAY_SAMPLINGMODE;
    Plantar.Selection_Row = 0;
    Plantar.Selection_Column = 0; 
    Plantar.Sampling_Delay = Plantar_Rate_Settings[1];
}

/**
 * @brief Channel_Judge �ж�ѹ������������ѡͨ���Ƿ���ȷ
 * 
 * @param uint8_t Line ��Ҫѡͨ����
 * @param uint8_t Column ��Ҫѡͨ����
 * @return int8_t -1 ����ѡͨ���д���      1 ����ѡͨ������ȷ
 */
int8_t Plantar_ChannelJudge(uint8_t Row, uint8_t Column)
{
    if(Row >= Sensor_Num_Row)     //�ж������������Ƿ���ȷ
    {
        PLANTAR_LOG("Channel select error \n\r");       //���������ݴ���
        return -1;
    }
    if(Column < Sensor_Num_Column[Row][0] || Column > Sensor_Num_Column[Row][1])      //�ж������������Ƿ���ȷ
    {
        PLANTAR_LOG("Channel select error \n\r");       //���������ݴ���
        return -1;
    }
    else
    {
        return 1;
    }     
}

/**
 * @brief Plantar_Channel_Change �ı�ѹ������������ѡͨ��
 * 
 * @param uint8_t Line ��Ҫѡͨ����
 * @param uint8_t Column ��Ҫѡͨ����
 * @return int8_t -1 �޸�ѡͨ��ʧ��      1 �޸�ѡͨ��ɹ�
 */
int8_t Plantar_ChannelChange(uint8_t Row, uint8_t Column)
{
    int8_t ret = 1;
    
    if(GV_MutexHandle != NULL)      //Plantar�������޸Ļ�������Ϊ��
    {
        if(xSemaphoreTake(GV_MutexHandle, 200) == pdTRUE)       //����ȫ�ֱ����޸Ļ�����
        {
            Plantar.Selection_Row = Row;
            Plantar.Selection_Column = Column;
            PLANTAR_LOG("Change the selection Line:%d Column:%d \n\r", Plantar.Selection_Row, Plantar.Selection_Column);
            xSemaphoreGive(GV_MutexHandle);           //�ͷ��޸�ѡͨ�����
        }
        else
        {
            Command_LOG("Application for mutex failed \n\r");      //����ȫ�ֱ����޸Ļ�����ʧ��
            ret =  0;
        }
    }
    else
    {
        Command_LOG("The mutex not created \n\r");      //ȫ�ֱ����޸Ļ�����δ����
        ret =  0;
    }
    return ret;
}

/**
 * @brief Plantar_Mode_Change �ı�ѹ�����������вɼ�ģʽ
 * @param uint8_t Mode �ɼ�ģʽ
 * @return int8_t -1 �޸Ĳɼ�ģʽʧ��      1 �޸Ĳɼ�ģʽ�ɹ�
 */
int8_t Plantar_Mode_Change(uint8_t Mode)
{
    char Print_buff[30];
    int8_t ret = 1;
    
    if(GV_MutexHandle != NULL)      //Plantar�������޸Ļ�������Ϊ��
    {
        if(xSemaphoreTake(GV_MutexHandle, 200) == pdTRUE)       //����ȫ�ֱ����޸Ļ�����
        {
            if(Plantar.Sampling_Mode <= INSTRUCTIONS_SINGLEMODE)
            {
                Plantar.Sampling_Mode = Mode;
                if(Mode == ARRAY_SAMPLINGMODE)              sprintf(Print_buff,"Array Samplingmode");
                else if(Mode == SINGLEPOINT_SAMPLINGMODE)   sprintf(Print_buff,"Singlepoint Samplingmode");
                else if(Mode == INSTRUCTIONS_ARRAYMODE)     sprintf(Print_buff,"Instructions Array Samplingmode");
                else if(Mode == INSTRUCTIONS_SINGLEMODE)    sprintf(Print_buff,"Instructions Singlepoint Samplingmode");      
                PLANTAR_LOG("Sampling Mode:%s \n\r", Print_buff);
            }
            else
            {
                PLANTAR_LOG("Invalid sampling mode \n\r");
                ret =  0;
            }
            xSemaphoreGive(GV_MutexHandle);           //�ͷ��޸�ѡͨ�����
        }
        else
        {
            PLANTAR_LOG("Application for mutex failed \n\r");      //����ȫ�ֱ����޸Ļ�����ʧ��
            ret =  0;
        }
    }
    else
    {
        PLANTAR_LOG("The mutex not created \n\r");      //ȫ�ֱ����޸Ļ�����δ����
        ret =  0;
    }
    return ret;
}

/**
 * @brief Plantar_Rate_Change �ı�ѹ�����������вɼ�ģʽ
 * @param uint8_t Rate �ɼ�����
 * @return int8_t -1 �޸Ĳɼ�ģʽʧ��      1 �޸Ĳɼ�ģʽ�ɹ�
 */
int8_t Plantar_Rate_Change(uint8_t Rate)
{
    int8_t ret = 1;
    
    if(GV_MutexHandle != NULL)      //Plantar�������޸Ļ�������Ϊ��
    {
        if(xSemaphoreTake(GV_MutexHandle, 200) == pdTRUE)       //����ȫ�ֱ����޸Ļ�����
        {
            if(Plantar.Sampling_Delay <= 10)
            {
                Plantar.Sampling_Delay = Plantar_Rate_Settings[Rate];
                PLANTAR_LOG("Sampling delay:%d \n\r", Plantar.Sampling_Delay);
            }
            else
            {
                PLANTAR_LOG("Invalid sampling delay \n\r");
                ret =  0;
            }
            xSemaphoreGive(GV_MutexHandle);           //�ͷ��޸�ѡͨ�����
        }
        else
        {
            PLANTAR_LOG("Application for mutex failed \n\r");      //����ȫ�ֱ����޸Ļ�����ʧ��
            ret =  0;
        }
    }
    else
    {
        PLANTAR_LOG("The mutex not created \n\r");      //ȫ�ֱ����޸Ļ�����δ����
        ret =  0;
    }
    return ret;
}

/**
 * @brief Row_Select ������ѡͨ
 * @param uint8_t Row  ѡͨ��
 */
static inline void Row_Select(uint8_t Row)
{
    if (Row < 5)            //ѡͨ����ǰ������
    {
        /* ������ѡͨ��ǰ���У� */
        MUX_SetPin(GATE_R0_GPIO, GATE_R0_PIN, (Row & 1) >> 0);
        MUX_SetPin(GATE_R1_GPIO, GATE_R1_PIN, (Row & 2) >> 1);
        MUX_SetPin(GATE_R2_GPIO, GATE_R2_PIN, (Row & 4) >> 2);
        MUX_SetPin(EN_INH2_GPIO, EN_INH2_PIN, ENABLE);      //�ߵ�ƽ-ʹ��ѡͨ��
        MUX_SetPin(EN_INH3_GPIO, EN_INH3_PIN, DISABLE);     //�͵�ƽ-����ѡͨ��
    } 
    else                    //ѡͨ���ں�������
    {
        Row -= 5;
        /* ������ѡͨ�������У� */
        MUX_SetPin(GATE_R3_GPIO, GATE_R3_PIN, (Row & 1) >> 0);
        MUX_SetPin(GATE_R4_GPIO, GATE_R4_PIN, (Row & 2) >> 1);
        MUX_SetPin(GATE_R5_GPIO, GATE_R5_PIN, (Row & 4) >> 2);
        MUX_SetPin(EN_INH2_GPIO, EN_INH2_PIN, DISABLE);     //�͵�ƽ-����ѡͨ��
        MUX_SetPin(EN_INH3_GPIO, EN_INH3_PIN, ENABLE);      //�ߵ�ƽ-ʹ��ѡͨ��
    }
}

/**
 * @brief Column_Select ������ѡͨ
 * @param uint8_t Column  ѡͨ��
 */
static inline void Column_Select(uint8_t Column)
{
    /* ������ѡͨ */
    MUX_SetPin(GATE_C0_GPIO, GATE_C0_PIN, (Column & 1) >> 0);
    MUX_SetPin(GATE_C1_GPIO, GATE_C1_PIN, (Column & 2) >> 1);
    MUX_SetPin(GATE_C2_GPIO, GATE_C2_PIN, (Column & 4) >> 2);
    MUX_SetPin(EN_INH1_GPIO, EN_INH1_PIN, ENABLE);      //�ߵ�ƽ-ʹ��ѡͨ��    
}

/**
 * @brief Channel_Disable ��������ѡͨ
 */
static inline void Channel_Disable(void)
{
    MUX_SetPin(EN_INH1_GPIO, EN_INH1_PIN, DISABLE);     //�͵�ƽ-����ѡͨ��
    MUX_SetPin(EN_INH2_GPIO, EN_INH2_PIN, DISABLE);     //�͵�ƽ-����ѡͨ��
    MUX_SetPin(EN_INH3_GPIO, EN_INH3_PIN, DISABLE);     //�͵�ƽ-����ѡͨ��
}
    
/**
 * @brief Singal_Point_Sampling �������
 * @param uint8_t Row  ѡͨ��
 * @param uint8_t Column  ѡͨ��
 * @param uint16_t *buf  ���ݴ洢����
 * @return int8_t -1 �ɼ�ʧ��      1 �ɼ��ɹ�
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
    nop_delay(240);           //ģ���źŽ���ʱ��
    *buf = (uint16_t)ADC_OneShot_Read()* (3.3/4096) *1000;
    //Channel_Disable();
    return 1;
}


/**
 * @brief Array_Scanning_Sampling ����ɨ��ɼ�
 * @return int8_t -1 �ɼ�ʧ��      1 �ɼ��ɹ�
 */
int8_t Array_Scanning_Sampling(void)
{
    uint8_t i = 0, j = 0, num = 0;
    while(i < Sensor_Num_Row)          //�ӵ�һ�п�ʼ��˳��ɼ�
    {
        j = Sensor_Num_Column[i][0];
        while(j <= Sensor_Num_Column[i][1])      //�ɼ�һ��ʱ�ӵ�һ�п�ʼ˳��ɼ�
        {
            Row_Select(i);          //ѡͨ��Ӧ����
            Column_Select(j);
            nop_delay(240);           //ģ���źŽ���ʱ��
            *(Plantar_Buff.Write_Buff + Plantar_Buff.Write_Frame*SENSOR_NUM_TOTAL + num) = (uint16_t)ADC_OneShot_Read()* (3.3/4096) *1000;
            num++;
            j++;
            //Channel_Disable();
        }
        i++;
    }
    return 1;
}

/**
 * @brief Plantar_Buff_Init ѹ�����������вɼ����ݻ�����
 */
void Plantar_Buff_Init(void)
{
    /* ��ʼ��Buff */
    Plantar_Buff.Write_Buff = Plantar_Voltage_Buff_1;
    Plantar_Buff.Read_Buff = Plantar_Voltage_Buff_2;
    Plantar_Buff.Write_Time_Buff = Plantar_Time_Stamp_1;
    Plantar_Buff.Read_Time_Buff = Plantar_Time_Stamp_2;
    Plantar_Buff.Write_Frame = 0;
}

/**
 * @brief Plantar_Buff_Init ѹ�����������вɼ����ݻ�����
 * @return int8_t  -1������δд��    1�������Ѿ�д��
 */
int8_t Plantar_Buff_Full_Judge(void)
{
    PLANTAR_LOG("Amount:%d \n\r",Plantar_Buff.Write_Frame); 
    if(Plantar_Buff.Write_Frame >= FRAME_IN_BUFF)
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
 * @brief Read_Write_Buff_Switch ������д������
 *
 * @return int8_t -1��ʾ��д�������쳣  1��ʾ��д�����������ɹ�
 */
int8_t Plantar_Read_Write_Buff_Switch(void)
{
    uint16_t *read_buff;        
    uint16_t *write_buff;
    uint32_t *read_time_buff;        
    uint32_t *write_time_buff;
    /* �ж϶�д������״̬�Ƿ��쳣 */
    if(Plantar_Buff.Write_Buff == NULL || Plantar_Buff.Read_Buff == NULL)         //��д��������ָ��NULL
    {
        PLANTAR_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_buff = Plantar_Buff.Read_Buff;
    write_buff = Plantar_Buff.Write_Buff;
    read_time_buff = Plantar_Buff.Read_Time_Buff;
    write_time_buff = Plantar_Buff.Write_Time_Buff;   
    /* ���ö�д������ */
    Plantar_Buff.Read_Buff = write_buff;
    Plantar_Buff.Write_Buff = read_buff;
    /* ����ʱ�����д������ */
    Plantar_Buff.Read_Time_Buff = write_time_buff;
    Plantar_Buff.Write_Time_Buff = read_time_buff;
    /* �������ݼ���ֵ */
    Plantar_Buff.Write_Frame = 0;
    return 1;                     
}

/**
 * @brief Plantar_TimeStamp ����ѹ���������ɼ�ʱ���
 */
void Plantar_TimeStamp(void)
{
    *(Plantar_Buff.Write_Time_Buff + Plantar_Buff.Write_Frame) = Get_TimeStamp();
}

