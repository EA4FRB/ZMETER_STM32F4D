/**
  ******************************************************************************
  * @file    sample.h
  * @author  Melchor Varela - EA4FRB
  * @brief   Analog to digital data acquisition
  ******************************************************************************
  * @copy
  *
  * <h2><center>&copy; COPYRIGHT 2020 Melchor Varela - EA4FRB </center></h2>
  * Melchor Varela, Madrid, Spain.
  * melchor.varela@gmail.com
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SAMPLE_H__
#define __SAMPLE_H__

/* Includes ------------------------------------------------------------------*/
#include <math.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* ((2xSAMPLE_BLOCK_SIZE)*FREQ)/FSAMPLE Shall be integer */

#define SAMPLING_RATE				218750
#define MEASUREMENT_FREQ			59659
#define SAMPLE_BLOCK_SIZE			(110)

#define SAMPLE_DUMMY_READS			1		/* Drops first ADC reads */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Configure necessary system resources.
  *
  * @retval None
  */
extern void Sample_Init (uint16_t u16BlockSize);

/**
  * @brief  Performs the ADC data acquisition
  *
  * @param  voltage
  * @param  current
  * @retval None
  */
extern void Sample_Take(uint16_t ch1[], uint16_t ch2[] );

#endif	 /* __SAMPLE_H__ */

/************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/

