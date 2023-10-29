#ifndef IC_SFR_H
#define	IC_SFR_H

/** Standard libs **/
#include <stdint.h>

/** Custom libs **/
#include "Sfr_types.h"


/** NOTE: Following code are true for PICMX1xx series (28 pin) **/

/******************************************************************************/
/*--------------------------------SFR Addresses-------------------------------*/
/******************************************************************************/

/** IC (Interrupt Controller) base address **/
#define IC_MODULE   (*(IcSfr_t *const)0xBF881000)

/******************************************************************************/
/*------------------------------Interrupt vectors-----------------------------*/
/******************************************************************************/

#define CORE_TIMER_VECTOR                        0
#define CORE_SOFTWARE_0_VECTOR                   1
#define CORE_SOFTWARE_1_VECTOR                   2
#define EXTERNAL_0_VECTOR                        3
#define TIMER_1_VECTOR                           4
#define INPUT_CAPTURE_1_VECTOR                   5
#define OUTPUT_COMPARE_1_VECTOR                  6
#define EXTERNAL_1_VECTOR                        7
#define TIMER_2_VECTOR                           8
#define INPUT_CAPTURE_2_VECTOR                   9
#define OUTPUT_COMPARE_2_VECTOR                  10
#define EXTERNAL_2_VECTOR                        11
#define TIMER_3_VECTOR                           12
#define INPUT_CAPTURE_3_VECTOR                   13
#define OUTPUT_COMPARE_3_VECTOR                  14
#define EXTERNAL_3_VECTOR                        15
#define TIMER_4_VECTOR                           16
#define INPUT_CAPTURE_4_VECTOR                   17
#define OUTPUT_COMPARE_4_VECTOR                  18
#define EXTERNAL_4_VECTOR                        19
#define TIMER_5_VECTOR                           20
#define INPUT_CAPTURE_5_VECTOR                   21
#define OUTPUT_COMPARE_5_VECTOR                  22
#define ADC_VECTOR                               23
#define FAIL_SAFE_MONITOR_VECTOR                 24
#define RTCC_VECTOR                              25
#define FCE_VECTOR                               26
#define COMPARATOR_1_VECTOR                      27
#define COMPARATOR_2_VECTOR                      28
#define COMPARATOR_3_VECTOR                      29
#define SPI_1_VECTOR                             31
#define UART_1_VECTOR                            32
#define I2C_1_VECTOR                             33
#define CHANGE_NOTICE_VECTOR                     34
#define PMP_VECTOR                               35
#define SPI_2_VECTOR                             36
#define UART_2_VECTOR                            37
#define I2C_2_VECTOR                             38
#define CTMU_VECTOR                              39
#define DMA_0_VECTOR                             40
#define DMA_1_VECTOR                             41
#define DMA_2_VECTOR                             42
#define DMA_3_VECTOR                             43

/******************************************************************************/
/*---------------------------------Bit Masks----------------------------------*/
/******************************************************************************/

/* ICxINTCON register */
#define IC_INT0EP_MASK      (1 << 0)
#define IC_INT1EP_MASK      (1 << 1)
#define IC_INT2EP_MASK      (1 << 2)
#define IC_INT3EP_MASK      (1 << 3)
#define IC_INT4EP_MASK      (1 << 4)
#define IC_TPC_POS          (24)
#define IC_MVEC_MASK        (1 << 12)

/* ICxINTSTAT register */
#define IC_VEC_POS          (0)
#define IC_SRIPL_POS        (8)

/* ICxIFS0 register */
#define IC_CTIF_MASK        (1 << 0)
#define IC_CS0IF_MASK       (1 << 1)
#define IC_CS1IF_MASK       (1 << 2)
#define IC_INT0IF_MASK      (1 << 3)
#define IC_T1IF_MASK        (1 << 4)
#define IC_IC1EIF_MASK      (1 << 5)
#define IC_IC1IF_MASK       (1 << 6)
#define IC_OC1IF_MASK       (1 << 7)
#define IC_INT1IF_MASK      (1 << 8)
#define IC_T2IF_MASK        (1 << 9)
#define IC_IC2EIF_MASK      (1 << 10)
#define IC_IC2IF_MASK       (1 << 11)
#define IC_OC2IF_MASK       (1 << 12)
#define IC_INT2IF_MASK      (1 << 13)
#define IC_T3IF_MASK        (1 << 14)
#define IC_IC3EIF_MASK      (1 << 15)
#define IC_IC3IF_MASK       (1 << 16)
#define IC_OC3IF_MASK       (1 << 17)
#define IC_INT3IF_MASK      (1 << 18)
#define IC_T4IF_MASK        (1 << 19)
#define IC_IC4EIF_MASK      (1 << 20)
#define IC_IC4IF_MASK       (1 << 21)
#define IC_OC4IF_MASK       (1 << 22)
#define IC_INT4IF_MASK      (1 << 23)
#define IC_T5IF_MASK        (1 << 24)
#define IC_IC5EIF_MASK      (1 << 25)
#define IC_IC5IF_MASK       (1 << 26)
#define IC_OC5IF_MASK       (1 << 27)
#define IC_AD1IF_MASK       (1 << 28)
#define IC_FSCMIF_MASK      (1 << 29)
#define IC_RTCCIF_MASK      (1 << 30)
#define IC_FCEIF_MASK       (1 << 31)

/* ICxIFS1 register */
#define IC_CMP1IF_MASK      (1 << 0)
#define IC_CMP2IF_MASK      (1 << 1)
#define IC_CMP3IF_MASK      (1 << 2)
#define IC_SPI1EIF_MASK     (1 << 4)
#define IC_SPI1RXIF_MASK    (1 << 5)
#define IC_SPI1TXIF_MASK    (1 << 6)
#define IC_U1EIF_MASK       (1 << 7)
#define IC_U1RXIF_MASK      (1 << 8)
#define IC_U1TXIF_MASK      (1 << 9)
#define IC_I2C1BIF_MASK     (1 << 10)
#define IC_I2C1SIF_MASK     (1 << 11)
#define IC_I2C1MIF_MASK     (1 << 12)
#define IC_CNAIF_MASK       (1 << 13)
#define IC_CNBIF_MASK       (1 << 14)
#define IC_CNCIF_MASK       (1 << 15)
#define IC_PMPIF_MASK       (1 << 16)
#define IC_PMPEIF_MASK      (1 << 17)
#define IC_SPI2EIF_MASK     (1 << 18)
#define IC_SPI2RXIF_MASK    (1 << 19)
#define IC_SPI2TXIF_MASK    (1 << 20)
#define IC_U2EIF_MASK       (1 << 21)
#define IC_U2RXIF_MASK      (1 << 22)
#define IC_U2TXIF_MASK      (1 << 23)
#define IC_I2C2BIF_MASK     (1 << 24)
#define IC_I2C2SIF_MASK     (1 << 25)
#define IC_I2C2MIF_MASK     (1 << 26)
#define IC_CTMUIF_MASK      (1 << 27)
#define IC_DMA0IF_MASK      (1 << 28)
#define IC_DMA1IF_MASK      (1 << 29)
#define IC_DMA2IF_MASK      (1 << 30)
#define IC_DMA3IF_MASK      (1 << 31)

/* ICxIEC0 register */
#define IC_CTIE_MASK        (1 << 0)
#define IC_CS0IE_MASK       (1 << 1)
#define IC_CS1IE_MASK       (1 << 2)
#define IC_INT0IE_MASK      (1 << 3)
#define IC_T1IE_MASK        (1 << 4)
#define IC_IC1EIE_MASK      (1 << 5)
#define IC_IC1IE_MASK       (1 << 6)
#define IC_OC1IE_MASK       (1 << 7)
#define IC_INT1IE_MASK      (1 << 8)
#define IC_T2IE_MASK        (1 << 9)
#define IC_IC2EIE_MASK      (1 << 10)
#define IC_IC2IE_MASK       (1 << 11)
#define IC_OC2IE_MASK       (1 << 12)
#define IC_INT2IE_MASK      (1 << 13)
#define IC_T3IE_MASK        (1 << 14)
#define IC_IC3EIE_MASK      (1 << 15)
#define IC_IC3IE_MASK       (1 << 16)
#define IC_OC3IE_MASK       (1 << 17)
#define IC_INT3IE_MASK      (1 << 18)
#define IC_T4IE_MASK        (1 << 19)
#define IC_IC4EIE_MASK      (1 << 20)
#define IC_IC4IE_MASK       (1 << 21)
#define IC_OC4IE_MASK       (1 << 22)
#define IC_INT4IE_MASK      (1 << 23)
#define IC_T5IE_MASK        (1 << 24)
#define IC_IC5EIE_MASK      (1 << 25)
#define IC_IC5IE_MASK       (1 << 26)
#define IC_OC5IE_MASK       (1 << 27)
#define IC_AD1IE_MASK       (1 << 28)
#define IC_FSCMIE_MASK      (1 << 29)
#define IC_RTCCIE_MASK      (1 << 30)
#define IC_FCEIE_MASK       (1 << 31)

/* ICxIEC1 register */
#define IC_CMP1IE_MASK      (1 << 0)
#define IC_CMP2IE_MASK      (1 << 1)
#define IC_CMP3IE_MASK      (1 << 2)
#define IC_SPI1EIE_MASK     (1 << 4)
#define IC_SPI1RXIE_MASK    (1 << 5)
#define IC_SPI1TXIE_MASK    (1 << 6)
#define IC_U1EIE_MASK       (1 << 7)
#define IC_U1RXIE_MASK      (1 << 8)
#define IC_U1TXIE_MASK      (1 << 9)
#define IC_I2C1BIE_MASK     (1 << 10)
#define IC_I2C1SIE_MASK     (1 << 11)
#define IC_I2C1MIE_MASK     (1 << 12)
#define IC_CNAIE_MASK       (1 << 13)
#define IC_CNBIE_MASK       (1 << 14)
#define IC_CNCIE_MASK       (1 << 15)
#define IC_PMPIE_MASK       (1 << 16)
#define IC_PMPEIE_MASK      (1 << 17)
#define IC_SPI2EIE_MASK     (1 << 18)
#define IC_SPI2RXIE_MASK    (1 << 19)
#define IC_SPI2TXIE_MASK    (1 << 20)
#define IC_U2EIE_MASK       (1 << 21)
#define IC_U2RXIE_MASK      (1 << 22)
#define IC_U2TXIE_MASK      (1 << 23)
#define IC_I2C2BIE_MASK     (1 << 24)
#define IC_I2C2SIE_MASK     (1 << 25)
#define IC_I2C2MIE_MASK     (1 << 26)
#define IC_CTMUIE_MASK      (1 << 27)
#define IC_DMA0IE_MASK      (1 << 28)
#define IC_DMA1IE_MASK      (1 << 29)
#define IC_DMA2IE_MASK      (1 << 30)
#define IC_DMA3IE_MASK      (1 << 31)

/* ICxIPC0 register */
#define IC_CTIS_POS         (0)
#define IC_CTIS_MASK        (3 << 0)
#define IC_CTIP_POS         (2)
#define IC_CTIP_MASK        (7 << 2)
#define IC_CS0IS_POS        (8)
#define IC_CS0IS_MASK       (3 << 8)
#define IC_CS0IP_POS        (10)
#define IC_CS0IP_MASK       (7 << 10)
#define IC_CS1IS_POS        (16)
#define IC_CS1IS_MASK       (3 << 16)
#define IC_CS1IP_POS        (18)
#define IC_CS1IP_MASK       (7 << 18)
#define IC_INT0IS_POS       (24)
#define IC_INT0IS_MASK      (3 << 24)
#define IC_INT0IP_POS       (26)
#define IC_INT0IP_MASK      (7 << 26)

/* ICxIPC1 register */
#define IC_T1IS_POS         (0)
#define IC_T1IS_MASK        (3 << 0)
#define IC_T1IP_POS         (2)
#define IC_T1IP_MASK        (7 << 2)
#define IC_IC1IS_POS        (8)
#define IC_IC1IS_MASK       (3 << 8)
#define IC_IC1IP_POS        (10)
#define IC_IC1IP_MASK       (7 << 10)
#define IC_OC1IS_POS        (16)
#define IC_OC1IS_MASK       (3 << 16)
#define IC_OC1IP_POS        (18)
#define IC_OC1IP_MASK       (7 << 18)
#define IC_INT1IS_POS       (24)
#define IC_INT1IS_MASK      (3 << 24)
#define IC_INT1IP_POS       (26)
#define IC_INT1IP_MASK      (7 << 26)

/* ICxIPC2 register */
#define IC_T2IS_POS         (0)
#define IC_T2IS_MASK        (3 << 0)
#define IC_T2IP_POS         (2)
#define IC_T2IP_MASK        (7 << 2)
#define IC_IC2IS_POS        (8)
#define IC_IC2IS_MASK       (3 << 8)
#define IC_IC2IP_POS        (10)
#define IC_IC2IP_MASK       (7 << 10)
#define IC_OC2IS_POS        (16)
#define IC_OC2IS_MASK       (3 << 16)
#define IC_OC2IP_POS        (18)
#define IC_OC2IP_MASK       (7 << 18)
#define IC_INT2IS_POS       (24)
#define IC_INT2IS_MASK      (3 << 24)
#define IC_INT2IP_POS       (26)
#define IC_INT2IP_MASK      (7 << 26)

/* ICxIPC3 register */
#define IC_T3IS_POS         (0)
#define IC_T3IS_MASK        (3 << 0)
#define IC_T3IP_POS         (2)
#define IC_T3IP_MASK        (7 << 2)
#define IC_IC3IS_POS        (8)
#define IC_IC3IS_MASK       (3 << 8)
#define IC_IC3IP_POS        (10)
#define IC_IC3IP_MASK       (7 << 10)
#define IC_OC3IS_POS        (16)
#define IC_OC3IS_MASK       (3 << 16)
#define IC_OC3IP_POS        (18)
#define IC_OC3IP_MASK       (7 << 18)
#define IC_INT3IS_POS       (24)
#define IC_INT3IS_MASK      (3 << 24)
#define IC_INT3IP_POS       (26)
#define IC_INT3IP_MASK      (7 << 26)

/* ICxIPC4 register */
#define IC_T4IS_POS         (0)
#define IC_T4IS_MASK        (3 << 0)
#define IC_T4IP_POS         (2)
#define IC_T4IP_MASK        (7 << 2)
#define IC_IC4IS_POS        (8)
#define IC_IC4IS_MASK       (3 << 8)
#define IC_IC4IP_POS        (10)
#define IC_IC4IP_MASK       (7 << 10)
#define IC_OC4IS_POS        (16)
#define IC_OC4IS_MASK       (3 << 16)
#define IC_OC4IP_POS        (18)
#define IC_OC4IP_MASK       (7 << 18)
#define IC_INT4IS_POS       (24)
#define IC_INT4IS_MASK      (3 << 24)
#define IC_INT4IP_POS       (26)
#define IC_INT4IP_MASK      (7 << 26)

/* ICxIPC5 register */
#define IC_T5IS_POS         (0)
#define IC_T5IS_MASK        (3 << 0)
#define IC_T5IP_POS         (2)
#define IC_T5IP_MASK        (7 << 2)
#define IC_IC5IS_POS        (8)
#define IC_IC5IS_MASK       (3 << 8)
#define IC_IC5IP_POS        (10)
#define IC_IC5IP_MASK       (7 << 10)
#define IC_OC5IS_POS        (16)
#define IC_OC5IS_MASK       (3 << 16)
#define IC_OC5IP_POS        (18)
#define IC_OC5IP_MASK       (7 << 18)
#define IC_ADI1IS_POS       (24)
#define IC_ADI1IS_MASK      (3 << 24)
#define IC_ADI1IP_POS       (26)
#define IC_ADI1IP_MASK      (7 << 26)

/* ICxIPC6 register */
#define IC_FSCMIS_POS       (0)
#define IC_FSCMIS_MASK      (3 << 0)
#define IC_FSCMIP_POS       (2)
#define IC_FSCMIP_MASK      (7 << 2)
#define IC_RTCCIS_POS       (8)
#define IC_RTCCIS_MASK      (3 << 8)
#define IC_RTCCIP_POS       (10)
#define IC_RTCCIP_MASK      (7 << 10)
#define IC_FCEIS_POS        (16)
#define IC_FCEIS_MASK       (3 << 16)
#define IC_FCEIP_POS        (18)
#define IC_FCEIP_MASK       (7 << 18)
#define IC_CMP1IS_POS       (24)
#define IC_CMP1IS_MASK      (3 << 24)
#define IC_CMP1IP_POS       (26)
#define IC_CMP1IP_MASK      (7 << 26)

/* ICxIPC7 register */
#define IC_CMP2IS_POS       (0)
#define IC_CMP2IS_MASK      (3 << 0)
#define IC_CMP2IP_POS       (2)
#define IC_CMP2IP_MASK      (7 << 2)
#define IC_CMP3IS_POS       (8)
#define IC_CMP3IS_MASK      (3 << 8)
#define IC_CMP3IP_POS       (10)
#define IC_CMP3IP_MASK      (7 << 10)
#define IC_SPI1IS_POS       (24)
#define IC_SPI1IS_MASK      (3 << 24)
#define IC_SPI1IP_POS       (26)
#define IC_SPI1IP_MASK      (7 << 26)

/* ICxIPC8 register */
#define IC_U1IS_POS         (0)
#define IC_U1IS_MASK        (3 << 0)
#define IC_U1IP_POS         (2)
#define IC_U1IP_MASK        (7 << 2)
#define IC_I2C1IS_POS       (8)
#define IC_I2C1IS_MASK      (3 << 8)
#define IC_I2C1IP_POS       (10)
#define IC_I2C1IP_MASK      (7 << 10)
#define IC_CNIS_POS         (16)
#define IC_CNIS_MASK        (3 << 16)
#define IC_CNIP_POS         (18)
#define IC_CNIP_MASK        (7 << 18)
#define IC_PMPIS_POS        (24)
#define IC_PMPIS_MASK       (3 << 24)
#define IC_PMPIP_POS        (26)
#define IC_PMPIP_MASK       (7 << 26)

/* ICxIPC9 register */
#define IC_SPI2IS_POS       (0)
#define IC_SPI2IS_MASK      (3 << 0)
#define IC_SPI2IP_POS       (2)
#define IC_SPI2IP_MASK      (7 << 2)
#define IC_U2IS_POS         (8)
#define IC_U2IS_MASK        (3 << 8)
#define IC_U2IP_POS         (10)
#define IC_U2IP_MASK        (7 << 10)
#define IC_I2C2IS_POS       (16)
#define IC_I2C2IS_MASK      (3 << 16)
#define IC_I2C2IP_POS       (18)
#define IC_I2C2IP_MASK      (7 << 18)
#define IC_CTMUIS_POS       (24)
#define IC_CTMUIS_MASK      (3 << 24)
#define IC_CTMUIP_POS       (26)
#define IC_CTMUIP_MASK      (7 << 26)

/* ICxIPC10 register */
#define IC_DMA0IS_POS       (0)
#define IC_DMA0IS_MASK      (3 << 0)
#define IC_DMA0IP_POS       (2)
#define IC_DMA0IP_MASK      (7 << 2)
#define IC_DMA1IS_POS       (8)
#define IC_DMA1IS_MASK      (3 << 8)
#define IC_DMA1IP_POS       (10)
#define IC_DMA1IP_MASK      (7 << 10)
#define IC_DMA2IS_POS       (16)
#define IC_DMA2IS_MASK      (3 << 16)
#define IC_DMA2IP_POS       (18)
#define IC_DMA2IP_MASK      (7 << 18)
#define IC_DMA3IS_POS       (24)
#define IC_DMA3IS_MASK      (3 << 24)
#define IC_DMA3IP_POS       (26)
#define IC_DMA3IP_MASK      (7 << 26)

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** NOTE: Members are added with "ICx" prefix to satisfy fixed naming convention
 *        and also to avoid incompatibility when working with Microchip device
 *        specific headers.
 **/

/** IC registers **/
typedef struct {
    Sfr_t       ICxINTCON;
    Word_t      ICxINTSTAT;
    Rsrvd_t     r1[3];
    Sfr_t       ICxIPTMR;
    Sfr_t       ICxIFS0;
    Sfr_t       ICxIFS1;
    Rsrvd_t     r2[4];
    Sfr_t       ICxIEC0;
    Sfr_t       ICxIEC1;
    Rsrvd_t     r3[4];
    Sfr_t       ICxIPC0;
    Sfr_t       ICxIPC1;
    Sfr_t       ICxIPC2;
    Sfr_t       ICxIPC3;
    Sfr_t       ICxIPC4;
    Sfr_t       ICxIPC5;
    Sfr_t       ICxIPC6;
    Sfr_t       ICxIPC7;
    Sfr_t       ICxIPC8;
    Sfr_t       ICxIPC9;
    Sfr_t       ICxIPC10;
} volatile IcSfr_t;

#endif	/* IC_SFR_H */

