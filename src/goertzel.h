/**
  ******************************************************************************
  * @file    goertzel.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GOERTZEL_H__
#define __GOERTZEL_H__

/* Includes ------------------------------------------------------------------*/
#include <math.h>

#include "complex.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void Goertzel_Init (uint16_t u16BlockSize, uint32_t u32Freq, uint32_t u32SampleRate);
extern void Goertzel_Calc (uint16_t txSampleData[], complex double *pvect);

#endif	/* __GOERTZEL_H__ */

/************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/
