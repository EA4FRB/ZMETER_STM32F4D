/**
  ******************************************************************************
  * @file    complex.h
  * @author  Melchor Varela - EA4FRB
  * @brief	 Complex math utility functions
  ******************************************************************************
  * @copy
  *
  * <h2><center>&copy; COPYRIGHT 2020 Melchor Varela - EA4FRB </center></h2>
  * Melchor Varela, Madrid, Spain.
  * melchor.varela@gmail.com
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMPLEX_H__
#define __COMPLEX_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
	double fMag;
  	float fPhase;
} TVECTOR_POLAR;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define RAD2DEG(RAD)	(((RAD)/M_PI)*180.0)
#define DEG2RAD(DEG) 	((DEG)*((M_PI)/(180.0)))

#define complex _Complex
#define _Complex_I 1.0fi
#define I _Complex_I

/* Exported functions ------------------------------------------------------- */
extern void Polar2Rect(TVECTOR_POLAR vpPolar, complex double *pxRect);
extern void Rect2Polar(complex double xRect, TVECTOR_POLAR *pvPolar);
extern double CAbs(complex double xOp);
extern complex double CSqrt(complex double cxX);
extern complex double CPow(complex double cxOp, double dfPow);
extern complex double CLog(complex double cxOp);
extern complex double CAtanh(complex double cxOp);

#endif	 /* __COMPLEX_H__ */

/************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/

