/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/*
*********************************************************************************************************
*	                                      变量声明
*********************************************************************************************************
*/
extern Plantar_Buff_TypeDef Plantar_Buff;              //压力传感器阵列Buff结构体声明
extern float Pressure_Buff[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];     //压力传感器拟合压力值缓存区
extern UART_RXBUF Uart_Receive;         // 串口指令接收缓冲区结构体声明
extern volatile uint16_t s_cDataUpdate;
extern volatile char s_cCmd;

uint8_t Receive_Buff[RX_BUFF_SIZE];     //Debug串口接收缓冲区
uint8_t Receive_Buff1[RX_BUFF_SIZE];    //SPP接收缓冲区
uint8_t Command_Buff[13];               //接收指令缓冲区
PLANTAR_S Plantar;                      // 压力传感器采集结构体
uint8_t RET_OK_BUFF[]={0x71,0x71,0x71,0x71,0x71,0x60,0x10,0x01,0x68,0x68,0x68,0x68,0x68};
uint8_t RET_ERROR_BUFF[]={0x71,0x71,0x71,0x71,0x71,0x60,0x10,0x00,0x68,0x68,0x68,0x68,0x68};
char RunTimeInfo[400];          //任务运行统计结果储存区

/* 定义初始任务 */
osThreadId_t Task_StartHandle;
const osThreadAttr_t Task_Start_attributes = 
{
    .name = "Start",
    .stack_size = 128 * 2,
    .priority = (osPriority_t) osPriorityNormal,
};
/* 定义LED任务 */
osThreadId_t Task_LEDHandle;
const osThreadAttr_t Task_LED_attributes = 
{
    .name = "LED",
    .stack_size = 128 * 1,
    .priority = (osPriority_t) osPriorityLow,
};
/* 定义Debug串口任务 */
osThreadId_t Task_DebugUsartHandle;
const osThreadAttr_t Task_DebugUsart_attributes = 
{
    .name = "Debug Usart",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};
/* 定义SPP接收任务 */
osThreadId_t Task_SPPRXHandle;
const osThreadAttr_t Task_SPPRX_attributes = 
{
    .name = "SPP Receive",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal2,
};
/* 定义指令处理任务 */
osThreadId_t Task_CommandHandle;
const osThreadAttr_t Task_Command_attributes = 
{
    .name = "Command",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal1,
};
/* 定义压力传感器采集任务 */
osThreadId_t Task_PlantarHandle;
const osThreadAttr_t Task_Plantar_attributes = 
{
    .name = "Plantar",
    .stack_size = 128 * 8,
    .priority = (osPriority_t) osPriorityRealtime,
};
/* 定义IMU采集任务 */
osThreadId_t Task_IMUHandle;
const osThreadAttr_t Task_IMU_attributes = 
{
    .name = "IMU",
    .stack_size = 128 * 8,
    .priority = (osPriority_t) osPriorityRealtime1,
};
/* 定义Plantar数据发送任务 */
osThreadId_t Task_PlantarDataHandle;
const osThreadAttr_t Task_PlantarData_attributes = 
{
    .name = "Plantar Data",
    .stack_size = 128 * 10,
    .priority = (osPriority_t) osPriorityHigh6,
};
/* 定义IMU数据发送任务 */
osThreadId_t Task_IMUDataHandle;
const osThreadAttr_t Task_IMUData_attributes = 
{
    .name = "IMU Data",
    .stack_size = 128 * 10,
    .priority = (osPriority_t) osPriorityHigh7,
};
/* 判断蓝牙连接任务 */
osThreadId_t Task_SPPConnectHandle;
const osThreadAttr_t Task_SPPConnect_attributes = 
{
    .name = "SPP Connect",
    .stack_size = 128 * 2,
    .priority = (osPriority_t) osPriorityNormal1,
};
/* 任务运行时间统计任务 */
osThreadId_t Task_RunTimeStatsHandle;
const osThreadAttr_t Task_RunTimeStats_attributes = 
{
    .name = "RunTime Stats",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};


/* 全局变量修改互斥锁 */
osMutexId_t GV_MutexHandle;
const osMutexAttr_t GV_Mutex_attributes = 
{
    .name = "Global Variable Mutex"
};
/* SPP接收互斥锁 */
osMutexId_t SPP_MutexHandle;
const osMutexAttr_t SPP_Mutex_attributes = 
{
    .name = "SPP Receive Mutex"
};
/* SPP发送互斥锁 */
osMutexId_t Transfer_MutexHandle;
const osMutexAttr_t Transfer_Mutex_attributes = 
{
    .name = "Transfer Mutex"
};
/* 指令处理信号量 */
osSemaphoreId_t Command_SemaphHandle;
const osSemaphoreAttr_t Command_Semaph_attributes = 
{
    .name = "Command Semaphore"
};
/* 压力传感器数据传输信号量 */
osSemaphoreId_t Plantar_SemaphHandle;
const osSemaphoreAttr_t Plantar_Semaph_attributes = 
{
    .name = "Plantar Transfer Semaphore"
};
/* IMU数据传输信号量 */
osSemaphoreId_t IMU_SemaphHandle;
const osSemaphoreAttr_t IMU_Semaph_attributes = 
{
    .name = "IMU Transfer Semaphore"
};
/* 指令单点采样信号量 */
osSemaphoreId_t SingSamp_SemaphHandle;
const osSemaphoreAttr_t SingSamp_Semaph_attributes = 
{
    .name = "SingSamp Semaphore"
};
/* 指令阵列采样信号量 */
osSemaphoreId_t ArraySamp_SemaphHandle;
const osSemaphoreAttr_t ArraySamp_Semaph_attributes = 
{
    .name = "ArraySamp Semaphore"
};
/* 任务运行统计信号量 */
osSemaphoreId_t RunTime_SemaphHandle;
const osSemaphoreAttr_t RunTime_Semaph_attributes = 
{
    .name = "RunTime Semaphore"
};
/* 蓝牙传输事件组 */
osEventFlagsId_t BluetoothEventHandle;
const osEventFlagsAttr_t Bluetooth_Event_attributes = 
{
    .name = "Bluetooth Event"
};
/* 传感器采集事件组 */
osEventFlagsId_t Sampling_EventHandle;
const osEventFlagsAttr_t Sampling_Event_attributes = 
{
    .name = "Sampling Event"
};

/* FreeRTOS initialization */
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* 任务函数声明 */
static void AppTask_Start(void *argument);
static void AppTask_Led(void *argument);
static void AppTask_DebugUsart(void *argument);
static void AppTask_SPPReceive(void *argument);
static void AppTask_Command(void *argument);
static void AppTask_Plantar(void *argument);
static void AppTask_IMU(void *argument);
static void AppTask_PlantarData(void *argument);
static void AppTask_IMUData(void *argument);
static void AppTask_SPPConnect(void *argument);
static void AppTask_RunTimeStats(void *argument);



/*
*********************************************************************************************************
*	                                      函数定义
*********************************************************************************************************
*/
/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) 
{
    /* 互斥锁创建 */
    GV_MutexHandle = osMutexNew(&GV_Mutex_attributes);
    SPP_MutexHandle = osMutexNew(&SPP_Mutex_attributes);
    Transfer_MutexHandle = osMutexNew(&Transfer_Mutex_attributes);
    /* 信号量创建 */
    Command_SemaphHandle = osSemaphoreNew(1,0,&Command_Semaph_attributes);
    Plantar_SemaphHandle = osSemaphoreNew(1,0,&Plantar_Semaph_attributes);
    IMU_SemaphHandle = osSemaphoreNew(1,0,&IMU_Semaph_attributes);
    SingSamp_SemaphHandle = osSemaphoreNew(1,0,&SingSamp_Semaph_attributes);
    ArraySamp_SemaphHandle = osSemaphoreNew(1,0,&ArraySamp_Semaph_attributes);
    RunTime_SemaphHandle = osSemaphoreNew(1,0,&RunTime_Semaph_attributes);
    /* 事件组创建 */
    BluetoothEventHandle = osEventFlagsNew(&Bluetooth_Event_attributes);
    Sampling_EventHandle = osEventFlagsNew(&Sampling_Event_attributes);    
    /* 创建起始任务 */
    Task_StartHandle = osThreadNew(AppTask_Start, NULL, &Task_Start_attributes);
}

/**
* @brief AppTask_Start  RTOS启动任务
* @param argument: Not used
* @retval None
*/
static void AppTask_Start(void *argument)
{
    /* 创建LED任务 */
    Task_LEDHandle = osThreadNew(AppTask_Led, NULL, &Task_LED_attributes);
    /* 创建DebugUsart任务 */
    Task_DebugUsartHandle = osThreadNew(AppTask_DebugUsart, NULL, &Task_DebugUsart_attributes);
    /* 创建SPP接收任务 */
    Task_SPPRXHandle = osThreadNew(AppTask_SPPReceive, NULL, &Task_SPPRX_attributes);
    /* 创建指令处理任务 */
    Task_CommandHandle = osThreadNew(AppTask_Command, NULL, &Task_Command_attributes);
    /* 创建压力传感器采集任务 */
    Task_PlantarHandle = osThreadNew(AppTask_Plantar, NULL, &Task_Plantar_attributes);
    /* 创建IMU采集任务 */
    Task_IMUHandle = osThreadNew(AppTask_IMU, NULL, &Task_IMU_attributes);
    /* 创建压力数据打包发送任务 */
    Task_PlantarDataHandle = osThreadNew(AppTask_PlantarData, NULL, &Task_PlantarData_attributes);
    /* 创建IMU数据打包发送任务 */
    Task_IMUDataHandle = osThreadNew(AppTask_IMUData, NULL, &Task_IMUData_attributes);
    /* 创建SPP连接状态监测任务 */
    Task_SPPConnectHandle = osThreadNew(AppTask_SPPConnect, NULL, &Task_SPPConnect_attributes);
    /* 创建任务运行时间统计任务 */
    Task_RunTimeStatsHandle = osThreadNew(AppTask_RunTimeStats, NULL, &Task_RunTimeStats_attributes);
    /* 删除自身任务 */
    vTaskDelete(NULL);
}

/**
* @brief AppTask_Led  用户LED闪烁任务
* @param argument: Not used
* @retval None
*/
static void AppTask_Led(void *argument)
{
    while(true)
    {
        LED_TOGGLE();
        osDelay(pdMS_TO_TICKS(1000));
        #if 0
            taskENTER_CRITICAL();       //进入基本临界区
            TASK_LOG("LED Toggle\n\r");
            taskEXIT_CRITICAL();        //退出基本临界区
        #endif
    }
}


/**
* @brief AppTask_DebugUsart  调试串口任务
* @param argument: Not used
* @retval None
*/
static void AppTask_DebugUsart(void *argument)
{
    uint8_t read;
    Uart_RxBuf_Init();
    
    while(true)
    {
        /* 接收到的串口命令处理 */
		while(comGetChar(DEBUG_COM, &read))
		{
			Uart_RxBuf_Putin(&read);
            if(read == '\n')
            {
                if(Uart_Load_Command() == RET_OK)
                {
                    /* 通知指令处理任务 */
                    if(Command_SemaphHandle != NULL)
                    {
                        if(xSemaphoreGive(Command_SemaphHandle) != pdTRUE)
                        {
                            TASK_LOG("Failed to release the semaphore \n\r");
                        }
                    }
                }
                else
                {
                    TASK_LOG("Command error \n\r");
                }
                break;
            }
		}
        osDelay(pdMS_TO_TICKS(400));
    }
}

/**
* @brief AppTask_SPPReceive  蓝牙接收任务
* @param argument: Not used
* @retval None
*/
static void AppTask_SPPReceive(void *argument)
{
    uint8_t read;
    SPP_RxBuf_Init();
    
    while(true)
    {
        if(SPP_MutexHandle != NULL)      //Plantar缓冲区修改互斥锁不为空
        {
            if(xSemaphoreTake(SPP_MutexHandle, pdMS_TO_TICKS(100)) == pdTRUE)       //申请SPP互斥锁
            {
                /* 接收到的串口命令处理 */
                while(comGetChar(SPP_COM, &read))
                {
                    SPP_RxBuf_Putin(&read);
                    if(read == '\n')
                    {
                        if(SPP_Load_Command() == RET_OK)
                        {
                            /* 通知指令处理任务 */
                            if(Command_SemaphHandle != NULL)
                            {
                                if(xSemaphoreGive(Command_SemaphHandle) != pdTRUE)
                                {
                                    TASK_LOG("Failed to release the semaphore \n\r");
                                }
                            }
                        }
                        else
                        {
                            TASK_LOG("Command error \n\r");
                        }
                        break;
                    }
                }
                xSemaphoreGive(SPP_MutexHandle);           //释放SPP互斥锁
            }
            else
            {
               TASK_LOG("Application for mutex failed \n\r");      //申请SPP互斥锁失败
            }
        }
        else
        {
            TASK_LOG("The mutex not created \n\r");      //SPP互斥锁未创建
        }
        osDelay(pdMS_TO_TICKS(400));
    }
}

/**
* @brief AppTask_Command  指令处理任务
* @param argument: Not used
* @retval None
*/
static void AppTask_Command(void *argument)
{
    int8_t result;
    
    while(true)
    {
        if(Command_SemaphHandle != NULL)
        {
            if(xSemaphoreTake(Command_SemaphHandle,portMAX_DELAY) == pdTRUE)
            {
                result = Command_Parsing(Command_Buff);
                if(result == RET_ERROR)
                {
                    comSendBuf(SPP_COM, RET_OK_BUFF, sizeof(RET_ERROR_BUFF));
                    TASK_LOG("Command parsing failure \r\n");
                }
                else if(result == RET_INVALID)
                {
                    comSendBuf(SPP_COM, RET_OK_BUFF, sizeof(RET_ERROR_BUFF));
                    TASK_LOG("Invalid command \r\n");
                }
                else
                {
                    comSendBuf(SPP_COM, RET_OK_BUFF, sizeof(RET_OK_BUFF));
                    TASK_LOG("Command executed successfully \r\n");
                }
            }
        }
    }
}

/**
* @brief AppTask_Plantar 压力传感器采集任务
* @param argument: Not used
* @retval None
*/
static void AppTask_Plantar(void *argument)
{
    osStatus_t  x_return = osOK;                   //定义一个创建信息返回值，默认为 osOK
    EventBits_t suxBits;            // 接收到的采集状态事件组位
    uint16_t temp;
    
    (void)x_return;
    
    /* 压力传感器数据缓冲区初始化 */
    Plantar_Buff_Init();
    /*  足底压力传感器采集设置初始化 */
    Plantar_SettingsInit();
    Plantar_Data_Frame_Init();
    
    while(true)
    {
        /* 判断是否开启压力传感器采集 */
        suxBits = xEventGroupWaitBits(Sampling_EventHandle,          //等待的事件组  
                                     PLANTAR_SAMPLING,              //等待的事件位
                                     pdFALSE,                       //返回前是否清除事件
                                     pdFALSE,                       //是否等待所有事件
                                     portMAX_DELAY);                //等待时间
        
        if((suxBits & PLANTAR_SAMPLING) != 0)
        {
            /* 阵列循环扫描采集 */
            if(Plantar.Sampling_Mode == ARRAY_SAMPLINGMODE)
            {
                Plantar_TimeStamp();    //存入采集时间戳
                Array_Scanning_Sampling();
                Plantar_Buff.Write_Frame++;
                if(Plantar_Buff_Full_Judge() == RET_OK)        //如果缓冲区已经写满
                {
                    TASK_LOG("Plantar write buff full \r\n");
                    if(Plantar_Read_Write_Buff_Switch() == RET_ERROR)     //交换缓冲区     
                    {
                        TASK_LOG("Plantar buff switch failed \r\n");                    
                    }
                    else
                    {
                        /* 发送消息给数据处理任务 */
                        if(xSemaphoreGive(Plantar_SemaphHandle) != pdTRUE)
                        {
                            TASK_LOG("Failed to release the semaphore \r\n");
                        }
                    }
                }
            }
            /* 单点循环扫描采集 */
            else if(Plantar.Sampling_Mode == SINGLEPOINT_SAMPLINGMODE)
            {
                Singal_Point_Sampling(Plantar.Selection_Row, Plantar.Selection_Column, &temp);
                Singel_Point_Calculation(&temp);
                TASK_LOG("voltage:%d mV \r\n",temp);
            }
            /* 指令阵列采集 */
            else if(Plantar.Sampling_Mode == INSTRUCTIONS_ARRAYMODE)
            {
                if(xSemaphoreTake(ArraySamp_SemaphHandle,portMAX_DELAY) == pdTRUE)
                {
                    Instr_Array_Sampling();
                }
            }
            /* 指令单点采集 */
            else if(Plantar.Sampling_Mode == INSTRUCTIONS_SINGLEMODE)
            {
                if(xSemaphoreTake(SingSamp_SemaphHandle,portMAX_DELAY) == pdTRUE)
                {
                    Singal_Point_Sampling(Plantar.Selection_Row, Plantar.Selection_Column, &temp);
                    Singel_Point_Calculation(&temp);
                    TASK_LOG("voltage:%d mV \r\n",temp);
                }
            }
            else
            {
                taskENTER_CRITICAL();       //进入基本临界区
                TASK_LOG("The sampling mode is incorrectly set \r\n");
                taskEXIT_CRITICAL();        //退出基本临界区
            }
            osDelay(pdMS_TO_TICKS(Plantar.Sampling_Delay));
        }
    }
}

/**
* @brief AppTask_IMU IMU采集任务
* @param argument: Not used
* @retval None
*/
static void AppTask_IMU(void *argument)
{
    
    osStatus_t  x_return = osOK;                //定义一个创建信息返回值，默认为 osOK
    EventBits_t uxBits;                         //接收到的事件组位
    float fAcc[3], fGyro[3], fAngle[3];
    int16_t sMag[3];
    int32_t dATMO, dHEIGHT;
    float fQUAT[4];
    uint8_t read;
    int i;
    
    (void)x_return;
    
    /* IMU初始化 */
    IMU_Init();
    IMU_Buff_Init();
    IMU_Data_Frame_Init();
     
    while(true)
    {
        /* 判断是否开启IMU采集 */
        uxBits = xEventGroupWaitBits(Sampling_EventHandle,          //等待的事件组  
                                     IMU_SAMPLING,                  //等待的事件位
                                     pdFALSE,                       //返回前是否清除事件
                                     pdFALSE,                       //是否等待所有事件
                                     portMAX_DELAY);                //等待时间
        if((uxBits & IMU_SAMPLING) != 0)
        {
            while(comGetChar(IMU_COM, &read))
            {
                WitSerialDataIn(read);
            }
            /* 如果有IMU数据更新 */
            if(s_cDataUpdate)
            {
                IMU_TimeStamp();     //存入采集时间戳
                /* 处理IMU数据 */            
                for(i = 0; i < 3; i++)
                {
                    fAcc[i] = sReg[AX+i] / 32768.0f * 16.0f * 9.8f;
                    fGyro[i] = sReg[GX+i] / 32768.0f * 2000.0f;
                    fAngle[i] = sReg[Roll+i] / 32768.0f * 180.0f;
                    sMag[i] = sReg[HX+i];
                }
                dATMO = (int32_t)(sReg[PressureH]<<16 | sReg[PressureL]);
                dHEIGHT = (int32_t)(sReg[HeightH]<<16 | sReg[HeightL]);
                for(i = 0; i < 4; i++)
                {
                    fQUAT[i] = sReg[q0+i] / 32768.0f;
                }
                if(s_cDataUpdate & ACC_UPDATE)          //打印加速度数据
                {
                    TASK_LOG("acc_x:%f acc_y:%f acc_z:%f \r\n", fAcc[0], fAcc[1], fAcc[2]);
                    s_cDataUpdate &= ~ACC_UPDATE;
                }
                if(s_cDataUpdate & GYRO_UPDATE)         //打印陀螺仪数据
                {
                    TASK_LOG("gyro_x:%f gyro_y:%f gyro_z:%f \r\n", fGyro[0], fGyro[1], fGyro[2]);
                    s_cDataUpdate &= ~GYRO_UPDATE;
                }
                if(s_cDataUpdate & ANGLE_UPDATE)        //打印倾角数据
                {
                    TASK_LOG("angle_x:%f angle_y:%f angle_z:%f \r\n", fAngle[0], fAngle[1], fAngle[2]);
                    s_cDataUpdate &= ~ANGLE_UPDATE;
                }
                if(s_cDataUpdate & MAG_UPDATE)          //打印磁力计数据
                {
                    TASK_LOG("mag_x:%d mag_y:%d mag_z:%d \r\n", sMag[0], sMag[1], sMag[2]);
                    s_cDataUpdate &= ~MAG_UPDATE;
                }
                if(s_cDataUpdate & ATMO_UPDATE)          //打印气压数据
                {
                    TASK_LOG("atmosphere pressure:%d \r\n", dATMO);
                    s_cDataUpdate &= ~ATMO_UPDATE;
                }
                if(s_cDataUpdate & HEIGHT_UPDATE)          //打印高度数据
                {
                    TASK_LOG("height:%d \r\n", dHEIGHT);
                    s_cDataUpdate &= ~HEIGHT_UPDATE;
                }
                if(s_cDataUpdate & QUAT_UPDATE)          //打印四元数数据
                {
                    TASK_LOG("q0:%f q1:%f q2:%f q3:%f \r\n", fQUAT[0], fQUAT[1], fQUAT[2], fQUAT[3]);
                    s_cDataUpdate &= ~QUAT_UPDATE;
                }
                s_cDataUpdate = 0;
                IMU_Put_into_Buff(fAcc, fGyro, fAngle, sMag, dATMO, dHEIGHT, fQUAT);      //将IMU数据放入缓冲区
            }
            if(IMU_Buff_Full_Judge() == RET_OK)        //如果缓冲区已经写满
            {
                TASK_LOG("IMU write buff full \r\n");
                if(IMU_Read_Write_Buff_Switch() == RET_ERROR)     //交换缓冲区     
                {
                    TASK_LOG("IMU buff switch failed \r\n");                    
                }
                else
                {
                    /* 发送消息给数据处理任务 */
                    if(xSemaphoreGive(IMU_SemaphHandle) != pdTRUE)
                    {
                        TASK_LOG("Failed to release the semaphore \r\n");
                    }
                }                    
            }
            osDelay(pdMS_TO_TICKS(6));      //采集间隔设置
        }
    }
}

/**
* @brief AppTask_PlantarData  压力传感器数据打包发送任务
* @param argument: Not used
* @retval None
*/
static void AppTask_PlantarData(void *argument)
{
    EventBits_t uxBits;                 //接收到的事件组位
    uint8_t i;
    
    while(true)
    {
        if(xSemaphoreTake(Plantar_SemaphHandle,portMAX_DELAY) == pdTRUE)
        {
            uxBits = xEventGroupWaitBits(BluetoothEventHandle,          //等待的事件组  
                             BLUETOOTH_START_TRANSFER,                  //等待的事件位
                             pdFALSE,                                   //返回前是否清除事件
                             pdFALSE,                                   //是否等待所有事件
                             pdMS_TO_TICKS(2));                         //等待时间
        
            if((uxBits & BLUETOOTH_TRANSFER) != 0)
            {
                if(Pressure_Calculation(Pressure_Buff) == RET_OK)       //如果压力值解算成功
                {
                    for(i=0; i<FRAME_IN_BUFF; i++)
                    {
                        Plantar_Data_Frame_Handling(i);
                        Plantar_Data_Frame_Transmit();
                    }                        
                }
            }
        }
    }
}


/**
* @brief AppTask_IMUData  IMU数据打包发送任务
* @param argument: Not used
* @retval None
*/
static void AppTask_IMUData(void *argument)
{
    EventBits_t uxBits;                 //接收到的事件组位
    uint8_t i;
    
    while(true)
    {
        if(xSemaphoreTake(IMU_SemaphHandle,portMAX_DELAY) == pdTRUE)
        {
            uxBits = xEventGroupWaitBits(BluetoothEventHandle,          //等待的事件组  
                             BLUETOOTH_START_TRANSFER,                  //等待的事件位
                             pdFALSE,                                   //返回前是否清除事件
                             pdFALSE,                                   //是否等待所有事件
                             pdMS_TO_TICKS(2));                         //等待时间
        
            if((uxBits & BLUETOOTH_TRANSFER) != 0)
            {
                for(i=0; i<FRAME_IN_BUFF; i++)
                {
                    IMU_Data_Frame_Handling(i);
                    IMU_Data_Frame_Transmit();
                }
            }
        }
    }
}

/**
* @brief AppTask_SPPConnect  判断蓝牙链接任务
* @param argument: Not used
* @retval None
*/
static void AppTask_SPPConnect(void *argument)
{
    EventBits_t uxBits;                 //接收到的事件组位
    BaseType_t xResult;                     //事件组发送结果
    
    osDelay(pdMS_TO_TICKS(500));
    if(HC04_Init() == RET_OK)
    {
        printf("SPP init succeeded!! \r\n");
    }
    else
    {
        printf("SPP init failed!! \r\n");
    }
                
    while(true)
    {
        uxBits = xEventGroupGetBits(BluetoothEventHandle);
        if((uxBits & 0x01) == SPPSTATE_INPUTPINSET)
        {
            osDelay(pdMS_TO_TICKS(20));
            if(SPPSTATE_INPUTPINSET)
            {
                TASK_LOG("SPP disconnect!! \r\n");
                /* 清除蓝牙链接事件位 */
                xResult = xEventGroupClearBits(BluetoothEventHandle, 
                                               BLUETOOTH_CONNECT);
                if(  xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
                /* 清除蓝牙传输事件位 */
                xResult = xEventGroupClearBits(BluetoothEventHandle,
                                               BLUETOOTH_TRANSFER);                     
                if( xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
                /* 清除压力传感器采集事件位 */
                xResult = xEventGroupClearBits(Sampling_EventHandle,
                                               PLANTAR_SAMPLING);                     
                if( xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
                /* 清除IMU采集事件位 */
                xResult = xEventGroupClearBits(Sampling_EventHandle,
                                               IMU_SAMPLING);                     
                if( xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
            }
            else
            {
                TASK_LOG("SPP connect!! \r\n");
                /* 置位压力传感器采集事件位 */
                xResult = xEventGroupSetBits(Sampling_EventHandle,
                                             PLANTAR_SAMPLING);                     
                if( xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
                /* 置位IMU采集事件位 */
                xResult = xEventGroupSetBits(Sampling_EventHandle,
                                             IMU_SAMPLING);                     
                if( xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
                /* 置位蓝牙链接事件位 */
                xResult = xEventGroupSetBits(BluetoothEventHandle,
                                             BLUETOOTH_CONNECT);
                if(  xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
                /* 置位蓝牙传输事件位 */
                xResult = xEventGroupSetBits(BluetoothEventHandle,
                                             BLUETOOTH_TRANSFER);                     
                if( xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
            }
        }
        osDelay(pdMS_TO_TICKS(1000));
    }
}

/**
* @brief AppTask_RunTimeStats  统计任务运行时间任务
* @param argument: Not used
* @retval None
*/
static void AppTask_RunTimeStats(void *argument)
{
	while(true)
	{
		if(RunTime_SemaphHandle != NULL)
        {
            if(xSemaphoreTake(RunTime_SemaphHandle,portMAX_DELAY) == pdTRUE)
            {
                memset(RunTimeInfo,'\0',400);				//信息缓冲区清零
                vTaskGetRunTimeStats(RunTimeInfo);		//获取任务运行时间信息
                printf("Task\t\tRunTime\t\tPercentage\r\n");
                printf("%s\r\n",RunTimeInfo);
            }
        }
	}
}

