#ifndef TMR_SFR_H
#define	TMR_SFR_H

/** Standard libs **/
#include <stdint.h>

/** Custom libs **/
#include "Sfr_types.h"


/** NOTE: Following code are true for PICMX1xx series (28 pin) **/

/******************************************************************************/
/*--------------------------------SFR Addresses-------------------------------*/
/******************************************************************************/

/** TMR (Timer) base addresses **/
#define TMR1_MODULE         (*(TmrSfr_t *const)0xBF800600)
#define TMR2_MODULE         (*(TmrSfr_t *const)0xBF800800)
#define TMR3_MODULE         (*(TmrSfr_t *const)0xBF800A00)
#define TMR4_MODULE         (*(TmrSfr_t *const)0xBF800C00)
#define TMR5_MODULE         (*(TmrSfr_t *const)0xBF800E00)


/******************************************************************************/
/*---------------------------------Bit Masks----------------------------------*/
/******************************************************************************/

/* TMRxCON register */
#define TMR_TCS_POS         (1)
#define TMR_TCS_MASK        (1 << 1)
#define TMR_TSYNC_POS       (2)
#define TMR_TSYNC_MASK      (1 << 2)
#define TMR_T32_POS         (3)
#define TMR_T32_MASK        (1 << 3)
#define TMR_TCKPS1_POS      (4)
#define TMR_TCKPS1_MASK     (3 << 4)    
#define TMR_TCKPS_POS       (4)
#define TMR_TCKPS_MASK      (7 << 4)
#define TMR_TGATE_POS       (7)
#define TMR_TGATE_MASK      (1 << 7)
#define TMR_TWIP_POS        (11)
#define TMR_TWIP_MASK       (1 << 11)
#define TMR_TWDIS_POS       (12)
#define TMR_TWDIS_MASK      (1 << 12)
#define TMR_SIDL_POS        (13)
#define TMR_SIDL_MASK       (1 << 13)
#define TMR_ON_POS          (15)
#define TMR_ON_MASK         (1 << 15)


/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** NOTE: Members are added with "ICx" prefix to satisfy fixed naming convention
 *        and also to avoid incompatibility when working with Microchip device
 *        specific headers.
 **/

/** TMR registers **/
typedef struct {
    Sfr_t       TMRxCON;
    Sfr_t       TMRxTMR;
    Sfr_t       TMRxPR;
} volatile TmrSfr_t;


#endif	/* TMR_SFR_H */