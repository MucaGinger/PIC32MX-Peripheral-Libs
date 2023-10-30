#ifndef TMR_H
#define	TMR_H


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

/** Standard libs **/
#include <stdint.h>

/** Custom libs **/
#include "Tmr_sfr.h"
#include "Osc.h"
#include "Pio.h"


/******************************************************************************/
/*---------------------------------Macros-------------------------------------*/
/******************************************************************************/

/* Must set if delay is used */
#ifndef TMR_DELAY_SYSCLK
#define TMR_DELAY_SYSCLK    40000000
#endif


/********************User-defined interrupt vector priority********************/

/* NOTE: IPL = 0 means interrupt disabled. ISR_IPL level must equal ICX_IPL */
/* NOTE: Vectors of same priority and sub-priority are services in their
 *       natural order */

/* User-defined (sub)priority levels (IPL: 0-7, ISL: 0-3) */
#define TMR1_ISR_IPL    IPL1SOFT
#define TMR1_ICX_IPL    1
#define TMR1_ICX_ISL    0

#define TMR2_ISR_IPL    IPL1SOFT
#define TMR2_ICX_IPL    1
#define TMR2_ICX_ISL    0

#define TMR3_ISR_IPL    IPL1SOFT
#define TMR3_ICX_IPL    1
#define TMR3_ICX_ISL    0

#define TMR4_ISR_IPL    IPL1SOFT
#define TMR4_ICX_IPL    1
#define TMR4_ICX_ISL    0

#define TMR5_ISR_IPL    IPL1SOFT
#define TMR5_ICX_IPL    1
#define TMR5_ICX_ISL    0

#define CT_ISR_IPL      IPL7SOFT
#define CT_ICX_IPL      7
#define CT_ICX_ISL      0

/******************************************************************************/
/*----------------------------Enumeration Types-------------------------------*/
/******************************************************************************/

typedef enum {
    TMR_MODE_TIMEOUT = 0,
    TMR_MODE_GATED = 1
} TmrMode_t;

typedef enum {
    TMR_CLK_DIV_1 = 0,
    TMR_CLK_DIV_2 = 1,
    TMR_CLK_DIV_4 = 2,
    TMR_CLK_DIV_8 = 3,
    TMR_CLK_DIV_16 = 4,
    TMR_CLK_DIV_32 = 5,
    TMR_CLK_DIV_64 = 6,
    TMR_CLK_DIV_256 = 7
} TmrClkDiv_t;

typedef enum {
    TMR_CLK_SRC_PBCLK = 0,
    TMR_CLK_SRC_EXTCLK = 1,
    TMR_CLK_SRC_SOSC = 2
} TmrClkSource_t;

typedef enum {
    TMR_BITMODE_16BIT = 0,
    TMR_BITMODE_32BIT = 1
} TmrBitMode_t;

typedef enum {
    TMR_TIME_UNIT_S = 1,
    TMR_TIME_UNIT_MS = 1000,
    TMR_TIME_UNIT_US = 1000000    
} TmrTimeUnit_t;

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/* Timeout mode timer settings */
typedef struct {
    TmrBitMode_t    bitMode;
    TmrClkDiv_t     clkDiv;
    TmrClkSource_t  clkSrc;
    TmrTimeUnit_t   timeUnit;
} const TmrTimeoutConfig_t;

/* Gated mode timer settings */
typedef struct {
    TmrBitMode_t    bitMode;
    TmrClkDiv_t     clkDiv;
    uint32_t        tckPin;
    bool            isGateCont;
} const TmrGatedConfig_t;

/******************************************************************************/
/*-----------------------------Function Prototypes----------------------------*/
/******************************************************************************/

/* Timeout and gated mode configuration functions */
bool TMR_ConfigTimeoutModeSfr(TmrSfr_t *const tmrSfr, TmrTimeoutConfig_t tmrConfig);
bool TMR_ConfigGatedModeSfr(TmrSfr_t *const tmrSfr, TmrGatedConfig_t tmrConfig);
bool TMR_SetCallback(TmrSfr_t *const tmrSfr, void (*isrHandler)(void));
bool TMR_SetCoreTimerCallback(void (*isrHandler)(void));
bool TMR_SetTimeoutPeriod(TmrSfr_t *const tmrSfr, uint32_t period);

/* Timer operation functions */
INLINE void TMR_StartTimer(TmrSfr_t *const tmrSfr);
INLINE void TMR_StopTimer(TmrSfr_t *const tmrSfr);

/* Time delay functions */
INLINE void TMR_DelayUs(uint32_t delay);
INLINE void TMR_DelayMs(uint32_t delay);

/* Timer read count value function */
INLINE uint32_t TMR_ReadTimer(TmrSfr_t *const tmrSfr);
INLINE double TMR_ReadTimerPeriod(TmrSfr_t *const tmrSfr);


/******************************************************************************/
/*-----------------------------Function In-lines------------------------------*/
/******************************************************************************/

/*
 *  Starts timeout for specific timer
 */
INLINE void TMR_StartTimer(TmrSfr_t *const tmrSfr)
{
    /* Timer ON and clear counter */
    tmrSfr->TMRxTMR.CLR = 0xFFFFFFFF;
    tmrSfr->TMRxCON.SET = TMR_ON_MASK;
}


/*
 *  Stops timeout for specific timer (before ISR is reached)
 */
INLINE void TMR_StopTimer(TmrSfr_t *const tmrSfr)
{
    /* Timer OFF */
    tmrSfr->TMRxCON.CLR = TMR_ON_MASK;
}


/*
 *  Generates simple microsecond delay using Core Timer
 */
INLINE void TMR_DelayUs(uint32_t delay)
{
    /* Translate delay value to microseconds (timer updates every other SYSCLK) */
    delay = delay * (TMR_DELAY_SYSCLK / 1000000 / 2);
    uint32_t timeout = _CP0_GET_COUNT() + delay;
    
    /* Timer compare */
    while (timeout > _CP0_GET_COUNT());
}


 
/*
 *  Generates simple millisecond delay using Core Timer
 */
INLINE void TMR_DelayMs(uint32_t delay)
{
    /* Translate delay value to milliseconds (timer updates every other SYSCLK) */
    delay = delay * (TMR_DELAY_SYSCLK / 1000 / 2);
    uint32_t timeout = _CP0_GET_COUNT() + delay;
    
    /* Timer compare */
    while (timeout > _CP0_GET_COUNT());
}



/*
 *  Read TMRx value
 */
INLINE uint32_t TMR_ReadTimer(TmrSfr_t *const tmrSfr)
{
    return tmrSfr->TMRxTMR.W;
}


/*
 *  Calculates time period (in seconds) based on TMRx count value
 */
INLINE double TMR_ReadTimerPeriod(TmrSfr_t *const tmrSfr)
{
    uint32_t clkDiv = (tmrSfr->TMRxCON.W & TMR_TCKPS_MASK) >> TMR_TCKPS_POS;
    uint32_t div = 0;
    
    /* Only DIV_1/8/64/256 available for TMR1 */
    if( tmrSfr == &TMR1_MODULE )
    {
        switch( clkDiv & 0x03 )
        {
            case 0: div = 1; break;
            case 1: div = 8; break;
            case 2: div = 64; break;
            case 3: div = 256; break;
        }
    }
    else
    {
        div = (clkDiv <= 6) ? (1 << clkDiv) : (256);
    }
    
    return ( (double)( tmrSfr->TMRxTMR.W * div) / (double)OSC_GetPbFreq() );
}


#endif	/* TMR_H */