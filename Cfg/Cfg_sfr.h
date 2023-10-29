#ifndef CFG_SFR_H
#define	CFG_SFR_H

/** Standard libs **/
#include <stdint.h>

/** Custom libs **/
#include "Sfr_types.h"

/** NOTE: Following code are true for PICMX1xx series (28 pin) **/

/******************************************************************************/
/*--------------------------------SFR Addresses-------------------------------*/
/******************************************************************************/

/** CFG (Configuration) base address **/
#define CFG_MODULE          (*(CfgSfr_t *const)0xBFC00BF0)
#define DRC_MODULE          (*(DrcSfr_t *const)0xBF80F200)

/********************************* TEMPORARY **********************************/
/** NOTE: Temporary definitions due to lack of DMA driver library **/

/** Used for suspending DMA temporarily (for (un)lock sequence) **/
#define DMA_DMACON          (*(volatile uint32_t *const)0xBF883000)
#define DMA_DMACONCLR       (*(volatile uint32_t *const)0xBF883004)
#define DMA_DMACONSET       (*(volatile uint32_t *const)0xBF883008)
#define DMA_DMACONINV       (*(volatile uint32_t *const)0xBF88300C)

/* DMACON register */
#define DMA_DMABUSY_MASK    (1 << 11)
#define DMA_SUSPEND_MASK    (1 << 12)
/********************************* TEMPORARY **********************************/

/******************************************************************************/
/*---------------------------------Bit Masks----------------------------------*/
/******************************************************************************/

/* DEVxCFG3 register */
#define CFG_USERID_POS      (0)
#define CFG_USERID_MASK     (0xFFFF << 0)
#define CFG_PMDL1WAY_POS    (28)
#define CFG_PMDL1WAY_MASK   (1 << 28)
#define CFG_IOL1WAY_POS     (29)
#define CFG_IOL1WAY_MASK    (1 << 29)
#define CFG_FUSBIDIO_POS    (30)
#define CFG_FUSBISIO_MASK   (1 << 30)
#define CFG_FVBUSONIO_POS   (31)
#define CFG_FVBUSONIO_MASK  (1 << 31)

/* DEVxCFG2 register */
#define CFG_FPLLIDIV_POS    (0)
#define CFG_FPLLIDIV_MASK   (7 << 0)
#define CFG_FPLLMUL_POS     (4)
#define CFG_FPLLMUL_MASK    (7 << 4)
#define CFG_FPLLODIV_POS    (16)
#define CFG_FPLLODIV_MASK   (7 << 16)

/* DEVxCFG1 register */
#define CFG_FNOSC_POS       (0)
#define CFG_FNOSC_MASK      (7 << 0)
#define CFG_FSOSCEN_POS     (5)
#define CFG_FSOSCEN_MASK    (1 << 5)
#define CFG_IESO_POS        (7)
#define CFG_IESO_MASK       (1 << 7)
#define CFG_POSCMOD_POS     (8)
#define CFG_POSCMOD_MASK    (3 << 8)
#define CFG_OSCIOFNC_POS    (10)
#define CFG_OSCIOFNC_MASK   (1 << 10)
#define CFG_FPBDIV_POS      (12)
#define CFG_FPBDIV_MASK     (3 << 12)
#define CFG_FCKSM_POS       (14)
#define CFG_FCKSM_MASK      (3 << 14)
#define CFG_FCKSM1_MASK     (2 << 14)
#define CFG_WDTPS_POS       (16)
#define CFG_WDTPS_MASK      (0x1F << 16)
#define CFG_WINDIS_POS      (22)
#define CFG_WINDIS_MASK     (1 << 22)
#define CFG_FWDTEN_POS      (23)
#define CFG_FWDTEN_MASK     (1 << 23)
#define CFG_FWDTWINSZ_POS   (24)
#define CFG_FWDTWINSZ_MASK  (3 << 24)

/* DEVxCFG0 register */
#define CFG_DEBUG_POS       (0)
#define CFG_DEBUG_MASK      (3 << 0)
#define CFG_JTAG_POS        (2)
#define CFG_JTAG_MASK       (1 << 2)
#define CFG_ICESEL_POS      (3)
#define CFG_ICESEL_MASK     (3 << 3)
#define CFG_PWP_POS         (10)
#define CFG_PWP_MASK        (0x3F << 10)
#define CFG_BWP_POS         (24)
#define CFG_BWP_MASK        (1 << 24)
#define CFG_CP_POS          (28)
#define CFG_CP_MASK         (1 << 28)

/* DEVxID register */
#define DRC_DEVID_POS       (0)
#define DRC_DEVID_MASK      (0x0FFFFFFF << 0)
#define DRC_VER_POS         (28)
#define DRC_VER_MASK        (15 << 28)

/* CFGxCON register */
#define DRC_TDOEN_POS       (0)
#define DRC_TDOEN_MASK      (1 << 0)
#define DRC_JTAGEN_POS      (3)
#define DRC_JTAGEN_MASK     (1 << 3)
#define DRC_PMDLOCK_POS     (12)
#define DRC_PMDLOCK_MASK    (1 << 12)
#define DRC_IOLOCK_POS      (13)
#define DRC_IOLOCK_MASK     (1 << 13)

/* SYSxKEY register */
#define DRC_SYSKEY_POS      (0)
#define DRC_SYSKEY_MASK     (0xFFFFFFFF << 0)

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** CFG registers **/
typedef const struct {
    Word_t      DEVxCFG3;
    Word_t      DEVxCFG2;
    Word_t      DEVxCFG1;
    Word_t      DEVxCFG0;
} volatile CfgSfr_t;

/** DRC (Device ID, Revision and Configuration) registers **/
typedef struct {
    Word_t      CFGxCON;
    Rsrvd_t     r1[7];
    Word_t      DEVxID;
    Rsrvd_t     r2[3];
    Word_t      SYSxKEY;
} volatile DrcSfr_t;

#endif	/* CFG_SFR_H */

