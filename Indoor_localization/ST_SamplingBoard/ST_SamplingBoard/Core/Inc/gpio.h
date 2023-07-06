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

/* �û�LED GPIO���� */
#define USERLED_PIN	        LL_GPIO_PIN_13
#define USERLED_GPIO        GPIOC

/* �û����� GPIO���� */
#define USERKEY_PIN	        LL_GPIO_PIN_0
#define USERKEY_GPIO        GPIOA

/* SPP State GPIO���� */
#define SPPSTATE_PIN	    LL_GPIO_PIN_9
#define SPPSTATE_GPIO       GPIOB
#define SPPSTATE_EXTI_LINE  LL_EXTI_LINE_9
#define SPPSTATE_EXTI_IRQn  EXTI9_5_IRQn
#define SPPSTATE_EXTI_LINE_ENABLE()                 LL_EXTI_EnableIT_0_31(SPPSTATE_EXTI_LINE)
#define SPPSTATE_EXTI_FALLING_TRIG_ENABLE()         LL_EXTI_EnableFallingTrig_0_31(SPPSTATE_EXTI_LINE)
#define SPPSTATE_EXTI_FALLING_TRIG_DISABLE()        LL_EXTI_DisableFallingTrig_0_31(SPPSTATE_EXTI_LINE)
#define SPPSTATE_EXTI_RISING_TRIG_ENABLE()          LL_EXTI_EnableRisingTrig_0_31(SPPSTATE_EXTI_LINE)
#define SPPSTATE_EXTI_RISING_TRIG_DISABLE()         LL_EXTI_DisableRisingTrig_0_31(SPPSTATE_EXTI_LINE)
#define SPPSTATE_SYSCFG_SET_EXTI()                  do{                                                                     \
                                                        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);                \
                                                        LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE9);  \
                                                    } \
                                                    while(0)
#define SPPSTATE_INPUTPINSET                        LL_GPIO_IsInputPinSet(SPPSTATE_GPIO, SPPSTATE_PIN)
#define SPPSTATE_IRQHANDLER                         EXTI9_5_IRQHandler


/* SPP Key GPIO���� */
#define SPPKEY_PIN	        LL_GPIO_PIN_4
#define SPPKEY_GPIO         GPIOA

/* ��ѡͨ���� GPIO���� */
#define GATE_C0_PIN         LL_GPIO_PIN_3
#define GATE_C0_GPIO        GPIOB
#define GATE_C1_PIN         LL_GPIO_PIN_15
#define GATE_C1_GPIO        GPIOA
#define GATE_C2_PIN         LL_GPIO_PIN_0
#define GATE_C2_GPIO        GPIOB

/* ��ѡͨ���� GPIO���� */
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

/* ѡͨ��ʹ�� GPIO���� */
#define EN_INH1_PIN         LL_GPIO_PIN_5
#define EN_INH1_GPIO        GPIOB
#define EN_INH2_PIN         LL_GPIO_PIN_4
#define EN_INH2_GPIO        GPIOB
#define EN_INH3_PIN         LL_GPIO_PIN_8
#define EN_INH3_GPIO        GPIOA

/* ��������ѡͨ������ GPIO */
#define MUX_GPIOA           GPIOA
#define MUX_PINA            LL_GPIO_PIN_8|LL_GPIO_PIN_15
#define MUX_GPIOB           GPIOB
#define MUX_PINB            LL_GPIO_PIN_1|LL_GPIO_PIN_3|LL_GPIO_PIN_4 |LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_8|\
                            LL_GPIO_PIN_10|LL_GPIO_PIN_12|LL_GPIO_PIN_14|LL_GPIO_PIN_15
/* LED�Ƶ�ƽ��ת */
#define LED_TOGGLE()        LL_GPIO_TogglePin(USERLED_GPIO,USERLED_PIN)
#define LED_ON()            LL_GPIO_ResetOutputPin(USERLED_GPIO,USERLED_PIN)
#define LED_OFF()           LL_GPIO_SetOutputPin(USERLED_GPIO,USERLED_PIN)

/* USER CODE END Private defines */
/**
 * @brief LED_SetPin ����LED���ŵ�ƽ
 * @param bool pin  ���ŵ�ƽ
 */
static inline void LED_SetPin(bool pin)
{
    WRITE_REG(USERLED_GPIO->BSRR, (USERLED_PIN << 16 * pin));
}
/**
 * @brief MUX_SetPin ���ö�·�������������ŵ�ƽ
 * @param PIO_TypeDef *GPIOx  GPIO
 * @param uint32_t PinMask  �������
 * @param bool pin  ���ŵ�ƽ
 */
static inline void MUX_SetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask, bool pin)
{
    if(pin) WRITE_REG(GPIOx->BSRR, PinMask);
    else WRITE_REG(GPIOx->BSRR, (PinMask << 16));
}

void bsp_InitGPIO(void);    //GPIO��ʼ��

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

