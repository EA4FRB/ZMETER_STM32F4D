/**
  ******************************************************************************
  * @file    windowing_fn.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WINDOWING_FN_H__
#define __WINDOWING_FN_H__

/* Includes ------------------------------------------------------------------*/
#include <math.h>

#include "sample.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

extern void Windowing_Calc (uint16_t gSampleData[]);
extern void Windowing_Init (uint16_t u16BlockSize);

#endif	/* __WINDOWING_FN_H__ */

/************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/

