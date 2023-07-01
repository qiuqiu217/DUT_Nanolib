/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    //LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    /* 初始化用户LED GPIO */
    GPIO_InitStruct.Pin = USERLED_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(USERLED_GPIO, &GPIO_InitStruct);
    LL_GPIO_SetOutputPin(USERLED_GPIO, USERLED_PIN);

    /* 初始化用户按键 GPIO */
    GPIO_InitStruct.Pin = USERKEY_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(USERKEY_GPIO, &GPIO_InitStruct);
    
    /* 初始化BOOT1 GPIO */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* 初始化SPP State GPIO */
    GPIO_InitStruct.Pin = SPPSTATE_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(SPPSTATE_GPIO, &GPIO_InitStruct);
    /* -2- Connect External Line to the GPIO*/
	SPPSTATE_SYSCFG_SET_EXTI();
	/*-3- Enable a falling trigger EXTI line 13 Interrupt */
	SPPSTATE_EXTI_LINE_ENABLE();
	SPPSTATE_EXTI_FALLING_TRIG_ENABLE();
    /*-4- Configure NVIC for EXTI15_10_IRQn */
	NVIC_EnableIRQ(SPPSTATE_EXTI_IRQn); 
	NVIC_SetPriority(SPPSTATE_EXTI_IRQn,0);
    
    /* 初始化SPP Key GPIO */
    GPIO_InitStruct.Pin = SPPKEY_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SPPKEY_GPIO, &GPIO_InitStruct);

    /* 初始化行列选通器控制 GPIO */
    GPIO_InitStruct.Pin = MUX_PINB;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(MUX_GPIOB, &GPIO_InitStruct);

    /* 初始化行列选通器控制 GPIO */
    GPIO_InitStruct.Pin = MUX_PINA;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(MUX_GPIOA, &GPIO_InitStruct);
    
    /* 初始化多路复用器控制IO电平 */
    LL_GPIO_ResetOutputPin(MUX_GPIOA,MUX_PINA);
    LL_GPIO_ResetOutputPin(MUX_GPIOB,MUX_PINB);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
