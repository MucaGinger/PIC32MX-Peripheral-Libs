/** Custom libs **/
#include "Spi.h"

int main(int argc, char** argv)
{
	/* Slave device SPI settings */
	SpiStandardConfig_t spiSlaveConfig = {
		.pinSelect = {
			.sdiPin = SDI1_RPB8,
			.sdoPin = SDO1_RPB11,
			.ss1Pin = SS1_RPB7
		},
		.isMasterEnabled = false,
		.frameWidth = SPI_WIDTH_8BIT,
		.clkMode = SPI_CLK_MODE_0
	};

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
	SpiSfr_t *spiSlaveSfr = &SPI1_MODULE;
	SpiSfr_t *spiMasterSfr = &SPI2_MODULE;

	/* Configure both SPI modules */
	SPI_ConfigStandardModeSfr(spiSlaveSfr, spiSlaveConfig);
	SPI_ConfigStandardModeSfr(spiMasterSfr, spiMasterConfig);

	/* Test data variables */
	uint32_t masterRxData[24] = {0};
	uint32_t masterTxData[24] = {0xABACADAF, 0x00000000, 0xCCEEDDEE,
								 0x00110011, 0xFFF1FFF1, 0xABCDEF00,
								 0x12345678, 0x21314151, 0xA0A0A0A0,
								 0x12BB32CC, 0x00000000, 0xF1EBC43A,
								 0xABACADAF, 0x00000000, 0xCCEEDDEE,
								 0x00110011, 0xFFF1FFF1, 0xABCDEF00,
								 0x12345678, 0x21314151, 0xA0A0A0A0,
								 0x12BB32CC, 0x00000000, 0xF1EBC43A};
	uint32_t slaveTxData[24] =  {0xF1DE5210, 0x12AB49CF, 0xA0015CD7,
								 0x12093472};

	/* Load Slave with data */
	SPI_SlaveWrite(spiSlaveSfr, slaveTxData, 4);

	/* Configure which Slave will be addressed */
	SPI_EnableSsState(spiMasterConfig.pinSelect.ss1Pin);

	/* Interrupt-based SPI write */
	SPI_MasterWrite(spiMasterSfr, masterRxData, masterTxData, 24);

	while(1)
	{
		/* Main program execution */
		
		if( SPI_IsSpiBusy(spiMasterSfr) )
		{
			/* RX data ready */
		}
	}

	return 0;
}