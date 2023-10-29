#ifndef _OSC_SFR_H
#define _OSC_SFR_H

/** Standard libs **/
#include <stdint.h>

/** Custom libs **/
#include "Sfr_types.h"

/** NOTE: Following code are true for PICMX1xx series (28 pin) **/

/******************************************************************************/
/*--------------------------------SFR Addresses-------------------------------*/
/******************************************************************************/

/** OSC (Oscillator Control) base address **/
#define OSC_MODULE   (*(OscSfr_t *const)0xBF80F000)

/******************************************************************************/
/*---------------------------------Bit Masks----------------------------------*/
/******************************************************************************/

/* OSCxCON register */
#define OSC_OSWEN_POS       (0)
#define OSC_OSWEN_MASK      (1 << 0)
#define OSC_SOSCEN_POS      (1)
#define OSC_SOSCEN_MASK     (1 << 1)
#define OSC_CF_POS          (3)
#define OSC_CF_MASK         (1 << 3)
#define OSC_SLPEN_POS       (4)
#define OSC_SLPEN_MASK      (1 << 4)
#define OSC_SLOCK_POS       (5)
#define OSC_SLOCK_MASK      (1 << 5)
#define OSC_CLKLOCK_POS     (7)
#define OSC_CLKLOCK_MASK    (1 << 7)
#define OSC_NOSC_POS        (8)
#define OSC_NOSC_MASK       (7 << 8)
#define OSC_COSC_POS        (12)
#define OSC_COSC_MASK       (7 << 12)
#define OSC_PLLMULT_POS     (16)
#define OSC_PLLMULT_MASK    (7 << 16)
#define OSC_PBDIV_POS       (19)
#define OSC_PBDIV_MASK      (3 << 19)
#define OSC_PBDIVRDY_POS    (21)
#define OSC_PBDIVRDY_MASK   (1 << 21)
#define OSC_SOSCRDY_POS     (22)
#define OSC_SOSCRDY_MASK    (1 << 22)
#define OSC_FRCDIV_POS      (24)
#define OSC_FRCDIV_MASK     (7 << 24)
#define OSC_PLLODIV_POS     (27)
#define OSC_PLLODIV_MASK    (7 << 27)

/* OSCxTUN register */
#define OSC_TUN_POS         (0)
#define OSC_TUN_MASK        (0x3F << 0)

/* REFOxCON register */
#define OSC_ROSEL_POS       (0)
#define OSC_ROSEL_MASK      (15 << 0)
#define OSC_ACTIVE_POS      (8)
#define OSC_ACTIVE_MASK     (1 << 8)
#define OSC_DIVSWEN_POS     (9)
#define OSC_DIVSWEN_MASK    (1 << 9)
#define OSC_RSLP_POS        (11)
#define OSC_RSLP_MASK       (1 << 11)
#define OSC_OE_POS          (12)
#define OSC_OE_MASK         (1 << 12)
#define OSC_SIDL_POS        (13)
#define OSC_SIDL_MASK       (1 << 13)
#define OSC_ON_POS          (15)
#define OSC_ON_MASK         (1 << 15)
#define OSC_RODIV_POS       (16)
#define OSC_RODIV_MASK      (0x7FFF << 16)

/* REFOxTRIM register */
#define OSC_ROTRIM_POS      (23)
#define OSC_ROTRIM_MASK     (0x1FF << 23)

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** OSC registers **/
typedef struct {
    Sfr_t       OSCxCON;
    Sfr_t       OSCxTUN;
    Sfr_t       REFOxCON;
    Sfr_t       REFOxTRIM;
} volatile OscSfr_t;

#endif /* OSC_SFR.H */