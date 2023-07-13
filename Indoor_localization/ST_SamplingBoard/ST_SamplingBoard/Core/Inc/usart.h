/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* LOG打印设置 */
#if 0
	#define USART_LOG     printf
#else
	#define USART_LOG(...)
#endif

/*-------------------------------- 串口GPIO定义 ----------------------------------*/
/* 串口1的GPIO  PA9, PA10   RS323 DB9接口 */
#define USART1_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()

#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_AF                     GPIO_AF7_USART1

#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_AF                     GPIO_AF7_USART1

/* 串口2的GPIO --- PA2 PA3 */
#define USART2_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()

#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_GPIO_PORT              GPIOA
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_TX_AF                     GPIO_AF7_USART2

#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_RX_GPIO_PORT              GPIOA
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_RX_AF                     GPIO_AF7_USART2

/* 串口6的GPIO --- PA11 PA12 */
#define USART6_CLK_ENABLE()              __HAL_RCC_USART6_CLK_ENABLE()

#define USART6_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART6_TX_GPIO_PORT              GPIOA
#define USART6_TX_PIN                    GPIO_PIN_11
#define USART6_TX_AF                     GPIO_AF8_USART6

#define USART6_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART6_RX_GPIO_PORT              GPIOA
#define USART6_RX_PIN                    GPIO_PIN_12
#define USART6_RX_AF                     GPIO_AF8_USART6

/*-------------------------------- 串口FIFO定义 ----------------------------------*/
/* 串口FIFO使能 */
#define	UART1_FIFO_EN	1
#define	UART2_FIFO_EN	1
#define	UART6_FIFO_EN	1

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD			460800
	#define UART1_TX_BUF_SIZE	4*1024
	#define UART1_RX_BUF_SIZE	4*1024   
#endif
#if UART2_FIFO_EN == 1
	#define UART2_BAUD			115200
	#define UART2_TX_BUF_SIZE	1*1024
	#define UART2_RX_BUF_SIZE	6*1024
#endif
#if UART6_FIFO_EN == 1
	#define UART6_BAUD			460800
	#define UART6_TX_BUF_SIZE	6*1024
	#define UART6_RX_BUF_SIZE	1*1024
#endif



/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1 */
	COM2 = 1,	/* USART2 */
	COM3 = 2,	/* USART6 */
}COM_PORT_E;

/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;		/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;		/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;	/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;		/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;	/* 还未读取的新数据个数 */

	void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
	uint8_t Sending;			/* 正在发送中 */
}UART_T;

/* 串口指令接收缓冲区结构体 */
typedef struct
{
    uint8_t *RxBuf;			    /* 接收缓冲区 */
    uint16_t BufSize;           /* 接收缓冲区大小 */
    uint16_t WriteCtr;          /* 接收缓冲区读指针 */
}UART_RXBUF;


/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
void bsp_InitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate);
void USART_SetBaudRate(USART_TypeDef* USARTx, uint32_t BaudRate);
void bsp_SetUartParam(USART_TypeDef *Instance,  uint32_t BaudRate, uint32_t Parity, uint32_t Mode);
uint8_t UartTxEmpty(COM_PORT_E _ucPort);

/* 串口指令接收处理函数 */
void Uart_RxBuf_Init(void);
void Uart_RxBuf_Putin(uint8_t *_pByte);
int8_t Uart_Load_Command(void);
/* SPP指令接收处理函数 */
void SPP_RxBuf_Init(void);
void SPP_RxBuf_Putin(uint8_t *_pByte);
int8_t SPP_Load_Command(void);

#if UART6_FIFO_EN == 0u
void SPP_SendString(uint8_t *str, uint16_t size);
#endif

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

