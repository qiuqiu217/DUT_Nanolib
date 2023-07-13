/*
*********************************************************************************************************
*
*	�ļ����� : Solution.cpp
*	��    �� : V1.0
*	˵    �� : �������ڶ�λ��Ŀ���ѹ������������ѹ����������ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-03-15     Qiuqiu&Meipo    �״η���
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "Solution.h"

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/* RTOS��ر��� */
extern osMutexId_t Transfer_MutexHandle;        /* SPP���ͻ����� */

/* ѹ�����ݻ����� */
extern Plantar_Buff_TypeDef Plantar_Buff;              //ѹ������������Buff�ṹ������
extern IMU_Buff_TypeDef IMU_Buff;                      //���Դ�����Buff�ṹ������
float Pressure_Buff[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];

/* �ɼ���ѹ��������� */
const float para_a = 624.1;      /* ������ʽ: y = a*exp(-((b*x)/c)^2) */
const float para_b = 0.6779;
const float para_c = 0.2652;
const float vref = 0.58;
const float Rf = 4700; 

/* ��������֡���ݴ洢�ռ� */
uint8_t Plantar_Transfer[4*SENSOR_NUM_TOTAL];
uint8_t Plantar_TimeStamp_Transfer[4];
uint8_t IMU_TimeStamp_Transfer[4];
uint8_t Accelerated_Transfer[12];
uint8_t Gyroscope_Transfer[12];
uint8_t Angle_Transfer[12];
uint8_t Magnetic_Transfer[6];
uint8_t Atmospheric_Transfer[4];
uint8_t Height_Transfer[4];
uint8_t Quaternion_Transfer[16];

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/

/**
 * @brief Singel_Point_Calculation  �����������õ�ѹ��ֵ
 * @param uint32_t *voltage
 */
float Singel_Point_Calculation(uint16_t *voltage)
{
    float vol;
    float conductance;
    float pressure;
    vol = (float)(*voltage)/1000.0f;       
    conductance = ((vol/vref)-1)/Rf;
    float index = (conductance*1000 - para_b)/para_c;
    pressure = para_a*exp(-index*index);
    Solution_LOG("Conductance :%f \n\r",conductance);     //����絼ֵ
    Solution_LOG("Pressure :%f g \n\r",pressure);     //���ѹ��ֵ

    return pressure;
}

/**
 * @brief Conductance_Calculation  �����������õ��絼ֵ
 * @brief uint32_t *voltage  �������ĵ�ѹֵ
 * @return float ���ص絼ֵ
 */
float Conductance_Calculation(uint16_t *voltage)
{
    float vol;
    float conductance;
    /* ����絼ֵ */
    vol = (float)(*voltage)/1000.0f;
    conductance = ((vol/vref)-1)/Rf;;
    Solution_LOG("Conductance :%f \n\r",conductance);     //����絼ֵ

    return conductance;        
}

/**
 * @brief Pressure_Calculation  �Ի������е����ݽ��н��㣬�õ�ѹ��ֵ
 * @param float *Buff
 * @return int8_t -1 ����ʧ��      1 ����ɹ�
 */
int8_t Pressure_Calculation(float *buff)
{
    float voltage;
    float conductance;
    uint16_t i;
    
    /* �ж϶�������״̬�Ƿ��쳣 */
    if(Plantar_Buff.Read_Buff == NULL)         //����������ָ��NULL
    {
        Solution_LOG("Read buffer pointer error \n\r");
        return RET_ERROR;       
    }
    else
    {
        /* ����ѹ��ֵ */
        for(i=0; i<SENSOR_NUM_TOTAL*FRAME_IN_BUFF; i++)
        {
            voltage = (float)(*(Plantar_Buff.Read_Buff + i))/1000.0f;       
            conductance = ((voltage/vref)-1)/Rf;;
            float index = (conductance*1000 - para_b)/para_c;
            *(buff + i) = para_a*exp(-index*index);
        }
        return RET_OK; 
    }       
}

/**
 * @brief Plantar_Data_Frame_Handling ѹ������������֡����
 * @param uint8_t frame  ��Ҫ�����ǻ������еڼ�֡
 */
void Plantar_Data_Frame_Handling(uint8_t frame)
{
    uint8_t i;

    /* ʱ������� */
    u32_u8Arry(*(Plantar_Buff.Read_Time_Buff + frame), Plantar_TimeStamp_Transfer);   
    /* ѹ�����������ݴ��� */
    for(i=0; i<SENSOR_NUM_TOTAL; i++)
    {
        float_u8Arry(Plantar_Transfer + 4*i, Pressure_Buff + frame*SENSOR_NUM_TOTAL + i, false);
    }
}

/**
 * @brief IMU_Data_Frame_Handling IMU����֡����
 * @param uint8_t frame  ��Ҫ�����ǻ������еڼ�֡
 */
void IMU_Data_Frame_Handling(uint8_t frame)
{
    uint8_t i;

    /* ʱ������� */
    u32_u8Arry(*(IMU_Buff.Read_Time_Buff + frame), IMU_TimeStamp_Transfer); 
    /* IMU���ݴ��� */
    for(i=0; i<3; i++)
    {
        float_u8Arry(Accelerated_Transfer + 4*i, IMU_Buff.Read_AccBuff + frame*3 + i, false);
        float_u8Arry(Gyroscope_Transfer + 4*i, IMU_Buff.Read_GyroBuff + frame*3 + i, false);
        float_u8Arry(Angle_Transfer + 4*i, IMU_Buff.Read_AngleBuff + frame*3 + i, false);
        u16_u8Arry(*(IMU_Buff.Read_MagBuff + frame*3 + i), Magnetic_Transfer + 2*i);        
    }
    u32_u8Arry(*(IMU_Buff.Read_AtmoBuff + frame), Atmospheric_Transfer);
    u32_u8Arry(*(IMU_Buff.Read_HightBuff + frame), Height_Transfer);
    for(i=0; i<4; i++)
    {
        float_u8Arry(Quaternion_Transfer + 4*i, IMU_Buff.Read_QuatBuff + frame*4 + i, false);        
    }    
}


/* ������������֡������ */
uint8_t Plantar_Transfer_Buffer[4*SENSOR_NUM_TOTAL + 15];
uint8_t Acc_Transfer_Buffer[27];
uint8_t Gyro_Transfer_Buffer[27];
uint8_t Angle_Transfer_Buffer[27];
uint8_t Mag_Transfer_Buffer[21];
uint8_t Atmo_Transfer_Buffer[19];
uint8_t Hight_Transfer_Buffer[19];
uint8_t Quat_Transfer_Buffer[31];

/**
 * @brief Plantar_Data_Frame_Init ѹ������������֡��ʼ��
 * 
 */
void Plantar_Data_Frame_Init(void)
{
    uint8_t i;
    
    /* ѹ�����������ݴ��� */
    for(i=0; i<5; i++)
    {
        Plantar_Transfer_Buffer[i] = 0x71;          //Э��ͷ
    }
    Plantar_Transfer_Buffer[5] = 0x40;              //��������
    for(i=0; i<5; i++)
    {
        Plantar_Transfer_Buffer[4*SENSOR_NUM_TOTAL + 10 +i] = 0x68;         //Э��β
    }
}

/**
 * @brief Plantar_Data_Frame_Transmit ����ѹ������������֡
 * 
 */
void Plantar_Data_Frame_Transmit(void)
{
    uint16_t i;
    if(Transfer_MutexHandle != NULL)      //Plantar�������޸Ļ�������Ϊ��
    {
        if(xSemaphoreTake(Transfer_MutexHandle, pdMS_TO_TICKS(20)) == pdTRUE)       //����SPP���ͻ�����
        {
            /* ѹ�����������ݴ��� */
            for(i=0; i<4; i++)
            {
                Plantar_Transfer_Buffer[6+i] = Plantar_TimeStamp_Transfer[i];           //ʱ���
            }
            for(i=0; i<4*SENSOR_NUM_TOTAL; i++)
            {
                Plantar_Transfer_Buffer[10+i] = Plantar_Transfer[i];            //����λ
            }
            comSendBuf(SPP_COM, Plantar_Transfer_Buffer, sizeof(Plantar_Transfer_Buffer));
            xSemaphoreGive(Transfer_MutexHandle);           //�ͷ�SPP���ͻ�����
        }
        else
        {
           TASK_LOG("Application for mutex failed \n\r");      //����SPP���ͻ�����ʧ��
        }
    }
    else
    {
        TASK_LOG("The mutex not created \n\r");      //SPP���ͻ�����δ����
    }
}



/**
 * @brief IMU_Data_Frame_Init IMU����֡��ʼ��
 * 
 */
void IMU_Data_Frame_Init(void)
{
    uint8_t i;
    
    /* ���ٶ����ݴ��� */
    for(i=0; i<5; i++)
    {
        Acc_Transfer_Buffer[i] = 0x71;          //Э��ͷ
    }
    Acc_Transfer_Buffer[5] = 0x50;              //��������
    for(i=0; i<5; i++)
    {
        Acc_Transfer_Buffer[22 + i] = 0x68;         //Э��β
    }
    /* ���������ݴ��� */
    for(i=0; i<5; i++)
    {
        Gyro_Transfer_Buffer[i] = 0x71;         //Э��ͷ
    }
    Gyro_Transfer_Buffer[5] = 0x51;             //��������
    for(i=0; i<5; i++)
    {
        Gyro_Transfer_Buffer[22 + i] = 0x68;        //Э��β
    }
    /* �Ƕ����ݴ��� */
    for(i=0; i<5; i++)
    {
        Angle_Transfer_Buffer[i] = 0x71;        //Э��ͷ
    }
    Angle_Transfer_Buffer[5] = 0x52;            //��������
    for(i=0; i<5; i++)
    {
        Angle_Transfer_Buffer[22 + i] = 0x68;        //Э��β
    }
    /* ���������ݴ��� */
    for(i=0; i<5; i++)
    {
        Mag_Transfer_Buffer[i] = 0x71;          //Э��ͷ
    }
    Mag_Transfer_Buffer[5] = 0x53;              //��������
    for(i=0; i<5; i++)
    {
        Mag_Transfer_Buffer[16 + i] = 0x68;         //Э��β
    }
    /* ��ѹ�����ݴ��� */
    for(i=0; i<5; i++)
    {
        Atmo_Transfer_Buffer[i] = 0x71;          //Э��ͷ
    }
    Atmo_Transfer_Buffer[5] = 0x54;              //��������
    for(i=0; i<5; i++)
    {
        Atmo_Transfer_Buffer[14 + i] = 0x68;         //Э��β
    }
    /* �߶����ݴ��� */
    for(i=0; i<5; i++)
    {
        Hight_Transfer_Buffer[i] = 0x71;          //Э��ͷ
    }
    Hight_Transfer_Buffer[5] = 0x55;              //��������
    for(i=0; i<5; i++)
    {
        Hight_Transfer_Buffer[14 + i] = 0x68;         //Э��β
    }
    /* ��Ԫ�����ݴ��� */
    for(i=0; i<5; i++)
    {
        Quat_Transfer_Buffer[i] = 0x71;          //Э��ͷ
    }
    Quat_Transfer_Buffer[5] = 0x56;              //��������
    for(i=0; i<5; i++)
    {
        Quat_Transfer_Buffer[26 + i] = 0x68;         //Э��β
    }
}

extern UART_HandleTypeDef huart6;

/**
 * @brief IMU_Data_Frame_Transmit ����IMU����֡
 * 
 */
void IMU_Data_Frame_Transmit(void)
{
    uint8_t i;
    
    if(Transfer_MutexHandle != NULL)      //Plantar�������޸Ļ�������Ϊ��
    {
        if(xSemaphoreTake(Transfer_MutexHandle, pdMS_TO_TICKS(20)) == pdTRUE)       //����SPP���ͻ�����
        {
            /* ���ٶ����ݴ��� */
            for(i=0; i<4; i++)
            {
                Acc_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];           //ʱ���
            }
            for(i=0; i<12; i++)
            {
                Acc_Transfer_Buffer[10 + i] = Accelerated_Transfer[i];        //����λ
            }
            comSendBuf(SPP_COM, Acc_Transfer_Buffer, sizeof(Acc_Transfer_Buffer));
            /* ���������ݴ��� */
            for(i=0; i<4; i++)
            {
                Gyro_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];            //ʱ���
            }
            for(i=0; i<12; i++)
            {
                Gyro_Transfer_Buffer[10 + i] = Gyroscope_Transfer[i];           //����λ
            }
            comSendBuf(SPP_COM, Gyro_Transfer_Buffer, sizeof(Gyro_Transfer_Buffer));
            /* �Ƕ����ݴ��� */
            for(i=0; i<4; i++)
            {
                Angle_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];           //ʱ���
            }
            for(i=0; i<12; i++)
            {
                Angle_Transfer_Buffer[10 + i] = Angle_Transfer[i];              //����λ
            }
            comSendBuf(SPP_COM, Angle_Transfer_Buffer, sizeof(Angle_Transfer_Buffer));
            /* ���������ݴ��� */
            for(i=0; i<4; i++)
            {
                Mag_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //ʱ���
            }
            for(i=0; i<6; i++)
            {
                Mag_Transfer_Buffer[10 + i] = Magnetic_Transfer[i];             //����λ
            }
            comSendBuf(SPP_COM, Mag_Transfer_Buffer, sizeof(Mag_Transfer_Buffer));
            /* ��ѹ�����ݴ��� */
            for(i=0; i<4; i++)
            {
                Atmo_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //ʱ���
            }
            for(i=0; i<4; i++)
            {
                Atmo_Transfer_Buffer[10 + i] = Atmospheric_Transfer[i];             //����λ
            }
            comSendBuf(SPP_COM, Atmo_Transfer_Buffer, sizeof(Atmo_Transfer_Buffer));
            /* �߶����ݴ��� */
            for(i=0; i<4; i++)
            {
                Hight_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //ʱ���
            }
            for(i=0; i<4; i++)
            {
                Hight_Transfer_Buffer[10 + i] = Height_Transfer[i];             //����λ
            }
            comSendBuf(SPP_COM, Hight_Transfer_Buffer, sizeof(Hight_Transfer_Buffer));
            /* ��Ԫ�����ݴ��� */
            for(i=0; i<4; i++)
            {
                Quat_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //ʱ���
            }
            for(i=0; i<16; i++)
            {
                Quat_Transfer_Buffer[10 + i] = Quaternion_Transfer[i];             //����λ
            }
            comSendBuf(SPP_COM, Quat_Transfer_Buffer, sizeof(Quat_Transfer_Buffer));
            xSemaphoreGive(Transfer_MutexHandle);           //�ͷ�SPP���ͻ�����
        }
        else
        {
           TASK_LOG("Application for mutex failed \n\r");      //����SPP���ͻ�����ʧ��
        }
    }
    else
    {
        TASK_LOG("The mutex not created \n\r");      //SPP���ͻ�����δ����
    }

}


