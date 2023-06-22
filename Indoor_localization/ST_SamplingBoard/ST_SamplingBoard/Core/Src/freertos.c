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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_Start(void *argument);
void AppTask_Led(void *argument);
void AppTask_DebugUsart(void *argument);

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

    /* creation of Task_LED */
    Task_LEDHandle = osThreadNew(AppTask_Led, NULL, &Task_LED_attributes);

    /* creation of Task_DebugUsart */
    Task_DebugUsartHandle = osThreadNew(AppTask_DebugUsart, NULL, &Task_DebugUsart_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

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
    /* USER CODE BEGIN AppTask_Start */
    /* Infinite loop */
    for(;;)
    {
    osDelay(1);
    }
    /* USER CODE END AppTask_Start */
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
    for(;;)
    {
        LED_Toggle();
        temp = ADC_OneShot_Read() * (3.3/4096);
        printf("Voltage:%f",temp);
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
    for(;;)
    {
        osDelay(1);
    }
    /* USER CODE END AppTask_DebugUsart */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

