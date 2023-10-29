#ifndef IC_H
#define	IC_H

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

/** Standard libs **/
#include <stdio.h>
#include <stdbool.h>

/** Compiler libs **/
#include <xc.h>
#include <cp0defs.h>
#include <sys/attribs.h>

/** Custom libs **/
#include "Ic_sfr.h"

/******************************************************************************/
/*---------------------------------Macros-------------------------------------*/
/******************************************************************************/

#ifndef INLINE
#define INLINE  inline __attribute__ ((always_inline))
#endif

#ifndef bool
#define bool  bool
#endif


/******************************************************************************/
/*-----------------------------Function Prototypes----------------------------*/
/******************************************************************************/

INLINE void IC_EnableInterrupts(void);
INLINE void IC_DisableInterrupts(void);
INLINE void IC_SetInterruptState(volatile uint32_t intrState);
INLINE volatile uint32_t IC_GetInterruptState(void);


/******************************************************************************/
/*-----------------------------Function In-lines------------------------------*/
/******************************************************************************/

/*
 *  Enables interrupts (all sources can be triggered)
 */
INLINE void IC_EnableInterrupts(void)
{
    __builtin_enable_interrupts();
}


/*
 *  Disables interrupts (neither of sources can be triggered)
 */
INLINE void IC_DisableInterrupts(void)
{
    __builtin_disable_interrupts();
}


/*
 *  Restores interrupt state (use with IC_GetInterruptState())
 */
INLINE void IC_SetInterruptState(volatile uint32_t intrState)
{
    __builtin_set_isr_state(intrState);
}


/*
 *  Reads state of interrupts (enabled or disabled)
 */
INLINE volatile uint32_t IC_GetInterruptState(void)
{
    return __builtin_get_isr_state();
}


#endif	/* IC_H */

