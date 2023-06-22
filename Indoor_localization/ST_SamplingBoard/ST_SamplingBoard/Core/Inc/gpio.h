/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* 用户LED GPIO定义 */
#define USERLED_PIN	        LL_GPIO_PIN_13
#define USERLED_GPIO        GPIOC

/* 用户按键 GPIO定义 */
#define USERKEY_PIN	        LL_GPIO_PIN_0
#define USERKEY_GPIO        GPIOA

/* 列选通控制 GPIO定义 */
#define GATE_C0_PIN         LL_GPIO_PIN_3
#define GATE_C0_GPIO        GPIOB
#define GATE_C1_PIN         LL_GPIO_PIN_15
#define GATE_C1_GPIO        GPIOA
#define GATE_C2_PIN         LL_GPIO_PIN_0
#define GATE_C2_GPIO        GPIOB

/* 行选通控制 GPIO定义 */
#define GATE_R0_PIN         LL_GPIO_PIN_1
#define GATE_R0_GPIO        GPIOB
#define GATE_R1_PIN         LL_GPIO_PIN_10
#define GATE_R1_GPIO        GPIOB
#define GATE_R2_PIN         LL_GPIO_PIN_12
#define GATE_R2_GPIO        GPIOB
#define GATE_R3_PIN         LL_GPIO_PIN_14
#define GATE_R3_GPIO        GPIOB
#define GATE_R4_PIN         LL_GPIO_PIN_15
#define GATE_R4_GPIO        GPIOB
#define GATE_R5_PIN         LL_GPIO_PIN_6
#define GATE_R5_GPIO        GPIOB

/* 选通器使能 GPIO定义 */
#define EN_INH1_PIN         LL_GPIO_PIN_5
#define EN_INH1_GPIO        GPIOB
#define EN_INH2_PIN         LL_GPIO_PIN_4
#define EN_INH2_GPIO        GPIOB
#define EN_INH3_PIN         LL_GPIO_PIN_8
#define EN_INH3_GPIO        GPIOA

/* 定义行列选通器控制 GPIO */
#define MUX_GPIOA           GPIOA
#define MUX_PINA            LL_GPIO_PIN_8|LL_GPIO_PIN_15
#define MUX_GPIOB           GPIOB
#define MUX_PINB            LL_GPIO_PIN_1|LL_GPIO_PIN_3|LL_GPIO_PIN_4 |LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_8|\
                            LL_GPIO_PIN_10|LL_GPIO_PIN_12|LL_GPIO_PIN_14|LL_GPIO_PIN_15
/* LED灯电平翻转 */
#define LED_Toggle()        LL_GPIO_TogglePin(USERLED_GPIO,USERLED_PIN)

/* USER CODE END Private defines */
/**
 * @brief LED_SetPin 设置LED引脚电平
 * @param bool pin  引脚电平
 */
static inline void LED_SetPin(bool pin)
{
    WRITE_REG(USERLED_GPIO->BSRR, (USERLED_PIN << 16 * pin));
}
/**
 * @brief MUX_SetPin 设置多路复用器控制引脚电平
 * @param PIO_TypeDef *GPIOx  GPIO
 * @param uint32_t PinMask  引脚序号
 * @param bool pin  引脚电平
 */
static inline void MUX_SetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask, bool pin)
{
    WRITE_REG(GPIOx->BSRR, (PinMask << 16 * pin));
}

void MX_GPIO_Init(void);    //GPIO初始化

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

