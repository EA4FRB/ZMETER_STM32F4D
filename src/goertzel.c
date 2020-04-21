/**
  ******************************************************************************
  * @file    goertzel.c
  * @author  Melchor Varela - EA4FRB
  * @brief   Goertzel algorithm
  ******************************************************************************
  * @copy
  *
  * <h2><center>&copy; COPYRIGHT 2020 Melchor Varela - EA4FRB </center></h2>
  * Melchor Varela, Madrid, Spain.
  * melchor.varela@gmail.com
 */

/** @addtogroup SARK
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "goertzel.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static double gfCoeff;
static double gfQ1;
static double gfQ2;
static double gfSine;
static double gfCosine;
static uint16_t gu16BlockSize;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Initializes Goertzel algorithm coefficients
  *
  * @param  None
  * @retval None
  */
void Goertzel_Init (uint16_t u16BlockSize, uint32_t u32Freq, uint32_t u32SampleRate)
{
	int	iK;
	double fN;
	double fOmega;

  	gu16BlockSize = u16BlockSize;
  	fN = (double) u16BlockSize;
  	iK = (int) (0.5 + ((fN * (double)u32Freq) / (double)u32SampleRate));
  	fOmega = (double)((2.0 * M_PI * iK) / fN);
  	gfSine = (double)sin(fOmega);
  	gfCosine = (double)cos(fOmega);
  	gfCoeff = (double)(2.0 * gfCosine);

  	gfQ2 = 0;
  	gfQ1 = 0;
}

/**
  * @brief Performs the Goertzel algorithm on sampled data.
  * Returns magnitude and phase.
  *
  * @param  txSampleData: data samples
  * @param  pvect: returns the complex vector
  * @retval None
  */
void Goertzel_Calc (uint16_t txSampleData[], complex double *pvect)
{
	complex double vect;
  	uint16_t u16Idx;

  	gfQ2 = 0;
  	gfQ1 = 0;

	/* Process the samples */
	for (u16Idx = 0; u16Idx < gu16BlockSize; u16Idx++)
	{
		double Q0;
		Q0 = gfCoeff * gfQ1 - gfQ2 + (double) txSampleData[u16Idx];
		gfQ2 = gfQ1;
		gfQ1 = Q0;
	}
	/* Do the "basic Goertzel" processing. */
  	__real__ vect = (gfQ1 - gfQ2 * gfCosine);
  	__imag__ vect = (gfQ2 * gfSine);

  	if (pvect)
  		*pvect = vect;
}

/************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/


