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
uint8_t Receive_Buff[RX_BUFF_SIZE];     //Debug串口接收缓冲区
uint8_t Receive_Buff1[RX_BUFF_SIZE];    //SPP接收缓冲区
uint8_t Command_Buff[13];               //接收指令缓冲区
PLANTAR_S Plantar;                      // 压力传感器采集结构体

extern float Pressure_Buff[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];     //压力传感器拟合压力值缓存区
extern Plantar_Buff_TypeDef Plantar_Buff;              //压力传感器阵列Buff结构体声明
extern UART_RXBUF Uart_Receive;         /* 串口指令接收缓冲区结构体声明 */
extern volatile uint16_t s_cDataUpdate;
extern volatile char s_cCmd;

/* 定义初始任务 */
osThreadId_t Task_StartHandle;
const osThreadAttr_t Task_Start_attributes = 
{
    .name = "Task_Start",
    .stack_size = 128 * 2,
    .priority = (osPriority_t) osPriorityNormal,
};
/* 定义LED任务 */
osThreadId_t Task_LEDHandle;
const osThreadAttr_t Task_LED_attributes = 
{
    .name = "Task_LED",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityLow,
};
/* 定义Debug串口任务 */
osThreadId_t Task_DebugUsartHandle;
const osThreadAttr_t Task_DebugUsart_attributes = 
{
    .name = "Task_DebugUsart",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};
/* 定义SPP接收任务 */
osThreadId_t Task_SPPRXHandle;
const osThreadAttr_t Task_SPPRX_attributes = 
{
    .name = "Task_SPPReceive",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal2,
};
/* 定义指令处理任务 */
osThreadId_t Task_CommandHandle;
const osThreadAttr_t Task_Command_attributes = 
{
    .name = "Task_Command",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal1,
};
/* 定义压力传感器采集任务 */
osThreadId_t Task_PlantarHandle;
const osThreadAttr_t Task_Plantar_attributes = 
{
    .name = "Task_Plantar",
    .stack_size = 128 * 8,
    .priority = (osPriority_t) osPriorityRealtime5,
};
/* 定义IMU采集任务 */
osThreadId_t Task_IMUHandle;
const osThreadAttr_t Task_IMU_attributes = 
{
    .name = "Task_IMU",
    .stack_size = 128 * 8,
    .priority = (osPriority_t) osPriorityRealtime7,
};
/* 定义数据处理打包任务 */
osThreadId_t Task_DataHandle;
const osThreadAttr_t Task_Data_attributes = 
{
    .name = "Task_Data",
    .stack_size = 128 * 8,
    .priority = (osPriority_t) osPriorityHigh6,
};
/* 定义数据发送任务 */
osThreadId_t Task_TransferHandle;
const osThreadAttr_t Task_Transfer_attributes = 
{
    .name = "Task_Transfer",
    .stack_size = 128 * 8,
    .priority = (osPriority_t) osPriorityHigh7,
};
/* 判断蓝牙连接任务 */
osThreadId_t Task_SPPConnectHandle;
const osThreadAttr_t Task_SPPConnect_attributes = 
{
    .name = "Task_SPPConnect",
    .stack_size = 128 * 2,
    .priority = (osPriority_t) osPriorityNormal1,
};



/* 全局变量修改互斥锁 */
osMutexId_t GV_MutexHandle;
const osMutexAttr_t GV_Mutex_attributes = 
{
    .name = "Global Variable Mutex"
};
/* 指令处理信号量 */
osSemaphoreId_t Command_SemaphHandle;
const osSemaphoreAttr_t Command_Semaph_attributes = 
{
    .name = "Command Semaphore"
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
/* 数据处理消息队列 */
osMessageQueueId_t Data_QueueHandle;
const osMessageQueueAttr_t Data_Queue_attributes = 
{
    .name = "Data Queue"
};
/* 数据传输消息队列 */
osMessageQueueId_t Transfer_QueueHandle;
const osMessageQueueAttr_t Transfer_Queue_attributes = 
{
    .name = "Transfer Queue"
};
/* 数据传输完成消息队列 */
osMessageQueueId_t TranDone_QueueHandle;
const osMessageQueueAttr_t TranDone_Queue_attributes = 
{
    .name = "Transfer Done Queue"
};


/* Private function prototypes -----------------------------------------------*/
void AppTask_Start(void *argument);
void AppTask_Led(void *argument);
void AppTask_DebugUsart(void *argument);
void AppTask_SPPReceive(void *argument);
void AppTask_Command(void *argument);
void AppTask_Plantar(void *argument);
void AppTask_IMU(void *argument);
void AppTask_Data(void *argument);
void AppTask_Transfer(void *argument);
void AppTask_SPPConnect(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) 
{
    /* 互斥锁创建 */
    GV_MutexHandle = osMutexNew(&GV_Mutex_attributes);
    /* 信号量创建 */
    Command_SemaphHandle = osSemaphoreNew(1,0,&Command_Semaph_attributes);
    /* 事件组创建 */
    BluetoothEventHandle = osEventFlagsNew(&Bluetooth_Event_attributes);
    Sampling_EventHandle = osEventFlagsNew(&Sampling_Event_attributes);    
    /* 消息队列创建 */
    Data_QueueHandle = osMessageQueueNew (10, sizeof(uint8_t), &Data_Queue_attributes);
    Transfer_QueueHandle = osMessageQueueNew (10, sizeof(uint8_t), &Transfer_Queue_attributes);
    TranDone_QueueHandle = osMessageQueueNew (10, sizeof(uint8_t), &TranDone_Queue_attributes);
    /* 创建起始任务 */
    Task_StartHandle = osThreadNew(AppTask_Start, NULL, &Task_Start_attributes);
}

/**
* @brief AppTask_Start  RTOS启动任务
* @param argument: Not used
* @retval None
*/
void AppTask_Start(void *argument)
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
    /* 创建数据处理打包任务 */
    Task_DataHandle = osThreadNew(AppTask_Data, NULL, &Task_Data_attributes);
    /* 创建数据传输任务 */
    Task_TransferHandle = osThreadNew(AppTask_Transfer, NULL, &Task_Transfer_attributes);
    /* 创建SPP连接状态监测任务 */
    Task_SPPConnectHandle = osThreadNew(AppTask_SPPConnect, NULL, &Task_SPPConnect_attributes);
    /* 删除自身任务 */
    vTaskDelete(NULL);
}

/**
* @brief AppTask_Led  用户LED闪烁任务
* @param argument: Not used
* @retval None
*/
void AppTask_Led(void *argument)
{
    while(1)
    {
        LED_TOGGLE();
        osDelay(1000);
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
void AppTask_DebugUsart(void *argument)
{
    uint8_t read;
    Uart_RxBuf_Init();
    while(1)
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
        osDelay(400);
    }
}

/**
* @brief AppTask_SPPReceive  蓝牙接收任务
* @param argument: Not used
* @retval None
*/
void AppTask_SPPReceive(void *argument)
{
    uint8_t read;
    SPP_RxBuf_Init();
    
    while(1)
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
        osDelay(400);
    }
}

/**
* @brief AppTask_Command  指令处理任务
* @param argument: Not used
* @retval None
*/
void AppTask_Command(void *argument)
{
    int8_t result;
    
    while(1)
    {
        if(Command_SemaphHandle != NULL)
        {
            if(xSemaphoreTake(Command_SemaphHandle,portMAX_DELAY) == pdTRUE)
            {
                result = Command_Parsing(Command_Buff);
                if(result == RET_ERROR)
                {
                    TASK_LOG("Command parsing failure \n\r");
                }
                else if(result == RET_INVALID)
                {
                    TASK_LOG("Invalid command \n\r");
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
void AppTask_Plantar(void *argument)
{
    osStatus_t  x_return = osOK;                   //定义一个创建信息返回值，默认为 osOK
    uint8_t s_queue = PRESSURE_SOLUTION;           //定义一个发送消息的变量
    EventBits_t suxBits;            // 接收到的采集状态事件组位
    uint16_t temp;
    
    (void)x_return;
    
    /* 压力传感器数据缓冲区初始化 */
    Plantar_Buff_Init();
    /*  足底压力传感器采集设置初始化 */
    Plantar_SettingsInit();
    
    while(1)
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
                //printf("Plantar:%d \n\r",Get_TimeStamp());
                Plantar_TimeStamp();    //存入采集时间戳
                Array_Scanning_Sampling();
                Plantar_Buff.Write_Frame++;
                if(Plantar_Buff_Full_Judge() == 1)        //如果缓冲区已经写满
                {
                    TASK_LOG("Plantar write buff full \n\r");
                    if(Plantar_Read_Write_Buff_Switch() == RET_ERROR)     //交换缓冲区     
                    {
                        TASK_LOG("Plantar buff switch failed \n\r");                    
                    }
                    else
                    {
                        /* 发送消息给数据处理任务 */
                        x_return = osMessageQueuePut( Data_QueueHandle,     //消息队列的句柄 
                                                    &s_queue,               //发送的消息内容 
                                                    NULL,                   //消息优先级
                                                    0 );                    //等待时间 0
                    }
                }
                //TASK_LOG("End Time:%d \n\r",Get_TimeStamp());
            }
            /* 单点循环扫描采集 */
            else if(Plantar.Sampling_Mode == SINGLEPOINT_SAMPLINGMODE)
            {
                Singal_Point_Sampling(Plantar.Selection_Row, Plantar.Selection_Column, &temp);
                Singel_Point_Calculation(&temp);
                //TASK_LOG("voltage:%d mV \n\r",temp);
                printf("voltage:%d mV \r\n",temp);
            }
            /* 指令阵列采集 */
            else if(Plantar.Sampling_Mode == INSTRUCTIONS_ARRAYMODE)
            {
            }
            /* 指令单点采集 */
            else if(Plantar.Sampling_Mode == INSTRUCTIONS_SINGLEMODE)
            {
            }
            else
            {
                taskENTER_CRITICAL();       //进入基本临界区
                TASK_LOG("The sampling mode is incorrectly set\n\r");
                taskEXIT_CRITICAL();        //退出基本临界区
            }
            osDelay(Plantar.Sampling_Delay);
        }
    }
}

/**
* @brief AppTask_IMU IMU采集任务
* @param argument: Not used
* @retval None
*/
void AppTask_IMU(void *argument)
{
    
    osStatus_t  x_return = osOK;                //定义一个创建信息返回值，默认为 osOK
    uint8_t s_queue = IMU_SOLUTION;             //定义一个发送消息的变量
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
     
    while(1)
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
                //printf("IMU:%d \n\r",Get_TimeStamp());
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
                    TASK_LOG("acc_x:%f acc_y:%f acc_z:%f \n\r", fAcc[0], fAcc[1], fAcc[2]);
                    s_cDataUpdate &= ~ACC_UPDATE;
                }
                if(s_cDataUpdate & GYRO_UPDATE)         //打印陀螺仪数据
                {
                    TASK_LOG("gyro_x:%f gyro_y:%f gyro_z:%f \n\r", fGyro[0], fGyro[1], fGyro[2]);
                    s_cDataUpdate &= ~GYRO_UPDATE;
                }
                if(s_cDataUpdate & ANGLE_UPDATE)        //打印倾角数据
                {
                    TASK_LOG("angle_x:%f angle_y:%f angle_z:%f \n\r", fAngle[0], fAngle[1], fAngle[2]);
                    s_cDataUpdate &= ~ANGLE_UPDATE;
                }
                if(s_cDataUpdate & MAG_UPDATE)          //打印磁力计数据
                {
                    TASK_LOG("mag_x:%d mag_y:%d mag_z:%d \n\r", sMag[0], sMag[1], sMag[2]);
                    s_cDataUpdate &= ~MAG_UPDATE;
                }
                if(s_cDataUpdate & ATMO_UPDATE)          //打印气压数据
                {
                    TASK_LOG("atmosphere pressure:%d \n\r", dATMO);
                    s_cDataUpdate &= ~ATMO_UPDATE;
                }
                if(s_cDataUpdate & HEIGHT_UPDATE)          //打印高度数据
                {
                    TASK_LOG("height:%d \n\r", dHEIGHT);
                    s_cDataUpdate &= ~HEIGHT_UPDATE;
                }
                if(s_cDataUpdate & QUAT_UPDATE)          //打印四元数数据
                {
                    TASK_LOG("q0:%f q1:%f q2:%f q3:%f \n\r", fQUAT[0], fQUAT[1], fQUAT[2], fQUAT[3]);
                    s_cDataUpdate &= ~QUAT_UPDATE;
                }
                s_cDataUpdate = 0;
            }
            IMU_Put_into_Buff(fAcc, fGyro, fAngle, sMag, dATMO, dHEIGHT, fQUAT);      //将IMU数据放入缓冲区
            if(IMU_Buff_Full_Judge() == 1)        //如果缓冲区已经写满
            {
                TASK_LOG("IMU write buff full \n\r");
                if(IMU_Read_Write_Buff_Switch() == RET_ERROR)     //交换缓冲区     
                {
                    TASK_LOG("IMU buff switch failed \n\r");                    
                }
                else
                {
                    /* 发送消息给数据处理任务 */
                    x_return = osMessageQueuePut( Data_QueueHandle,     //消息队列的句柄 
                                                &s_queue,               //发送的消息内容 
                                                NULL,                   //消息优先级
                                                0 );                    //等待时间 0   
                }                    
            }
            osDelay(5);      //采集间隔设置
        }
    }
}

/**
* @brief AppTask_Data  数据处理打包任务
* @param argument: Not used
* @retval None
*/
void AppTask_Data(void *argument)
{
    EventBits_t uxBits;                 //接收到的事件组位
    osStatus_t x_return = osOK;         //定义一个创建信息返回值，默认为 osOK 
    uint8_t r_queue;                    //定义一个接收消息的变量 
    uint8_t s_queue;                    //定义一个发送消息的变量
    uint8_t i;
    
    while(1)
    {
        uxBits = xEventGroupWaitBits(BluetoothEventHandle,          //等待的事件组  
                                     BLUETOOTH_START_TRANSFER,      //等待的事件位
                                     pdFALSE,                       //返回前是否清除事件
                                     pdFALSE,                       //是否等待所有事件
                                     portMAX_DELAY);                //等待时间
        
        if((uxBits & BLUETOOTH_TRANSFER) != 0)
        {
            x_return = osMessageQueueGet( Data_QueueHandle,         /* 消息队列的句柄 */ 
                                      &r_queue,                 /* 发送的消息内容 */ 
                                      0,
                                      portMAX_DELAY);           /* 等待时间:一直等 */
            if (x_return == osOK) 
            {
                if(r_queue == PRESSURE_SOLUTION)        //如果是压力值解算消息
                {
                    if(Pressure_Calculation(Pressure_Buff) == RET_OK)       //如果压力值解算成功
                    {
                        for(i=0; i<FRAME_IN_BUFF; i++)
                        {
                            Plantar_Data_Frame_Handling(i);
                            /* 告知发送任务将压力传感器数据发送 */             
                            s_queue = PRESSURE_TRANSFER;
                            x_return = osMessageQueuePut( Transfer_QueueHandle,     //消息队列的句柄 
                                                        &s_queue,                   //发送的消息内容 
                                                        NULL,                       //消息优先级
                                                        0 );                        //等待时间 0
                            /* 等待压力传感器数据发送完成 */
                            x_return = osMessageQueueGet( TranDone_QueueHandle,     //消息队列的句柄 
                                                        &r_queue,                   //发送的消息内容 
                                                        NULL,                       //消息优先级
                                                        5);                         //等待时间:5ms                                              
                            if (x_return != osOK && r_queue != PRESSURE_DONE)
                            {
                                TASK_LOG("no.%d plantar data transfer error \n\r",i);
                            }       
                        }                        
                    }                
                }
                else if(r_queue == IMU_SOLUTION)       //如果是IMU数据消息
                {
                    for(i=0; i<FRAME_IN_BUFF; i++)
                    {
                        IMU_Data_Frame_Handling(i);
                        /* 告知发送任务将IMU数据发送 */
                        s_queue = IMU_TRANSFER;
                        x_return = osMessageQueuePut( Transfer_QueueHandle,     //消息队列的句柄 
                                                    &s_queue,                   //发送的消息内容 
                                                    NULL,                       //消息优先级
                                                    0 );                        //等待时间 0
                        /* 等待IMU数据发送完成 */
                        x_return = osMessageQueueGet( TranDone_QueueHandle,     //消息队列的句柄 
                                                     &r_queue,                  //发送的消息内容 
                                                     NULL,                      //消息优先级
                                                     5);                        //等待时间:5ms
                        if (x_return != osOK && r_queue != IMU_DONE)
                        {
                            TASK_LOG("no.%d IMU data transfer error \n\r",i);
                        }
                    }
                }
            }
            else
            {
                TASK_LOG("The received message value error \n\r");
            } 
        }
        osDelay(10);      //采集间隔设置
    }
}


/**
* @brief AppTask_Transfer  数据发送任务
* @param argument: Not used
* @retval None
*/
void AppTask_Transfer(void *argument)
{
    osStatus_t x_return = osOK;         //定义一个创建信息返回值，默认为 osOK 
    uint8_t r_queue;                    //定义一个接收消息的变量
    uint8_t s_queue;                    //定义一个发送消息的变量
     
    while(1)
    {
        x_return = osMessageQueueGet( Transfer_QueueHandle,     //消息队列的句柄
                                    &r_queue,                   //发送的消息内容
                                    NULL,                       //消息优先级        
                                    portMAX_DELAY);             //等待时间:一直等
        if (x_return == osOK)
        {
            if(r_queue == PRESSURE_TRANSFER)        //如果是压力传感器数据传输
            {
                Plantar_Data_Frame_Transmit();
                TASK_LOG("Plantar transfer \n\r");
                /* 告知处理任务压力传感器数据已经传输完成 */
                s_queue = PRESSURE_DONE;
                x_return = osMessageQueuePut(TranDone_QueueHandle,      //消息队列的句柄 
                                            &s_queue,                   //发送的消息内容
                                            NULL,                       //消息优先级                
                                            0 );                        //等待时间 0 
            }
            else if(r_queue == IMU_TRANSFER)        //如果是IMU数据传输
            {
                IMU_Data_Frame_Transmit();
                TASK_LOG("IMU transfer \n\r");
                /* 告知处理任务IMU数据已经传输完成 */
                s_queue = IMU_DONE;
                x_return = osMessageQueuePut(TranDone_QueueHandle,      //消息队列的句柄 
                                            &s_queue,                   //发送的消息内容
                                            NULL,                       //消息优先级                
                                            0 );                        //等待时间 0
            }
        }
        else
        {
            TASK_LOG("The received message value error \n\r");
        }
    }
}

/**
* @brief AppTask_SPPConnect  判断蓝牙链接任务
* @param argument: Not used
* @retval None
*/
void AppTask_SPPConnect(void *argument)
{
    EventBits_t uxBits;                 //接收到的事件组位
    BaseType_t xResult;                     //事件组发送结果
    
    while(1)
    {
        uxBits = xEventGroupGetBits(BluetoothEventHandle);
        if((uxBits & 0x01) == SPPSTATE_INPUTPINSET)
        {
            osDelay(20);
            if(SPPSTATE_INPUTPINSET)
            {
                TASK_LOG("SPP disconnect!! \r\n");
                xResult = xEventGroupClearBits(BluetoothEventHandle, 
                                               BLUETOOTH_CONNECT);
                if(  xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
            }
            else
            {
                TASK_LOG("SPP connect!! \r\n");
                xResult = xEventGroupSetBits(BluetoothEventHandle,
                                             BLUETOOTH_CONNECT);
                if(  xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
                /* 置位蓝牙链接事件位 */
                xResult = xEventGroupSetBits(BluetoothEventHandle,
                                             BLUETOOTH_TRANSFER);                     
                if( xResult == pdFAIL )
                {
                    TASK_LOG("Failed to set the event group \r\n");
                }
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
            }
        }
        osDelay(600);
    }
}


