/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* ADC1 init function */
void MX_ADC1_Init(void)
{
    uint16_t timeout = 1000;

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    /**ADC1 GPIO Configuration
    PA1   ------> ADC1_IN1
    */
    GPIO_InitStruct.Pin = ADC_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ADC_GPIO, &GPIO_InitStruct);

    /* ADC1 interrupt Init */
    NVIC_SetPriority(ADC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
    NVIC_EnableIRQ(ADC_IRQn);

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */
    /** Common config
    */
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;                        //12λ�ֱ���
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;                   //�Ҷ��뷽ʽ
    ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;              //ʧ��ɨ��ģʽ
    LL_ADC_Init(ADC1, &ADC_InitStruct);
    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;              //����Զ�����
    ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;         //ʧ��ADC�鳣������������
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;     //ʧ��ADC��˳��ɼ�
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;           //ʧ��ADC����ת��ģʽ
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;            //�ر�DMA����
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
    LL_ADC_REG_SetFlagEndOfConversion(ADC1, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);
    ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV8;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
    /** Configure Regular Channel
    */
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_15CYCLES);
    
    
    /* USER CODE BEGIN ADC1_Init 2 */
    LL_ADC_EnableIT_EOCS(ADC1);
    LL_ADC_Enable(ADC1);
    for(;;)
    {
        if(LL_ADC_IsEnabled(ADC1) == SET) 
        {
            ADC_LOG("ADC enable succeed");
            break;
        }
        if(! timeout)
        {
            ADC_LOG("ADC enable failed");
            break;
        }
        timeout--;
    }
    /* USER CODE END ADC1_Init 2 */

}

/* USER CODE BEGIN 1 */
uint16_t ADC_OneShot_Read(void)
{
    uint16_t temp;
    uint8_t i;
    for(i=0;i<4;i++)
    {
        LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
        LL_ADC_REG_StartConversionSWStart(ADC1);
        while(LL_ADC_IsEnabledIT_EOCS(ADC1) != SET);
        LL_ADC_ClearFlag_EOCS(ADC1);
        LL_ADC_REG_SetFlagEndOfConversion(ADC1,LL_ADC_REG_FLAG_EOC_SEQUENCE_CONV);
        temp += LL_ADC_REG_ReadConversionData12(ADC1);

    }
    
    return temp>>2;
}


/* USER CODE END 1 */

