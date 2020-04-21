/**
  ******************************************************************************
  * @file    windowing_fn.c
  * @author  Melchor Varela - EA4FRB
  * @brief   Windowing functions for sampled data
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

#include "sample.h"
#include "windowing_fn.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*#define WINDOW_HAMMING	1 */
#define WINDOW_HANNING	1		/* Hanning window provides slightly better results */

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static float gWn[SAMPLE_BLOCK_SIZE];
static uint16_t gu16BlockSize;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


/**
  * @brief Apply the windowing function to the samples array
  *
  * @param  gSampleData
  * @retval None
  */
void Windowing_Calc (uint16_t txSampleData[])
{
  	int ii;

	for (ii = 0; ii < gu16BlockSize; ii++)
  	{
		txSampleData[ii] = (uint16_t) (gWn[ii] * txSampleData[ii]);
	}
}

/**
  * @brief Initializes the windowing function coefficients
  *
  * @param  None
  * @retval None
  */
void Windowing_Init (uint16_t u16BlockSize)
{
  	int ii;
  	gu16BlockSize = u16BlockSize;

  	#ifdef WINDOW_HAMMING
	for (ii = 0; ii < u16BlockSize; ii++)
  	{
		gWn[ii] = 0.54 - 0.46*cos((2.0*M_PI*ii)/(float)u16BlockSize-1);
	}
#endif
#ifdef WINDOW_HANNING
	for (ii = 0; ii < u16BlockSize; ii++)
  	{
		gWn[ii] = (float)(0.5 - 0.5*cos((2.0*M_PI*ii)/(float)u16BlockSize-1));
	}
#endif
#ifdef WINDOW_BLACKMAN
	for (ii = 0; ii < u16BlockSize; ii++)
  	{
		gWn[ii] = 0.426591 - 0.496561 * cos((2*M_PI*ii)/(float)u16BlockSize-1) + 0.076848 * cos((4*PI*ii)/(FLOATING)u16BlockSize-1);
	}
#endif
}

/************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/

