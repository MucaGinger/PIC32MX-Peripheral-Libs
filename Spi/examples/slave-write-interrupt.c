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

	/* SPI module base address set */
	SpiSfr_t *spiSlaveSfr = &SPI1_MODULE;

	/* Configure SPI module */
	SPI_ConfigStandardModeSfr(spiSlaveSfr, spiSlaveConfig);

	/* Test data variables */
	uint32_t slaveRxData[24] =  {0};
	uint32_t slaveTxData[24] =  {0xF1DE5210, 0x12AB49CF, 0xA0015CD7,
								 0x12093472, 0x001100AB, 0xF344BDEF,
								 0xEDC1B33A, 0x93A3B8CE, 0xFFF1FFF1,
								 0xABCDEF00, 0x12345678, 0x21314151,
								 0xF1DE5210, 0x12AB49CF, 0xA0015CD7,
								 0x12093472, 0x001100AB, 0xF344BDEF,
								 0xEDC1B33A, 0x93A3B8CE, 0xFFF1FFF1,
								 0xABCDEF00, 0x12345678, 0x21314151};

	/* Load Slave with data */
	SPI_SlaveWrite(spiSlaveSfr, slaveTxData, 4);

	uint8_t ptrIncr = 1;
    uint8_t msgSize = (sizeof(slaveTxData) / sizeof(uint32_t)) / 4;

	/* Main program execution */
	while (ptrIncr <= msgSize)
	{
        /* Application code... */
		
		/* Re-load Slave with data and read data */
		if (SPI_IsRxDone(spiSlaveSfr, 4))
		{
			SPI_SlaveRead(spiSlaveSfr, slaveRxData + ptrIncr * 4);
			SPI_SlaveWrite(spiSlaveSfr, slaveTxData + ptrIncr * 4, 4);
			ptrIncr++;
		}

        /* Application code... */
	}

	return 0;
}