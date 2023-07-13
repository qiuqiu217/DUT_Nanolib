/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

UART_RXBUF Uart_Receive;        /* 串口指令接收缓冲区结构体声明 */
UART_RXBUF SPP_Receive;         /* 蓝牙指令接收缓冲区结构体声明 */

/* Debug串口接收缓冲区 */
extern uint8_t Receive_Buff[];
/* SPP接收缓冲区 */
extern uint8_t Receive_Buff1[];
/* 接收指令缓冲区 */
extern uint8_t Command_Buff[];
 
 /* 定义每个串口结构体变量 */
#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif
#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif
#if UART6_FIFO_EN == 1
	static UART_T g_tUart6;
	static uint8_t g_TxBuf6[UART6_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf6[UART6_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif
 
static void UartVarInit(void);
static void InitHardUart(void);
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
static void UartIRQ(UART_T *_pUart);


/* 定义串口结构体，用于阻塞型串口收发 */
#if UART1_FIFO_EN == 0u
    UART_HandleTypeDef Uart1Handle;
#endif
#if UART2_FIFO_EN == 0u
    UART_HandleTypeDef Uart2Handle;
#endif
#if UART6_FIFO_EN == 0u
    UART_HandleTypeDef Uart6Handle;
#endif

 /*
*********************************************************************************************************
*	函 数 名: bsp_InitUart
*	功能说明: 初始化串口硬件，并对全局变量赋初值.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	UartVarInit();		/* 必须先初始化全局变量,再配置硬件 */
	InitHardUart();		/* 配置串口的硬件参数(波特率等) */
}

/*
*********************************************************************************************************
*	函 数 名: ComToUart
*	功能说明: 将COM端口号转换为UART指针
*	形    参: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: uart指针
*********************************************************************************************************
*/
UART_T *ComToUart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return &g_tUart1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return &g_tUart2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART6_FIFO_EN == 1
			return &g_tUart6;
		#else
			return 0;
		#endif
	}
	else
	{
		Error_Handler();
		return 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: ComToUart
*	功能说明: 将COM端口号转换为 USART_TypeDef* USARTx
*	形    参: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5，USART6，UART7，UART8。
*********************************************************************************************************
*/
USART_TypeDef *ComToUSARTx(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return USART1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return USART2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART6_FIFO_EN == 1
			return USART6;
		#else
			return 0;
		#endif
	}
	else
	{
		/* 不做任何处理 */
		return 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: comSendBuf
*	功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM8)
*			  _ucaBuf: 待发送的数据缓冲区
*			  _usLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	UartSend(pUart, _ucaBuf, _usLen);
}
/*
*********************************************************************************************************
*	函 数 名: comSendChar
*	功能说明: 向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM8)
*			  _ucByte: 待发送的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}

/*
*********************************************************************************************************
*	函 数 名: comGetChar
*	功能说明: 从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回。
*	形    参: _ucPort: 端口号(COM1 - COM8)
*			  _pByte: 接收到的数据存放在这个地址
*	返 回 值: 0 表示无数据, 1 表示读取到有效字节
*********************************************************************************************************
*/
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return 0;
	}
	return UartGetChar(pUart, _pByte);
}
/*
*********************************************************************************************************
*	函 数 名: comClearTxFifo
*	功能说明: 清零串口发送缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: 无
*********************************************************************************************************
*/
void comClearTxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}
	pUart->usTxWrite = 0;
	pUart->usTxRead = 0;
	pUart->usTxCount = 0;
}

/*
*********************************************************************************************************
*	函 数 名: comClearRxFifo
*	功能说明: 清零串口接收缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: 无
*********************************************************************************************************
*/
void comClearRxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}
	pUart->usRxWrite = 0;
	pUart->usRxRead = 0;
	pUart->usRxCount = 0;
}
/*
*********************************************************************************************************
*	函 数 名: comSetBaud
*	功能说明: 设置串口的波特率. 本函数固定设置为无校验，收发都使能模式
*	形    参: _ucPort: 端口号(COM1 - COM8)
*			  _BaudRate: 波特率，8倍过采样  波特率.0-12.5Mbps
*                                16倍过采样 波特率.0-6.25Mbps
*	返 回 值: 无
*********************************************************************************************************
*/
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate)
{
	USART_TypeDef* USARTx;
	
	USARTx = ComToUSARTx(_ucPort);
	if (USARTx == 0)
	{
		return;
	}
	bsp_SetUartParam(USARTx,  _BaudRate, UART_PARITY_NONE, UART_MODE_TX_RX);
}
/*
*********************************************************************************************************
*	函 数 名: UartVarInit
*	功能说明: 初始化串口相关的变量
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartVarInit(void)
{
    #if UART1_FIFO_EN == 1
        g_tUart1.uart = USART1;						/* STM32 串口设备 */
        g_tUart1.pTxBuf = g_TxBuf1;					/* 发送缓冲区指针 */
        g_tUart1.pRxBuf = g_RxBuf1;					/* 接收缓冲区指针 */
        g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* 发送缓冲区大小 */
        g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* 接收缓冲区大小 */
        g_tUart1.usTxWrite = 0;						/* 发送FIFO写索引 */
        g_tUart1.usTxRead = 0;						/* 发送FIFO读索引 */
        g_tUart1.usRxWrite = 0;						/* 接收FIFO写索引 */
        g_tUart1.usRxRead = 0;						/* 接收FIFO读索引 */
        g_tUart1.usRxCount = 0;						/* 接收到的新数据个数 */
        g_tUart1.usTxCount = 0;						/* 待发送的数据个数 */
        g_tUart1.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
        g_tUart1.Sending = 0;						/* 正在发送中标志 */
    #endif

    #if UART2_FIFO_EN == 1
        g_tUart2.uart = USART2;						/* STM32 串口设备 */
        g_tUart2.pTxBuf = g_TxBuf2;					/* 发送缓冲区指针 */
        g_tUart2.pRxBuf = g_RxBuf2;					/* 接收缓冲区指针 */
        g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* 发送缓冲区大小 */
        g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* 接收缓冲区大小 */
        g_tUart2.usTxWrite = 0;						/* 发送FIFO写索引 */
        g_tUart2.usTxRead = 0;						/* 发送FIFO读索引 */
        g_tUart2.usRxWrite = 0;						/* 接收FIFO写索引 */
        g_tUart2.usRxRead = 0;						/* 接收FIFO读索引 */
        g_tUart2.usRxCount = 0;						/* 接收到的新数据个数 */
        g_tUart2.usTxCount = 0;						/* 待发送的数据个数 */
        g_tUart2.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
        g_tUart2.Sending = 0;						/* 正在发送中标志 */
    #endif

    #if UART6_FIFO_EN == 1
        g_tUart6.uart = USART6;						/* STM32 串口设备 */
        g_tUart6.pTxBuf = g_TxBuf6;					/* 发送缓冲区指针 */
        g_tUart6.pRxBuf = g_RxBuf6;					/* 接收缓冲区指针 */
        g_tUart6.usTxBufSize = UART6_TX_BUF_SIZE;	/* 发送缓冲区大小 */
        g_tUart6.usRxBufSize = UART6_RX_BUF_SIZE;	/* 接收缓冲区大小 */
        g_tUart6.usTxWrite = 0;						/* 发送FIFO写索引 */
        g_tUart6.usTxRead = 0;						/* 发送FIFO读索引 */
        g_tUart6.usRxWrite = 0;						/* 接收FIFO写索引 */
        g_tUart6.usRxRead = 0;						/* 接收FIFO读索引 */
        g_tUart6.usRxCount = 0;						/* 接收到的新数据个数 */
        g_tUart6.usTxCount = 0;						/* 待发送的数据个数 */
        g_tUart6.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
        g_tUart6.Sending = 0;						/* 正在发送中标志 */
    #endif
}
/*
*********************************************************************************************************
*	函 数 名: bsp_SetUartParam
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32- H7开发板
*	形    参: Instance   USART_TypeDef类型结构体
*             BaudRate   波特率
*             Parity     校验类型，奇校验或者偶校验
*             Mode       发送和接收模式使能
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetUartParam(USART_TypeDef *Instance,  uint32_t BaudRate, uint32_t Parity, uint32_t Mode)
{
	UART_HandleTypeDef UartHandle;	
	
	/*##-1- 配置串口硬件参数 ######################################*/
	/* 异步串口模式 (UART Mode) */
	/* 配置如下:
	  - 字长    = 8 位
	  - 停止位  = 1 个停止位
	  - 校验    = 参数Parity
	  - 波特率  = 参数BaudRate
	  - 硬件流控制关闭 (RTS and CTS signals) */

	UartHandle.Instance        = Instance;
	UartHandle.Init.BaudRate   = BaudRate;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = Parity;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = Mode;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		Error_Handler();
	}
}
/*
*********************************************************************************************************
*	函 数 名: InitHardUart
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-H7开发板
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitHardUart(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

    #if UART1_FIFO_EN == 1		/* 串口1 */
        /* 使能 GPIO TX/RX 时钟 */
        USART1_TX_GPIO_CLK_ENABLE();
        USART1_RX_GPIO_CLK_ENABLE();
        
        /* 使能 USARTx 时钟 */
        USART1_CLK_ENABLE();	

        /* 配置TX引脚 */
        GPIO_InitStruct.Pin       = USART1_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART1_TX_AF;
        HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* 配置RX引脚 */
        GPIO_InitStruct.Pin = USART1_RX_PIN;
        GPIO_InitStruct.Alternate = USART1_RX_AF;
        HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

        /* 配置NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART1_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
      
        /* 配置波特率、奇偶校验 */
        bsp_SetUartParam(USART1,  UART1_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

        CLEAR_BIT(USART1->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
        CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
        SET_BIT(USART1->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
    #else
        /* 使能 GPIO TX/RX 时钟 */
        USART1_TX_GPIO_CLK_ENABLE();
        USART1_RX_GPIO_CLK_ENABLE();
        
        /* 使能 USARTx 时钟 */
        USART1_CLK_ENABLE();	

        /* 配置TX引脚 */
        GPIO_InitStruct.Pin       = USART1_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART1_TX_AF;
        HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* 配置RX引脚 */
        GPIO_InitStruct.Pin = USART1_RX_PIN;
        GPIO_InitStruct.Alternate = USART1_RX_AF;
        HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

        /* 配置NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 2);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
      
        /* 配置波特率、奇偶校验 */
        Uart1Handle.Instance        = USART1;
        Uart1Handle.Init.BaudRate   = 460800;
        Uart1Handle.Init.WordLength = UART_WORDLENGTH_8B;
        Uart1Handle.Init.StopBits   = UART_STOPBITS_1;
        Uart1Handle.Init.Parity     = UART_PARITY_NONE;
        Uart1Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
        Uart1Handle.Init.Mode       = UART_MODE_TX_RX;
        Uart1Handle.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&Uart1Handle) != HAL_OK)
        {
            Error_Handler();
        }

        CLEAR_BIT(USART1->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
        CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
        SET_BIT(USART1->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
    #endif

    #if UART2_FIFO_EN == 1		/* 串口2 */
        /* 使能 GPIO TX/RX 时钟 */
        USART2_TX_GPIO_CLK_ENABLE();
        USART2_RX_GPIO_CLK_ENABLE();
        
        /* 使能 USARTx 时钟 */
        USART2_CLK_ENABLE();	

        /* 配置TX引脚 */
        GPIO_InitStruct.Pin       = USART2_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART2_TX_AF;
        HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* 配置RX引脚 */
        GPIO_InitStruct.Pin = USART2_RX_PIN;
        GPIO_InitStruct.Alternate = USART2_RX_AF;
        HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

        /* 配置NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
      
        /* 配置波特率、奇偶校验 */
        bsp_SetUartParam(USART2,  UART2_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);	// UART_MODE_TX_RX

        CLEAR_BIT(USART2->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
        CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
        SET_BIT(USART2->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
    #else
        /* 使能 GPIO TX/RX 时钟 */
        USART2_TX_GPIO_CLK_ENABLE();
        USART2_RX_GPIO_CLK_ENABLE();
        
        /* 使能 USARTx 时钟 */
        USART2_CLK_ENABLE();	

        /* 配置TX引脚 */
        GPIO_InitStruct.Pin       = USART2_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART2_TX_AF;
        HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* 配置RX引脚 */
        GPIO_InitStruct.Pin = USART2_RX_PIN;
        GPIO_InitStruct.Alternate = USART2_RX_AF;
        HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

        /* 配置NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 3);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
      
        /* 配置波特率、奇偶校验 */
        Uart2Handle.Instance        = USART2;
        Uart2Handle.Init.BaudRate   = 460800;
        Uart2Handle.Init.WordLength = UART_WORDLENGTH_8B;
        Uart2Handle.Init.StopBits   = UART_STOPBITS_1;
        Uart2Handle.Init.Parity     = UART_PARITY_NONE;
        Uart2Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
        Uart2Handle.Init.Mode       = UART_MODE_TX_RX;
        Uart2Handle.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&Uart2Handle) != HAL_OK)
        {
            Error_Handler();
        }

        CLEAR_BIT(USART2->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
        CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
        SET_BIT(USART2->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
    #endif

    #if UART6_FIFO_EN == 1			/* USART6 */
        /* 使能 GPIO TX/RX 时钟 */
        USART6_TX_GPIO_CLK_ENABLE();
        USART6_RX_GPIO_CLK_ENABLE();
        
        /* 使能 USARTx 时钟 */
        USART6_CLK_ENABLE();	

        /* 配置TX引脚 */
        GPIO_InitStruct.Pin       = USART6_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART6_TX_AF;
        HAL_GPIO_Init(USART6_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* 配置RX引脚 */
        GPIO_InitStruct.Pin = USART6_RX_PIN;
        GPIO_InitStruct.Alternate = USART6_RX_AF;
        HAL_GPIO_Init(USART6_RX_GPIO_PORT, &GPIO_InitStruct);

        /* 配置NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART6_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USART6_IRQn);
        
        /* 配置波特率、奇偶校验 */
        bsp_SetUartParam(USART6,  UART6_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

        CLEAR_BIT(USART6->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
        CLEAR_BIT(USART6->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
        SET_BIT(USART6->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
    #else
        /* 使能 GPIO TX/RX 时钟 */
        USART6_TX_GPIO_CLK_ENABLE();
        USART6_RX_GPIO_CLK_ENABLE();
        
        /* 使能 USARTx 时钟 */
        USART6_CLK_ENABLE();	

        /* 配置TX引脚 */
        GPIO_InitStruct.Pin       = USART6_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART6_TX_AF;
        HAL_GPIO_Init(USART6_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* 配置RX引脚 */
        GPIO_InitStruct.Pin = USART6_RX_PIN;
        GPIO_InitStruct.Alternate = USART6_RX_AF;
        HAL_GPIO_Init(USART6_RX_GPIO_PORT, &GPIO_InitStruct);

        /* 配置NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART6_IRQn, 0, 1);
        HAL_NVIC_EnableIRQ(USART6_IRQn);
        
        /* 配置波特率、奇偶校验 */
        Uart6Handle.Instance        = USART6;
        Uart6Handle.Init.BaudRate   = 460800;
        Uart6Handle.Init.WordLength = UART_WORDLENGTH_8B;
        Uart6Handle.Init.StopBits   = UART_STOPBITS_1;
        Uart6Handle.Init.Parity     = UART_PARITY_NONE;
        Uart6Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
        Uart6Handle.Init.Mode       = UART_MODE_TX_RX;
        Uart6Handle.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&Uart6Handle) != HAL_OK)
        {
            Error_Handler();
        }

        CLEAR_BIT(USART6->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
        CLEAR_BIT(USART6->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
        SET_BIT(USART6->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
    #endif
}



/*
*********************************************************************************************************
*	函 数 名: UartSend
*	功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
		while (1)
		{
			__IO uint16_t usCount;

			vPortEnterCritical();       //禁止全局中断
			usCount = _pUart->usTxCount;
			vPortExitCritical();        //使能全局中断

			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
			else if(usCount == _pUart->usTxBufSize)/* 数据已填满缓冲区 */
			{
				if((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0)
				{
					SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);
				}  
			}
		}

		/* 将新数据填入发送缓冲区 */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		vPortEnterCritical();       //禁止全局中断
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		vPortExitCritical();        //使能全局中断
	}

	SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);	/* 使能发送中断（缓冲区空） */
}

/*
*********************************************************************************************************
*	函 数 名: UartGetChar
*	功能说明: 从串口接收缓冲区读取1字节数据 （用于主程序调用）
*	形    参: _pUart : 串口设备
*			  _pByte : 存放读取数据的指针
*	返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
	vPortEnterCritical();       //禁止全局中断
	usCount = _pUart->usRxCount;
	vPortExitCritical();        //使能全局中断

	/* 如果读和写索引相同，则返回0 */
	if (usCount == 0)	/* 已经没有数据 */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* 从串口接收FIFO取1个数据 */

		/* 改写FIFO读索引 */
		vPortEnterCritical();       //禁止全局中断
		if (++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		vPortExitCritical();        //使能全局中断
		return 1;
	}
}

/*
*********************************************************************************************************
*   函 数 名: UartTxEmpty
*   功能说明: 判断发送缓冲区是否为空。
*   形    参:  _pUart : 串口设备
*   返 回 值: 1为空。0为不空。
*********************************************************************************************************
*/
uint8_t UartTxEmpty(COM_PORT_E _ucPort)
{
    UART_T *pUart;
    uint8_t Sending;
   
    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return 0;
    }

    Sending = pUart->Sending;

    if (Sending != 0)
    {
        return 0;
    }
    return 1;
}
/*
*********************************************************************************************************
*	函 数 名: UartIRQ
*	功能说明: 供中断服务程序调用，通用串口中断处理函数
*	形    参: _pUart : 串口设备
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
	uint32_t isrflags   = READ_REG(_pUart->uart->SR);
	uint32_t cr1its     = READ_REG(_pUart->uart->CR1);
	uint32_t cr3its     = READ_REG(_pUart->uart->CR3);
    
    UBaseType_t uxSavedInterruptStatus;
	
    /* 进入临界段，临界段可以嵌套 */
	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	/* 处理接收中断  */
	if ((isrflags & USART_SR_RXNE) != RESET)
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		uint8_t ch;

		ch = READ_REG(_pUart->uart->DR);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}
		/* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch); /* 比如，交给MODBUS解码程序处理字节流 */
			}
		}
	}

	/* 处理发送缓冲区空中断 */
	if ( ((isrflags & USART_SR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if(_pUart->usTxCount == 0)
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			//USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);

			/* 使能数据发送完毕中断 */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
			SET_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
		}
		else
		{
			_pUart->Sending = 1;
			
			/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			_pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
			if(++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
	/* 数据bit位全部发送完毕的中断 */
	if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if(_pUart->usTxCount == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
		}
		else
		{
			/* 正常情况下，不会进入此分支 */

			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			_pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
			if(++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
    
    /* 退出临界段 */
	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus); 	
}

/*
*********************************************************************************************************
*	函 数 名: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler等
*	功能说明: USART中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if UART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
	UartIRQ(&g_tUart1);
}
#else

#endif

#if UART2_FIFO_EN == 1
void USART2_IRQHandler(void)
{
	UartIRQ(&g_tUart2);
}
#else

#endif

#if UART6_FIFO_EN == 1
void USART6_IRQHandler(void)
{
	UartIRQ(&g_tUart6);
}
#else
void  USART6_IRQHandler(void)
{
  uint8_t ch=0; 
  
	if(__HAL_UART_GET_FLAG( &Uart6Handle, UART_FLAG_RXNE ) != RESET)
	{		
        ch=( uint16_t)READ_REG(Uart6Handle.Instance->DR);
        //WRITE_REG(UartHandle.Instance->DR,ch); 
 
	}
}
#endif

#if UART6_FIFO_EN == 0u
void SPP_SendString(uint8_t *str, uint16_t size)
{
	unsigned int k=0;
    HAL_UART_Transmit(&Uart6Handle,(uint8_t *)(str + k) ,size,1000);
  
}
#endif

/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
    #if 1	/* 将需要printf的字符通过串口中断FIFO发送出去，printf函数会立即返回 */
        comSendChar(DEBUG_COM, ch);
        
        return ch;
    #else	/* 采用阻塞方式发送每个字符,等待数据发送完毕 */
        /* 写一个字节到USART1 */
        USART1->DR = ch;
        
        /* 等待发送结束 */
        while((USART1->SR & USART_SR_TC) == 0)
        {}
        
        return ch;
    #endif
}

/*
*********************************************************************************************************
*	函 数 名: fgetc
*	功能说明: 重定义getc函数，这样可以使用getchar函数从串口1输入数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
    #if 1	/* 从串口接收FIFO中取1个数据, 只有取到数据才返回 */
        uint8_t ucData;

        while(comGetChar(DEBUG_COM, &ucData) == 0);

        return ucData;
    #else
        /* 等待接收到数据 */
        while((USART1->SR & USART_SR_RXNE) == 0)
        {}

        return (int)USART1->DR;
    #endif
}

/* USER CODE BEGIN 1 */
/**
 * @brief Uart_RxBuf_Init     串口指令接收缓冲区初始化
 */
void Uart_RxBuf_Init(void)
{
    Uart_Receive.RxBuf = Receive_Buff;                  //初始化指令接收缓冲区
    Uart_Receive.BufSize = RX_BUFF_SIZE;                //初始化指令接收缓冲区大小
    Uart_Receive.WriteCtr = 0;                          //初始化指令缓冲区写指针
}

/**
 * @brief Uart_RxBuf_Clear     串口指令接收缓冲区清空
 */
static inline void Uart_RxBuf_Clear(void)
{
    memset(Uart_Receive.RxBuf, '\0', Uart_Receive.BufSize);
    Uart_Receive.WriteCtr = 0;
}

/**
 * @brief Uart_RxBuf_Putin     串口指令接收缓冲区存入
 * @param uint8_t *_pByte   存入的字符
 */
void Uart_RxBuf_Putin(uint8_t *_pByte)
{
    *(Uart_Receive.RxBuf + Uart_Receive.WriteCtr) = *_pByte;
    Uart_Receive.WriteCtr ++;
    /* 指令缓冲区是否溢出 */
    if(Uart_Receive.WriteCtr >= Uart_Receive.BufSize)
    {
        Uart_Receive.WriteCtr = 0;
        USART_LOG("Uart receive buff full\n\r");
    }
}

/**
 * @brief Uart_Load_Command     存入命令
 * @return int8_t -1命令提取错误  1命令提取成功
 */
int8_t Uart_Load_Command(void)
{
    uint16_t i;
    if(Uart_Receive.WriteCtr < 15)
    {
        Uart_RxBuf_Clear();
        return RET_ERROR;
    }
    for(i=0;i<13;i++)
    {
        Command_Buff[i] = *(Uart_Receive.RxBuf + i + Uart_Receive.WriteCtr - 15);
        printf("%02X ", Command_Buff[i]);
    }
    Uart_RxBuf_Clear();
    #if 0
        USART_LOG("Buff receive :%s\n\r",Command_Buff);
    #endif
    
    return RET_OK;
}

/**
 * @brief SPP_RxBuf_Init     SPP指令接收缓冲区初始化
 */
void SPP_RxBuf_Init(void)
{
    SPP_Receive.RxBuf = Receive_Buff1;                  //初始化指令接收缓冲区
    SPP_Receive.BufSize = RX_BUFF_SIZE;                //初始化指令接收缓冲区大小
    SPP_Receive.WriteCtr = 0;                          //初始化指令缓冲区写指针
}

/**
 * @brief SPP_RxBuf_Clear     SPP指令接收缓冲区清空
 */
static inline void SPP_RxBuf_Clear(void)
{
    memset(SPP_Receive.RxBuf, '\0', SPP_Receive.BufSize);
    SPP_Receive.WriteCtr = 0;
}

/**
 * @brief SPP_RxBuf_Putin     SPP指令接收缓冲区存入
 * @param uint8_t *_pByte   存入的字符
 */
void SPP_RxBuf_Putin(uint8_t *_pByte)
{
    *(SPP_Receive.RxBuf + SPP_Receive.WriteCtr) = *_pByte;
    SPP_Receive.WriteCtr ++;
    /* 指令缓冲区是否溢出 */
    if(SPP_Receive.WriteCtr >= SPP_Receive.BufSize)
    {
        SPP_Receive.WriteCtr = 0;
        USART_LOG("SPP receive buff full\n\r");
    }
}

/**
 * @brief SPP_Load_Command     存入命令
 * @return int8_t -1命令提取错误  1命令提取成功
 */
int8_t SPP_Load_Command(void)
{
    uint16_t i;
    if(SPP_Receive.WriteCtr < 15)
    {
        SPP_RxBuf_Clear();
        return RET_ERROR;
    }
    for(i=0;i<13;i++)
    {
        Command_Buff[i] = *(SPP_Receive.RxBuf + i + SPP_Receive.WriteCtr - 14);
        printf("%02X ", Command_Buff[i]);
    }
    SPP_RxBuf_Clear();
    #if 0
        USART_LOG("Buff receive :%s\n\r",Command_Buff);
    #endif
    
    return RET_OK;
}

/* USER CODE END 1 */
