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

UART_RXBUF Uart_Receive;        /* ����ָ����ջ������ṹ������ */
UART_RXBUF SPP_Receive;         /* ����ָ����ջ������ṹ������ */

/* Debug���ڽ��ջ����� */
extern uint8_t Receive_Buff[];
/* SPP���ջ����� */
extern uint8_t Receive_Buff1[];
/* ����ָ����� */
extern uint8_t Command_Buff[];
 
 /* ����ÿ�����ڽṹ����� */
#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* ���ջ����� */
#endif
#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ����� */
#endif
#if UART6_FIFO_EN == 1
	static UART_T g_tUart6;
	static uint8_t g_TxBuf6[UART6_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf6[UART6_RX_BUF_SIZE];		/* ���ջ����� */
#endif
 
static void UartVarInit(void);
static void InitHardUart(void);
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
static void UartIRQ(UART_T *_pUart);


/* ���崮�ڽṹ�壬���������ʹ����շ� */
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
*	�� �� ��: bsp_InitUart
*	����˵��: ��ʼ������Ӳ��������ȫ�ֱ�������ֵ.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	UartVarInit();		/* �����ȳ�ʼ��ȫ�ֱ���,������Ӳ�� */
	InitHardUart();		/* ���ô��ڵ�Ӳ������(�����ʵ�) */
}

/*
*********************************************************************************************************
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��ΪUARTָ��
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*	�� �� ֵ: uartָ��
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
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��Ϊ USART_TypeDef* USARTx
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*	�� �� ֵ: USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5��USART6��UART7��UART8��
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
		/* �����κδ��� */
		return 0;
	}
}
/*
*********************************************************************************************************
*	�� �� ��: comSendBuf
*	����˵��: �򴮿ڷ���һ�����ݡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*			  _ucaBuf: �����͵����ݻ�����
*			  _usLen : ���ݳ���
*	�� �� ֵ: ��
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
*	�� �� ��: comSendChar
*	����˵��: �򴮿ڷ���1���ֽڡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*			  _ucByte: �����͵�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}

/*
*********************************************************************************************************
*	�� �� ��: comGetChar
*	����˵��: �ӽ��ջ�������ȡ1�ֽڣ��������������������ݾ��������ء�
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*			  _pByte: ���յ������ݴ���������ַ
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
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
*	�� �� ��: comClearTxFifo
*	����˵��: ���㴮�ڷ��ͻ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*	�� �� ֵ: ��
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
*	�� �� ��: comClearRxFifo
*	����˵��: ���㴮�ڽ��ջ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*	�� �� ֵ: ��
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
*	�� �� ��: comSetBaud
*	����˵��: ���ô��ڵĲ�����. �������̶�����Ϊ��У�飬�շ���ʹ��ģʽ
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*			  _BaudRate: �����ʣ�8��������  ������.0-12.5Mbps
*                                16�������� ������.0-6.25Mbps
*	�� �� ֵ: ��
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
*	�� �� ��: UartVarInit
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartVarInit(void)
{
    #if UART1_FIFO_EN == 1
        g_tUart1.uart = USART1;						/* STM32 �����豸 */
        g_tUart1.pTxBuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
        g_tUart1.pRxBuf = g_RxBuf1;					/* ���ջ�����ָ�� */
        g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* ���ͻ�������С */
        g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* ���ջ�������С */
        g_tUart1.usTxWrite = 0;						/* ����FIFOд���� */
        g_tUart1.usTxRead = 0;						/* ����FIFO������ */
        g_tUart1.usRxWrite = 0;						/* ����FIFOд���� */
        g_tUart1.usRxRead = 0;						/* ����FIFO������ */
        g_tUart1.usRxCount = 0;						/* ���յ��������ݸ��� */
        g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
        g_tUart1.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
        g_tUart1.Sending = 0;						/* ���ڷ����б�־ */
    #endif

    #if UART2_FIFO_EN == 1
        g_tUart2.uart = USART2;						/* STM32 �����豸 */
        g_tUart2.pTxBuf = g_TxBuf2;					/* ���ͻ�����ָ�� */
        g_tUart2.pRxBuf = g_RxBuf2;					/* ���ջ�����ָ�� */
        g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* ���ͻ�������С */
        g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* ���ջ�������С */
        g_tUart2.usTxWrite = 0;						/* ����FIFOд���� */
        g_tUart2.usTxRead = 0;						/* ����FIFO������ */
        g_tUart2.usRxWrite = 0;						/* ����FIFOд���� */
        g_tUart2.usRxRead = 0;						/* ����FIFO������ */
        g_tUart2.usRxCount = 0;						/* ���յ��������ݸ��� */
        g_tUart2.usTxCount = 0;						/* �����͵����ݸ��� */
        g_tUart2.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
        g_tUart2.Sending = 0;						/* ���ڷ����б�־ */
    #endif

    #if UART6_FIFO_EN == 1
        g_tUart6.uart = USART6;						/* STM32 �����豸 */
        g_tUart6.pTxBuf = g_TxBuf6;					/* ���ͻ�����ָ�� */
        g_tUart6.pRxBuf = g_RxBuf6;					/* ���ջ�����ָ�� */
        g_tUart6.usTxBufSize = UART6_TX_BUF_SIZE;	/* ���ͻ�������С */
        g_tUart6.usRxBufSize = UART6_RX_BUF_SIZE;	/* ���ջ�������С */
        g_tUart6.usTxWrite = 0;						/* ����FIFOд���� */
        g_tUart6.usTxRead = 0;						/* ����FIFO������ */
        g_tUart6.usRxWrite = 0;						/* ����FIFOд���� */
        g_tUart6.usRxRead = 0;						/* ����FIFO������ */
        g_tUart6.usRxCount = 0;						/* ���յ��������ݸ��� */
        g_tUart6.usTxCount = 0;						/* �����͵����ݸ��� */
        g_tUart6.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
        g_tUart6.Sending = 0;						/* ���ڷ����б�־ */
    #endif
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_SetUartParam
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32- H7������
*	��    ��: Instance   USART_TypeDef���ͽṹ��
*             BaudRate   ������
*             Parity     У�����ͣ���У�����żУ��
*             Mode       ���ͺͽ���ģʽʹ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetUartParam(USART_TypeDef *Instance,  uint32_t BaudRate, uint32_t Parity, uint32_t Mode)
{
	UART_HandleTypeDef UartHandle;	
	
	/*##-1- ���ô���Ӳ������ ######################################*/
	/* �첽����ģʽ (UART Mode) */
	/* ��������:
	  - �ֳ�    = 8 λ
	  - ֹͣλ  = 1 ��ֹͣλ
	  - У��    = ����Parity
	  - ������  = ����BaudRate
	  - Ӳ�������ƹر� (RTS and CTS signals) */

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
*	�� �� ��: InitHardUart
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32-H7������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitHardUart(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

    #if UART1_FIFO_EN == 1		/* ����1 */
        /* ʹ�� GPIO TX/RX ʱ�� */
        USART1_TX_GPIO_CLK_ENABLE();
        USART1_RX_GPIO_CLK_ENABLE();
        
        /* ʹ�� USARTx ʱ�� */
        USART1_CLK_ENABLE();	

        /* ����TX���� */
        GPIO_InitStruct.Pin       = USART1_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART1_TX_AF;
        HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* ����RX���� */
        GPIO_InitStruct.Pin = USART1_RX_PIN;
        GPIO_InitStruct.Alternate = USART1_RX_AF;
        HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

        /* ����NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART1_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
      
        /* ���ò����ʡ���żУ�� */
        bsp_SetUartParam(USART1,  UART1_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

        CLEAR_BIT(USART1->SR, USART_SR_TC);   /* ���TC������ɱ�־ */
        CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
        SET_BIT(USART1->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
    #else
        /* ʹ�� GPIO TX/RX ʱ�� */
        USART1_TX_GPIO_CLK_ENABLE();
        USART1_RX_GPIO_CLK_ENABLE();
        
        /* ʹ�� USARTx ʱ�� */
        USART1_CLK_ENABLE();	

        /* ����TX���� */
        GPIO_InitStruct.Pin       = USART1_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART1_TX_AF;
        HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* ����RX���� */
        GPIO_InitStruct.Pin = USART1_RX_PIN;
        GPIO_InitStruct.Alternate = USART1_RX_AF;
        HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

        /* ����NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 2);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
      
        /* ���ò����ʡ���żУ�� */
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

        CLEAR_BIT(USART1->SR, USART_SR_TC);   /* ���TC������ɱ�־ */
        CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
        SET_BIT(USART1->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
    #endif

    #if UART2_FIFO_EN == 1		/* ����2 */
        /* ʹ�� GPIO TX/RX ʱ�� */
        USART2_TX_GPIO_CLK_ENABLE();
        USART2_RX_GPIO_CLK_ENABLE();
        
        /* ʹ�� USARTx ʱ�� */
        USART2_CLK_ENABLE();	

        /* ����TX���� */
        GPIO_InitStruct.Pin       = USART2_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART2_TX_AF;
        HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* ����RX���� */
        GPIO_InitStruct.Pin = USART2_RX_PIN;
        GPIO_InitStruct.Alternate = USART2_RX_AF;
        HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

        /* ����NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
      
        /* ���ò����ʡ���żУ�� */
        bsp_SetUartParam(USART2,  UART2_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);	// UART_MODE_TX_RX

        CLEAR_BIT(USART2->SR, USART_SR_TC);   /* ���TC������ɱ�־ */
        CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
        SET_BIT(USART2->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
    #else
        /* ʹ�� GPIO TX/RX ʱ�� */
        USART2_TX_GPIO_CLK_ENABLE();
        USART2_RX_GPIO_CLK_ENABLE();
        
        /* ʹ�� USARTx ʱ�� */
        USART2_CLK_ENABLE();	

        /* ����TX���� */
        GPIO_InitStruct.Pin       = USART2_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART2_TX_AF;
        HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* ����RX���� */
        GPIO_InitStruct.Pin = USART2_RX_PIN;
        GPIO_InitStruct.Alternate = USART2_RX_AF;
        HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

        /* ����NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 3);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
      
        /* ���ò����ʡ���żУ�� */
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

        CLEAR_BIT(USART2->SR, USART_SR_TC);   /* ���TC������ɱ�־ */
        CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
        SET_BIT(USART2->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
    #endif

    #if UART6_FIFO_EN == 1			/* USART6 */
        /* ʹ�� GPIO TX/RX ʱ�� */
        USART6_TX_GPIO_CLK_ENABLE();
        USART6_RX_GPIO_CLK_ENABLE();
        
        /* ʹ�� USARTx ʱ�� */
        USART6_CLK_ENABLE();	

        /* ����TX���� */
        GPIO_InitStruct.Pin       = USART6_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART6_TX_AF;
        HAL_GPIO_Init(USART6_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* ����RX���� */
        GPIO_InitStruct.Pin = USART6_RX_PIN;
        GPIO_InitStruct.Alternate = USART6_RX_AF;
        HAL_GPIO_Init(USART6_RX_GPIO_PORT, &GPIO_InitStruct);

        /* ����NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART6_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USART6_IRQn);
        
        /* ���ò����ʡ���żУ�� */
        bsp_SetUartParam(USART6,  UART6_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

        CLEAR_BIT(USART6->SR, USART_SR_TC);   /* ���TC������ɱ�־ */
        CLEAR_BIT(USART6->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
        SET_BIT(USART6->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
    #else
        /* ʹ�� GPIO TX/RX ʱ�� */
        USART6_TX_GPIO_CLK_ENABLE();
        USART6_RX_GPIO_CLK_ENABLE();
        
        /* ʹ�� USARTx ʱ�� */
        USART6_CLK_ENABLE();	

        /* ����TX���� */
        GPIO_InitStruct.Pin       = USART6_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART6_TX_AF;
        HAL_GPIO_Init(USART6_TX_GPIO_PORT, &GPIO_InitStruct);	
        
        /* ����RX���� */
        GPIO_InitStruct.Pin = USART6_RX_PIN;
        GPIO_InitStruct.Alternate = USART6_RX_AF;
        HAL_GPIO_Init(USART6_RX_GPIO_PORT, &GPIO_InitStruct);

        /* ����NVIC the NVIC for UART */   
        HAL_NVIC_SetPriority(USART6_IRQn, 0, 1);
        HAL_NVIC_EnableIRQ(USART6_IRQn);
        
        /* ���ò����ʡ���żУ�� */
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

        CLEAR_BIT(USART6->SR, USART_SR_TC);   /* ���TC������ɱ�־ */
        CLEAR_BIT(USART6->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
        SET_BIT(USART6->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
    #endif
}



/*
*********************************************************************************************************
*	�� �� ��: UartSend
*	����˵��: ��д���ݵ�UART���ͻ�����,�����������жϡ��жϴ�����������Ϻ��Զ��رշ����ж�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* ������ͻ������Ѿ����ˣ���ȴ��������� */
		while (1)
		{
			__IO uint16_t usCount;

			vPortEnterCritical();       //��ֹȫ���ж�
			usCount = _pUart->usTxCount;
			vPortExitCritical();        //ʹ��ȫ���ж�

			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
			else if(usCount == _pUart->usTxBufSize)/* ���������������� */
			{
				if((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0)
				{
					SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);
				}  
			}
		}

		/* �����������뷢�ͻ����� */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		vPortEnterCritical();       //��ֹȫ���ж�
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		vPortExitCritical();        //ʹ��ȫ���ж�
	}

	SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);	/* ʹ�ܷ����жϣ��������գ� */
}

/*
*********************************************************************************************************
*	�� �� ��: UartGetChar
*	����˵��: �Ӵ��ڽ��ջ�������ȡ1�ֽ����� ��������������ã�
*	��    ��: _pUart : �����豸
*			  _pByte : ��Ŷ�ȡ���ݵ�ָ��
*	�� �� ֵ: 0 ��ʾ������  1��ʾ��ȡ������
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite �������жϺ����б���д���������ȡ�ñ���ʱ����������ٽ������� */
	vPortEnterCritical();       //��ֹȫ���ж�
	usCount = _pUart->usRxCount;
	vPortExitCritical();        //ʹ��ȫ���ж�

	/* �������д������ͬ���򷵻�0 */
	if (usCount == 0)	/* �Ѿ�û������ */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* �Ӵ��ڽ���FIFOȡ1������ */

		/* ��дFIFO������ */
		vPortEnterCritical();       //��ֹȫ���ж�
		if (++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		vPortExitCritical();        //ʹ��ȫ���ж�
		return 1;
	}
}

/*
*********************************************************************************************************
*   �� �� ��: UartTxEmpty
*   ����˵��: �жϷ��ͻ������Ƿ�Ϊ�ա�
*   ��    ��:  _pUart : �����豸
*   �� �� ֵ: 1Ϊ�ա�0Ϊ���ա�
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
*	�� �� ��: UartIRQ
*	����˵��: ���жϷ��������ã�ͨ�ô����жϴ�����
*	��    ��: _pUart : �����豸
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
	uint32_t isrflags   = READ_REG(_pUart->uart->SR);
	uint32_t cr1its     = READ_REG(_pUart->uart->CR1);
	uint32_t cr3its     = READ_REG(_pUart->uart->CR3);
    
    UBaseType_t uxSavedInterruptStatus;
	
    /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	/* ��������ж�  */
	if ((isrflags & USART_SR_RXNE) != RESET)
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
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
		/* �ص�����,֪ͨӦ�ó����յ�������,һ���Ƿ���1����Ϣ��������һ����� */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch); /* ���磬����MODBUS����������ֽ��� */
			}
		}
	}

	/* �����ͻ��������ж� */
	if ( ((isrflags & USART_SR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if(_pUart->usTxCount == 0)
		{
			/* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
			//USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);

			/* ʹ�����ݷ�������ж� */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
			SET_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
		}
		else
		{
			_pUart->Sending = 1;
			
			/* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			_pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
			if(++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
	/* ����bitλȫ��������ϵ��ж� */
	if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if(_pUart->usTxCount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
		}
		else
		{
			/* ��������£��������˷�֧ */

			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			_pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
			if(++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
    
    /* �˳��ٽ�� */
	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus); 	
}

/*
*********************************************************************************************************
*	�� �� ��: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler��
*	����˵��: USART�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
    #if 1	/* ����Ҫprintf���ַ�ͨ�������ж�FIFO���ͳ�ȥ��printf�������������� */
        comSendChar(DEBUG_COM, ch);
        
        return ch;
    #else	/* ����������ʽ����ÿ���ַ�,�ȴ����ݷ������ */
        /* дһ���ֽڵ�USART1 */
        USART1->DR = ch;
        
        /* �ȴ����ͽ��� */
        while((USART1->SR & USART_SR_TC) == 0)
        {}
        
        return ch;
    #endif
}

/*
*********************************************************************************************************
*	�� �� ��: fgetc
*	����˵��: �ض���getc��������������ʹ��getchar�����Ӵ���1��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
    #if 1	/* �Ӵ��ڽ���FIFO��ȡ1������, ֻ��ȡ�����ݲŷ��� */
        uint8_t ucData;

        while(comGetChar(DEBUG_COM, &ucData) == 0);

        return ucData;
    #else
        /* �ȴ����յ����� */
        while((USART1->SR & USART_SR_RXNE) == 0)
        {}

        return (int)USART1->DR;
    #endif
}

/* USER CODE BEGIN 1 */
/**
 * @brief Uart_RxBuf_Init     ����ָ����ջ�������ʼ��
 */
void Uart_RxBuf_Init(void)
{
    Uart_Receive.RxBuf = Receive_Buff;                  //��ʼ��ָ����ջ�����
    Uart_Receive.BufSize = RX_BUFF_SIZE;                //��ʼ��ָ����ջ�������С
    Uart_Receive.WriteCtr = 0;                          //��ʼ��ָ�����дָ��
}

/**
 * @brief Uart_RxBuf_Clear     ����ָ����ջ��������
 */
static inline void Uart_RxBuf_Clear(void)
{
    memset(Uart_Receive.RxBuf, '\0', Uart_Receive.BufSize);
    Uart_Receive.WriteCtr = 0;
}

/**
 * @brief Uart_RxBuf_Putin     ����ָ����ջ���������
 * @param uint8_t *_pByte   ������ַ�
 */
void Uart_RxBuf_Putin(uint8_t *_pByte)
{
    *(Uart_Receive.RxBuf + Uart_Receive.WriteCtr) = *_pByte;
    Uart_Receive.WriteCtr ++;
    /* ָ������Ƿ���� */
    if(Uart_Receive.WriteCtr >= Uart_Receive.BufSize)
    {
        Uart_Receive.WriteCtr = 0;
        USART_LOG("Uart receive buff full\n\r");
    }
}

/**
 * @brief Uart_Load_Command     ��������
 * @return int8_t -1������ȡ����  1������ȡ�ɹ�
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
 * @brief SPP_RxBuf_Init     SPPָ����ջ�������ʼ��
 */
void SPP_RxBuf_Init(void)
{
    SPP_Receive.RxBuf = Receive_Buff1;                  //��ʼ��ָ����ջ�����
    SPP_Receive.BufSize = RX_BUFF_SIZE;                //��ʼ��ָ����ջ�������С
    SPP_Receive.WriteCtr = 0;                          //��ʼ��ָ�����дָ��
}

/**
 * @brief SPP_RxBuf_Clear     SPPָ����ջ��������
 */
static inline void SPP_RxBuf_Clear(void)
{
    memset(SPP_Receive.RxBuf, '\0', SPP_Receive.BufSize);
    SPP_Receive.WriteCtr = 0;
}

/**
 * @brief SPP_RxBuf_Putin     SPPָ����ջ���������
 * @param uint8_t *_pByte   ������ַ�
 */
void SPP_RxBuf_Putin(uint8_t *_pByte)
{
    *(SPP_Receive.RxBuf + SPP_Receive.WriteCtr) = *_pByte;
    SPP_Receive.WriteCtr ++;
    /* ָ������Ƿ���� */
    if(SPP_Receive.WriteCtr >= SPP_Receive.BufSize)
    {
        SPP_Receive.WriteCtr = 0;
        USART_LOG("SPP receive buff full\n\r");
    }
}

/**
 * @brief SPP_Load_Command     ��������
 * @return int8_t -1������ȡ����  1������ȡ�ɹ�
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
