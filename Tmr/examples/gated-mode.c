/** Custom libs **/
#include "Tmr.h"

/** Test prototype **/
static void TestFunct(void);

int main(int argc, char** argv)
{
	/* Configuration structure for gated mode */
	TmrGatedConfig_t tmrGatedConfig = {
		.bitMode = TMR_BITMODE_32BIT,
		.clkDiv = TMR_CLK_DIV_1,
		.tckPin = T2CK_RPB4,
		.isGateCont = true
	};

	/* Configure signal generator */
	PIO_ConfigGpioPin(GPIO_RPA2, PIO_TYPE_DIGITAL, PIO_DIR_OUTPUT);

	/* Configure TMR module for gated mode */
	TMR_ConfigSfrGatedMode(&TMR2_MODULE, tmrGatedConfig);

	/* Configure a function pointer (optional) */
	TMR_SetCallback(&TMR2_MODULE, TestFunct);

	/* Initial state of signal */
	PIO_ClearPin(GPIO_RPA2);

	/* Start measuring a signal */
	TMR_StartTimer(&TMR2_MODULE);

	/* Simulate a signal */
	PIO_SetPin(GPIO_RPA2);
	TMR_DelayUs(1000);
	PIO_ClearPin(GPIO_RPA2);

	/* Simulate absence of a signal */
	TMR_DelayUs(1000);

	/* Signal reappears */
	PIO_SetPin(GPIO_RPA2);
	TMR_DelayUs(1000);
	PIO_ClearPin(GPIO_RPA2);

	/* Simulate absence of a signal */
	TMR_DelayUs(1000);

	/* Stop measuring a signal */
	TMR_StopTimer(&TMR2_MODULE);

	/* Signal reappears (but doesn't get measured) */
	PIO_SetPin(GPIO_RPA2);
	TMR_DelayUs(1000);
	PIO_ClearPin(GPIO_RPA2);

	/* Read result of measurement */
	double tmrPeriod = TMR_ReadTimerPeriod(&TMR2_MODULE);

	return 0;
}


/** Test function **/
static void TestFunct(void)
{
	/* Toggle a pin or process some data */
}