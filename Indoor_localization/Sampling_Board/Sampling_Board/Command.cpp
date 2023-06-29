#include <stdint.h>
/*
*********************************************************************************************************
*
*	文件名称 : Command.cpp
*	版    本 : V1.0
*	说    明 : 这是室内定位项目串口调试指令集程序文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-12     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "Command.h"

/* IMU初始化互斥锁 */
extern SemaphoreHandle_t IMU_Cali_Mutex;
/* 指令采集消息队列 */
extern QueueHandle_t ComSampling_Queue;
/* 传感器采集事件组 */
extern EventGroupHandle_t Sampling_Event;
/* 蓝牙传输事件组 */
extern EventGroupHandle_t Bluetooth_Event;
/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
/**
 * @brief Command
 */
Command::Command()
{
    Command_LOG("Command Object Created \n\r");
}

/**
 * @brief ~Command
 */
Command::~Command()
{
    Command_LOG("Command Object Deleted \n\r");
}

StaticJsonDocument<200> Json_Parsing;   //声明一个JsonDocument对象
Plantar Plantar;
/**
 * @brief Command_Parsing  解析debug串口收到的命令
 * @param const char *json_Command 需要解析的JSON命令
 * @return int8_t -1 命令解析失败      0 无效命令/命令执行失败      1命令执行成功
 */
int8_t Command::Command_Parsing(const char *json_Command) 
{
    BaseType_t x_return = pdTRUE;           // 定义一个创建信息返回值，默认为 pdTRUE 
    uint32_t s_queue;                       // 定义一个接收消息的变量
    BaseType_t xResult;                     //事件组发送结果 

    DeserializationError error = deserializeJson(Json_Parsing, json_Command);         //反序列化JSON数据
    if (error)          //JSON解析失败
    {
        Command_LOG("Command parsing error \n\r");
        return -1;
    }
    else                //JSON解析成功
    {
        const char *Command_Receive = Json_Parsing["Command"];
        Command_LOG("The command received:%s\n\r",Command_Receive);
        /* 接收到的是否是 Change the bluetooth transfer state 命令 */
        if(!strcmp(Command_Receive,"Change the bluetooth transfer state"))
        {
            return Change_Bluetooth_Transfer_State();
        }
        /* 接收到的是否是 Change the plantar sampling state 命令 */
        if(!strcmp(Command_Receive,"Change the plantar sampling state"))
        {
            return Change_Plantar_Sampling_State();
        }
        /* 接收到的是否是 Change the IMU sampling state 命令 */
        if(!strcmp(Command_Receive,"Change the IMU sampling state"))
        {
            return Change_IMU_Sampling_State();
        }
        /* 接收到的是否是 Change the sampling mod 命令 */
        else if(!strcmp(Command_Receive,"Change the sampling mod"))
        {
            return Change_Plantar_SamplingMode();     
        }
        /* 接收到的是否是 Change the selection 命令 */
        else if(!strcmp(Command_Receive,"Change the selection"))
        {
            return Change_Plantar_Selection();
        } 
        /* 接收到的是否是 Change the sampling rate 命令 */
        else if(!strcmp(Command_Receive,"Change the sampling rate"))
        {
            return Change_Plantar_SamplingRate();
        }
        /* 接收到的是否是 Single sampling 命令 */
        else if(!strcmp(Command_Receive,"Single sampling"))
        {
            return Plantar_Single_Sampling();
        }      
        /* 接收到的是否是 Array sampling 命令 */
        else if(!strcmp(Command_Receive,"Array sampling"))
        {
            return Plantar_Array_Sampling();
        }
        /* 接收到的是否是 IMU calibration 命令 */
        else if(!strcmp(Command_Receive,"IMU calibration"))
        {
            return IMU_Calibration();
        }
        else return 0;
    }
}


/**
 * @brief Change_Bluetooth_Transfer_State  修改蓝牙串口传输状态
 * @return int8_t 0 命令执行失败     1命令执行成功
 */
int8_t Command::Change_Bluetooth_Transfer_State(void)
{   
    int8_t ret = 1;
    BaseType_t xResult;                     //事件组发送结果
    
    uint8_t Transfer_State_Receive = Json_Parsing["Transfer State"];
    if(Transfer_State_Receive)      //开启蓝牙传输
    {
        /* 置位蓝牙链接事件位 */
        xResult = xEventGroupSetBits(Bluetooth_Event,
                                     BLUETOOTH_TRANSFER);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable bluetooth transmission \n\r");
            ret = 0;
        }                 
    }
    else                //关闭蓝牙传输
    {
        /* 清除蓝牙链接事件位 */
        xResult = xEventGroupClearBits(Bluetooth_Event,
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
 * @brief Change_Plantar_Sampling_State  修改压力传感器采集状态
 * @return int8_t 0 命令执行失败     1命令执行成功
 */
int8_t Command::Change_Plantar_Sampling_State(void)
{
    int8_t ret = 1;
    BaseType_t xResult;                     //事件组发送结果
    
    uint8_t Sampling_State_Receive = Json_Parsing["Sampling State"];
    if(Sampling_State_Receive)      //开启压力传感器采集
    {
        /* 置位压力传感器采集事件位 */
        xResult = xEventGroupSetBits(Sampling_Event,
                                     PLANTAR_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable plantar sampling \n\r");
            ret = 0;
        }                 
    }
    else                //关闭压力传感器采集
    {
        /* 清除压力传感器采集事件位 */
        xResult = xEventGroupClearBits(Sampling_Event,
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
 * @brief Change_IMU_Sampling_State  修改IMU采集状态
 * @return int8_t 0 命令执行失败     1命令执行成功
 */
int8_t Command::Change_IMU_Sampling_State(void)
{   
    int8_t ret = 1;
    BaseType_t xResult;                     //事件组发送结果
    
    uint8_t Sampling_State_Receive = Json_Parsing["Sampling State"];
    if(Sampling_State_Receive)      //开启IMU采集
    {
        /* 置位IMU采集事件位 */
        xResult = xEventGroupSetBits(Sampling_Event,
                                     IMU_SAMPLING);                     
        if( xResult == pdFAIL )
        {
            Command_LOG("Failed to enable IMU sampling \n\r");
            ret = 0;
        }                 
    }
    else                //关闭IMU采集
    {
        /* 清除IMU采集事件位 */
        xResult = xEventGroupClearBits(Sampling_Event,
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
 * @brief Change_Plantar_SamplingMode  修改压力传感器采集模式
 * @return int8_t 0 命令执行失败     1命令执行成功
 */
int8_t Command::Change_Plantar_SamplingMode(void)
{  
    uint8_t Sampling_Mode_Receive = Json_Parsing["Sampling Mode"];
    if(Plantar.Sampling_Mode_Change(Sampling_Mode_Receive) > 0)
    {
        return 1;
    }
    else return 0;   
}

/**
 * @brief Change_Plantar_Selection  修改压力传感器选通点
 * @return int8_t 0 命令执行失败     1命令执行成功
 */
int8_t Command::Change_Plantar_Selection(void)
{     
    uint8_t Channel_Line_Receive = Json_Parsing["Channel"]["Line"];
    uint8_t Channel_Column_Receive = Json_Parsing["Channel"]["Column"];
    /* 判断指令输入行列是否正确，修改选通项 */
    if(Plantar.Channel_Judge(Channel_Line_Receive, Channel_Column_Receive) == 1)        //判断输入行列数据是否正确
    {
        if(Plantar.Channel_Change(Channel_Line_Receive, Channel_Column_Receive) == 1)       //修改单点循环采用的选通项
        {
            Command_LOG("Change the selection succeed");
            return 1;                    
        }
        else
        {
            Command_LOG("Change the selection failed");
            return 0; 
        }
    }
    else
    {
        Command_LOG("Selection input error");
        return 0; 
    }
}

/**
 * @brief Change_Plantar_SamplingRate  修改压力传感器采集模式
 * @return int8_t 0 命令执行失败     1命令执行成功
 */
int8_t Command::Change_Plantar_SamplingRate(void)
{  
    uint8_t Sampling_Rate_Receive = Json_Parsing["Sampling Rate"];
    if(Plantar.Sampling_Rate_Change(Sampling_Rate_Receive) > 0)
    {
        return 1;
    }
    else return 0;   
}

/**
 * @brief Plantar_Single_Sampling  压力传感器单点指令采集
 * @return int8_t 0 命令执行失败     1命令执行成功
 */
int8_t Command::Plantar_Single_Sampling(void)
{
    BaseType_t x_return = pdTRUE;           // 定义一个创建信息返回值，默认为 pdTRUE 
    uint32_t s_queue;                       // 定义一个接收消息的变量
      
    uint8_t Sampling_Delay_Receive = Json_Parsing["Sampling Delay"];
    vTaskDelay(Sampling_Delay_Receive / portTICK_PERIOD_MS);        //采集延时设置
    s_queue = SINGLE_INSTRUCTION;
    x_return = xQueueSend( ComSampling_Queue,                 //消息队列的句柄 
                           &s_queue,                    //发送的消息内容 
                           0 );                         //等待时间 0
    if(x_return == pdTRUE) return 1;
    else return 0;
}

/**
 * @brief Plantar_Array_Sampling  压力传感器阵列指令采集
 * @return int8_t 0 命令执行失败     1命令执行成功
 */
int8_t Command::Plantar_Array_Sampling(void)
{  
    BaseType_t x_return = pdTRUE;           // 定义一个创建信息返回值，默认为 pdTRUE 
    uint32_t s_queue;                       // 定义一个接收消息的变量
      
    uint8_t Sampling_Delay_Receive = Json_Parsing["Sampling Delay"];
    vTaskDelay(Sampling_Delay_Receive / portTICK_PERIOD_MS);        //采集延时设置
    s_queue = ARRAY_INSTRUCTION;
    x_return = xQueueSend( ComSampling_Queue,                 //消息队列的句柄 
                           &s_queue,                    //发送的消息内容 
                           0 );                         //等待时间 0
    if(x_return == pdTRUE) return 1;
    else return 0;
}

/**
 * @brief IMU_Calibration  IMU校正
 * @return int8_t 0 命令执行失败     1命令执行成功
 */
int8_t Command::IMU_Calibration(void)
{
    uint8_t Calibration_Type_Receive = Json_Parsing["Calibration Type"];
    if(Calibration_Type_Receive == 0)
    {
        return Magnetometer_calibration();
    }
    else if(Calibration_Type_Receive == 1)
    {
        return Acceleration_calibration();
    }
    else if(Calibration_Type_Receive == 2)
    {
        return Angle_calibration();
    }
}

/**
 * @brief Magnetometer_calibration  初始化磁力计
 * @return int8_t 0 无效命令/命令执行失败      1命令执行成功
 */
int8_t Magnetometer_calibration(void)
{
    BaseType_t xResult;                     //事件组发送结果
    EventBits_t IMU_SamplingBits;           //原来的IMU采集事件组位
    EventBits_t Transfer_SamplingBits;      //原来的蓝牙传输事件组位
    int8_t ret = 1;

    if(IMU_Cali_Mutex != NULL)      //修改全局变量互斥锁不为空
    {
        if(xSemaphoreTake(IMU_Cali_Mutex, 10/portTICK_PERIOD_MS) == pdTRUE)       //申请IMU校正互斥锁
        {
            /* 停止IMU输出 */
            WitSetContent(RSW_MASK);
            /* 停止IMU采集 */
            IMU_SamplingBits = xEventGroupGetBits(Sampling_Event);
            if((IMU_SamplingBits & IMU_SAMPLING) != 0)
            {
                xResult = xEventGroupClearBits(Sampling_Event,
                                            IMU_SAMPLING ); 
                if( xResult == pdFAIL )
                {
                    Command_LOG("Failed to disable IMU sampling \n\r");
                    ret = 0;
                }       
            }
            /* 磁力计校准 */
            if(WitStartMagCali() == WIT_HAL_OK)
            {
                vTaskDelay(10000 / portTICK_PERIOD_MS);      //校准时间
                WitStopMagCali();
            }
            else ret = 0;
            /* 恢复IMU采集 */
            if((IMU_SamplingBits & IMU_SAMPLING) != 0)
            {
                xResult = xEventGroupSetBits(Sampling_Event,
                                            IMU_SAMPLING ); 
                if( xResult == pdFAIL )
                {
                    Command_LOG("Failed to enable IMU sampling \n\r");
                    ret = 0;
                }       
            }
            /* 恢复IMU输出 */
            WitSetContent(RSW_ACC|RSW_GYRO|RSW_ANGLE|RSW_MAG|RSW_PRESS|RSW_Q);
            xSemaphoreGive(IMU_Cali_Mutex);           //释放修改选通项互斥锁
            return ret;
        }
        else
        {
            Command_LOG("Application for IMU_Cali_Mutex failed \n\r");
            return 0;                        
        }
    }
    else
    {
        Command_LOG("The IMU_Cali_Mutex is not created \n\r");
        return 0;         
    }
}

/**
 * @brief Acceleration_calibration  加速度校准
 * @return int8_t 0 无效命令/命令执行失败      1命令执行成功
 */
int8_t Acceleration_calibration(void)
{
    BaseType_t xResult;                     //事件组发送结果
    EventBits_t IMU_SamplingBits;           //原来的IMU采集事件组位
    EventBits_t Transfer_SamplingBits;      //原来的蓝牙传输事件组位
    int8_t ret = 1;

    if(IMU_Cali_Mutex != NULL)      //修改全局变量互斥锁不为空
    {
        if(xSemaphoreTake(IMU_Cali_Mutex, 10/portTICK_PERIOD_MS) == pdTRUE)       //申请IMU校正互斥锁
        {
            /* 停止IMU采集 */
            IMU_SamplingBits = xEventGroupGetBits(Sampling_Event);
            if((IMU_SamplingBits & IMU_SAMPLING) != 0)
            {
                xResult = xEventGroupClearBits(Sampling_Event,
                                            IMU_SAMPLING ); 
                if( xResult == pdFAIL )
                {
                    Command_LOG("Failed to disable IMU sampling \n\r");
                    ret = 0;
                }       
            }
            /* 角速度校准 */
            if(WitStartAccCali() == WIT_HAL_OK)
            {
                vTaskDelay(2000 / portTICK_PERIOD_MS);      //校准时间
                WitStopAccCali();
            }
            else ret = 0;
            /* 恢复IMU采集 */
            if((IMU_SamplingBits & IMU_SAMPLING) != 0)
            {
                xResult = xEventGroupSetBits(Sampling_Event,
                                            IMU_SAMPLING ); 
                if( xResult == pdFAIL )
                {
                    Command_LOG("Failed to enable IMU sampling \n\r");
                    ret = 0;
                }       
            }
            xSemaphoreGive(IMU_Cali_Mutex);           //释放修改选通项互斥锁
            return ret;
        }
        else
        {
            Command_LOG("Application for IMU_Cali_Mutex failed \n\r");
            return 0;                        
        }
    }
    else
    {
        Command_LOG("The IMU_Cali_Mutex is not created \n\r");
        return 0;         
    }
}

/**
 * @brief Angle_calibration  角度校准
 * @return int8_t 0 无效命令/命令执行失败      1命令执行成功
 */
int8_t Angle_calibration(void)
{
    BaseType_t xResult;                     //事件组发送结果
    EventBits_t IMU_SamplingBits;           //原来的IMU采集事件组位
    EventBits_t Transfer_SamplingBits;      //原来的蓝牙传输事件组位
    int8_t ret = 1;

    if(IMU_Cali_Mutex != NULL)      //修改全局变量互斥锁不为空
    {
        if(xSemaphoreTake(IMU_Cali_Mutex, 10/portTICK_PERIOD_MS) == pdTRUE)       //申请IMU校正互斥锁
        {
            /* 停止IMU采集 */
            IMU_SamplingBits = xEventGroupGetBits(Sampling_Event);
            if((IMU_SamplingBits & IMU_SAMPLING) != 0)
            {
                xResult = xEventGroupClearBits(Sampling_Event,
                                            IMU_SAMPLING ); 
                if( xResult == pdFAIL )
                {
                    Command_LOG("Failed to disable IMU sampling \n\r");
                    ret = 0;
                }       
            }
            /* 角度校准 */
            if(WitCaliRefAngle() == WIT_HAL_OK);
            else ret = 0;
            /* 恢复IMU采集 */
            if((IMU_SamplingBits & IMU_SAMPLING) != 0)
            {
                xResult = xEventGroupSetBits(Sampling_Event,
                                            IMU_SAMPLING ); 
                if( xResult == pdFAIL )
                {
                    Command_LOG("Failed to enable IMU sampling \n\r");
                    ret = 0;
                }       
            }
            xSemaphoreGive(IMU_Cali_Mutex);           //释放修改选通项互斥锁
            return ret;
        }
        else
        {
            Command_LOG("Application for IMU_Cali_Mutex failed \n\r");
            return 0;                        
        }
    }
    else
    {
        Command_LOG("The IMU_Cali_Mutex is not created \n\r");
        return 0;         
    }
}
