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
/* �������ɼ��¼��� */
osEventFlagsId_t Sampling_EventHandle;
const osEventFlagsAttr_t Sampling_Event_attributes = 
{
    .name = "SamplingEvent"
};
/* ѹ���������ɼ�ģʽ�¼��� */
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

    /* �¼��鴴�� */
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
    /* ����LED���� */
    Task_LEDHandle = osThreadNew(AppTask_Led, NULL, &Task_LED_attributes);
    /* ����DebugUsart���� */
    Task_DebugUsartHandle = osThreadNew(AppTask_DebugUsart, NULL, &Task_DebugUsart_attributes);
    /* ����ѹ���������ɼ����� */
    Task_DebugUsartHandle = osThreadNew(AppTask_Plantar, NULL, &Task_Plantar_attributes);
    /* ����IMU�ɼ����� */
    Task_DebugUsartHandle = osThreadNew(AppTask_IMU, NULL, &Task_IMU_attributes);
    /* ɾ���������� */
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
    BaseType_t x_return = pdPASS;                   // ����һ��������Ϣ����ֵ��Ĭ��Ϊ pdPASS
    uint32_t r_queue;                               // ����һ��������Ϣ�ı��� 
    uint32_t s_queue = PRESSURE_SOLUTION;           // ����һ��������Ϣ�ı���
    EventBits_t uxBits;                             //���յ����¼���λ
    Plantar_Sampling_TypeDef Plantar;               //ѹ���������ɼ��ṹ��
    uint16_t temp;
    /* ѹ�����������ݻ�������ʼ�� */
    Plantar_Buff_Init();
    
    while(1)
    {
        /* �ж��Ƿ���ѹ���������ɼ� */
        uxBits = xEventGroupWaitBits(Sampling_EventHandle,          //�ȴ����¼���  
                                     PLANTAR_SAMPLING,              //�ȴ����¼�λ
                                     pdFALSE,                       //����ǰ�Ƿ�����¼�
                                     pdFALSE,                       //�Ƿ�ȴ������¼�
                                     portMAX_DELAY);                //�ȴ�ʱ��
        
        if((uxBits & PLANTAR_SAMPLING) != 0)
        {
            /* �ж�ѹ���������ɼ�ģʽ */
            uxBits = xEventGroupWaitBits(PlantarMode_EventHandle,       //�ȴ����¼���  
                                         PLANTAR_SAMPLING_MODE,         //�ȴ����¼�λ
                                         pdFALSE,                       //����ǰ�Ƿ�����¼�
                                         pdFALSE,                       //�Ƿ�ȴ������¼�
                                         0);                            //�ȴ�ʱ��
            /* ����ѭ��ɨ��ɼ� */
            if((uxBits & ARRAY_SAMPLINGMODE) != 0)
            {
                Array_Scanning_Sampling();
                if(Plantar_Buff_Full_Judge() == 1)        //����������Ѿ�д��
                {
                    TASK_LOG("Plantar write buff full \n\r");
                    if(Plantar_Read_Write_Buff_Switch())     //����������     
                    {
                        TASK_LOG("Plantar buff switch succeed \n\r");                    
                    }
                    /* ������Ϣ�����ݴ������� */
                }
            }
            /* ����ѭ��ɨ��ɼ� */
            else if((uxBits & SINGLEPOINT_SAMPLINGMODE) != 0)
            {
                Singal_Point_Sampling(Plantar.Selection_Row, Plantar.Selection_Column, &temp);
                TASK_LOG("voltage:%d mV \n\r",temp);
            }
            /* ָ�����вɼ� */
            else if((uxBits & INSTRUCTIONS_ARRAYMODE) != 0)
            {
            }
            /* ָ���ɼ� */
            else if((uxBits & INSTRUCTIONS_SINGLEMODE) != 0)
            {
            }
            else
            {
                taskENTER_CRITICAL();        //��������ٽ���
                TASK_LOG("The sampling mode is incorrectly set\n\r");
                taskEXIT_CRITICAL();        //�˳������ٽ���
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

