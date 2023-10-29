/** Custom libs **/
#include "Pio.h"

/** Test variable **/
static volatile uint32_t inputChangeCounter = 0;

/** Test prototype **/
static void TestFunct(void);

int main(int argc, char** argv)
{
	/* Configure signal generator */
	PIO_ConfigGpioPin(GPIO_RPA2, PIO_TYPE_DIGITAL, PIO_DIR_OUTPUT);

	/* Set idle state of input signal */
	PIO_ClearPin(GPIO_RPA2);

	/* Configure input change feature of PIO module */
	PIO_ConfigInputChange(GPIO_RPB4, PIO_CN_NONE);

	/* Configure a function pointer */
	PIO_SetIsrHandler(GPIO_RPB4, TestFunct);

	/* Simulate input signal */
	PIO_SetPin(GPIO_RPA2);
	PIO_ClearPin(GPIO_RPA2);
	PIO_SetPin(GPIO_RPA2);
	PIO_ClearPin(GPIO_RPA2);

	while (1)
	{
		/* 'TestFunct()' is executed from ISR each time a change in input
		   at pin RPA2 is detected */
	}

	return 0;
}

/** Test function **/
static void TestFunct(void)
{
	inputChangeCounter++;
}