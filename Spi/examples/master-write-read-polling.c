/** Custom libs **/
#include "Spi.h"

int main(int argc, char** argv)
{
	/* Master device SPI settings */
	SpiStandardConfig_t spiMasterConfig = {
		.pinSelect = {
			.sdiPin = SDI2_RPB13,
			.sdoPin = SDO2_RPB2,
			.ss1Pin = GPIO_RPB10
		},
		.isMasterEnabled = true,
		.frameWidth = SPI_WIDTH_8BIT,
		.clkMode = SPI_CLK_MODE_0,
		.sckFreq = 10000000
	};

	/* SPI module base address set */
	SpiSfr_t *spiMasterSfr = &SPI2_MODULE;

	/* Configure SPI module */
	SPI_ConfigStandardModeSfr(spiMasterSfr, spiMasterConfig);

	/* Test data variables */
	uint8_t masterRxData[24] = {0};
	uint8_t masterTxData[24] = {0xAB, 0x00, 0xCC, 0x00,
								0xFF, 0xAB, 0x12, 0x21,
								0xA0, 0x12, 0x00, 0xF1,
								0xAB, 0x00, 0xCC, 0x00,
								0xFF, 0xAB, 0x12, 0x21,
								0xA0, 0x12, 0x00, 0xF1};

	/* Configure which Slave will be addressed */
	SPI_EnableSsState(spiMasterConfig.pinSelect.ss1Pin);

	/* Polling-based SPI read and write */
	SPI_MasterReadWrite(spiMasterSfr, masterRxData, masterTxData, 24);

	while(1)
	{
		/* Main program execution */
	}

	return 0;
}