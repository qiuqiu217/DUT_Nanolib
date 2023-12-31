/*
*********************************************************************************************************
*
*	文件名称 : Command.c
*	版    本 : V1.0
*	说    明 : 这是室内定位项目串口调试指令集程序文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-06-24         Qiuqiu       首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "Command.h"

/*
*********************************************************************************************************
*	                                      变量声明
*********************************************************************************************************
*/
/* RTOS相关变量 */
extern osEventFlagsId_t Sampling_EventHandle;       //采集事件组
extern osEventFlagsId_t BluetoothEventHandle;       //蓝牙传输事件组
extern osMutexId_t GV_MutexHandle;                  //全局变量互斥锁
extern osSemaphoreId_t SingSamp_SemaphHandle;       //指令单点采样信号量
extern osSemaphoreId_t ArraySamp_SemaphHandle;      //指令阵列采样信号量
extern osSemaphoreId_t RunTime_SemaphHandle;        //任务运行状态统计信号量

extern PLANTAR_S Plantar;                      // 压力传感器采集结构体
extern const uint16_t Plantar_Rate_Settings[10];      //可设置采样延时

/*
*********************************************************************************************************
*	                                      函数定义
*********************************************************************************************************
*/
/**
 * @brief Command_Verify  命令校验
 * @param _COMM_TYPE *COMM_Buf  需要解析的命令
 * @return _RET_TYPE RET_ERROR 命令校验失败    1命令校验成功
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
 * @brief Command_Parsing  解析debug串口收到的命令
 * @param _COMM_TYPE *COMM_Buf  需要解析的命令
 * @return _RET_TYPE RET_ERROR 命令解析失败      RET_INVALID 无效命令/命令执行失败      1命令执行成功
 */
_RET_TYPE Command_Parsing(_COMM_TYPE *COMM_Buf)
{
    _RET_TYPE ret = RET_OK;
    
    /* 指令储存区域 */
	_COMM_TYPE command_type1;
    _COMM_TYPE command_type2;
    _COMM_TYPE command_set;
    
    if(Command_Verify(COMM_Buf) == RET_OK)
    {
        Command_LOG("Received command \n\r");
        command_type1 = *(COMM_Buf + 5);
        command_type2 = *(COMM_Buf + 6);
        command_set = *(COMM_Buf + 7);
        /* 如果是系统设置命令 */
        if(command_type1 == SYS_COMM)
        {
            /* 开启/关闭蓝牙传输 */
            if(command_type2 == SPP_TRAN)
            {
                ret = Change_Bluetooth_Transfer_State(command_set);
            }
            else if(command_type2 == SYS_TASK)
            {
                ret = SYS_Task_RunTimeStats(command_set);
            }
            /* 无效命令 */
            else ret = RET_INVALID;
        }
        /* 如果是压力传感器采集设置命令 */
        else if(command_type1 == PLANTAR_COMM)
        {
            /* 开启/关闭压力传感器采集 */
            if(command_type2 == PLA_SAMP)
            {
                ret = Change_Plantar_Sampling_State(command_set);
            }
            /* 修改压力传感器采集模式 */
            else if(command_type2 == PLA_MODE)
            {
                ret = Change_Plantar_SamplingMode(command_set); 
            }
            /* 指定单点循环采样模式下改变选通点 */
            else if(command_type2 == PLA_SELC)
            {
                ret = Change_Plantar_Selection(command_set); 
            }
            /* 修改压力传感器阵列采样延时时间 */
            else if(command_type2 == PLA_RATE)
            {
                ret = Change_Plantar_SamplingRate(command_set); 
            }
            /* 指定延时时间的阵列采样指令 */
            else if(command_type2 == PLA_INSA)
            {
                ret = RET_OK;
                ret = Plantar_Array_Sampling(command_set); 
            }
            /* 指定延时时间的单点采样指令 */
            else if(command_type2 == PLA_INSS)
            {
                ret = RET_OK;
                ret = Plantar_Single_Sampling(command_set);
            }
            /* 无效命令 */
            else ret = RET_INVALID;
        }
        /* 如果是IMU采集设置命令 */
        else if(command_type1 == IMU_COMM)
        {
            /* 开启/关闭IMU采集 */
            if(command_type2 == IMU_SAMP)
            {
                ret = Change_IMU_Sampling_State(command_set);
            }
            /* 设置IMU输出内容 */
            else if(command_type2 == IMU_OUT)
            {
                ret = Change_IMU_Content(command_set);
            }
            /* 设置IMU回传速率 */
            else if(command_type2 == IMU_RATE)
            {
                ret = Change_IMU_OutputRate(command_set);
            }
            /* 设置IMU通讯波特率 */
            else if(command_type2 == IMU_BAUD)
            {
                ret = Change_IMU_Baud(command_set);
            }
            /* 设置IMU带宽 */
            else if(command_type2 == IMU_BW)
            {
                ret = Change_IMU_Bandwidth(command_set);
            }
            /* 加速度计校准 */
            else if(command_type2 == IMU_ACC)
            {
                ret = Change_IMU_AccCali(command_set);
            }
            /* 磁力计校准 */
            else if(command_type2 == IMU_MAG)
            {
                ret = Change_IMU_MagCali(command_set);
            }
            /* 无效命令 */
            else ret = RET_INVALID;
        }
        /* 无效类型 */
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
 * @brief Change_Bluetooth_Transfer_State  修改蓝牙串口传输状态
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
 */
_RET_TYPE Change_Bluetooth_Transfer_State(_COMM_TYPE set)
{   
    BaseType_t xResult;                     //事件组发送结果
    _RET_TYPE ret = RET_OK;
    
    if(set)      //开启蓝牙传输
    {
        /* 置位蓝牙链接事件位 */
        xResult = xEventGroupSetBits(BluetoothEventHandle,
                                     BLUETOOTH_TRANSFER);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable bluetooth transmission \n\r");
            ret = RET_ERROR;
        }
    }
    else                //关闭蓝牙传输
    {
        /* 清除蓝牙链接事件位 */
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
 * @brief SYS_Task_RunTimeStats  执行任务状态统计
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
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
 * @brief Change_Plantar_Sampling_State  修改压力传感器采集状态
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
 */
_RET_TYPE Change_Plantar_Sampling_State(_COMM_TYPE set)
{
    BaseType_t xResult;                     //事件组发送结果
    _RET_TYPE ret = RET_OK;
    
    if(set)      //开启压力传感器采集
    {
         /* 置位压力传感器采集事件位 */
        xResult = xEventGroupSetBits(Sampling_EventHandle,
                                     PLANTAR_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable plantar sampling \n\r");
            ret = RET_ERROR;
        }  
    }
    else                //关闭压力传感器采集
    {
        /* 清除压力传感器采集事件位 */
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
 * @brief Change_Plantar_SamplingMode  修改压力传感器采集模式
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
 */
_RET_TYPE Change_Plantar_SamplingMode(_COMM_TYPE set)
{
    _RET_TYPE ret;
    
    ret = Plantar_Mode_Change(set);
    
    return ret;
}

/**
 * @brief Change_Plantar_Selection  修改压力传感器选通点
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
 */
_RET_TYPE Change_Plantar_Selection(_COMM_TYPE set)
{
    _RET_TYPE ret = RET_OK;
    uint8_t row,column;
    
    row = set>>4;
    column = set&0x0f;
    /* 判断指令输入行列是否正确，修改选通项 */
    if(Plantar_ChannelJudge(row, column) == RET_OK)        //判断输入行列数据是否正确
    {
        if(Plantar_ChannelChange(row, column) == RET_OK)       //修改单点循环采用的选通项
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
 * @brief Change_Plantar_SamplingRate  修改压力传感器采集延时
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
 */
_RET_TYPE Change_Plantar_SamplingRate(_COMM_TYPE set)
{
    _RET_TYPE ret;
    
    ret = Plantar_Rate_Change(set);
    
    return ret;
}

/**
 * @brief Plantar_Array_Sampling  指令阵列采样
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
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
 * @brief Plantar_Single_Sampling  指令单点采样
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
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
 * @brief Change_IMU_Sampling_State  修改IMU采集状态
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
 */
_RET_TYPE Change_IMU_Sampling_State(_COMM_TYPE set)
{   
    BaseType_t xResult;                     //事件组发送结果
    _RET_TYPE ret = RET_OK;
    
    if(set)      //开启压力传感器采集
    {
        /* 置位IMU采集事件位 */
        xResult = xEventGroupSetBits(Sampling_EventHandle,
                                     IMU_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable IMU sampling \n\r");
            ret = RET_ERROR;
        }
    }
    else                //关闭压力传感器采集
    {
       /* 清除IMU采集事件位 */
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
 * @brief Change_IMU_Content  修改IMU输出内容
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
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
 * @brief Change_IMU_OutputRate  修改IMU输出速率
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
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
 * @brief Change_IMU_Baud  修改IMU通讯波特率
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
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
 * @brief Change_IMU_Bandwidth  修改IMU带宽
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
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
 * @brief Change_IMU_AccCali  IMU加速度计校准
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
 */
_RET_TYPE Change_IMU_AccCali(_COMM_TYPE set)
{   
    _RET_TYPE ret = RET_OK;
    (void) set;
    
    if(WitStartAccCali() != WIT_HAL_OK) ret = RET_ERROR;
    
    return ret;
}

/**
 * @brief Change_IMU_MagCali  IMU磁力计校准
 * @param _COMM_TYPE set  设置参数
 * @return _RET_TYPE RET_ERROR 命令执行失败     RET_OK 命令执行成功
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
