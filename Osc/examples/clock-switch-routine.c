/** Custom libs **/
#include "Osc.h"
#include "Pio.h"

int main(int argc, char** argv)
{
	/* Oscillator initial configuration parameters */
	OscConfig_t oscConfig = {
		.oscSource = OSC_COSC_FRCPLL,
		.sysFreq = 20000000,
		.pbFreq = 20000000
	};

	uint32_t cntr, sysFreq1, sysFreq2, sysFreq3;

	/* Configure indicating pin */
	PIO_ClearPin(GPIO_RPB4);
	PIO_ConfigGpioPin(GPIO_RPB4, PIO_TYPE_DIGITAL, PIO_DIR_OUTPUT);

	/* Initial oscillator configuration */
	OSC_ConfigOsc(oscConfig);
	sysFreq1 = OSC_GetSysFreq();

	/* Toggle pin */
	cntr = 1000;
	while (cntr--)
	{
		PIO_TogglePin(GPIO_RPB4);
	}

	PIO_ClearPin(GPIO_RPB4);

	/* Re-configure oscillator first time */
	oscConfig.sysFreq = 1000000;
	OSC_ConfigOsc(oscConfig);
	sysFreq2 = OSC_GetSysFreq();

	/* Toggle pin */
	cntr = 100;
	while (cntr--)
	{
		PIO_TogglePin(GPIO_RPB4);
	}

	/* Re-configure oscillator second time */
	oscConfig.oscSource = OSC_COSC_LPRC;
	OSC_ConfigOsc(oscConfig);
	sysFreq3 = OSC_GetSysFreq();

	/* Toggle pin */
	cntr = 10;
	while (cntr--)
	{
		PIO_TogglePin(GPIO_RPB4);
	}

	while (1) {}

	return 0;
}