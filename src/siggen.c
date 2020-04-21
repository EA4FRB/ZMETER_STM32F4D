/**
  ******************************************************************************
  * @file    siggen.c
  * @author	 Melchor Varela - EA4FRB
  * @brief   Numeric oscillator
  ******************************************************************************
   * @copy
  *
  * <h2><center>&copy; COPYRIGHT 2020 Melchor Varela - EA4FRB </center></h2>
  * Melchor Varela, Madrid, Spain.
  * melchor.varela@gmail.com
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HIGH_RES_SIN		1	/* Comment for low resolution sine wave */

#define DAC_DHR12R1_ADDRESS    0x40007408

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static DAC_InitTypeDef  DAC_InitStructure;
static uint8_t gu8Enabled = 0;

#ifdef HIGH_RES_SIN
static const uint16_t gtu16Sine12bit[] =
{
	2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897,
	2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640,
	3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002,
	4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894,
	3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344,
	3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483,
	2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517,
	1424, 1332, 1242, 1154, 1068, 985, 904, 826, 751, 679,
	610, 545, 484, 426, 373, 323, 278, 237, 201, 169,
	141, 118, 100, 87, 79, 75, 76, 82, 93, 109,
	129, 154, 184, 218, 257, 300, 347, 399, 455, 514,
	577, 644, 714, 788, 865, 944, 1026, 1111, 1198, 1287,
	1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047
};
#else
static const uint16_t gtu16Sine12bit[32] =
{
	2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
	3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
	599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647
};
#endif

/* Private function prototypes -----------------------------------------------*/
static void TIM6_Config(void);
static void DAC_Ch1_SineWaveConfig(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief
  * @param  None
  * @retval None
  */
void SigGen_Init (void)
{
	/* Preconfiguration before using DAC----------------------------------------*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/* DMA1 clock and GPIOA clock enable (to be used with DAC) */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_GPIOA, ENABLE);

	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	/* DAC channel 1 (DAC_OUT1 = PA.4) configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* TIM6 Configuration ------------------------------------------------------*/
	TIM6_Config();

	gu8Enabled = 0;
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void SigGen_Enable (void)
{
	if (gu8Enabled)
		return;
	gu8Enabled = 1;
	DAC_Ch1_SineWaveConfig();
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void SigGen_Disable (void)
{
	gu8Enabled = 0;
	DAC_DeInit();
}

/**
  * @brief  TIM6 Configuration
  * @note   TIM6 configuration is based on CPU @168MHz and APB1 @42MHz
  * @param  None
  * @retval None
  */
static void TIM6_Config(void)
{
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	/* TIM6 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
#ifdef HIGH_RES_SIN
	TIM_TimeBaseStructure.TIM_Period = 10;		// 59659 Hz N=128
#else
	TIM_TimeBaseStructure.TIM_Period = 43;		// 59659 Hz N=32
#endif
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	/* TIM6 TRGO selection */
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	/* TIM6 enable counter */
	TIM_Cmd(TIM6, ENABLE);
}

/**
  * @brief  DAC  Channel2 SineWave Configuration
  * @param  None
  * @retval None
  */
static void DAC_Ch1_SineWaveConfig(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	/* DAC channel1 Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	/* DMA1_Stream5 channel7 configuration **************************************/
	DMA_DeInit(DMA1_Stream5);
	DMA_InitStructure.DMA_Channel = DMA_Channel_7;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12R1_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&gtu16Sine12bit;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = sizeof(gtu16Sine12bit)/sizeof(uint16_t);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);

	/* Enable DMA1_Stream5 */
	DMA_Cmd(DMA1_Stream5, ENABLE);

	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);

	/* Enable DMA for DAC Channel2 */
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}

/******************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/
