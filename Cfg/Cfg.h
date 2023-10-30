#ifndef CFG_H
#define	CFG_H

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

/** Standard libs **/
#include <stdio.h>
#include <stdbool.h>

/** Custom libs **/
#include "Cfg_sfr.h"
#include "Ic.h"

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
/*----------------------------Enumeration Types-------------------------------*/
/******************************************************************************/

typedef enum {
    CFG_FPLLIDIV_1 = 0,
    CFG_FPLLIDIV_2 = 1,
    CFG_FPLLIDIV_3 = 2,
    CFG_FPLLIDIV_4 = 3,
    CFG_FPLLIDIV_5 = 4,
    CFG_FPLLIDIV_6 = 5,
    CFG_FPLLIDIV_10 = 6,
    CFG_FPLLIDIV_12 = 7
} CfgPllInDiv_t;

/******************************************************************************/
/*---------------------------- Function Prototypes----------------------------*/
/******************************************************************************/

/* System registers unlock functions */
INLINE volatile uint32_t CFG_UnlockSystemAccess(void);
INLINE volatile void CFG_LockSystemAccess(uint32_t intrStatus);

/* Peripheral Pin Select registers unlock functions */
INLINE volatile uint32_t CFG_UnlockPpsAccess(void);
INLINE volatile void CFG_LockPpsAccess(uint32_t intrStatus);

/******************************************************************************/
/*-----------------------------Function In-lines------------------------------*/
/******************************************************************************/

/** 
 *  Perform system unlock sequence (temporarily disables interrupts and DMA)
 **/
INLINE volatile uint32_t CFG_UnlockSystemAccess(void)
{
    DrcSfr_t *const drcSfr = &DRC_MODULE;
    
    /* Obtain old interrupt status and disable interrupts */
    volatile uint32_t intrStatus = IC_GetInterruptState();
    IC_DisableInterrupts();

    /* Suspend DMA */
    DMA_DMACONSET = DMA_SUSPEND_MASK;
    while( (DMA_DMACON & DMA_DMABUSY_MASK) == 1 );

    /* Unlock sequence */
    drcSfr->SYSxKEY.W = 0x00000000;
    drcSfr->SYSxKEY.W = 0xAA996655;
    drcSfr->SYSxKEY.W = 0x556699AA;
    
    return intrStatus;
}


/** 
 *  Perform system lock sequence (re-enables interrupts if needed and DMA)
 **/
INLINE volatile void CFG_LockSystemAccess(uint32_t intrStatus)
{
    DrcSfr_t *const drcSfr = &DRC_MODULE;
    
    /* Lock sequence */
    drcSfr->SYSxKEY.W = 0x33333333;
    
    /* DMA operates normally */
    DMA_DMACONCLR = DMA_SUSPEND_MASK;
    
    /* Restore interrupt state */
    IC_SetInterruptState(intrStatus);
}


/** 
 *  Perform PPS unlock (temporarily disables interrupts and DMA)
 * 
 *  NOTE: Multiple PPS reconfigurations modifiable only during device programming
 **/
INLINE volatile uint32_t CFG_UnlockPpsAccess(void)
{
    DrcSfr_t *const drcSfr = &DRC_MODULE;
    
    uint32_t intrStatus = CFG_UnlockSystemAccess();
    
    /* Unlock PPS access */
    drcSfr->CFGxCON.W &= ~DRC_IOLOCK_MASK;
    
    return intrStatus;
}


/** 
 *  Perform PPS lock (re-enables interrupts if needed and DMA)
 **/
INLINE volatile void CFG_LockPpsAccess(uint32_t intrStatus)
{
    DrcSfr_t *const drcSfr = &DRC_MODULE;
    
    /* Lock PPS access */
    drcSfr->CFGxCON.W |= DRC_IOLOCK_MASK;
    
    CFG_LockSystemAccess(intrStatus);
}

#endif	/* CFG_H */