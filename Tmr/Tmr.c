#include "Tmr.h"

#define CORE_TIMER_PERIOD_MS        1
#define CORE_TIMER_CALLBACK_COUNT   9

/******************************************************************************/
/*--------------------------Local Data Variables------------------------------*/
/******************************************************************************/

/** Pointer for Interrupt Controller **/
static IcSfr_t *const icSfr = &IC_MODULE;

typedef struct {
    uint32_t            sysClk;
    uint32_t            clkDiv;
    uint32_t            timeUnit;
} TmrToutParam_t;

typedef struct {
    bool              isMode32;
    bool              isGateCont;
} TmrStatus_t;

/** Timeout parameters **/
/* Configured in TMR_TimeoutSfrConfig(), used in TMR_SetTimerPeriod */
typedef struct {
    TmrToutParam_t      t1;
    TmrToutParam_t      t2;
    TmrToutParam_t      t3;
    TmrToutParam_t      t4;
    TmrToutParam_t      t5;
} ToutParam_t ;

static volatile ToutParam_t toutParam;

/** General timer flags **/
/* Configured in TMR_ConfigSfrXxxMode(), used in ISRx() */
typedef struct {
    TmrStatus_t         t1;
    TmrStatus_t         t2;
    TmrStatus_t         t3;
    TmrStatus_t         t4;
    TmrStatus_t         t5;
} ConfigFlag_t;

/** Static variables **/
static volatile ConfigFlag_t isrFlag;
static volatile uint32_t coreTimerPeriod;
static uint32_t isrHandlerAddr[CORE_TIMER_CALLBACK_COUNT];

/** Default ISR empty handler **/
static void IsrDefaultHandler(void);

/** ISR function pointer **/
static void (*Isr1HandlerPtr)(void) = IsrDefaultHandler;
static void (*Isr2HandlerPtr)(void) = IsrDefaultHandler;
static void (*Isr3HandlerPtr)(void) = IsrDefaultHandler;
static void (*Isr4HandlerPtr)(void) = IsrDefaultHandler;
static void (*Isr5HandlerPtr)(void) = IsrDefaultHandler;

/** Special-case use function pointers (number of function pointers reflects
 ** to size of "isrHandlerAddr" **/
static void (*IsrTimeout1HandlerPtr)(void) = IsrDefaultHandler;
static void (*IsrTimeout2HandlerPtr)(void) = IsrDefaultHandler;
static void (*IsrTimeout3HandlerPtr)(void) = IsrDefaultHandler;
static void (*IsrTimeout4HandlerPtr)(void) = IsrDefaultHandler;
static void (*IsrTimeout5HandlerPtr)(void) = IsrDefaultHandler;
static void (*IsrTimeout6HandlerPtr)(void) = IsrDefaultHandler;
static void (*IsrTimeout7HandlerPtr)(void) = IsrDefaultHandler;
static void (*IsrTimeout8HandlerPtr)(void) = IsrDefaultHandler;
static void (*IsrTimeout9HandlerPtr)(void) = IsrDefaultHandler;

/******************************************************************************/
/*------------------------Local Function Prototypes---------------------------*/
/******************************************************************************/

INLINE static void InterruptSfrConfig(TmrSfr_t *const tmrSfr);
INLINE static void TimeoutParamSet(TmrSfr_t *const tmrSfr, uint32_t sysClk, uint32_t clkDiv, uint32_t timeUnit);
INLINE static void IsrFlagSet(TmrSfr_t *const tmrSfr, bool isMode32, bool isGateCont);
INLINE static TmrToutParam_t TimeoutParamRead(TmrSfr_t *const tmrSfr);

/******************************************************************************/
/*----------------------External Function Definitions-------------------------*/
/******************************************************************************/

/*
 *  Configures a specific timer for timeout mode (ISR-based)
 *  (Timeout mode: after specific period, user-defined function is executed)
 */
extern bool TMR_ConfigTimeoutModeSfr(TmrSfr_t *const tmrSfr, TmrTimeoutConfig_t tmrConfig)
{
    /* Timer base address check */
    if( !(tmrSfr == &TMR1_MODULE) && !(tmrSfr == &TMR2_MODULE) && 
        !(tmrSfr == &TMR3_MODULE) && !(tmrSfr == &TMR4_MODULE) &&
        !(tmrSfr == &TMR5_MODULE) )
    {
        return false;
    }
    
    /* SOSC only applicable for Timer1 */
    if( (tmrSfr == &TMR1_MODULE) && (tmrConfig.clkSrc == TMR_CLK_SRC_SOSC) )
    {
        /* SOSC enabled check */
        if( OSC_GetClkSource() != OSC_COSC_SOSC )
        {
            return false;
        }
    }
    
    /* Disable the module */
    tmrSfr->TMRxCON.CLR = TMR_ON_MASK;
    asm("nop");
    
    /* Clear all TMRx SFRs */
    tmrSfr->TMRxCON.CLR = 0xFFFFFFFF;
    tmrSfr->TMRxTMR.CLR = 0xFFFFFFFF;
    tmrSfr->TMRxPR.CLR = 0xFFFFFFFF;
    
    TmrSfr_t *tmrSfr2 = NULL;
    bool isMode32 = false;
    
    /* 32-bit mode only possible when configuring Timer2 or Timer4 */
    if( tmrConfig.bitMode == TMR_BITMODE_32BIT )
    {
        /* TMRx and TMRy must be cleared during 32-bit mode*/
        if( (tmrSfr == &TMR2_MODULE) || (tmrSfr == &TMR4_MODULE) )
        {
            /* Pointer for next timer access */
            tmrSfr2 = ((TmrSfr_t *)((uint8_t *)tmrSfr + 0x200));
            
            /* Reset other Timer of 32-bit pair */
            tmrSfr2->TMRxCON.CLR = TMR_ON_MASK;
            asm("nop");
            tmrSfr2->TMRxCON.CLR = 0xFFFFFFFF;
            tmrSfr2->TMRxTMR.CLR = 0xFFFFFFFF;
            tmrSfr2->TMRxPR.CLR = 0xFFFFFFFF;
        }
        else
        {
            return false;
        }
        
        isMode32 = true;
    }
    
    uint32_t clkDiv = 0;
    bool isExtClk = false;
    
    /* Timer1 has only 4 pre-scaler options */
    if( tmrSfr == &TMR1_MODULE )
    {
        /* Only DIV_1/8/64/256 available */
        clkDiv = (tmrConfig.clkDiv & 0x01) | (tmrConfig.clkDiv & 0x04);
    }
    else
    {
        clkDiv = tmrConfig.clkDiv;
    }

    /* Configure Timer SFR */
    tmrSfr->TMRxCON.SET = (tmrConfig.bitMode << TMR_T32_POS) |
                          (clkDiv << TMR_TCKPS_POS) |
                          (isExtClk << TMR_TCS_POS);
    
    /* Set timeout parameters for PRx compare value calculation */
    TimeoutParamSet(tmrSfr, OSC_GetSysFreq(), tmrConfig.clkDiv, tmrConfig.timeUnit);
    
    /* Set timer status flags (for ISR) */
    IsrFlagSet(tmrSfr, isMode32, false);
    
    /* Configure interrupt SFRs */
    if( tmrConfig.bitMode == TMR_BITMODE_32BIT )
    {
        InterruptSfrConfig(tmrSfr2);
    }
    else
    {
        InterruptSfrConfig(tmrSfr);
    }
    
    /* Enable interrupts */
    IC_EnableInterrupts();
        
    return true;
}


/*
 *  Configures a specific timer for gated mode
 *  (Gated mode: timer counts presence of HIGH signal at T1CK pin)
 */
extern bool TMR_ConfigGatedModeSfr(TmrSfr_t *const tmrSfr, TmrGatedConfig_t tmrConfig)
{
    /* Timer base address check */
    if( !(tmrSfr == &TMR1_MODULE) && !(tmrSfr == &TMR2_MODULE) && 
        !(tmrSfr == &TMR3_MODULE) && !(tmrSfr == &TMR4_MODULE) &&
        !(tmrSfr == &TMR5_MODULE) )
    {
        return false;
    }

    /* TxCK pin set check */
    if( !tmrConfig.tckPin )
    {
        return false;
    }
    
    /* Disable the module */
    tmrSfr->TMRxCON.CLR = TMR_ON_MASK;
    asm("nop");
    
    /* Clear all TMRx SFRs */
    tmrSfr->TMRxCON.CLR = 0xFFFFFFFF;
    tmrSfr->TMRxTMR.CLR = 0xFFFFFFFF;
    tmrSfr->TMRxPR.CLR = 0xFFFFFFFF;
    
    TmrSfr_t *tmrSfr2 = NULL;
    bool isMode32 = false;
    
    /* 32-bit mode only possible when configuring Timer2 or Timer4 */
    if( tmrConfig.bitMode == TMR_BITMODE_32BIT )
    {
        /* TMRx and TMRy must be cleared during 32-bit mode*/
        if( (tmrSfr == &TMR2_MODULE) || (tmrSfr == &TMR4_MODULE) )
        {
            /* Pointer for next timer access */
            tmrSfr2 = ((TmrSfr_t *)((uint8_t *)tmrSfr + 0x200));
            
            /* Reset other Timer of 32-bit pair */
            tmrSfr2->TMRxCON.CLR = TMR_ON_MASK;
            asm("nop");
            tmrSfr2->TMRxCON.CLR = 0xFFFFFFFF;
            tmrSfr2->TMRxTMR.CLR = 0xFFFFFFFF;
            tmrSfr2->TMRxPR.CLR = 0xFFFFFFFF;
        }
        else
        {
            return false;
        }
        
        isMode32 = true;
    }
    
    uint32_t clkDiv = 0;
    
    /* Timer1 has only 4 pre-scaler options */
    if( tmrSfr == &TMR1_MODULE )
    {
        /* Only DIV_1/8/64/256 available */
        clkDiv = (tmrConfig.clkDiv & 0x01) | (tmrConfig.clkDiv & 0x04);
    }
    else
    {
        clkDiv = tmrConfig.clkDiv;
    }
    
    /* Configure PPS and PIO settings of TxCK pin */
    PIO_ConfigPpsSfr(tmrConfig.tckPin);
    PIO_ConfigPpsPin(tmrConfig.tckPin, PIO_TYPE_DIGITAL);
    
    /* Configure Timer SFR */
    tmrSfr->TMRxCON.SET = (tmrConfig.bitMode << TMR_T32_POS) |
                          (clkDiv << TMR_TCKPS_POS) |
                          (1 << TMR_TGATE_POS);
    
    /* Compare on max. length period for gated mode */
    tmrSfr->TMRxPR.SET = 0xFFFFFFFF;
    
    /* Set timer status flags (for ISR) */
    IsrFlagSet(tmrSfr, isMode32, tmrConfig.isGateCont);
    
    /* Configure interrupt SFRs */
    if( tmrConfig.bitMode == TMR_BITMODE_32BIT )
    {
        InterruptSfrConfig(tmrSfr2);
    }
    else
    {
        InterruptSfrConfig(tmrSfr);
    }
    
    /* Enable interrupts */
    IC_EnableInterrupts();
    
    return true;
}


/*
 *  Sets a function to be executed after timeout for specific timer
 */
extern bool TMR_SetCallback(TmrSfr_t *const tmrSfr, void (*isrHandler)(void))
{
    /* Input protection */
    if (isrHandler == NULL)
    {
        return false;
    }
    
    /* Set function pointer to user-defined function */
    if( tmrSfr == &TMR1_MODULE )
    {
        Isr1HandlerPtr = isrHandler;
    }
    else if( tmrSfr == &TMR2_MODULE )
    {
        /* 32-bit mode */
        if( isrFlag.t2.isMode32 == true )
        {
            Isr3HandlerPtr = isrHandler;
        }
        /* 16-bit mode */
        else
        {
            Isr2HandlerPtr = isrHandler;
        }
    }
    else if( tmrSfr == &TMR3_MODULE )
    {
        Isr3HandlerPtr = isrHandler;
    }
    else if( tmrSfr == &TMR4_MODULE )
    {
        /* 32-bit mode */
        if( isrFlag.t2.isMode32 == true )
        {
            Isr5HandlerPtr = isrHandler;
        }
        /* 16-bit mode */
        else
        {
            Isr4HandlerPtr = isrHandler;
        }
    }
    else if( tmrSfr == &TMR5_MODULE )
    {
        Isr5HandlerPtr = isrHandler;
    }
    /* TMR module base address check */
    else
    {
        return false;
    }
    
    return true;
}


/*
 *  Sets a function to be executed after timeout for Core Timer
 */
extern bool TMR_SetCoreTimerCallback(void (*isrHandler)(void))
{
    /* Input protection */
    if (isrHandler == NULL)
    {
        return false;
    }
    
    static uint8_t addrIdx = 0;
    
    /* Check if handler already set */
    for (uint8_t idx = 0; idx < addrIdx; idx++)
    {
        /* Return true means handler is already set */
        if (isrHandlerAddr[idx] == (uint32_t)isrHandler)
        {
            return true;
        }
    }
    
    /* Use handler if unassigned to any other user-defined callback */
    if (IsrTimeout1HandlerPtr == IsrDefaultHandler)
    {
        IsrTimeout1HandlerPtr = isrHandler;
    }
    else if (IsrTimeout2HandlerPtr == IsrDefaultHandler)
    {
        IsrTimeout2HandlerPtr = isrHandler;
    }
    else if (IsrTimeout3HandlerPtr == IsrDefaultHandler)
    {
        IsrTimeout3HandlerPtr = isrHandler;
    }
    else if (IsrTimeout4HandlerPtr == IsrDefaultHandler)
    {
        IsrTimeout4HandlerPtr = isrHandler;
    }
    else if (IsrTimeout5HandlerPtr == IsrDefaultHandler)
    {
        IsrTimeout5HandlerPtr = isrHandler;
    }
    else if (IsrTimeout6HandlerPtr == IsrDefaultHandler)
    {
        IsrTimeout6HandlerPtr = isrHandler;
    }
    else if (IsrTimeout7HandlerPtr == IsrDefaultHandler)
    {
        IsrTimeout7HandlerPtr = isrHandler;
    }
    else if (IsrTimeout8HandlerPtr == IsrDefaultHandler)
    {
        IsrTimeout8HandlerPtr = isrHandler;
    }
    else if (IsrTimeout9HandlerPtr == IsrDefaultHandler)
    {
        IsrTimeout9HandlerPtr = isrHandler;
    }
    /* No empty handler */
    else
    {
        return false;
    }
    
    /* Store address of given handler */
    isrHandlerAddr[addrIdx] = (uint32_t)isrHandler;
    addrIdx++;
    
    /* Configure and enable CT the first time a callback is set */
    static bool isCoreTimerConfigured = false;
    if( isCoreTimerConfigured == false )
    {
        icSfr->ICxINTCON.SET = IC_MVEC_MASK;
        icSfr->ICxIEC0.CLR = IC_CTIE_MASK;
        icSfr->ICxIPC0.CLR = (IC_CTIS_MASK | IC_CTIP_MASK);
        icSfr->ICxIPC0.SET = ((CT_ICX_IPL << IC_CTIS_POS) | (CT_ICX_ISL << IC_CTIP_POS));
        icSfr->ICxIFS0.CLR = IC_CTIF_MASK;
        icSfr->ICxIEC0.SET = IC_CTIE_MASK;
        
        isCoreTimerConfigured = true;
        coreTimerPeriod = CORE_TIMER_PERIOD_MS * (OSC_GetSysFreq() / 1000 / 2);
        _CP0_SET_COMPARE(_CP0_GET_COUNT() + coreTimerPeriod);
    }
    
    return true;
}


/*
 *  Set timeout period for specific timer
 */
extern bool TMR_SetTimeoutPeriod(TmrSfr_t *const tmrSfr, uint32_t period)
{
    /* Timer base address check */
    if( !(tmrSfr == &TMR1_MODULE) && !(tmrSfr == &TMR2_MODULE) && 
        !(tmrSfr == &TMR3_MODULE) && !(tmrSfr == &TMR4_MODULE) &&
        !(tmrSfr == &TMR5_MODULE) )
    {
        return false;
    }
    
    /* PRx = 0 does not generate interrupt flag */
    if( period == 0 )
    {
        return false;
    }
    
    /* No period setting in gated mode */
    if( tmrSfr->TMRxCON.W & TMR_TGATE_MASK )
    {
        return false;
    }
    
    /* Reset counter */
    tmrSfr->TMRxTMR.CLR = 0xFFFFFFFF;
    tmrSfr->TMRxPR.CLR = 0xFFFFFFFF;
    
    /* Get timeout info for specific timer (set during configuration) */
    TmrToutParam_t toutParam = TimeoutParamRead(tmrSfr);
    
    /* Calculate compare value */
    tmrSfr->TMRxPR.SET = ((uint64_t)period * toutParam.sysClk) / (toutParam.clkDiv * toutParam.timeUnit);
    
    return true;
}

/******************************************************************************/
/*------------------------Local Function Definitions--------------------------*/
/******************************************************************************/

/*
 *  Configures interrupt SFRs for timer modules
 */
INLINE static void InterruptSfrConfig(TmrSfr_t *tmrSfr)
{
    icSfr->ICxINTCON.SET = IC_MVEC_MASK;    // Multi-vector interrupt mode
    
    /* (Sub)priority for Timer1 */
    if( tmrSfr == &TMR1_MODULE )
    {
        icSfr->ICxIFS0.CLR = IC_T1IF_MASK;
        icSfr->ICxIEC0.CLR = IC_T1IE_MASK;
        icSfr->ICxIPC1.CLR = (IC_T1IS_MASK | IC_T1IP_MASK);
        icSfr->ICxIPC1.SET = ((TMR1_ICX_ISL << IC_T1IS_POS) | (TMR1_ICX_IPL << IC_T1IP_POS));
        icSfr->ICxIEC0.SET = IC_T1IE_MASK;
    }
    /* (Sub)priority for Timer2 */
    else if( tmrSfr == &TMR2_MODULE )
    {
        icSfr->ICxIFS0.CLR = IC_T2IF_MASK;
        icSfr->ICxIEC0.CLR = IC_T2IE_MASK;
        icSfr->ICxIPC2.CLR = (IC_T2IS_MASK | IC_T2IP_MASK);
        icSfr->ICxIPC2.SET = ((TMR2_ICX_ISL << IC_T2IS_POS) | (TMR2_ICX_IPL << IC_T2IP_POS));
        icSfr->ICxIEC0.SET = IC_T2IE_MASK;
    }
    /* (Sub)priority for Timer3 */
    else if( tmrSfr == &TMR3_MODULE )
    {
        icSfr->ICxIFS0.CLR = IC_T3IF_MASK;
        icSfr->ICxIEC0.CLR = IC_T3IE_MASK;
        icSfr->ICxIPC3.CLR = (IC_T3IS_MASK | IC_T3IP_MASK);
        icSfr->ICxIPC3.SET = ((TMR3_ICX_ISL << IC_T3IS_POS) | (TMR3_ICX_IPL << IC_T3IP_POS));
        icSfr->ICxIEC0.SET = IC_T3IE_MASK;
    }
    /* (Sub)priority for Timer4 */
    else if( tmrSfr == &TMR4_MODULE )
    {
        icSfr->ICxIFS0.CLR = IC_T4IF_MASK;
        icSfr->ICxIEC0.CLR = IC_T4IE_MASK;
        icSfr->ICxIPC4.CLR = (IC_T4IS_MASK | IC_T4IP_MASK);
        icSfr->ICxIPC4.SET = ((TMR4_ICX_ISL << IC_T4IS_POS) | (TMR4_ICX_IPL << IC_T4IP_POS));
        icSfr->ICxIEC0.SET = IC_T4IE_MASK;
    }
    /* (Sub)priority for Timer5 */
    else
    {
        icSfr->ICxIFS0.CLR = IC_T5IF_MASK;
        icSfr->ICxIEC0.CLR = IC_T5IE_MASK;
        icSfr->ICxIPC5.CLR = (IC_T5IS_MASK | IC_T5IP_MASK);
        icSfr->ICxIPC5.SET = ((TMR5_ICX_ISL << IC_T5IS_POS) | (TMR5_ICX_IPL << IC_T5IP_POS));
        icSfr->ICxIEC0.SET = IC_T5IE_MASK;
    }
}


/*
 *  Stores parameters needed for calculation of PRx value into local structure
 */
INLINE static void TimeoutParamSet(TmrSfr_t *const tmrSfr, uint32_t sysClk, uint32_t clkDiv, uint32_t timeUnit)
{
    /* Offset index = (&TMRx - &TMR1) / (TMR(x+1) - TMRx) */
    uint32_t regOffset = ( (uint32_t)&tmrSfr->TMRxCON.W - (uint32_t)&TMR1_MODULE.TMRxCON.W ) / 0x200;
    
    uint32_t structRegOffset = regOffset * sizeof(TmrToutParam_t);
    uint32_t structRegBase = (uint32_t)&toutParam.t1.sysClk;
    
    /* Pointer to x-th member of structure (for TMRx timeout parameters) */
    volatile uint32_t *toutParamPtr = (uint32_t *)(structRegBase + structRegOffset);
    
    /* Modify members through pointer */
    *(toutParamPtr + 0) = OSC_GetSysFreq();
    *(toutParamPtr + 1) = (clkDiv <= 6) ? (1 << clkDiv) : (256);
    *(toutParamPtr + 2) = timeUnit;
}


/*
 *  Reads parameters needed for calculation of PRx value from local structure
 */
INLINE static TmrToutParam_t TimeoutParamRead(TmrSfr_t *const tmrSfr)
{
    /* Offset index = (&TMRx - &TMR1) / (TMR(x+1) - TMRx) */
    uint32_t regOffset = ( (uint32_t)&tmrSfr->TMRxCON.W - (uint32_t)&TMR1_MODULE.TMRxCON.W ) / 0x200;
    
    uint32_t structRegOffset = regOffset * sizeof(TmrToutParam_t);
    uint32_t structRegBase = (uint32_t)&toutParam.t1.sysClk;
    
    /* Pointer to x-th member of structure (for TMRx timeout parameters) */
    volatile uint32_t *toutParamPtr = (uint32_t *)(structRegBase + structRegOffset);
    
    TmrToutParam_t toutParam;
    
    /* Read members through pointer */
    toutParam.sysClk = *(toutParamPtr + 0);
    toutParam.clkDiv = *(toutParamPtr + 1);
    toutParam.timeUnit = *(toutParamPtr + 2);
    
    return toutParam;
}


/*
 *  Sets flags read in ISR for checking if 32-bit or continuous gate mode is set
 */
INLINE static void IsrFlagSet(TmrSfr_t *const tmrSfr, bool isMode32, bool isGateCont)
{
    /* Offset index = (&TMRx - &TMR1) / (TMR(x+1) - TMRx) */
    uint32_t regOffset = ( (uint32_t)&tmrSfr->TMRxCON.W - (uint32_t)&TMR1_MODULE.TMRxCON.W ) / 0x200;
    
    uint32_t structRegOffset = regOffset * sizeof(TmrStatus_t);
    uint32_t structRegBase = (uint32_t)&isrFlag.t1.isMode32;
    
    /* Pointer to x-th member of structure (for TMRx configuration flags) */
    volatile uint8_t *configFlagPtr = (uint8_t *)(structRegBase + structRegOffset);
    
    /* Modify members through pointer */
    *(configFlagPtr + 0) = isMode32;
    *(configFlagPtr + 1) = isGateCont;
}


/*
 *  Empty default ISR handler
 */
static void IsrDefaultHandler(void)
{
    
}

/******************************************************************************/
/*-----------------------------ISR  Definitions-------------------------------*/
/******************************************************************************/

void __ISR(TIMER_1_VECTOR, TMR1_ISR_IPL) ISR_Tmr1(void)
{
    if( (icSfr->ICxIEC0.W & IC_T1IE_MASK) && (icSfr->ICxIFS0.W & IC_T1IF_MASK) )
    {   
        icSfr->ICxIFS0.CLR = IC_T1IF_MASK;
        
        /* Leave timer ON only in continuous gating mode */
        if( isrFlag.t1.isGateCont == false )
        {
            TMR1_MODULE.TMRxCON.CLR = TMR_ON_MASK;      // Timer OFF
        }
        
        /* User-defined function */
        Isr1HandlerPtr();
    }
}


void __ISR(TIMER_2_VECTOR, TMR2_ISR_IPL) ISR_Tmr2(void)
{
    if( (icSfr->ICxIEC0.W & IC_T2IE_MASK) && (icSfr->ICxIFS0.W & IC_T2IF_MASK) )
    {   
        icSfr->ICxIFS0.CLR = IC_T2IF_MASK;
                
        /* Leave timer ON only in continuous gating mode */
        if( isrFlag.t2.isGateCont == false )
        {
            TMR2_MODULE.TMRxCON.CLR = TMR_ON_MASK;      // Timer OFF
        }
        
        /* User-defined function */
        Isr2HandlerPtr();
    }
}

void __ISR(TIMER_3_VECTOR, TMR3_ISR_IPL) ISR_Tmr3(void)
{
    if( (icSfr->ICxIEC0.W & IC_T3IE_MASK) && (icSfr->ICxIFS0.W & IC_T3IF_MASK) )
    {   
        icSfr->ICxIFS0.CLR = IC_T3IF_MASK;
                
        /* 32-bit mode */
        if( TMR2_MODULE.TMRxCON.W & TMR_T32_MASK )
        {
            /* Leave timer ON only in continuous gating mode */
            if( isrFlag.t2.isGateCont == false )
            {
                TMR2_MODULE.TMRxCON.CLR = TMR_ON_MASK;  // Timer OFF
            }
        }
        /* 16-bit mode */
        else
        {
            /* Leave timer ON only in continuous gating mode */
            if( isrFlag.t3.isGateCont == false )
            {
                TMR3_MODULE.TMRxCON.CLR = TMR_ON_MASK;  // Timer OFF
            }
        }
        
        /* User-defined function */
        Isr3HandlerPtr();
    }
}

void __ISR(TIMER_4_VECTOR, TMR4_ISR_IPL) ISR_Tmr4(void)
{
    if( (icSfr->ICxIEC0.W & IC_T4IE_MASK) && (icSfr->ICxIFS0.W & IC_T4IF_MASK) )
    {   
        icSfr->ICxIFS0.CLR = IC_T4IF_MASK;
                
        /* Leave timer ON only in continuous gating mode */
        if( isrFlag.t4.isGateCont == false )
        {
            TMR4_MODULE.TMRxCON.CLR = TMR_ON_MASK;      // Timer OFF
        }
        
        /* User-defined function */
        Isr4HandlerPtr();
    }
}

void __ISR(TIMER_5_VECTOR, TMR5_ISR_IPL) ISR_Tmr5(void)
{
    if( (icSfr->ICxIEC0.W & IC_T5IE_MASK) && (icSfr->ICxIFS0.W & IC_T5IF_MASK) )
    {  
        icSfr->ICxIFS0.CLR = IC_T5IF_MASK;
                
        /* 32-bit mode */
        if( TMR4_MODULE.TMRxCON.W & TMR_T32_MASK )
        {
            /* Leave timer ON only in continuous gating mode */
            if( isrFlag.t4.isGateCont == false )
            {
                TMR4_MODULE.TMRxCON.CLR = TMR_ON_MASK;  // Timer OFF
            }
        }
        /* 16-bit mode */
        else
        {
            /* Leave timer ON only in continuous gating mode */
            if( isrFlag.t5.isGateCont == false )
            {
                TMR5_MODULE.TMRxCON.CLR = TMR_ON_MASK;  // Timer OFF
            }
        }
        
        /* User-defined function */
        Isr5HandlerPtr();
    }
}


/* Used as periodical 1ms triggered handler */
void __ISR(CORE_TIMER_VECTOR, CT_ISR_IPL) ISR_CoreTmr(void)
{
    /* Set next compare count */
    icSfr->ICxIFS0.CLR = IC_CTIF_MASK;
    uint32_t tick = ((_CP0_GET_COUNT() - _CP0_GET_COMPARE() + coreTimerPeriod / 2) \
                    / coreTimerPeriod + 1) * coreTimerPeriod;
    _CP0_SET_COMPARE(_CP0_GET_COUNT() + tick);
    
    /* User-defined functions */
    IsrTimeout1HandlerPtr();
    IsrTimeout2HandlerPtr();
    IsrTimeout3HandlerPtr();
    IsrTimeout4HandlerPtr();
    IsrTimeout5HandlerPtr();
    IsrTimeout6HandlerPtr();
    IsrTimeout7HandlerPtr();
    IsrTimeout8HandlerPtr();
    IsrTimeout9HandlerPtr();
}