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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_Start */
osThreadId_t Task_StartHandle;
const osThreadAttr_t Task_Start_attributes = 
{
    .name = "Task_Start",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_LED */
osThreadId_t Task_LEDHandle;
const osThreadAttr_t Task_LED_attributes = 
{
    .name = "Task_LED",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_DebugUsart */
osThreadId_t Task_DebugUsartHandle;
const osThreadAttr_t Task_DebugUsart_attributes = 
{
    .name = "Task_DebugUsart",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_Plantar */
osThreadId_t Task_Plantar;
const osThreadAttr_t Task_Plantar_attributes = 
{
    .name = "Task_Plantar",
    .stack_size = 128 * 8,
    .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for Task_IMU */
osThreadId_t Task_IMU;
const osThreadAttr_t Task_IMU_attributes = 
{
    .name = "Task_IMU",
    .stack_size = 128 * 8,
    .priority = (osPriority_t) osPriorityRealtime1,
};


/* Definitions for myMutex01 */
osMutexId_t myMutex01Handle;
const osMutexAttr_t myMutex01_attributes = 
{
    .name = "myMutex01"
};
/* 传感器采集事件组 */
osEventFlagsId_t Sampling_EventHandle;
const osEventFlagsAttr_t Sampling_Event_attributes = 
{
    .name = "SamplingEvent"
};
/* 压力传感器采集模式事件组 */
osEventFlagsId_t PlantarMode_EventHandle;
const osEventFlagsAttr_t PlantarMode_Event_attributes = 
{
    .name = "PlantarModeEvent"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_Start(void *argument);
void AppTask_Led(void *argument);
void AppTask_DebugUsart(void *argument);
void AppTask_Plantar(void *argument);
void AppTask_IMU(void *argument);


void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) 
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */
    /* Create the mutex(es) */
    /* creation of myMutex01 */
    myMutex01Handle = osMutexNew(&myMutex01_attributes);

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of Task_Start */
    Task_StartHandle = osThreadNew(AppTask_Start, NULL, &Task_Start_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* 事件组创建 */
    Sampling_EventHandle = osEventFlagsNew(&Sampling_Event_attributes);
    PlantarMode_EventHandle = osEventFlagsNew(&PlantarMode_Event_attributes);

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_Start */
/**
  * @brief  Function implementing the Task_Start thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Start */
void AppTask_Start(void *argument)
{
    /* 创建LED任务 */
    Task_LEDHandle = osThreadNew(AppTask_Led, NULL, &Task_LED_attributes);
    /* 创建DebugUsart任务 */
    Task_DebugUsartHandle = osThreadNew(AppTask_DebugUsart, NULL, &Task_DebugUsart_attributes);
    /* 创建压力传感器采集任务 */
    Task_DebugUsartHandle = osThreadNew(AppTask_Plantar, NULL, &Task_Plantar_attributes);
    /* 创建IMU采集任务 */
    Task_DebugUsartHandle = osThreadNew(AppTask_IMU, NULL, &Task_IMU_attributes);
    /* 删除自身任务 */
    vTaskDelete(NULL);
}

/* USER CODE BEGIN Header_AppTask_Led */
/**
* @brief Function implementing the Task_LED thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_Led */
void AppTask_Led(void *argument)
{
    /* USER CODE BEGIN AppTask_Led */
    float temp;
    /* Infinite loop */
    while(1)
    {
        LED_Toggle();
        osDelay(1000);
    }
    /* USER CODE END AppTask_Led */
}

/* USER CODE BEGIN Header_AppTask_DebugUsart */
/**
* @brief Function implementing the Task_DebugUsart thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_DebugUsart */
void AppTask_DebugUsart(void *argument)
{
    /* USER CODE BEGIN AppTask_DebugUsart */
    /* Infinite loop */
    while(1)
    {
        osDelay(1);
    }
    /* USER CODE END AppTask_DebugUsart */
}

void AppTask_Plantar(void *argument)
{
    BaseType_t x_return = pdPASS;                   // 定义一个创建信息返回值，默认为 pdPASS
    uint32_t r_queue;                               // 定义一个接收消息的变量 
    uint32_t s_queue = PRESSURE_SOLUTION;           // 定义一个发送消息的变量
    EventBits_t uxBits;                             //接收到的事件组位
    Plantar_Sampling_TypeDef Plantar;               //压力传感器采集结构体
    uint16_t temp;
    /* 压力传感器数据缓冲区初始化 */
    Plantar_Buff_Init();
    
    while(1)
    {
        /* 判断是否开启压力传感器采集 */
        uxBits = xEventGroupWaitBits(Sampling_EventHandle,          //等待的事件组  
                                     PLANTAR_SAMPLING,              //等待的事件位
                                     pdFALSE,                       //返回前是否清除事件
                                     pdFALSE,                       //是否等待所有事件
                                     portMAX_DELAY);                //等待时间
        
        if((uxBits & PLANTAR_SAMPLING) != 0)
        {
            /* 判断压力传感器采集模式 */
            uxBits = xEventGroupWaitBits(PlantarMode_EventHandle,       //等待的事件组  
                                         PLANTAR_SAMPLING_MODE,         //等待的事件位
                                         pdFALSE,                       //返回前是否清除事件
                                         pdFALSE,                       //是否等待所有事件
                                         0);                            //等待时间
            /* 阵列循环扫描采集 */
            if((uxBits & ARRAY_SAMPLINGMODE) != 0)
            {
                Array_Scanning_Sampling();
                if(Plantar_Buff_Full_Judge() == 1)        //如果缓冲区已经写满
                {
                    TASK_LOG("Plantar write buff full \n\r");
                    if(Plantar_Read_Write_Buff_Switch())     //交换缓冲区     
                    {
                        TASK_LOG("Plantar buff switch succeed \n\r");                    
                    }
                    /* 发送消息给数据处理任务 */
                }
            }
            /* 单点循环扫描采集 */
            else if((uxBits & SINGLEPOINT_SAMPLINGMODE) != 0)
            {
                Singal_Point_Sampling(Plantar.Selection_Row, Plantar.Selection_Column, &temp);
                TASK_LOG("voltage:%d mV \n\r",temp);
            }
            /* 指令阵列采集 */
            else if((uxBits & INSTRUCTIONS_ARRAYMODE) != 0)
            {
            }
            /* 指令单点采集 */
            else if((uxBits & INSTRUCTIONS_SINGLEMODE) != 0)
            {
            }
            else
            {
                taskENTER_CRITICAL();        //进入基本临界区
                TASK_LOG("The sampling mode is incorrectly set\n\r");
                taskEXIT_CRITICAL();        //退出基本临界区
            }
            osDelay(20);
        }
    }
}

void AppTask_IMU(void *argument)
{
    while(1)
    {
        osDelay(1);
    }
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

