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

extern PLANTAR_S Plantar;                      // ѹ���������ɼ��ṹ��
extern osEventFlagsId_t Sampling_EventHandle;       //�ɼ��¼���
extern osEventFlagsId_t BluetoothEventHandle;       //���������¼���
extern osMutexId_t GV_MutexHandle;                  //ȫ�ֱ���������


inline int8_t Command_Verify(_COMM_TYPE* COMM_Buf)
{
    uint8_t i;
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
 * @return int8_t -1 �������ʧ��      0 ��Ч����/����ִ��ʧ��      1����ִ�гɹ�
 */
int8_t Command_Parsing(_COMM_TYPE *COMM_Buf)
{
    int8_t ret = RET_OK;
    
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
                //ret = Plantar_Array_Sampling(command_set); 
            }
            /* ָ����ʱʱ��ĵ������ָ�� */
            else if(command_type2 == PLA_INSS)
            {
                ret = RET_OK;
                //ret = Plantar_Single_Sampling(command_set);
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
            /* ��Ч���� */
            else ret = RET_INVALID;
        }
        /* ��Ч���� */
        else
        {
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
 * @return int8_t 0 ����ִ��ʧ��     1����ִ�гɹ�
 */
int8_t Change_Bluetooth_Transfer_State(_COMM_TYPE set)
{   
    BaseType_t xResult;                     //�¼��鷢�ͽ��
    int8_t ret = 1;
    
    if(set)      //������������
    {
        /* ��λ���������¼�λ */
        xResult = xEventGroupSetBits(BluetoothEventHandle,
                                     BLUETOOTH_TRANSFER);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable bluetooth transmission \n\r");
            ret = 0;
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
            ret = 0;
        }
    }
    
    return ret;
}

/**
 * @brief Change_Plantar_Sampling_State  �޸�ѹ���������ɼ�״̬
 * @param _COMM_TYPE set  ���ò���
 * @return int8_t 0 ����ִ��ʧ��     1����ִ�гɹ�
 */
int8_t Change_Plantar_Sampling_State(_COMM_TYPE set)
{
    BaseType_t xResult;                     //�¼��鷢�ͽ��
    int8_t ret = 1;
    
    if(set)      //����ѹ���������ɼ�
    {
         /* ��λѹ���������ɼ��¼�λ */
        xResult = xEventGroupSetBits(Sampling_EventHandle,
                                     PLANTAR_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable plantar sampling \n\r");
            ret = 0;
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
            ret = 0;
        }
    }
    
    return ret;
}

/**
 * @brief Change_IMU_Sampling_State  �޸�IMU�ɼ�״̬
 * @param _COMM_TYPE set  ���ò���
 * @return int8_t 0 ����ִ��ʧ��     1����ִ�гɹ�
 */
int8_t Change_IMU_Sampling_State(_COMM_TYPE set)
{   
    BaseType_t xResult;                     //�¼��鷢�ͽ��
    int8_t ret = 1;
    
    if(set)      //����ѹ���������ɼ�
    {
        /* ��λIMU�ɼ��¼�λ */
        xResult = xEventGroupSetBits(Sampling_EventHandle,
                                     IMU_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable IMU sampling \n\r");
            ret = 0;
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
            ret = 0;
        }
    }
    
    return ret;
}

/**
 * @brief Change_Plantar_SamplingMode  �޸�ѹ���������ɼ�ģʽ
 * @param _COMM_TYPE set  ���ò���
 * @return int8_t 0 ����ִ��ʧ��     1����ִ�гɹ�
 */
int8_t Change_Plantar_SamplingMode(_COMM_TYPE set)
{
    int8_t ret = 1;
    
    ret = Plantar_Mode_Change(set);
    
    return ret;
}

/**
 * @brief Change_Plantar_Selection  �޸�ѹ��������ѡͨ��
 * @param _COMM_TYPE set  ���ò���
 * @return int8_t 0 ����ִ��ʧ��     1����ִ�гɹ�
 */
int8_t Change_Plantar_Selection(_COMM_TYPE set)
{
    int8_t ret = 1;
    uint8_t row,column;
    
    row = set>>4;
    column = set&0x0f;
    printf("row:%d column:%d",row,column);
    /* �ж�ָ�����������Ƿ���ȷ���޸�ѡͨ�� */
    if(Plantar_ChannelJudge(row, column) == 1)        //�ж��������������Ƿ���ȷ
    {
        if(Plantar_ChannelChange(row, column) == 1)       //�޸ĵ���ѭ�����õ�ѡͨ��
        {
            Command_LOG("Change the selection succeed");                  
        }
        else
        {
            Command_LOG("Change the selection failed");
            ret = 0; 
        }
    }
    else
    {
        Command_LOG("Selection input error");
        ret = 0; 
    }
    return ret;
}

/**
 * @brief Change_Plantar_SamplingRate  �޸�ѹ���������ɼ���ʱ
 * @param _COMM_TYPE set  ���ò���
 * @return int8_t 0 ����ִ��ʧ��     1����ִ�гɹ�
 */
int8_t Change_Plantar_SamplingRate(_COMM_TYPE set)
{
    int8_t ret = 1;
    
    ret = Plantar_Rate_Change(set);
    return ret;
}


