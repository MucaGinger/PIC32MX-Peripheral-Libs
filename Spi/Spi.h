#ifndef SPI_H
#define	SPI_H


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

/** Standard libs **/
#include <stdio.h>

/** Custom libs **/
#include "Spi_sfr.h"
#include "Pio.h"
#include "Osc.h"

/******************************************************************************/
/*---------------------------------Macros-------------------------------------*/
/******************************************************************************/

/********************User-defined interrupt vector priority********************/

/* NOTE: IPL = 0 means interrupt disabled. ISR_IPL level must equal ICX_IPL */
/* NOTE: Vectors of same priority and sub-priority are services in their
 *       natural order */

/* User-defined (sub)priority levels (IPL: 0-7, ISL: 0-3) */
#define SPI1_ISR_IPL    IPL1SOFT
#define SPI1_ICX_IPL    1
#define SPI1_ICX_ISL    0

#define SPI2_ISR_IPL    IPL1SOFT
#define SPI2_ICX_IPL    1
#define SPI2_ICX_ISL    0

/******************************************************************************/

/******************************************************************************/
/*----------------------------Enumeration Types-------------------------------*/
/******************************************************************************/

/** Type for ISR function pointer configuration **/
typedef enum {
    ISR_SPI_MODE_0 = 0,
    ISR_SPI_MODE_1 = 1
} IsrSpiMode_t;


/** Types for SFR bit-field values **/
typedef enum {
    SPI_CLK_MODE_0 = 1,
    SPI_CLK_MODE_1 = 0,
    SPI_CLK_MODE_2 = 2,
    SPI_CLK_MODE_3 = 3
} SpiClkMode_t;

typedef enum {
    SPI_WIDTH_8BIT = 0,
    SPI_WIDTH_16BIT = 1,
    SPI_WIDTH_32BIT = 2
} SpiFrameWidth_t;

typedef enum {
    SPI_WIDTH_16DATA_16CH = 0,
    SPI_WIDTH_16DATA_32CH = 1,
    SPI_WIDTH_32DATA_32CH = 2,
    SPI_WIDTH_24DATA_32CH = 3
} SpiAudioDataWidth_t;

typedef enum {
    SPI_AUDMOD_I2S = 0,
    SPI_AUDMOD_LEFT_JUSTIFIED = 1,
    SPI_AUDMOD_RIGHT_JUSTIFIED = 2,
    SPI_AUDMOD_PMC_DSP = 3
} SpiAudioProtocolMode_t;

typedef enum {
    SPI_STXISEL_INTR_WHEN_LAST_TRANSFER_DONE = 0,
    SPI_STXISEL_INTR_WHEN_BUFF_EMPTY = 1,
    SPI_STXISEL_INTR_WHEN_BUFF_HALF_EMPTY = 2,
    SPI_STXISEL_INTR_WHEN_BUFF_NOT_FULL = 3
} SpiTxBufferIntrMode_t;

typedef enum {
    SPI_SRXISEL_INTR_WHEN_LAST_WORD_READ = 0,
    SPI_SRXISEL_INTR_WHEN_BUFF_NOT_EMPTY = 1,
    SPI_SRXISEL_INTR_WHEN_BUFF_HALF_FULL = 2,
    SPI_SRXISEL_INTR_WHEN_BUFF_FULL = 3
} SpiRxBufferIntrMode_t;

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/* Used with SsGetState() and SsSetState() */
typedef struct {
    uint32_t    pioA;
    uint32_t    pioB;
} SpiSsState_t;

/* SPI pin assignment related codes */
typedef struct {
    uint32_t    sdiPin;
    uint32_t    sdoPin;
    uint32_t    ss1Pin;     // Use SS1 in Slave mode
    uint32_t    ss2Pin;
    uint32_t    ss3Pin;
    uint32_t    ss4Pin;
    uint32_t    ss5Pin;
} SpiPin_t;

/* SPI related operation settings */
typedef struct {
    /* Standard features */
    bool                isMasterEnabled;
    SpiPin_t              pinSelect;
    SpiFrameWidth_t       frameWidth;
    SpiClkMode_t          clkMode;
    uint32_t              sckFreq;      // Master mode only
} SpiStandardConfig_t;


/******************************************************************************/
/*---------------------------- Function Prototypes----------------------------*/
/******************************************************************************/

/* SPI configuration functions */
bool SPI_ConfigStandardModeSfr(SpiSfr_t *const spiSfr, SpiStandardConfig_t spiConfig);
bool SPI_EnableSsState(const uint32_t pinCode);
bool SPI_DisableSsState(const uint32_t pinCode);
SpiSsState_t SPI_GetSsState(void);
void SPI_SetSsState(const SpiSsState_t state);

/* SPI status check functions */
bool SPI_IsSpiBusy(SpiSfr_t *const spiSfr);
bool SPI_IsRxDone(SpiSfr_t *const spiSfr, uint8_t rxbufSize);

/* SPI Master mode operation functions */
bool SPI_MasterReadWrite(SpiSfr_t *const spiSfr, volatile void *rxPtr, volatile void *txPtr, uint32_t txSize);
bool SPI_MasterWrite(SpiSfr_t *const spiSfr, volatile void *rxPtr, volatile void *txPtr, uint32_t txSize);
bool SPI_MasterWrite2(SpiSfr_t *const spiSfr, volatile void *rxPtr, volatile void *txPtr, uint32_t txSize, volatile void (*isrHandler)(void));

/* SPI Slave mode operation functions */
bool SPI_DummyRead(SpiSfr_t *const spiSfr);
bool SPI_SlaveWrite(SpiSfr_t *const spiSfr, volatile void *txPtr, uint32_t txSize);
bool SPI_SlaveRead(SpiSfr_t *const spiSfr, volatile void *rxPtr);

#endif	/* SPI_H */