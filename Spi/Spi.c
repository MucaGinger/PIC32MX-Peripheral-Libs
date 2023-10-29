#include "Spi.h"

/******************************************************************************/
/*--------------------------Local Data Variables------------------------------*/
/******************************************************************************/

/** ISR TX and RX data variables **/
static volatile void *txDataPtr;
static volatile void *rxDataPtr;
static volatile uint8_t txDataSize;
static volatile uint32_t intrStatus;

/** Dummy write/read variables **/
static volatile uint32_t dummyTxData = 0;
static volatile uint32_t dummyRxData = 0;
static volatile bool isDummyWrite = false;
static volatile bool isDummyRead = false;

/** Pointers for PIO access for Slave Select control **/
static PioSfr_t *const pioSfrA = &PIOA_MODULE;
static PioSfr_t *const pioSfrB = &PIOB_MODULE;

/** Pointers for IC and SPI access within ISR **/
static IcSfr_t *const icSfr = &IC_MODULE;
static SpiSfr_t *isrSpiSfr;

/** ISR function pointers **/
static void (*isrTxHandlerPtr)(void);
static void (*isrRxHandlerPtr)(void);
static void (*isrExtraHandlerPtr)(void);

/** Indicates current Slave Select active pins during SPI operation **/
static volatile SpiSsState_t ssState;
static volatile SpiSsState_t ssMask;


/******************************************************************************/
/*--------------------------Local Data Structures-----------------------------*/
/******************************************************************************/

/** Interrupt enable and flag masks **/
static volatile struct {
    uint32_t    spiTxIe;
    uint32_t    spiTxIf;
    uint32_t    spiRxIe;
    uint32_t    spiRxIf;
} ic;


/******************************************************************************/
/*------------------------Local Function Prototypes---------------------------*/
/******************************************************************************/

/** Non-ISR sub-function **/
static INLINE void IsrHandlerPtrConfig(IsrSpiMode_t isrMode);
static INLINE bool SpiInterruptMaskSet(SpiSfr_t * spiSfr);

/** ISR and non-ISR sub-functions **/
static INLINE void SpiWrite8(void);
static INLINE void SpiWrite16(void);
static INLINE void SpiWrite32(void);
static INLINE void SpiRead8(void);
static INLINE void SpiRead16(void);
static INLINE void SpiRead32(void);

/** ISR handlers **/
static void ISR_SpiTxHandler_MasterWrite(void);
static void ISR_SpiRxHandler_MasterWrite(void);
static void ISR_SpiTxHandler_MasterWrite2(void);
static void ISR_SpiRxHandler_MasterWrite2(void);


/******************************************************************************/
/*----------------------External Function Definitions-------------------------*/
/******************************************************************************/

/*
 *  Configures SPI module in Standard mode with passed structure settings
 *  Returns false, if any input restriction is triggered
 */
extern bool SPI_ConfigStandardModeSfr(SpiSfr_t *const spiSfr, SpiStandardConfig_t spiConfig)
{
    /* SPI base address check */
    if( !(spiSfr == &SPI1_MODULE) && !(spiSfr == &SPI2_MODULE) )
    {
        return false;
    }
    
    /* Not safe to be configured while operating */
    if( SPI_IsSpiBusy(spiSfr) )
    {
        return false;
    }
    
    /* Disable the module */
    spiSfr->SPIxCON.CLR = SPI_ON_MASK;
    asm("nop");
    
    /* Clear all SPIx SFRs */
    spiSfr->SPIxCON.CLR = 0xFFFFFFFF;
    spiSfr->SPIxBRG.CLR = 0xFFFFFFFF;
    spiSfr->SPIxCON2.CLR = 0xFFFFFFFF;
    spiSfr->SPIxSTAT.CLR = 0xFFFFFFFF;
    
    /* If value not assigned to pin setting the pin code is zero */
    bool isSdiEnabled = spiConfig.pinSelect.sdiPin ? true : false;
    bool isSdoEnabled = spiConfig.pinSelect.sdoPin ? true : false;
    bool isSsEnabled  = spiConfig.pinSelect.ss1Pin ? true : false;
    
    /* If pin not used, disable it in hardware */
    if( !isSdiEnabled )
    {
        spiSfr->SPIxCON.SET = SPI_DISSDI_MASK;
    }
    if( !isSdoEnabled )
    {
        spiSfr->SPIxCON.SET = SPI_DISSDO_MASK;
    }
    /* If Slave mode */
    if( !spiConfig.isMasterEnabled )
    {
        if( isSsEnabled )
        {
            spiSfr->SPIxCON.SET = SPI_SSEN_MASK;
        }
        /* SS pin selection wasn't provided by user */
        else
        {
            return false;
        }
    }
    
    /* Other user controllable settings */
    spiSfr->SPIxCON.SET = ((spiConfig.clkMode & 0x01) << SPI_CKE_POS)  |
                          ((spiConfig.clkMode & 0x02) << SPI_CKP_POS)  |
                          (spiConfig.isMasterEnabled << SPI_MSTEN_POS) |
                          (spiConfig.frameWidth << SPI_MODE_POS);
    /* Fixed setting */
    spiSfr->SPIxCON.SET = (0x01 << SPI_ENHBUF_POS);
    
    /* Indirect access of "ssState" structure members */
    uint32_t *ssMembPtr = (uint32_t *)&ssState.pioA;
    const uint8_t ssMembCount = sizeof(SpiSsState_t) / sizeof(uint32_t);
    uint8_t ssModuleOffset = 0;
    uint8_t ssPinPos = 0;
    
    /* Reset SS mask */
    for(uint8_t j = 0; j < ssMembCount; j++, ssMembPtr++)
    {
        *ssMembPtr = 0;
    }
    
    /* Configure PPS and PIO */
    if( isSdiEnabled || isSdoEnabled )
    {
        /* Indirect access of structure members */
        const uint32_t *membPtr = (uint32_t *)&spiConfig.pinSelect;
        const uint8_t membCount = sizeof(SpiPin_t) / sizeof(uint32_t);

        /* PIO configuration of each valid member */
        for(uint8_t i = 0; i < membCount; i++, membPtr++)
        {
            /* Configure Peripheral Pin Select register (non-GPIO) */
            PIO_ConfigPpsSfr(*membPtr);

            /* GPIO SS pin digital output pin */
            if( spiConfig.isMasterEnabled && (membPtr >= &spiConfig.pinSelect.ss1Pin) )
            {
                PIO_SetPin(*membPtr);
                PIO_ConfigGpioPin(*membPtr, PIO_TYPE_DIGITAL, PIO_DIR_OUTPUT);
                
                /* If valid GPIO pin, modify SS mask */
                if( *membPtr & 0xFFFF )
                {   
                    ssModuleOffset = (*membPtr >> 28);
                    ssPinPos = PIO_ReadPinPosition(*membPtr);

                    /* Set a bit in appropriate member */
                    *(ssMembPtr + ssModuleOffset) |= (1 << ssPinPos);
                }
            }
            /* SDI, SDO (or SSI when in Slave mode) PPS pin */
            else
            {
                PIO_ConfigPpsPin(*membPtr, PIO_TYPE_DIGITAL);
            }
        }
        
        /* SCK digital pin */
        if( spiSfr == &SPI1_MODULE )
        {
            PIO_ConfigGpioPinType(GPIO_RPB14, PIO_TYPE_DIGITAL);
        }
        else
        {
            PIO_ConfigGpioPinType(GPIO_RPB15, PIO_TYPE_DIGITAL);
        }
    }
    /* Either SDI or SDO must be enabled for any SPI operation */
    else
    {
        return false;
    }
    
    /* Set baud rate in SPI Master mode */
    if( spiConfig.isMasterEnabled )
    {
        /* Peripheral bus frequency */
        uint32_t pbFreq = OSC_GetPbFreq();
        
        uint32_t baudRate;
        
        /* Max. F_sck is PBCLK/2 */
        if( spiConfig.sckFreq >= (pbFreq/2) )
        {
            baudRate = 0;
        }
        else
        {
            baudRate = (uint32_t)( (pbFreq / (2 * spiConfig.sckFreq)) - 1 );
        }
        
        /* Max. baud rate value is 511 (slowest setting) */
        if( baudRate > 511 )
        {
            baudRate = 511;
        }
        
        spiSfr->SPIxBRG.SET = baudRate;
    }
    
    /* Dummy buffer read */
    SPI_DummyRead(spiSfr);
    
    /* Multi-vector interrupt mode */
    icSfr->ICxINTCON.SET = IC_MVEC_MASK;

    /* Interrupt sources for SPI1 */
    if( spiSfr == &SPI1_MODULE )
    {
        icSfr->ICxIFS1.CLR = (IC_SPI1EIF_MASK | IC_SPI1RXIF_MASK | IC_SPI1TXIF_MASK);                   // Clear flags
        icSfr->ICxIPC7.CLR = (IC_SPI1IP_MASK | IC_SPI1IS_MASK);                                         // Clear (sub)priority
        icSfr->ICxIPC7.SET = ( (SPI1_ICX_IPL << IC_SPI1IP_POS) | (SPI1_ICX_ISL << IC_SPI1IS_POS) );     // Set (sub)priority
        icSfr->ICxIEC1.CLR = (IC_SPI1EIE_MASK | IC_SPI1RXIE_MASK | IC_SPI1TXIE_MASK);                   // Disable sources
    }
    /* Interrupt sources for SPI2 */
    else if ( spiSfr == &SPI2_MODULE )
    {
        icSfr->ICxIFS1.CLR = (IC_SPI2EIF_MASK | IC_SPI2RXIF_MASK | IC_SPI2TXIF_MASK);
        icSfr->ICxIPC9.CLR = (IC_SPI2IP_MASK | IC_SPI2IS_MASK);
        icSfr->ICxIPC9.SET = ( (SPI2_ICX_IPL << IC_SPI2IP_POS) | (SPI2_ICX_ISL << IC_SPI2IS_POS) );
        icSfr->ICxIEC1.CLR = (IC_SPI2EIE_MASK | IC_SPI2RXIE_MASK | IC_SPI2TXIE_MASK);
    }
    /* This is also SPI base address check */
    else
    {
        return false;
    }
    
    /* Clear the overflow flag */
    spiSfr->SPIxSTAT.CLR = SPI_SPIROV_MASK;
    
    /* Enable the module */
    spiSfr->SPIxCON.SET = SPI_ON_MASK;
    asm("nop");
    
    return true;
}


/*
 *  Modifies status of Slave Select devices to be activated during communication
 *  Returns "true" if proper pin code was passed to a function
 */
extern bool SPI_EnableSsState(const uint32_t pinCode)
{
    PinInfo_t pinInfo = PIO_ReadPinCode(pinCode);
    
    /* Each bit corresponds to each Slave current status (1 - active, 0 - not active) */
    if( pinInfo.pinMod == &PIOA_MODULE )
    {
        ssState.pioA |= (1 << pinInfo.pinPos);
    }
    else if( pinInfo.pinMod == &PIOB_MODULE )
    {
        ssState.pioB |= (1 << pinInfo.pinPos);
    }
    /* Other non-available module codes */
    else
    {
        return false;
    }
    
    return true;
}


/*
 *  Modifies status of Slave Select devices to be activated during transmission
 *  Returns "true" if proper pin code was passed to a function
 */
extern bool SPI_DisableSsState(const uint32_t pinCode)
{
    PinInfo_t pinInfo = PIO_ReadPinCode(pinCode);
    
    /* Each bit corresponds to each Slave current status (1 - active, 0 - not active) */
    if( pinInfo.pinMod == &PIOA_MODULE )
    {
        ssState.pioA &= ~(1 << pinInfo.pinPos);
    }
    else if( pinInfo.pinMod == &PIOB_MODULE )
    {
        ssState.pioB &= ~(1 << pinInfo.pinPos);
    }
    /* Other non-available module codes */
    else
    {
        return false;
    }
    
    return true;
}


/*
 *  Gets status of Slave Select devices to be activated during transmission
 */
extern SpiSsState_t SPI_GetSsState(void)
{
    return ssState;
}


/*
 *  Sets status of Slave Select devices to be activated during transmission
 */
extern void SPI_SetSsState(const SpiSsState_t input)
{
    /* Indirect access of "SpiSsState_t" structure members */
    const uint32_t *inputMembPtr = (uint32_t *)&input.pioA;
    const uint32_t *maskMembPtr = (uint32_t *)&ssMask.pioA;
    uint32_t *stateMembPtr = (uint32_t *)&ssState.pioA;
    uint8_t membCount = sizeof(SpiSsState_t) / sizeof(uint32_t);
    
    /* Copy input SS state into "ssState", don't copy any non-SS pin bits */
    while( membCount-- )
    {
        *stateMembPtr = *inputMembPtr & *maskMembPtr;
        
        inputMembPtr++;
        maskMembPtr++;
        stateMembPtr++;
    }
}


/*
 *  Returns "true" if SPI is currently busy with some transaction or ISR is in
 *  progress
 */
extern bool SPI_IsSpiBusy(SpiSfr_t *const spiSfr)
{
    bool flag;
    
    /* Interrupt sources for SPI1 */
    if( spiSfr == &SPI1_MODULE )
    {
        flag = (icSfr->ICxIEC1.W & IC_SPI1TXIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI1TXIF_MASK) || \
               (icSfr->ICxIEC1.W & IC_SPI1RXIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI1RXIF_MASK) || \
               (icSfr->ICxIEC1.W & IC_SPI1EIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI1EIF_MASK);
    }
    /* Interrupt sources for SPI2 */
    else if ( spiSfr == &SPI2_MODULE )
    {
        flag = (icSfr->ICxIEC1.W & IC_SPI2TXIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI2TXIF_MASK) || \
               (icSfr->ICxIEC1.W & IC_SPI2RXIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI2RXIF_MASK) || \
               (icSfr->ICxIEC1.W & IC_SPI2EIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI2EIF_MASK);  
    }
    /* Faulty SPI base address */
    else
    {
        return false;
    }
    
    return flag || ((spiSfr->SPIxSTAT.W & SPI_SPIBUSY_MASK) >> SPI_SPIBUSY_POS);
}


/*
 *  Check if N-elements waiting in RX FIFO (Slave mode)
 *  Returns false if any input restriction is triggered
 */
extern bool SPI_IsRxDone(SpiSfr_t *const spiSfr, uint8_t rxbufSize)
{
    /* SPI base address check */
    if( !(spiSfr == &SPI1_MODULE) && !(spiSfr == &SPI2_MODULE) )
    {
        return false;
    }
    
    /* RX buffer size check */
    if( rxbufSize == 0 )
    {
        return false;
    }
    
    return ((spiSfr->SPIxSTAT.W & SPI_RXBUFELM_MASK) >> SPI_RXBUFELM_POS) >= rxbufSize;
}


/*
 *  Transmit data in Master mode and wait for Slave data (polling)
 *  Returns false if any input restriction is triggered
 */
extern bool SPI_MasterReadWrite(SpiSfr_t *const spiSfr, volatile void *rxPtr, volatile void *txPtr, uint32_t txSize)
{
    /* Set SPI base address for SFR access */
    isrSpiSfr = spiSfr;     // ISR not used, this variable used for read/write function only
    
    /* SPI base address check */
    if( !(spiSfr == &SPI1_MODULE) && !(spiSfr == &SPI2_MODULE) )
    {
        return false;
    }
    
    /* Packet size check */
    if( txSize == 0 )
    {
        return false;
    }
        
    /* Number of active slaves must be positive */
    if( (ssState.pioA == 0) && (ssState.pioB == 0) )
    {
        return false;
    }
    
    /* Proceed only if Master mode is enabled */
    if( !(spiSfr->SPIxCON.W & SPI_MSTEN_MASK) )
    {
        return false;
    }
    
    /* One SPI activity at a time check */
    if( SPI_IsSpiBusy(spiSfr) )
    {
        return false;
    }
    
    /* Dummy write/read if pointer NULL */
    isDummyWrite = (txPtr == NULL) ? true : false;
    isDummyRead = (rxPtr == NULL) ? true : false;
    
    /* Either write/read only (or both) must be selected */
    if( isDummyWrite && isDummyRead )
    {
        return false;
    }
    
    /* TX/RX data and size are handled by private variables */
    txDataPtr = txPtr;
    txDataSize = txSize;
    rxDataPtr = rxPtr;
    
    /* Read RX FIFO until empty */
    SPI_DummyRead(spiSfr);
    
    SpiFrameWidth_t frameWidth = (spiSfr->SPIxCON.W & SPI_MODE_MASK) >> SPI_MODE_POS;
    
    /* Obtain old interrupt status and disable interrupts */
    intrStatus = IC_GetInterruptState();
    IC_DisableInterrupts();             // Ensure atomic operation for SPIxBUF
    
    /* Enable appropriate Slave Select pins */
    pioSfrA->PIOxLAT.CLR = ssState.pioA;
    pioSfrB->PIOxLAT.CLR = ssState.pioB;
    
    /* Write N-bit wide data frame packet */
    if( frameWidth == SPI_WIDTH_8BIT )
    {
        do{
            SpiWrite8();
            
            /* Wait until SPI bus free */
            while( (spiSfr->SPIxSTAT.W & SPI_SPIBUSY_MASK) >> SPI_SPIBUSY_POS );
            
            SpiRead8();
            
        } while( txDataSize != 0);
    }
    else if( frameWidth == SPI_WIDTH_16BIT)
    {
        do{
            SpiWrite16();
            
            /* Wait until SPI bus free */
            while( (spiSfr->SPIxSTAT.W & SPI_SPIBUSY_MASK) >> SPI_SPIBUSY_POS );
            
            SpiRead16();
            
        } while( txDataSize != 0);
    }
    else if( frameWidth == SPI_WIDTH_32BIT )
    {
        do{
            SpiWrite32();
            
            /* Wait until SPI bus free */
            while( (spiSfr->SPIxSTAT.W & SPI_SPIBUSY_MASK) >> SPI_SPIBUSY_POS );
            
            SpiRead32();
            
        } while( txDataSize != 0);
    }
    /* Out of range "MODE" SFR value */
    else
    {
        return false;
    }
    
    /* Disable appropriate Slave Select pins */
    pioSfrA->PIOxLAT.SET = ssState.pioA;
    pioSfrB->PIOxLAT.SET = ssState.pioB;
    
    /* Restore interrupt state */
    IC_SetInterruptState(intrStatus);
    
    return true;
}


/*
 *  Transmit data in Master mode (RX data available after SPI not busy,
 *  interrupt-based)
 *  Returns false if any input restriction is triggered
 */
extern bool SPI_MasterWrite(SpiSfr_t *const spiSfr, volatile void *rxPtr, volatile void *txPtr, uint32_t txSize)
{
    /* Set SPI base address for SFR access */
    isrSpiSfr = spiSfr;
    
    /* Set SPI interrupt IE and IF bit-masks (also an SPI module check) */
    if( !SpiInterruptMaskSet(isrSpiSfr) )
    {
        return false;
    }
    
    /* Packet size check */
    if( txSize == 0 )
    {
        return false;
    }
        
    /* Number of active slaves must be positive */
    if( (ssState.pioA == 0) && (ssState.pioB == 0) )
    {
        return false;
    }
    
    /* Proceed only if Master mode is enabled */
    if( !(isrSpiSfr->SPIxCON.W & SPI_MSTEN_MASK) )
    {
        return false;
    }
    
    /* One SPI activity at a time check */
    if( SPI_IsSpiBusy(spiSfr) )
    {
        return false;
    }
    
    /* Dummy write/read if pointer NULL */
    isDummyWrite = (txPtr == NULL) ? true : false;
    isDummyRead = (rxPtr == NULL) ? true : false;
    
    /* Either write/read only (or both) must be selected */
    if( isDummyWrite && isDummyRead )
    {
        return false;
    }
    
    /* TX/RX data and size are handled by private variables */
    txDataPtr = txPtr;
    txDataSize = txSize;
    rxDataPtr = rxPtr;
    
    /* Use appropriate TX handler */
    IsrHandlerPtrConfig(ISR_SPI_MODE_0);
    
    /* Read RX FIFO until empty */
    SPI_DummyRead(isrSpiSfr);
    
    /* TX and RX interrupt flag trigger setting */
    isrSpiSfr->SPIxCON.CLR = SPI_STXISEL_MASK;       // TX flag set on last SPISR transfer
    
    SpiFrameWidth_t frameWidth = (isrSpiSfr->SPIxCON.W & SPI_MODE_MASK) >> SPI_MODE_POS;
    
    /* Obtain old interrupt status and disable interrupts */
    intrStatus = IC_GetInterruptState();
    IC_DisableInterrupts();             // Ensure atomic operation for SPIxBUF
    
    /* Enable appropriate Slave Select pins */
    pioSfrA->PIOxLAT.CLR = ssState.pioA;
    pioSfrB->PIOxLAT.CLR = ssState.pioB;
    
    /* Write N-bit wide data frame packet */
    if( frameWidth == SPI_WIDTH_8BIT )
    {
        SpiWrite8();
    }
    else if( frameWidth == SPI_WIDTH_16BIT)
    {
        SpiWrite16();
    }
    else if( frameWidth == SPI_WIDTH_32BIT )
    {
        SpiWrite32();
    }
    /* Out of range "MODE" SFR value */
    else
    {
        return false;
    }
    
    /* Clear TX flag only if transfer is still in progress */
    if( isrSpiSfr->SPIxSTAT.W & SPI_SPIBUSY_MASK )
    {
        icSfr->ICxIFS1.CLR = ic.spiTxIf;
    }
    icSfr->ICxIEC1.SET = ic.spiTxIe;    // TX source enabled
    
    /* Restore interrupt state */
    IC_SetInterruptState(intrStatus);
    
    return true;
}


/*
 *  Transmit data in Master mode (RX data available after SPI not busy,
 *  interrupt-based)
 *  Returns false if any input restriction is triggered
 * 
 *  NOTE: This function is exactly the same as the SPI_MasterWrite(),
 *        except that a modified ISR handler is used where isrHandler() is used
 */
extern bool SPI_MasterWrite2(SpiSfr_t *const spiSfr, volatile void *rxPtr, volatile void *txPtr, uint32_t txSize, volatile void (*isrHandler)(void))
{
    /* Use appropriate TX handler and configure additional function pointer */
    IsrHandlerPtrConfig(ISR_SPI_MODE_1);
    isrExtraHandlerPtr = isrHandler;
    
    /* Execute standard functions */
    return SPI_MasterWrite(spiSfr, rxPtr, txPtr, txSize);
}


/*
 *  Empties RX FIFO buffer if there is any RX data pending
 */
extern bool SPI_DummyRead(SpiSfr_t *const spiSfr)
{
    /* SPI base address check */
    if( (spiSfr == &SPI1_MODULE) || (spiSfr == &SPI2_MODULE) )
    {
        /* Read RX FIFO until empty */
        while( !(spiSfr->SPIxSTAT.W & SPI_SPIRBE_MASK) )
        {
            dummyRxData = spiSfr->SPIxBUF.W;
        }
        
        return true;
    }
    else
    {
        return false;
    }
}


/*
 *  Load Slave TX data
 *  Returns false if any input restriction is triggered
 */
extern bool SPI_SlaveWrite(SpiSfr_t *const spiSfr, volatile void *txPtr, uint32_t txSize)
{
    /* Set SPI base address for SFR access */
    isrSpiSfr = spiSfr;         // ISR not used, this variable used for write function only
    
    /* SPI base address check */
    if( !(spiSfr == &SPI1_MODULE) && !(spiSfr == &SPI2_MODULE) )
    {
        return false;
    }
    
    /* Packet size check */
    if( txSize == 0 )
    {
        return false;
    }
    
    /* Proceed only if Slave mode is enabled */
    if( spiSfr->SPIxCON.W & SPI_MSTEN_MASK )
    {
        return false;
    }
    
    /* One SPI activity at a time check */
    if( SPI_IsSpiBusy(spiSfr) )
    {
        return false;
    }
    
    /* Dummy write/read if pointer NULL */
    isDummyWrite = (txPtr == NULL) ? true : false;
    
    /* TX data and size are handled by global variables */
    txDataPtr = txPtr;
    txDataSize = txSize;
    
    SpiFrameWidth_t frameWidth = (spiSfr->SPIxCON.W & SPI_MODE_MASK) >> SPI_MODE_POS;
    
    /* Obtain old interrupt status and disable interrupts */
    volatile uint32_t intrStatus = IC_GetInterruptState();
    IC_DisableInterrupts();                                 // Ensure atomic operation for SPIxBUF
    
    /* Write N-bit wide data frame packet */
    if( frameWidth == SPI_WIDTH_8BIT )
    {
        SpiWrite8();
    }
    else if( frameWidth == SPI_WIDTH_16BIT)
    {
        SpiWrite16();
    }
    else if( frameWidth == SPI_WIDTH_32BIT )
    {
        SpiWrite32();
    }
    /* Out of range "MODE" SFR value */
    else
    {
        return false;
    }
    
    /* Restore interrupt state */
    IC_SetInterruptState(intrStatus);
    
    return true;
}



/*
 *  Read Slave RX data
 *  Returns false if any input restriction is triggered
 */
extern bool SPI_SlaveRead(SpiSfr_t *const spiSfr, volatile void *rxPtr)
{
    /* Set SPI base address for SFR access */
    isrSpiSfr = spiSfr;         // ISR not used, this variable used for write function only
    
    /* SPI base address check */
    if( !(spiSfr == &SPI1_MODULE) && !(spiSfr == &SPI2_MODULE) )
    {
        return false;
    }
    
    /* Proceed only if Slave mode is enabled */
    if( spiSfr->SPIxCON.W & SPI_MSTEN_MASK )
    {
        return false;
    }
    
    /* One SPI activity at a time check */
    if( SPI_IsSpiBusy(spiSfr) )
    {
        return false;
    }
    
    /* Dummy write/read if pointer NULL */
    isDummyRead = (rxPtr == NULL) ? true : false;
    
    /* RX data is handled by private variables */
    rxDataPtr = rxPtr;
    
    SpiFrameWidth_t frameWidth = (spiSfr->SPIxCON.W & SPI_MODE_MASK) >> SPI_MODE_POS;
    
    /* Read N-bit wide data frame packet */
    if( frameWidth == SPI_WIDTH_8BIT )
    {
        SpiRead8();
    }
    else if( frameWidth == SPI_WIDTH_16BIT)
    {
        SpiRead16();
    }
    else if( frameWidth == SPI_WIDTH_32BIT )
    {
        SpiRead32();
    }
    /* Out of range "MODE" SFR value */
    else
    {
        return false;
    }
    
    return true;
}


/******************************************************************************/
/*------------------------Local Function Definitions--------------------------*/
/******************************************************************************/

/*
 *  Write 8-bit data to SPI TX FIFO
 */
static INLINE void SpiWrite8(void)
{
    uint32_t tempSize;
    
    /* Load only maximum amount of bytes to TX FIFO */
    tempSize = (txDataSize > 16) ? 16 : txDataSize;
    txDataSize -= tempSize;
    
    /* Dummy write */
    if( isDummyWrite )
    {
        while( tempSize-- )
        {
            isrSpiSfr->SPIxBUF.W = dummyTxData;
        }
    }
    /* Normal write */
    else
    {
        while( tempSize-- )
        {
            isrSpiSfr->SPIxBUF.W = *((uint8_t *)txDataPtr);
            txDataPtr = (uint8_t *)txDataPtr + 1;
        }
    }
}


/*
 *  Write 16-bit data to SPI TX FIFO
 */
static INLINE void SpiWrite16(void)
{
    uint32_t tempSize;
    
    /* Load only maximum amount of bytes to TX FIFO */
    tempSize = (txDataSize > 8) ? 8 : txDataSize;
    txDataSize -= tempSize;

    /* Dummy write */
    if( isDummyWrite )
    {
        while( tempSize-- )
        {
            isrSpiSfr->SPIxBUF.W = dummyTxData;
        }
    }
    /* Normal write */
    else
    {
        while( tempSize-- )
        {
            isrSpiSfr->SPIxBUF.W = *((uint16_t *)txDataPtr);
            txDataPtr = (uint16_t *)txDataPtr + 1;
        }
    }
}


/*
 *  Write 32-bit data to SPI TX FIFO
 */
static INLINE void SpiWrite32(void)
{
    uint32_t tempSize;
    
    /* Load only maximum amount of bytes to TX FIFO */
    tempSize = (txDataSize > 4) ? 4 : txDataSize;
    txDataSize -= tempSize;

    /* Dummy write */
    if( isDummyWrite )
    {
        while( tempSize-- )
        {
            isrSpiSfr->SPIxBUF.W = dummyTxData;
        }
    }
    /* Normal write */
    else
    {
        while( tempSize-- )
        {
            isrSpiSfr->SPIxBUF.W = *((uint32_t *)txDataPtr);
            txDataPtr = (uint32_t *)txDataPtr + 1;
        }
    }
}


/*
 *  Read 8-bit data from SPI RX FIFO
 */
static INLINE void SpiRead8(void)
{
    /* Dummy read */
    if( isDummyRead )
    {
        do
        {
            dummyRxData = isrSpiSfr->SPIxBUF.W;
        }
        while( !(isrSpiSfr->SPIxSTAT.W & SPI_SPIRBE_MASK) );
    }
    /* Normal read */
    else
    {
        do
        {
            *((uint8_t *)rxDataPtr) = isrSpiSfr->SPIxBUF.W;
            rxDataPtr = (uint8_t *)rxDataPtr + 1;
        }
        while( !(isrSpiSfr->SPIxSTAT.W & SPI_SPIRBE_MASK) );
    }
}


/*
 *  Read 16-bit data from SPI RX FIFO
 */
static INLINE void SpiRead16(void)
{
    /* Dummy read */
    if( isDummyRead )
    {
        do
        {
            dummyRxData = isrSpiSfr->SPIxBUF.W;
        }
        while( !(isrSpiSfr->SPIxSTAT.W & SPI_SPIRBE_MASK) );
    }
    /* Normal read */
    else
    {
        do
        {
            *((uint16_t *)rxDataPtr) = isrSpiSfr->SPIxBUF.W;
            rxDataPtr = (uint16_t *)rxDataPtr + 1;
        }
        while( !(isrSpiSfr->SPIxSTAT.W & SPI_SPIRBE_MASK) );
    }
}


/*
 *  Read 32-bit data from SPI RX FIFO
 */
static INLINE void SpiRead32(void)
{
    /* Dummy read */
    if( isDummyRead )
    {
        do
        {
            dummyRxData = isrSpiSfr->SPIxBUF.W;
        }
        while( !(isrSpiSfr->SPIxSTAT.W & SPI_SPIRBE_MASK) );
    }
    /* Normal read */
    else
    {
        do
        {
            *((uint32_t *)rxDataPtr) = isrSpiSfr->SPIxBUF.W;
            rxDataPtr = (uint32_t *)rxDataPtr + 1;
        }
        while( !(isrSpiSfr->SPIxSTAT.W & SPI_SPIRBE_MASK) );
    }
}


/*
 *  Sets interrupt IE and IF masks for accessing proper
 *  bit fields in the IC related SFRs (bit masks depend on SPI module selected)
 */
static INLINE bool SpiInterruptMaskSet(SpiSfr_t * spiSfr)
{
    if( isrSpiSfr == &SPI1_MODULE )
    {
        ic.spiTxIe = IC_SPI1TXIE_MASK;
        ic.spiRxIe = IC_SPI1RXIE_MASK;
        ic.spiTxIf = IC_SPI1TXIF_MASK;
        ic.spiRxIf = IC_SPI1RXIF_MASK;
    }
    else if( isrSpiSfr == &SPI2_MODULE )
    {
        ic.spiTxIe = IC_SPI2TXIE_MASK;
        ic.spiRxIe = IC_SPI2RXIE_MASK;
        ic.spiTxIf = IC_SPI2TXIF_MASK;
        ic.spiRxIf = IC_SPI2RXIF_MASK;
    }
    /* SPI base address check */
    else
    {
        return false;
    }
    
    return true;
}


/*
 *  Configures TX and RX ISR handler function pointers
 */
static INLINE void IsrHandlerPtrConfig(IsrSpiMode_t isrMode)
{
    switch( isrMode )
    {
        /* Standard Master mode transmission */
        case ISR_SPI_MODE_0:
            isrTxHandlerPtr = ISR_SpiTxHandler_MasterWrite;
            isrRxHandlerPtr = ISR_SpiRxHandler_MasterWrite;
            break;
        /* Standard Master mode transmission with extra function at the end */
        case ISR_SPI_MODE_1:
            isrTxHandlerPtr = ISR_SpiTxHandler_MasterWrite2;
            isrRxHandlerPtr = ISR_SpiRxHandler_MasterWrite2;
            break;
        /* Non-valid input */
        default:
            break;
    }
}


/*
 *  ISR TX handler for SPI_MasterWrite()
 */
static void ISR_SpiTxHandler_MasterWrite(void)
{
    /* Wait if RX FIFO is still being loaded */
    while( isrSpiSfr->SPIxSTAT.W & SPI_SPIBUSY_MASK );
    
    /* Disable Slave devices before Master handles RX data */
    if( txDataSize == 0 )
    {
        pioSfrA->PIOxLAT.SET = ssState.pioA;
        pioSfrB->PIOxLAT.SET = ssState.pioB;
    }

    SpiFrameWidth_t frameWidth = (isrSpiSfr->SPIxCON.W & SPI_MODE_MASK) >> SPI_MODE_POS;

    /* Read N-bit wide data frame packet */
    if( frameWidth == SPI_WIDTH_8BIT )
    {
        SpiRead8();
    }
    else if( frameWidth == SPI_WIDTH_16BIT)
    {
        SpiRead16();
    }
    else if( frameWidth == SPI_WIDTH_32BIT )
    {
        SpiRead32();
    }
    /* Out of range "MODE" SFR value */
    else {}

    /* More data needs to be transmitted */
    if( txDataSize != 0 )
    {
        /* Ensure atomic operation for SPIxBUF */
        IC_DisableInterrupts();
        
        /* Write N-bit wide data frame packet */
        if( frameWidth == SPI_WIDTH_8BIT )
        {
            SpiWrite8();
        }
        else if( frameWidth == SPI_WIDTH_16BIT)
        {
            SpiWrite16();
        }
        else if( frameWidth == SPI_WIDTH_32BIT )
        {
            SpiWrite32();
        }
        /* Out of range "MODE" SFR value */
        else {}

        /* Clear TX flag only if transfer still in progress */
        if( isrSpiSfr->SPIxSTAT.W & SPI_SPIBUSY_MASK )
        {
            icSfr->ICxIFS1.CLR = ic.spiTxIf;
        }
        
        /* Enable interrupts */
        IC_EnableInterrupts();
    }
    /* Transmission is complete */
    else
    {
        /* Disable TX interrupt source */
        icSfr->ICxIFS1.CLR = ic.spiTxIf;
        icSfr->ICxIEC1.CLR = ic.spiTxIe;
        
        /* Restore interrupt state */
        IC_SetInterruptState(intrStatus);
    }
}


/*
 *  ISR TX handler for SPI_MasterWrite()
 */
static void ISR_SpiRxHandler_MasterWrite(void)
{
    /* All data handled in TX handler */
}


/*
 *  ISR TX handler for SPI_MasterWrite2()
 */
static void ISR_SpiTxHandler_MasterWrite2(void)
{
    /* Same TX handler as for the original MasterReadOnly */
    ISR_SpiTxHandler_MasterWrite();
    
    /* Additional function */
    isrExtraHandlerPtr();
}


/*
 *  ISR RX handler for SPI_MasterWrite2()
 */
static void ISR_SpiRxHandler_MasterWrite2(void)
{
    /* All data handled in TX handler */
}


/******************************************************************************/
/*-----------------------------ISR Definition--------------------------------*/
/******************************************************************************/

/*
 *  ISR for SPI_1 module
 */
void __ISR(SPI_1_VECTOR, SPI1_ISR_IPL) ISR_Spi1(void)
{
    /* RX Interrupt */
    if( (icSfr->ICxIEC1.W & IC_SPI1RXIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI1RXIF_MASK) )
    {   
        isrRxHandlerPtr();
    }

    /* TX Interrupt */
    if( (icSfr->ICxIEC1.W & IC_SPI1TXIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI1TXIF_MASK) )
    {
        isrTxHandlerPtr();
    }
}


/*
 *  ISR for SPI_2 module
 */
void __ISR(SPI_2_VECTOR, SPI2_ISR_IPL) ISR_Spi2(void)
{
    /* RX Interrupt */
    if( (icSfr->ICxIEC1.W & IC_SPI2RXIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI2RXIF_MASK) )
    {
        isrRxHandlerPtr();
    }

    /* TX Interrupt */
    if( (icSfr->ICxIEC1.W & IC_SPI2TXIE_MASK) && (icSfr->ICxIFS1.W & IC_SPI2TXIF_MASK) )
    {
        isrTxHandlerPtr();
    }
}