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


/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/* RTOS��ر��� */
extern osMutexId_t GV_MutexHandle;      //ȫ�ֱ���������

extern PLANTAR_S Plantar;                       //ѹ���������ɼ��ṹ��

/* ѹ���������������������� */
const uint8_t Sensor_Num_Row = SENSOR_NUM_ROW;       //ѹ����������������
const uint8_t Sensor_Num_Column[SENSOR_NUM_ROW][2] = { {0,2}, {0,3}, {0,3}, {0,3}, {0,3}, {0,4}, {0,5}, {0,5}, {1,5}, {2,5} };      //ѹ����������������
const uint16_t Plantar_Rate_Settings[10] = {5,10,15,20,50,100,200,500,1000,2000};      //�����ò�����ʱ

Plantar_Buff_TypeDef Plantar_Buff;              //ѹ������������Buff�ṹ������

/* ѹ���������������ݻ������������� */
uint16_t Plantar_Voltage_Buff_1[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];          //ѹ��������Buff1
uint16_t Plantar_Voltage_Buff_2[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];          //ѹ��������Buff2
/* ʱ����������������� */
uint32_t Plantar_Time_Stamp_1[FRAME_IN_BUFF];
uint32_t Plantar_Time_Stamp_2[FRAME_IN_BUFF];

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/**
 * @brief Plantar_SettingsInit ���ѹ���������ɼ����ó�ʼ��
 */
void Plantar_SettingsInit(void)
{
    Plantar.Sampling_Mode  = ARRAY_SAMPLINGMODE;
    Plantar.Selection_Row = 0;
    Plantar.Selection_Column = 0; 
    Plantar.Sampling_Delay = Plantar_Rate_Settings[1];
    Channel_Disable();
}

/**
 * @brief Channel_Judge �ж�ѹ������������ѡͨ���Ƿ���ȷ
 * 
 * @param uint8_t Line ��Ҫѡͨ����
 * @param uint8_t Column ��Ҫѡͨ����
 * @return _RET_TYPE RET_ERROR ����ѡͨ���д���      RET_OK ����ѡͨ������ȷ
 */
_RET_TYPE Plantar_ChannelJudge(uint8_t Row, uint8_t Column)
{
    if(Row >= Sensor_Num_Row)     //�ж������������Ƿ���ȷ
    {
        PLANTAR_LOG("Channel select error \n\r");       //���������ݴ���
        return RET_ERROR;
    }
    if(Column < Sensor_Num_Column[Row][0] || Column > Sensor_Num_Column[Row][1])      //�ж������������Ƿ���ȷ
    {
        PLANTAR_LOG("Channel select error \n\r");       //���������ݴ���
        return RET_ERROR;
    }
    else
    {
        return RET_OK;
    }     
}

/**
 * @brief Plantar_Channel_Change �ı�ѹ������������ѡͨ��
 * 
 * @param uint8_t Line ��Ҫѡͨ����
 * @param uint8_t Column ��Ҫѡͨ����
 * @return _RET_TYPE RET_ERROR �޸�ѡͨ��ʧ��      RET_OK �޸�ѡͨ��ɹ�
 */
_RET_TYPE Plantar_ChannelChange(uint8_t Row, uint8_t Column)
{
    _RET_TYPE ret = RET_OK;
    
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
            ret =  RET_ERROR;
        }
    }
    else
    {
        Command_LOG("The mutex not created \n\r");      //ȫ�ֱ����޸Ļ�����δ����
        ret =  RET_ERROR;
    }
    return ret;
}

/**
 * @brief Plantar_Mode_Change �ı�ѹ�����������вɼ�ģʽ
 * @param uint8_t Mode �ɼ�ģʽ
 * @return _RET_TYPE RET_ERROR �޸Ĳɼ�ģʽʧ��      RET_OK �޸Ĳɼ�ģʽ�ɹ�
 */
_RET_TYPE Plantar_Mode_Change(uint8_t Mode)
{
    char print_buff[30];
    _RET_TYPE ret = RET_OK;
    
    if(GV_MutexHandle != NULL)      //Plantar�������޸Ļ�������Ϊ��
    {
        if(xSemaphoreTake(GV_MutexHandle, 200) == pdTRUE)       //����ȫ�ֱ����޸Ļ�����
        {
            if(Plantar.Sampling_Mode <= INSTRUCTIONS_SINGLEMODE)
            {
                Plantar.Sampling_Mode = Mode;
                if(Mode == ARRAY_SAMPLINGMODE)              sprintf(print_buff,"Array Samplingmode");
                else if(Mode == SINGLEPOINT_SAMPLINGMODE)   sprintf(print_buff,"Singlepoint Samplingmode");
                else if(Mode == INSTRUCTIONS_ARRAYMODE)     sprintf(print_buff,"Instructions Array Samplingmode");
                else if(Mode == INSTRUCTIONS_SINGLEMODE)    sprintf(print_buff,"Instructions Singlepoint Samplingmode");      
                PLANTAR_LOG("Sampling Mode:%s \n\r", print_buff);
            }
            else
            {
                PLANTAR_LOG("Invalid sampling mode \n\r");
                ret =  RET_ERROR;
            }
            xSemaphoreGive(GV_MutexHandle);           //�ͷ��޸�ѡͨ�����
        }
        else
        {
            PLANTAR_LOG("Application for mutex failed \n\r");      //����ȫ�ֱ����޸Ļ�����ʧ��
            ret =  RET_ERROR;
        }
    }
    else
    {
        PLANTAR_LOG("The mutex not created \n\r");      //ȫ�ֱ����޸Ļ�����δ����
        ret =  RET_ERROR;
    }
    return ret;
}

/**
 * @brief Plantar_Rate_Change �ı�ѹ�����������вɼ�ģʽ
 * @param uint8_t Rate �ɼ�����
 * @return _RET_TYPE RET_ERROR �޸Ĳɼ�ģʽʧ��      RET_OK �޸Ĳɼ�ģʽ�ɹ�
 */
_RET_TYPE Plantar_Rate_Change(uint8_t Rate)
{
    _RET_TYPE ret = RET_OK;
    
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
                ret =  RET_ERROR;
            }
            xSemaphoreGive(GV_MutexHandle);           //�ͷ��޸�ѡͨ�����
        }
        else
        {
            PLANTAR_LOG("Application for mutex failed \n\r");      //����ȫ�ֱ����޸Ļ�����ʧ��
            ret =  RET_ERROR;
        }
    }
    else
    {
        PLANTAR_LOG("The mutex not created \n\r");      //ȫ�ֱ����޸Ļ�����δ����
        ret =  RET_ERROR;
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
 * @return _RET_TYPE    RET_ERROR �ɼ�ʧ��      RET_OK �ɼ��ɹ�
 */
_RET_TYPE Singal_Point_Sampling(uint8_t Row, uint8_t Column, uint16_t *buf)
{
    if(buf == NULL)
    {
        PLANTAR_LOG("Invalid memory address");
        return RET_ERROR;
    }
    if((Row >Sensor_Num_Row) || (Column > Sensor_Num_Column[Row][1]))
    {
        PLANTAR_LOG("Invalid row and column value");
        return RET_ERROR;
    }
    Row_Select(Row);
    Column_Select(Column);
    nop_delay(200);           //ģ���źŽ���ʱ��
    *buf = (uint16_t)ADC_OneShot_Read()* (3.3/4096) *1000;
    //Channel_Disable();
    return RET_OK;
}


/**
 * @brief Array_Scanning_Sampling ����ɨ��ɼ�
 * @return _RET_TYPE    RET_ERROR �ɼ�ʧ��      RET_OK �ɼ��ɹ�
 */
_RET_TYPE Array_Scanning_Sampling(void)
{
    uint8_t i = 0, j = 0, num = 0;
    while(i < Sensor_Num_Row)          //�ӵ�һ�п�ʼ��˳��ɼ�
    {
        j = Sensor_Num_Column[i][0];
        while(j <= Sensor_Num_Column[i][1])      //�ɼ�һ��ʱ�ӵ�һ�п�ʼ˳��ɼ�
        {
            Row_Select(i);          //ѡͨ��Ӧ����
            Column_Select(j);
            nop_delay(200);           //ģ���źŽ���ʱ��
            *(Plantar_Buff.Write_Buff + Plantar_Buff.Write_Frame*SENSOR_NUM_TOTAL + num) = (uint16_t)ADC_OneShot_Read()* (3.3/4096) *1000;
            num++;
            j++;
            //Channel_Disable();
        }
        i++;
    }
    return RET_OK;
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
 * @return _RET_TYPE  RET_ERROR ������δд��    RET_OK �������Ѿ�д��
 */
int8_t Plantar_Buff_Full_Judge(void)
{
    PLANTAR_LOG("Amount:%d \n\r",Plantar_Buff.Write_Frame); 
    if(Plantar_Buff.Write_Frame >= FRAME_IN_BUFF)
    {
        PLANTAR_LOG("Plantar buffer overflow");
        return RET_OK;
    }
    else
    {
        PLANTAR_LOG("Plantar buffer not overflow");
        return RET_ERROR;
    }
}

/**
 * @brief Read_Write_Buff_Switch ������д������
 *
 * @return _RET_TYPE    RET_ERROR ��ʾ��д�������쳣      RET_OK ��ʾ��д�����������ɹ�
 */
_RET_TYPE Plantar_Read_Write_Buff_Switch(void)
{
    uint16_t *read_buff;        
    uint16_t *write_buff;
    uint32_t *read_time_buff;        
    uint32_t *write_time_buff;
    
    /* �ж϶�д������״̬�Ƿ��쳣 */
    if(Plantar_Buff.Write_Buff == NULL || Plantar_Buff.Read_Buff == NULL)         //��д��������ָ��NULL
    {
        PLANTAR_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
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
    return RET_OK;                     
}

/**
 * @brief Plantar_TimeStamp ����ѹ���������ɼ�ʱ���
 */
void Plantar_TimeStamp(void)
{
    *(Plantar_Buff.Write_Time_Buff + Plantar_Buff.Write_Frame) = Get_TimeStamp();
}

