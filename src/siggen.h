/**
  ******************************************************************************
  * @file    siggen.h
  * @author  Melchor Varela - EA4FRB
  * @brief   Numeric oscillator
  ******************************************************************************
  * @copy
  *
  * <h2><center>&copy; COPYRIGHT 2020 Melchor Varela - EA4FRB </center></h2>
  * Melchor Varela, Madrid, Spain.
  * melchor.varela@gmail.com
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SIGGEN_H
#define __SIGGEN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

extern void SigGen_Init (void);
extern void SigGen_Enable (void);
extern void SigGen_Disable (void);

#endif /* __SIGGEN_H */
