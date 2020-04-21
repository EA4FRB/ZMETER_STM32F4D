/**
  ******************************************************************************
  * @file    sample.c
  * @author  Melchor Varela - EA4FRB
  * @brief   Analog to digital data acquisition
  ******************************************************************************
  * @copy
  *
  * <h2><center>&copy; COPYRIGHT 2020 Melchor Varela - EA4FRB </center></h2>
  * Melchor Varela, Madrid, Spain.
  * melchor.varela@gmail.com
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "sample.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC_CCR_ADDRESS    ((uint32_t)0x40012308)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t gu16BlockSize;

/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration();
static void GPIO_Configuration(void);
static void LowNoiseContext (int enter);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure necessary system resources.
  *
  * @retval None
  */
void Sample_Init (uint16_t u16BlockSize)
{
  	gu16BlockSize = u16BlockSize;

  	/* System clocks configuration ---------------------------------------------*/
  	RCC_Configuration();

  	/* GPIO configuration ------------------------------------------------------*/
  	GPIO_Configuration();
}

/**
  * @brief  Performs the ADC data acquisition
  *
  * SysCLK: 168MHz
  * APB2: 84MHz
  * ADC Clock: 21Mhz
  * Tconv = Sampling time + 12 cycles
  *
  * Tconv = 84 + 12 = 218.750sps (60Khz)
  *
  * @param  ch1
  * @param  ch2
  * @retval None
  */
void Sample_Take(uint16_t ch1[], uint16_t ch2[] )
{
	int ii;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	__IO uint32_t ADCSamples[SAMPLE_BLOCK_SIZE+SAMPLE_DUMMY_READS];

	/* DMA2 stream0 configuration ----------------------------------------------*/
	DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CCR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCSamples;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = gu16BlockSize+SAMPLE_DUMMY_READS;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	/* Enable DMA2 stream0 */
	DMA_Cmd(DMA2_Stream0, ENABLE);

	/* ADC common init --------------------------------------------------------*/
	ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_2;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel1 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* ADC2 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC2, &ADC_InitStructure);

	/* ADC2 regular channel2 configuration */
	ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 1, ADC_SampleTime_84Cycles);

	ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC2 */
	ADC_Cmd(ADC2, ENABLE);

	/* Low noise context enter */
	LowNoiseContext(1);

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConv(ADC1);

	while (DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0)==RESET)
	{;}

	/* Clear DMA1 channel1 transfer complete flag */
	DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);

	ADC_Cmd(ADC1, DISABLE);
	ADC_Cmd(ADC2, DISABLE);
	ADC_DMACmd(ADC1, DISABLE);
	DMA_DeInit(DMA2_Stream0);

	/* Low noise context exit */
	LowNoiseContext(0);

	/* Discard first sample: first ADC2 sample is wrong */
	for (ii=0;ii<gu16BlockSize;ii++)
	{
		ch1[ii] = (ADCSamples[ii+SAMPLE_DUMMY_READS]&0xffff);
		ch2[ii] = (ADCSamples[ii+SAMPLE_DUMMY_READS]>>16);
	}
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
static void RCC_Configuration(void)
{
	/* Enable peripheral clocks ------------------------------------------------*/
	/* Enable DMA1 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable ADC1, ADC2 and GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 , ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void GPIO_Configuration(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	/* Configure PA.01 (ADC Channel1) & PA.02 (ADC Channel2) as analog input -------------------------*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief Systick interrupt during measurements would cause noise -probably due to buttons scan.
  * Disables temporarily the systick interrupt during measurements.
  * @param  None
  * @retval None
  */
static void LowNoiseContext (int enter)
{
#if 1
	if (enter)
	{
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
//		FLASH_PrefetchBufferCmd(DISABLE);
//		FLASH_DataCacheCmd(DISABLE);
	}
	else
	{
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
//		FLASH_PrefetchBufferCmd(ENABLE);
//		FLASH_DataCacheCmd(ENABLE);
	}
#endif
}


/************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/
