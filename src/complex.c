/**
  ******************************************************************************
  * @file    complex.c
  * @author  Melchor Varela - EA4FRB
  * @brief	 Complex math utility functions
  ******************************************************************************
  * @copy
  *
  * <h2><center>&copy; COPYRIGHT 2020 Melchor Varela - EA4FRB </center></h2>
  * Melchor Varela, Madrid, Spain.
  * melchor.varela@gmail.com
 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

#include "complex.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief Calculates the modulus of a complex number
  *
  * @param  xOp:  Operand
  * @retval result
  */
double CAbs(complex double xOp)
{
	return sqrt((__real__ xOp * __real__ xOp)+(__imag__ xOp * __imag__ xOp));
}

/**
  * @brief
  *
  * @brief Convert polar to rectangular
  *
  * @param pvpPolar	number in polar form
  * @param pxRect	number in rectangular form
  * @retval None
  */
void Polar2Rect(TVECTOR_POLAR vpPolar, complex double *pxRect)
{
	complex double xRect;

	__real__ xRect =  (vpPolar.fMag * cos(vpPolar.fPhase));
	__imag__ xRect =  (vpPolar.fMag * sin(vpPolar.fPhase));

	if (pxRect)
		*pxRect = xRect;
}

/**
  * @brief Convert rectangular to polar
  *
  * @param pxRect	number in rectangular form
  * @param pvpPolar	number in polar form
  * @retval None
  */
void Rect2Polar(complex double cxRect, TVECTOR_POLAR *pvpPolar)
{
	TVECTOR_POLAR vpPolar;

	vpPolar.fMag = CAbs(cxRect);

	vpPolar.fPhase = (float)(atan2(__imag__ cxRect, __real__ cxRect));

	if (pvpPolar)
		*pvpPolar = vpPolar;
}

/**
  * @brief Square root of a complex number
  *
  * @param cxOp
  * @retval result
  */
complex double CSqrt(complex double cxOp)
{
	complex double cxRes;
	double radius, angle;

	radius = sqrt((__real__ cxOp * __real__ cxOp) + (__imag__ cxOp * __imag__ cxOp));
	if (radius == 0)
		angle = 0;
	else
		angle = atan2(__imag__ cxOp, __real__ cxOp);
	__real__ cxRes = sqrt(radius) * cos (angle/2);
	__imag__ cxRes = sqrt(radius) * sin (angle/2);

	return cxRes;
}

/**
  * @brief Power of a complex number
  *
  * @param cxOp
  * @retval result
  */
complex double CPow(complex double cxOp, double dfPow)
{
	complex double cxRes;
	double radius, angle;

	radius = sqrt((__real__ cxOp * __real__ cxOp) + (__imag__ cxOp * __imag__ cxOp));
	if (radius == 0)
		angle = 0;
	else
		angle = atan2(__imag__ cxOp, __real__ cxOp);
	__real__ cxRes = pow(radius, dfPow) * cos (dfPow * angle);
	__imag__ cxRes = pow(radius, dfPow) * sin (dfPow * angle);

	return cxRes;
}

/**
  * @brief Log (natural) of a complex number
  *
  * @param cxOp
  * @retval result
  */
complex double CLog(complex double cxOp)
{
	complex double cxRes;

	__real__ cxOp = __real__ cxOp + 1e-38;
	__real__ cxRes = log(sqrt(__real__ cxOp * __real__ cxOp + __imag__ cxOp * __imag__ cxOp));
	__imag__ cxRes = atan2(__imag__ cxOp, __real__ cxOp);

	return cxRes;
}

/**
  * @brief Atanh of a complex number
  * See http://live.boost.org/doc/libs/1_34_0/doc/html/boost_math/inverse_complex.html
  * @param cxOp
  * @retval result
  */
complex double CAtanh(complex double cxOp)
{
	complex double cxRes;
	complex double cxOne = 1.0f+0.0f*_Complex_I;
	complex double cxTwo = 2.0f+0.0f*_Complex_I;

	cxRes = (CLog(cxOne+cxOp)-CLog(cxOne-cxOp))/cxTwo;
	return cxRes;
}

/************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/

