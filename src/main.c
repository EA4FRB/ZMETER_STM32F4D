/**
  ******************************************************************************
  * @file    main.c
  * @author  Melchor Varela - EA4FRB
  * @brief	 Impedance analyzer main program
  ******************************************************************************
  * @copy
  *
  * <h2><center>&copy; COPYRIGHT 2020 Melchor Varela - EA4FRB </center></h2>
  * Melchor Varela, Madrid, Spain.
  * melchor.varela@gmail.com
 */

/* Includes */
#include <stdio.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "usbd_cdc_core.h"
#include "sample.h"
#include "siggen.h"
#include "windowing_fn.h"
#include "goertzel.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "complex.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_Z_MAG			999999.9
#define NUM_AVG				8
#define REFERENCE_R			4740.0		/* Adjust to the actual implemented value */

#define LIMIT_MAX_C			999999.99
#define LIMIT_MAX_L			999999.99
#define LIMIT_MIN_R			0.1
#define LIMIT_MIN_X			1.0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment = 4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
static __IO uint32_t TimingDelay;

static const char gszWelcome[] = "\n\r*** Z Meter for STM32F4 ***\n\r\n\r";

/* These are external variables imported from CDC core to be used for IN
 * transfer management. */
extern uint8_t APP_Rx_Buffer[]; /* Write CDC received data in this buffer.
                                 * These data will be sent over USB IN endpoint
                                 * in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;  /* Increment this pointer or roll it back to
                                 * start address when writing received data in
                                 * the buffer APP_Rx_Buffer. */

/* Private function prototypes -----------------------------------------------*/
static void MeasureInit (void);
static void Measure (complex double *pvect_ch1, complex double *pvect_ch2);
static void MeasureZ (complex double *pZ);
static void MeasureVector (TVECTOR_POLAR *pch1, TVECTOR_POLAR *pch2);
void Delay(__IO uint32_t nTime);
static int USB_Send (char data[], uint8_t len);
static int CheckButton (void);
static void CalcLs (uint32_t freq, complex double zs, double *pLs);
static void CalcCs (uint32_t freq, complex double zs, double *pLs);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Main program
  *
  * @param  None
  * @retval None
  */
int main(void)
{
	int ii = 0;

	USBD_Init(&USB_OTG_dev,
		#ifdef USE_USB_OTG_HS
		  USB_OTG_HS_CORE_ID,
		#else
		  USB_OTG_FS_CORE_ID,
		#endif
		  &USR_desc,
		  &USBD_CDC_cb,
		  &USR_cb);

	/* Setup 1ms systick interrupts */
	if (SysTick_Config(SystemCoreClock / 1000))
	{
	  /* Capture error */
	  while (1);
	}
	/**
	*  IMPORTANT NOTE!
	*  The symbol VECT_TAB_SRAM needs to be defined when building the project
	*  if code has been located to RAM and interrupts are used.
	*  Otherwise the interrupt table located in flash will be used.
	*  See also the <system_*.c> file and how the SystemInit() function updates
	*  SCB->VTOR register.
	*  E.g.  SCB->VTOR = 0x20000000;
	*/
	/* Initialize LEDs */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);

	/* Turn on LEDs */
	STM_EVAL_LEDOn(LED3);
	STM_EVAL_LEDOn(LED4);
	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED6);

	/* Init measurement engine */
	MeasureInit();

	/* Welcome prompt */
	Delay(1000);
	USB_Send(gszWelcome, strlen(gszWelcome));

	/* Infinite loop */
	while (1)
	{
		if (CheckButton())
		{
			complex double z;
			TVECTOR_POLAR vZ;
			char text[100];
			double cs, ls;

			MeasureZ(&z);
			CalcCs(MEASUREMENT_FREQ, z, &cs);
			CalcLs(MEASUREMENT_FREQ, z, &ls);
			Rect2Polar(z, &vZ);

			sprintf(text, "%.2f<%.2f, R:%.2f, X:%.2f, Cs:%.2f, Ls:%.2f\n\r", vZ.fMag, RAD2DEG(vZ.fPhase), __real__ z, __imag__ z, cs, ls);
			USB_Send(text, strlen(text));
		}
		/* Other stuff */
		ii++;
		if (ii&1)
			STM_EVAL_LEDOn(LED6);
		else
			STM_EVAL_LEDOff(LED6);
	}
}


/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}

/**
  * @brief Initialize measurements
  *
  * @param  None
  * @retval None
  */
static void MeasureInit (void)
{
	/* Detectors */
	Sample_Init(SAMPLE_BLOCK_SIZE);
	Windowing_Init(SAMPLE_BLOCK_SIZE);
	Goertzel_Init(SAMPLE_BLOCK_SIZE, MEASUREMENT_FREQ, SAMPLING_RATE);

	/* Turn on signal generator */
	SigGen_Init();
	SigGen_Enable();
}

/**
  * @brief Perform measurements
  *
  * @param  None
  * @retval None
  */
static void Measure (complex double *pvect_ch1, complex double *pvect_ch2)
{
	uint16_t ch1[SAMPLE_BLOCK_SIZE];
	uint16_t ch2[SAMPLE_BLOCK_SIZE];
	complex double vect_ch1, vect_ch2;

	/* Sampling */
	Sample_Take(ch1, ch2);

	/* Signal processing */
	Windowing_Calc(ch1);
	Windowing_Calc(ch2);
	Goertzel_Calc(ch1, &vect_ch1);
	Goertzel_Calc(ch2, &vect_ch2);

	if (pvect_ch1)
		*pvect_ch1 = vect_ch1;
	if (pvect_ch2)
		*pvect_ch2 = vect_ch2;
}

/**
  * @brief
  *
  * @param  None
  * @retval None
  */
static void MeasureZ (complex double *pZ)
{
	complex double vr;
	complex double vm;
	complex double z = 0;
	int ii;

	for (ii = 0; ii < NUM_AVG; ii++)
	{
		Measure (&vr, &vm);
		/* Derives impedance */
		if (vr==vm)
			z += 99999999.99;
		else
			z += REFERENCE_R * vm / (vr-vm);
	}
	z = z / (double)NUM_AVG;

	/* Outputs the value */
	if (pZ)
		*pZ = z;
}

/**
  * @brief
  *
  * @param  None
  * @retval None
  */
static void MeasureVector (TVECTOR_POLAR *pch1, TVECTOR_POLAR *pch2)
{
	complex double ch1;
	complex double ch2;

	Measure (&ch1, &ch2);
	if (pch1)
		Rect2Polar(ch1, pch1);
	if (pch2)
		Rect2Polar(ch2, pch2);
}

/**
  * @brief
  *
  * @param  None
  * @retval None
  */
static int USB_Send (char data[], uint8_t len)
{
	int ii;

	for (ii = 0; ii < len; ii++)
	{
		APP_Rx_Buffer[APP_Rx_ptr_in] = data[ii];
		if (++APP_Rx_ptr_in == APP_RX_DATA_SIZE)
			APP_Rx_ptr_in = 0;
	}
	return len;
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

/**
  * @brief  Checks if button is pressed.
  * @param  None
  * @retval None
  */
static int CheckButton (void)
{
	int ii;

	if (!STM_EVAL_PBGetState(BUTTON_USER))
		return 0;
	/* Debounce */
	for (ii = 0; ii < 5; ii++)
	{
		if (!STM_EVAL_PBGetState(BUTTON_USER))
			return 0;
		Delay(10);
	}
	/* Wait release */
	for (ii = 0; ii < 100; ii++)
	{
		if (!STM_EVAL_PBGetState(BUTTON_USER))
			break;
		Delay(10);
	}
	return 1;
}

/**
  * @brief Calculates inductance
  *
  * @param i32Freq	Frequency
  * @param fX		Reactance
  * @retval Inductance in uH
  */
static void CalcLs (uint32_t freq, complex double zs, double *pLs)
{
	double dfLs;

	dfLs = (__imag__ zs)/(double)(2.0*M_PI*((double)freq/1000000.0));

	if (fabs(dfLs) > LIMIT_MAX_L)
		dfLs = LIMIT_MAX_L;

	if (pLs)
		*pLs = dfLs;
}

/**
  * @brief Calculates capacitance
  *
  * @param i32Freq	Frequency
  * @param fX		Reactance
  * @retval Capacitance in pF
  */
static void CalcCs (uint32_t freq, complex double zs, double *pLs)
{
	double dfCs;

	if (fabs(__imag__ zs) > LIMIT_MIN_X)
		dfCs = ((double)(-1000000.0)/(double)(__imag__ zs*2.0*M_PI*((double)freq)/1000000.0));
	else
		dfCs = -LIMIT_MAX_C;

	if (fabs(dfCs) > LIMIT_MAX_C)
		dfCs = -LIMIT_MAX_C;

	if (pLs)
		*pLs = dfCs;
}

/************* (C) COPYRIGHT 2020 Melchor Varela - EA4FRB *****END OF FILE****/


