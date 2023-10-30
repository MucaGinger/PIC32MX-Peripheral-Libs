/** Custom libs **/
#include "Tmr.h"

/** Test prototype **/
static void TestFunct(void);

int main(int argc, char** argv)
{
	/* Configuration structure for timeout mode */
	TmrTimeoutConfig_t tmrTimeoutConfig = {
		.bitMode = TMR_BITMODE_32BIT,
		.clkDiv = TMR_CLK_DIV_1,
		.clkSrc = TMR_CLK_SRC_PBCLK,
		.timeUnit = TMR_TIME_UNIT_US
	};

	/* Configure indication pin */
	PIO_ConfigGpioPin(GPIO_RPB2, PIO_TYPE_DIGITAL, PIO_DIR_OUTPUT);

	/* Configure TMR module for timeout mode */
	TMR_ConfigSfrTimeoutMode(&TMR2_MODULE, tmrTimeoutConfig);

	/* Configure a function pointer */
	TMR_SetCallback(&TMR2_MODULE, TestFunct);

	/* Set a timeout period */
	TMR_SetTimerPeriod(&TMR2_MODULE, 100000);

	/* Set pin (clear it after timeout) */
	PIO_ClearPin(GPIO_RPB2);
	PIO_SetPin(GPIO_RPB2);

	/* Start a timeout */
	TMR_StartTimer(&TMR2_MODULE);

	while (1)
	{
		/* Main program execution */
		/* Stop timeout if this is reached before "TestFunct()" */
		TMR_StopTimer(&TMR2_MODULE);
		PIO_ClearPin(GPIO_RPB2);
	}

	return 0;
}

/** Test function **/
static void TestFunct(void)
{
	PIO_TogglePin(GPIO_RPB2);
}