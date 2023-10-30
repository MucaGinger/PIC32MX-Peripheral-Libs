#ifndef OSC_H
#define	OSC_H

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

/** Standard libs **/
#include <stdint.h>

/** Custom libs **/
#include "Osc_sfr.h"
#include "Cfg.h"

/******************************************************************************/
/*---------------------------------Macros-------------------------------------*/
/******************************************************************************/

/** Crystal frequency define **/
#ifndef OSC_XTAL_FREQ
#define OSC_XTAL_FREQ   8000000
#endif

/** Device specific constants (change only if other than PIC32MX device) **/
#define OSC_FRC_FREQ    8000000
#define OSC_LPRC_FREQ   31250
#define OSC_SOSC_FREQ   32768

#define OSC_SYSCLK_MAX  50000000

/******************************************************************************/
/*----------------------------Enumeration Types-------------------------------*/
/******************************************************************************/

typedef enum {
    OSC_COSC_FRC = 0,
    OSC_COSC_FRCPLL = 1,
    OSC_COSC_POSC = 2,
    OSC_COSC_POSCPLL = 3,
    OSC_COSC_SOSC = 4,
    OSC_COSC_LPRC = 5,
    OSC_COSC_FRCDIV16 = 6,
    OSC_COSC_FRCDIV = 7
} OscClkSource_t;

typedef enum {
    OSC_PLLODIV_1 = 0,
    OSC_PLLODIV_2 = 1,
    OSC_PLLODIV_4 = 2,
    OSC_PLLODIV_8 = 3,
    OSC_PLLODIV_16 = 4,
    OSC_PLLODIV_32 = 5,
    OSC_PLLODIV_64 = 6,
    OSC_PLLODIV_256 = 7
} OscPllOutDiv_t;

typedef enum {
    OSC_PLLMULT_15 = 0,
    OSC_PLLMULT_16 = 1,
    OSC_PLLMULT_17 = 2,
    OSC_PLLMULT_18 = 3,
    OSC_PLLMULT_19 = 4,
    OSC_PLLMULT_20 = 5,
    OSC_PLLMULT_21 = 6,
    OSC_PLLMULT_24 = 7
} OscPllMult_t;

typedef enum {
    OSC_FRCDIV_1 = 0,
    OSC_FRCDIV_2 = 1,
    OSC_FRCDIV_4 = 2,
    OSC_FRCDIV_8 = 3,
    OSC_FRCDIV_16 = 4,
    OSC_FRCDIV_32 = 5,
    OSC_FRCDIV_64 = 6,
    OSC_FRCDIV_256 = 7
} OscFrcDiv_t;

typedef enum {
    OSC_PBDIV_1 = 0,
    OSC_PBDIV_2 = 1,
    OSC_PBDIV_4 = 2,
    OSC_PBDIV_8 = 3
} OscPbDiv_t;

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/* Oscillator configuration structure */
typedef struct {
    OscClkSource_t  oscSource;
    uint32_t        sysFreq;
    uint32_t        pbFreq;
} OscConfig_t;

/******************************************************************************/
/*---------------------------- Function Prototypes----------------------------*/
/******************************************************************************/

/* Oscillator configure function */
bool OSC_ConfigOsc(OscConfig_t oscConfig);

/* Read clock value functions */
uint32_t OSC_GetSysFreq(void);
uint32_t OSC_GetPbFreq(void);
INLINE OscClkSource_t OSC_GetClkSource(void);

/******************************************************************************/
/*-----------------------------Function In-lines------------------------------*/
/******************************************************************************/

INLINE OscClkSource_t OSC_GetClkSource(void)
{
    return (OscClkSource_t)((OSC_MODULE.OSCxCON.W & OSC_COSC_MASK) >> OSC_COSC_POS);
}


#endif	/* OSC_H */

