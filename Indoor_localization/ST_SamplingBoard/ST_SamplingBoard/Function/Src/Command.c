/*
*********************************************************************************************************
*
*	�ļ����� : Command.c
*	��    �� : V1.0
*	˵    �� : �������ڶ�λ��Ŀ���ڵ���ָ������ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-06-24         Qiuqiu       �״η���
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "Command.h"

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/* RTOS��ر��� */
extern osEventFlagsId_t Sampling_EventHandle;       //�ɼ��¼���
extern osEventFlagsId_t BluetoothEventHandle;       //���������¼���
extern osMutexId_t GV_MutexHandle;                  //ȫ�ֱ���������
extern osSemaphoreId_t SingSamp_SemaphHandle;       //ָ�������ź���
extern osSemaphoreId_t ArraySamp_SemaphHandle;      //ָ�����в����ź���
extern osSemaphoreId_t RunTime_SemaphHandle;        //��������״̬ͳ���ź���

extern PLANTAR_S Plantar;                      // ѹ���������ɼ��ṹ��
extern const uint16_t Plantar_Rate_Settings[10];      //�����ò�����ʱ

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/**
 * @brief Command_Verify  ����У��
 * @param _COMM_TYPE *COMM_Buf  ��Ҫ����������
 * @return _RET_TYPE RET_ERROR ����У��ʧ��    1����У��ɹ�
 */
inline _RET_TYPE Command_Verify(_COMM_TYPE* COMM_Buf)
{
    _RET_TYPE i;
    
    for(i=0;i<5;i++)
    {
        if(* (COMM_Buf + i) != 0x71) return RET_ERROR;
    }
    for(i=0;i<5;i++)
    {
        if(* (COMM_Buf + i + 8) != 0x68) return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Command_Parsing  ����debug�����յ�������
 * @param _COMM_TYPE *COMM_Buf  ��Ҫ����������
 * @return _RET_TYPE RET_ERROR �������ʧ��      RET_INVALID ��Ч����/����ִ��ʧ��      1����ִ�гɹ�
 */
_RET_TYPE Command_Parsing(_COMM_TYPE *COMM_Buf)
{
    _RET_TYPE ret = RET_OK;
    
    /* ָ������� */
	_COMM_TYPE command_type1;
    _COMM_TYPE command_type2;
    _COMM_TYPE command_set;
    
    if(Command_Verify(COMM_Buf) == RET_OK)
    {
        Command_LOG("Received command \n\r");
        command_type1 = *(COMM_Buf + 5);
        command_type2 = *(COMM_Buf + 6);
        command_set = *(COMM_Buf + 7);
        /* �����ϵͳ�������� */
        if(command_type1 == SYS_COMM)
        {
            /* ����/�ر��������� */
            if(command_type2 == SPP_TRAN)
            {
                ret = Change_Bluetooth_Transfer_State(command_set);
            }
            else if(command_type2 == SYS_TASK)
            {
                ret = SYS_Task_RunTimeStats(command_set);
            }
            /* ��Ч���� */
            else ret = RET_INVALID;
        }
        /* �����ѹ���������ɼ��������� */
        else if(command_type1 == PLANTAR_COMM)
        {
            /* ����/�ر�ѹ���������ɼ� */
            if(command_type2 == PLA_SAMP)
            {
                ret = Change_Plantar_Sampling_State(command_set);
            }
            /* �޸�ѹ���������ɼ�ģʽ */
            else if(command_type2 == PLA_MODE)
            {
                ret = Change_Plantar_SamplingMode(command_set); 
            }
            /* ָ������ѭ������ģʽ�¸ı�ѡͨ�� */
            else if(command_type2 == PLA_SELC)
            {
                ret = Change_Plantar_Selection(command_set); 
            }
            /* �޸�ѹ�����������в�����ʱʱ�� */
            else if(command_type2 == PLA_RATE)
            {
                ret = Change_Plantar_SamplingRate(command_set); 
            }
            /* ָ����ʱʱ������в���ָ�� */
            else if(command_type2 == PLA_INSA)
            {
                ret = RET_OK;
                ret = Plantar_Array_Sampling(command_set); 
            }
            /* ָ����ʱʱ��ĵ������ָ�� */
            else if(command_type2 == PLA_INSS)
            {
                ret = RET_OK;
                ret = Plantar_Single_Sampling(command_set);
            }
            /* ��Ч���� */
            else ret = RET_INVALID;
        }
        /* �����IMU�ɼ��������� */
        else if(command_type1 == IMU_COMM)
        {
            /* ����/�ر�IMU�ɼ� */
            if(command_type2 == IMU_SAMP)
            {
                ret = Change_IMU_Sampling_State(command_set);
            }
            /* ����IMU������� */
            else if(command_type2 == IMU_OUT)
            {
                ret = Change_IMU_Content(command_set);
            }
            /* ����IMU�ش����� */
            else if(command_type2 == IMU_RATE)
            {
                ret = Change_IMU_OutputRate(command_set);
            }
            /* ����IMUͨѶ������ */
            else if(command_type2 == IMU_BAUD)
            {
                ret = Change_IMU_Baud(command_set);
            }
            /* ����IMU���� */
            else if(command_type2 == IMU_BW)
            {
                ret = Change_IMU_Bandwidth(command_set);
            }
            /* ���ٶȼ�У׼ */
            else if(command_type2 == IMU_ACC)
            {
                ret = Change_IMU_AccCali(command_set);
            }
            /* ������У׼ */
            else if(command_type2 == IMU_MAG)
            {
                ret = Change_IMU_MagCali(command_set);
            }
            /* ��Ч���� */
            else ret = RET_INVALID;
        }
        /* ��Ч���� */
        else
        {
            ret = RET_INVALID;
        }
    }
    else
    {
        Command_LOG("Command error \n\r");
        ret = RET_ERROR;
    }
    
    return ret;
}

/**
 * @brief Change_Bluetooth_Transfer_State  �޸��������ڴ���״̬
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_Bluetooth_Transfer_State(_COMM_TYPE set)
{   
    BaseType_t xResult;                     //�¼��鷢�ͽ��
    _RET_TYPE ret = RET_OK;
    
    if(set)      //������������
    {
        /* ��λ���������¼�λ */
        xResult = xEventGroupSetBits(BluetoothEventHandle,
                                     BLUETOOTH_TRANSFER);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable bluetooth transmission \n\r");
            ret = RET_ERROR;
        }
    }
    else                //�ر���������
    {
        /* ������������¼�λ */
        xResult = xEventGroupClearBits(BluetoothEventHandle,
                                       BLUETOOTH_TRANSFER);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to disable bluetooth transmission \n\r");
            ret = RET_ERROR;
        }
    }
    
    return ret;
}


/**
 * @brief SYS_Task_RunTimeStats  ִ������״̬ͳ��
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE SYS_Task_RunTimeStats(_COMM_TYPE set)
{   
    _RET_TYPE ret = RET_OK;
    
    if(xSemaphoreGive(RunTime_SemaphHandle) != pdTRUE)
    {
        ret = RET_ERROR;
    }
    
    return ret;
}

/**
 * @brief Change_Plantar_Sampling_State  �޸�ѹ���������ɼ�״̬
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_Plantar_Sampling_State(_COMM_TYPE set)
{
    BaseType_t xResult;                     //�¼��鷢�ͽ��
    _RET_TYPE ret = RET_OK;
    
    if(set)      //����ѹ���������ɼ�
    {
         /* ��λѹ���������ɼ��¼�λ */
        xResult = xEventGroupSetBits(Sampling_EventHandle,
                                     PLANTAR_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable plantar sampling \n\r");
            ret = RET_ERROR;
        }  
    }
    else                //�ر�ѹ���������ɼ�
    {
        /* ���ѹ���������ɼ��¼�λ */
        xResult = xEventGroupClearBits(Sampling_EventHandle,
                                       PLANTAR_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to disable plantar sampling \n\r");
            ret = RET_ERROR;
        }
    }
    
    return ret;
}

/**
 * @brief Change_Plantar_SamplingMode  �޸�ѹ���������ɼ�ģʽ
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_Plantar_SamplingMode(_COMM_TYPE set)
{
    _RET_TYPE ret;
    
    ret = Plantar_Mode_Change(set);
    
    return ret;
}

/**
 * @brief Change_Plantar_Selection  �޸�ѹ��������ѡͨ��
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_Plantar_Selection(_COMM_TYPE set)
{
    _RET_TYPE ret = RET_OK;
    uint8_t row,column;
    
    row = set>>4;
    column = set&0x0f;
    /* �ж�ָ�����������Ƿ���ȷ���޸�ѡͨ�� */
    if(Plantar_ChannelJudge(row, column) == RET_OK)        //�ж��������������Ƿ���ȷ
    {
        if(Plantar_ChannelChange(row, column) == RET_OK)       //�޸ĵ���ѭ�����õ�ѡͨ��
        {
            Command_LOG("Change the selection succeed");                  
        }
        else
        {
            Command_LOG("Change the selection failed");
            ret = RET_ERROR; 
        }
    }
    else
    {
        Command_LOG("Selection input error");
        ret = RET_ERROR; 
    }
    
    return ret;
}

/**
 * @brief Change_Plantar_SamplingRate  �޸�ѹ���������ɼ���ʱ
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_Plantar_SamplingRate(_COMM_TYPE set)
{
    _RET_TYPE ret;
    
    ret = Plantar_Rate_Change(set);
    
    return ret;
}

/**
 * @brief Plantar_Array_Sampling  ָ�����в���
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Plantar_Array_Sampling(_COMM_TYPE set)
{
    _RET_TYPE ret = RET_OK;
    
    osDelay(pdMS_TO_TICKS(Plantar_Rate_Settings[set]));
    if(xSemaphoreGive(ArraySamp_SemaphHandle) != pdTRUE)
    {
        ret = RET_ERROR;
    }

    return ret;
}

/**
 * @brief Plantar_Single_Sampling  ָ������
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Plantar_Single_Sampling(_COMM_TYPE set)
{
    _RET_TYPE ret = RET_OK;
    
    osDelay(pdMS_TO_TICKS(Plantar_Rate_Settings[set]));
    if(xSemaphoreGive(SingSamp_SemaphHandle) != pdTRUE)
    {
        ret = RET_ERROR;
    }
                              
    return ret;
}


/**
 * @brief Change_IMU_Sampling_State  �޸�IMU�ɼ�״̬
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_IMU_Sampling_State(_COMM_TYPE set)
{   
    BaseType_t xResult;                     //�¼��鷢�ͽ��
    _RET_TYPE ret = RET_OK;
    
    if(set)      //����ѹ���������ɼ�
    {
        /* ��λIMU�ɼ��¼�λ */
        xResult = xEventGroupSetBits(Sampling_EventHandle,
                                     IMU_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable IMU sampling \n\r");
            ret = RET_ERROR;
        }
    }
    else                //�ر�ѹ���������ɼ�
    {
       /* ���IMU�ɼ��¼�λ */
        xResult = xEventGroupClearBits(Sampling_EventHandle,
                                       IMU_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to disable IMU sampling \n\r");
            ret = RET_ERROR;
        }
    }
    
    return ret;
}


/**
 * @brief Change_IMU_Content  �޸�IMU�������
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_IMU_Content(_COMM_TYPE set)
{   
    _RET_TYPE ret = RET_OK;
    
    if(set == 0x00)
    {
        if(WitSetContent(RSW_ACC) != WIT_HAL_OK)  ret = RET_ERROR;
    }
    else if(set == 0x01)
    {
        if(WitSetContent(RSW_ACC|RSW_GYRO|RSW_ANGLE|RSW_MAG) != WIT_HAL_OK)  ret = RET_ERROR;
    }
    else if(set == 0x02)
    {
        if(WitSetContent(RSW_ACC|RSW_GYRO|RSW_ANGLE|RSW_MAG|RSW_PRESS|RSW_Q) != WIT_HAL_OK)  ret = RET_ERROR;
    }
    else
    {
        ret = RET_ERROR;
    }
    
    return ret;
}

/**
 * @brief Change_IMU_OutputRate  �޸�IMU�������
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_IMU_OutputRate(_COMM_TYPE set)
{   
    _RET_TYPE ret = RET_OK;
    
    if(set == 0x00)
    {
        if(WitSetOutputRate(RRATE_10HZ) != WIT_HAL_OK)   ret = RET_ERROR;
    }
    else if(set == 0x01)
    {
        if(WitSetOutputRate(RRATE_100HZ) != WIT_HAL_OK)  ret = RET_ERROR;
    }
    else if(set == 0x02)
    {
        if(WitSetOutputRate(RRATE_200HZ) != WIT_HAL_OK)  ret = RET_ERROR;
    }
    else
    {
        ret = RET_ERROR;
    }
    
    return ret;
}

/**
 * @brief Change_IMU_Baud  �޸�IMUͨѶ������
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_IMU_Baud(_COMM_TYPE set)
{   
    _RET_TYPE ret = RET_OK;
    
    if(set == 0x00)
    {
        if(WitSetUartBaud(WIT_BAUD_9600) != WIT_HAL_OK)   ret = RET_ERROR;
        else comSetBaud(IMU_COM, 9600);
    }
    else if(set == 0x01)
    {
        if(WitSetUartBaud(WIT_BAUD_115200) != WIT_HAL_OK)  ret = RET_ERROR;
        else comSetBaud(IMU_COM, 115200);
    }
    else if(set == 0x02)
    {
        if(WitSetUartBaud(WIT_BAUD_460800) != WIT_HAL_OK)  ret = RET_ERROR;
        else comSetBaud(IMU_COM, 460800);
    }
    else if(set == 0x03)
    {
        if(WitSetUartBaud(WIT_BAUD_921600) != WIT_HAL_OK)  ret = RET_ERROR;
        else comSetBaud(IMU_COM, 921600);
    }
    else
    {
        ret = RET_ERROR;
    }
    
    return ret;
}

/**
 * @brief Change_IMU_Bandwidth  �޸�IMU����
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_IMU_Bandwidth(_COMM_TYPE set)
{   
    _RET_TYPE ret = RET_OK;
    
    if(set == 0x00)
    {
        if(WitSetBandwidth(BANDWIDTH_5HZ) != WIT_HAL_OK)    ret = RET_ERROR;
    }
    else if(set == 0x01)
    {
        if(WitSetBandwidth(BANDWIDTH_94HZ) != WIT_HAL_OK)   ret = RET_ERROR;
    }
    else if(set == 0x02)
    {
        if(WitSetBandwidth(BANDWIDTH_256HZ) != WIT_HAL_OK)   ret = RET_ERROR;
    }
    else
    {
        ret = RET_ERROR;
    }
    
    return ret;
}

/**
 * @brief Change_IMU_AccCali  IMU���ٶȼ�У׼
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_IMU_AccCali(_COMM_TYPE set)
{   
    _RET_TYPE ret = RET_OK;
    (void) set;
    
    if(WitStartAccCali() != WIT_HAL_OK) ret = RET_ERROR;
    
    return ret;
}

/**
 * @brief Change_IMU_MagCali  IMU������У׼
 * @param _COMM_TYPE set  ���ò���
 * @return _RET_TYPE RET_ERROR ����ִ��ʧ��     RET_OK ����ִ�гɹ�
 */
_RET_TYPE Change_IMU_MagCali(_COMM_TYPE set)
{   
    _RET_TYPE ret = RET_OK;
    
    if(set == 0x00)
    {
        if(WitStopMagCali() != WIT_HAL_OK)    ret = RET_ERROR;
    }
    else if(set == 0x01)
    {
        if(WitStartMagCali() != WIT_HAL_OK)   ret = RET_ERROR;
    }
    else
    {
        ret = RET_ERROR;
    }
    
    return ret;
}
