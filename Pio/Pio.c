#include "Pio.h"


/******************************************************************************/
/*--------------------------Local Data Variables------------------------------*/
/******************************************************************************/

/** Pointer for Peripheral Pin Select **/
static PpsSfr_t *const ppsSfr = &PPS_MODULE;

/** Pointer for Interrupt Controller **/
static IcSfr_t *const icSfr = &IC_MODULE;

/** Dummy var for ISR **/
static volatile uint32_t dummy;


static volatile void IsrDefaultHandler(void);

/** ISR function pointer **/
static volatile void (*Isr1HandlerPtr)(void) = IsrDefaultHandler;
static volatile void (*Isr2HandlerPtr)(void) = IsrDefaultHandler;


/******************************************************************************/
/*----------------------External Function Definitions-------------------------*/
/******************************************************************************/

/* 
 *  Configures PPS register
 *  Returns false if pin recognized as GPIO
 *  Temporarily disables interrupts (then restore)
 */
extern bool PIO_ConfigPpsSfr(const uint32_t pinCode)
{
    /* Don't access PPS if pin is GPIO */
    if( (pinCode & 0xFFFF) == 0xFFFF )
    {
        return false;
    }
    
    uint32_t regOffset = ((pinCode >> 0) & 0xFF) / 4;
    uint32_t regCode = (pinCode >> 8) & 0xFF;
    
    volatile uint32_t *ppsReg = NULL;       // Pointer to PPS register
    
    /* Temporarily enable PPS reconfiguration */
    volatile uint32_t intrStatus = CFG_UnlockPpsAccess();
    
    /* If pin input */
    if( PIO_PIN_DIR(pinCode) == PIO_DIR_INPUT )
    {
        ppsReg = &ppsSfr->PPSxIN.INT1 + regOffset - 1;   // Base + offset
    }
    /* If pin output */
    else
    {
        ppsReg = &ppsSfr->PPSxOUT.RPA0 + regOffset;  // Base + offset
    }
    
    *ppsReg = regCode;
    
    /* Disable PPS reconfiguration */
    CFG_LockPpsAccess(intrStatus);
    
    return true;
}


/* 
 *  Releases PPS control over output pin (input pin doesn't need to be released)
 *  Returns false if pin recognized as GPIO
 *  Temporarily disables interrupts (then restore)
 */
extern bool PIO_ReleasePpsSfr(const uint32_t pinCode)
{
    /* Don't access PPS if pin is GPIO */
    if( (pinCode & 0xFFFF) == 0xFFFF )
    {
        return false;
    }
    
    uint32_t regOffset = ((pinCode >> 0) & 0xFF) / 4;
    
    volatile uint32_t *ppsReg = NULL;       // Pointer to PPS register
    
    /* Temporarily enable PPS reconfiguration */
    volatile uint32_t intrStatus = CFG_UnlockPpsAccess();
    
    /* If pin output */
    if( PIO_PIN_DIR(pinCode) == PIO_DIR_OUTPUT )
    {
        ppsReg = &ppsSfr->PPSxOUT.RPA0 + regOffset;  // Base + offset
    }
    /* If pin input, releasing from PPS control isn't applicable */
    else
    {
        return false;
    }
    
    *ppsReg = 0x00;
    
    /* Disable PPS reconfiguration */
    CFG_LockPpsAccess(intrStatus);
    
    return true;
}


/*
 *  Configures Change Notice module for corresponding pin
 *  (CN: Any change (rising/falling edge) on given pin triggers CNx ISR)
 */
extern bool PIO_ConfigInputChange(const uint32_t pinCode, PioPullType_t pullType)
{
    PioSfr_t *const pioSfr = PIO_ReadPinModule(pinCode);
    
    /* Enable Change Notice Control module */
    pioSfr->PIOxCNCON.SET = PIO_ON_MASK;
    
    /* Configure pin as digital input and resistor pull type */
    PIO_ConfigGpioPin(pinCode, PIO_TYPE_DIGITAL, PIO_DIR_INPUT);
    PIO_ConfigGpioPinPull(pinCode, pullType);
    
    /* Enable CN for specific pin */
    pioSfr->PIOxCNEN.SET = 1 << PIO_ReadPinPosition(pinCode);
    
    /* Multi-vector interrupt mode */
    icSfr->ICxINTCON.SET = IC_MVEC_MASK;
    
    /* Configure interrupt SFRs */
    if( pioSfr == &PIOA_MODULE )
    {
        icSfr->ICxIFS1.CLR = IC_CNAIF_MASK;
        icSfr->ICxIEC1.CLR = IC_CNAIE_MASK;
        icSfr->ICxIPC8.CLR = (IC_CNIP_MASK | IC_CNIS_MASK);
        icSfr->ICxIPC8.SET = ((CN_ICX_IPL << IC_CNIP_POS) | (CN_ICX_ISL << IC_CNIS_POS));
        icSfr->ICxIEC1.SET = IC_CNAIE_MASK;
    }
    else if( pioSfr == &PIOB_MODULE )
    {
        icSfr->ICxIFS1.CLR = IC_CNBIF_MASK;
        icSfr->ICxIEC1.CLR = IC_CNBIE_MASK;
        icSfr->ICxIPC8.CLR = (IC_CNIP_MASK | IC_CNIS_MASK);
        icSfr->ICxIPC8.SET = ((CN_ICX_IPL << IC_CNIP_POS) | (CN_ICX_ISL << IC_CNIS_POS));
        icSfr->ICxIEC1.SET = IC_CNBIE_MASK;
    }
    /* PIO module base address check */
    else
    {
        return false;
    }
    
    /* Enable interrupts */
    IC_EnableInterrupts();
    
    return true;
}


/*
 *  Sets a function to be executed after input state change for a given pin
 * 
 *  NOTE: Call anytime after PIO_InputChangeConfig()
 */
extern bool PIO_SetIsrHandler(const uint32_t pinCode, volatile void (*isrHandler)(void))
{
    PioSfr_t *const pioSfr = PIO_ReadPinModule(pinCode);
    
    /* Set function pointer to user-defined function */
    if( pioSfr == &PIOA_MODULE )
    {
        Isr1HandlerPtr = isrHandler;
    }
    else if( pioSfr == &PIOB_MODULE )
    {
        Isr2HandlerPtr = isrHandler;
    }
    /* PIO module base address check */
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
 *  Empty default ISR handler
 */
static volatile void IsrDefaultHandler(void)
{
    
}

/******************************************************************************/
/*-----------------------------ISR  Definitions-------------------------------*/
/******************************************************************************/

void __ISR(CHANGE_NOTICE_VECTOR, CN_ISR_IPL) ISR_ChangeNotice(void)
{
    /* CNA register set */
    if( (icSfr->ICxIEC1.W & IC_CNAIE_MASK) && (icSfr->ICxIFS1.W & IC_CNAIF_MASK) )
    {   
        /* User-defined function */
        Isr1HandlerPtr();
        
        /* Clear persistent interrupt flag */
        dummy = PIOA_MODULE.PIOxPORT.W;
        icSfr->ICxIFS1.CLR = IC_CNAIF_MASK;
    }
    
    /* CNB register set */
    if( (icSfr->ICxIEC1.W & IC_CNBIE_MASK) && (icSfr->ICxIFS1.W & IC_CNBIF_MASK) )
    {   
        /* User-defined function */
        Isr2HandlerPtr();
        
        /* Clear persistent interrupt flag */
        dummy = PIOB_MODULE.PIOxPORT.W;
        icSfr->ICxIFS1.CLR = IC_CNBIF_MASK;
    }
}