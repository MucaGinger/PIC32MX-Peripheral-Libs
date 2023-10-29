#ifndef SPI_SFR_H
#define	SPI_SFR_H

/** Standard libs **/
#include <stdint.h>

/** Custom libs **/
#include "Sfr_types.h"


/** NOTE: Following code are true for PICMX1xx series (28 pin) **/

/******************************************************************************/
/*--------------------------------SFR Addresses-------------------------------*/
/******************************************************************************/

/** SPI (Serial Peripheral Interface) base address **/
#define SPI1_MODULE     (*(SpiSfr_t *const)0xBF805800)
#define SPI2_MODULE     (*(SpiSfr_t *const)0xBF805A00)


/******************************************************************************/
/*---------------------------------Bit Masks----------------------------------*/
/******************************************************************************/

/* SPIxCON register */
#define SPI_SRXISEL_POS     (0)
#define SPI_SRXISEL_MASK    (3 << 0)
#define SPI_STXISEL_POS     (2)
#define SPI_STXISEL_MASK    (3 << 2)
#define SPI_DISSDI_POS      (4)
#define SPI_DISSDI_MASK     (1 << 4)
#define SPI_MSTEN_POS       (5)
#define SPI_MSTEN_MASK      (1 << 5)
#define SPI_CKP_POS         (6)
#define SPI_CKP_MASK        (1 << 6)
#define SPI_SSEN_POS        (7)
#define SPI_SSEN_MASK       (1 << 7)
#define SPI_CKE_POS         (8)
#define SPI_CKE_MASK        (1 << 8)
#define SPI_SMP_POS         (9)
#define SPI_SMP_MASK        (1 << 9)
#define SPI_MODE_POS        (10)
#define SPI_MODE_MASK       (3 << 10)
#define SPI_DISSDO_POS      (12)
#define SPI_DISSDO_MASK     (1 << 12)
#define SPI_SIDL_POS        (13)
#define SPI_SIDL_MASK       (1 << 13)
#define SPI_ON_POS          (15)
#define SPI_ON_MASK         (1 << 15)
#define SPI_ENHBUF_POS      (16)
#define SPI_ENHBUF_MASK     (1 << 16)
#define SPI_SPIFE_POS       (17)
#define SPI_SPIFE_MASK      (1 << 17)
#define SPI_MCLKSEL_POS     (23)
#define SPI_MCLKSEL_MASK    (1 << 23)
#define SPI_FRMCNT_POS      (24)
#define SPI_FRMCNT_MASK     (7 << 24)
#define SPI_FRMSYPW_POS     (27)
#define SPI_FRMSYPW_MASK    (1 << 27)
#define SPI_MSSEN_POS       (28)
#define SPI_MSSEN_MASK      (1 << 28)
#define SPI_FRMPOL_POS      (29)
#define SPI_FRMPOL_MASK     (1 << 29)
#define SPI_FRMSYNC_POS     (30)
#define SPI_FRMSYNC_MASK    (1 << 30)
#define SPI_FRMEN_POS       (31)
#define SPI_FRMEN_MASK      (1 << 31)

/* SPIxSTAT register */
#define SPI_SPIRBF_POS      (0)
#define SPI_SPIRBF_MASK     (1 << 0)
#define SPI_SPITBF_POS      (1)
#define SPI_SPITBF_MASK     (1 << 1)
#define SPI_SPITBE_POS      (3)
#define SPI_SPITBE_MASK     (1 << 3)
#define SPI_SPIRBE_POS      (5)
#define SPI_SPIRBE_MASK     (1 << 5)
#define SPI_SPIROV_POS      (6)
#define SPI_SPIROV_MASK     (1 << 6)
#define SPI_SRMT_POS        (7)
#define SPI_SRMT_MASK       (1 << 7)
#define SPI_SPITUR_POS      (8)
#define SPI_SPITUR_MASK     (1 << 8)
#define SPI_SPIBUSY_POS     (11)
#define SPI_SPIBUSY_MASK    (1 << 11)
#define SPI_FRMERR_POS      (12)
#define SPI_FRMERR_MASK     (1 << 12)
#define SPI_TXBUFELM_POS    (16)
#define SPI_TXBUFELM_MASK   (31 << 16)
#define SPI_RXBUFELM_POS    (24)
#define SPI_RXBUFELM_MASK   (31 << 24)

/* SPIxCON2 register */
#define SPI_AUDMOD_POS      (0)
#define SPI_AUDMOD_MASK     (3 << 0)
#define SPI_AUDMONO_POS     (3)
#define SPI_AUDMONO_MASK    (1 << 3)
#define SPI_AUDEN_POS       (7)
#define SPI_AUDEN_MASK      (1 << 7)
#define SPI_IGNTUR_POS      (8)
#define SPI_IGNTUR_MASK     (1 << 8)
#define SPI_IGNROV_POS      (9)
#define SPI_IGNROV_MASK     (1 << 9)
#define SPI_SPITUREN_POS    (10)
#define SPI_SPITUREN_MASK   (1 << 10)
#define SPI_SPIROVEN_POS    (11)
#define SPI_SPIROVEN_MASK   (1 << 11)
#define SPI_FRMERREN_POS    (12)
#define SPI_FRMERREN_MASK   (1 << 12)
#define SPI_SPISGNEXT_POS   (15)
#define SPI_SPISGNEXT_MASK  (1 << 15)

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** SPI registers **/
typedef struct {
    Sfr_t       SPIxCON;
    Sfr_t       SPIxSTAT;
    Word_t      SPIxBUF;    /* Writing to this bit is not atomic! */
    Rsrvd_t     r1[3];
    Sfr_t       SPIxBRG;
    Sfr_t       SPIxCON2;
} volatile SpiSfr_t;


#endif	/* SPI_SFR_H */

