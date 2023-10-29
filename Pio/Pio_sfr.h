#ifndef PIO_SFR_H
#define	PIO_SFR_H

/** Standard libs **/
#include <stdint.h>

/** Custom libs **/
#include "Sfr_types.h"


/** NOTE: Following code is true for PICMX1xx series (28 pin) **/

/******************************************************************************/
/*--------------------------------SFR Addresses-------------------------------*/
/******************************************************************************/

/** NOTE: For IO register I chose "PIO" prefix instead of "PORT" (as written in
 *        datasheet). This is because former makes more sense and latter is name
 *        of existing SFR.
 **/

/** PPS (Peripheral Pin Select) base address **/
#define PPS_MODULE      (*(PpsSfr_t *const)0xBF80FA04)

/** PIOx (Programmable Input/Output) base address **/
#define PIOA_MODULE     (*(PioSfr_t *const)0xBF886000)
#define PIOB_MODULE     (*(PioSfr_t *const)0xBF886100)


/******************************************************************************/
/*-----------------------PPS and PIO pin codes--------------------------------*/
/******************************************************************************/

/** NOTE:   These codes store following information:
 * 
 *          0x    ___          ___       ___   ___   ___   ___    ___   ___
 *                 |            |            |           |            |
 *             PIO_MODULE -   IN/OUT  - PIN_POSITION - PPS_CODE - PPS_REGISTER
 *                 |            |            |           |            |
 *             0x00/0x01   0x01/0x00    0x00 - 0x0F  reg. code   reg. offset
 **/

/** NOTE:   PPS_CODE/REGISTER field is only defined with non-GPIO related pins
 *          (0xFF used for detection of non-PPS register pin codes)
 * 
 *  NOTE:   IN/OUT field is only defined with non-GPIO related pins
 **/


/** General purpose input/output pins codes **/
#define GPIO_RPA0       0x0F00FFFF
#define GPIO_RPA1       0x0F01FFFF
#define GPIO_RPA2       0x0F02FFFF
#define GPIO_RPA3       0x0F03FFFF
#define GPIO_RPA4       0x0F04FFFF
#define GPIO_RPB0       0x1F00FFFF
#define GPIO_RPB1       0x1F01FFFF
#define GPIO_RPB2       0x1F02FFFF
#define GPIO_RPB3       0x1F03FFFF
#define GPIO_RPB4       0x1F04FFFF
#define GPIO_RPB5       0x1F05FFFF
#define GPIO_RPB6       0x1F06FFFF
#define GPIO_RPB7       0x1F07FFFF
#define GPIO_RPB8       0x1F08FFFF
#define GPIO_RPB9       0x1F09FFFF
#define GPIO_RPB10      0x1F0AFFFF
#define GPIO_RPB11      0x1F0BFFFF
#define GPIO_RPB13      0x1F0DFFFF
#define GPIO_RPB14      0x1F0EFFFF
#define GPIO_RPB15      0x1F0FFFFF

/** External Interrupt (IRQ) pin codes **/
#define INT0_RPB7       0x0107FFFF
/********************************/
#define INT1_RPA3       0x01030004
#define INT1_RPB0       0x11000204
#define INT1_RPB9       0x11090404
#define INT1_RPB10      0x110A0304
#define INT1_RPB14      0x110E0104
/********************************/
#define INT2_RPA2       0x01020008
#define INT2_RPA4       0x01040208
#define INT2_RPB2       0x11020408
#define INT2_RPB6       0x11060108
#define INT2_RPB13      0x110D0308
/********************************/
#define INT3_RPA1       0x0101000C
#define INT3_RPB1       0x1101020C
#define INT3_RPB5       0x1105010C
#define INT3_RPB8       0x1108040C
#define INT3_RPB11      0x110B030C
/********************************/
#define INT4_RPA0       0x01000010
#define INT4_RPB3       0x11030110
#define INT4_RPB4       0x11040210
#define INT4_RPB7       0x11070410
#define INT4_RPB15      0x110F0310

/** Timer Clock (TxCK) pin codes **/
#define T1CK_RPA4       0x0104FFFF
/********************************/
#define T2CK_RPA0       0x01000018
#define T2CK_RPB3       0x11030118
#define T2CK_RPB4       0x11040218
#define T2CK_RPB7       0x11070418
#define T2CK_RPB15      0x110F0318
/********************************/
#define T3CK_RPA1       0x0101001C
#define T3CK_RPB1       0x1101021C
#define T3CK_RPB5       0x1105011C
#define T3CK_RPB8       0x1108041C
#define T3CK_RPB11      0x110B031C
/********************************/
#define T4CK_RPA2       0x01020020
#define T4CK_RPA4       0x01040220
#define T4CK_RPB2       0x11020420
#define T4CK_RPB6       0x11060120
#define T4CK_RPB13      0x110D0320
/********************************/
#define T5CK_RPA3       0x01030024
#define T5CK_RPB0       0x11000224
#define T5CK_RPB9       0x11090424
#define T5CK_RPB10      0x110A0324
#define T5CK_RPB14      0x110E0124

/** Slave Select (SSx) pin codes **/
#define SS1_RPA0        0x01000088  
#define SS1_RPB3        0x11030188
#define SS1_RPB4        0x11040288
#define SS1_RPB7        0x11070488
#define SS1_RPB15       0x110F0388
/********************************/
#define SS2_RPA3        0x01030094
#define SS2_RPB0        0x11000294
#define SS2_RPB9        0x11090494
#define SS2_RPB10       0x110A0394
#define SS2_RPB14       0x110E0194

/** Serial Data Out (SDOx) pin codes **/
#define SDO1_RPA1       0x00010304 
#define SDO1_RPA2       0x00020308
#define SDO1_RPA4       0x00040310
#define SDO1_RPB1       0x10010330
#define SDO1_RPB2       0x10020334
#define SDO1_RPB5       0x10050340
#define SDO1_RPB6       0x10060344
#define SDO1_RPB8       0x1008034C
#define SDO1_RPB11      0x100B0358
#define SDO1_RPB13      0x100D0360
/********************************/
#define SDO2_RPA1       0x00010404 
#define SDO2_RPA2       0x00020408
#define SDO2_RPA4       0x00040410
#define SDO2_RPB1       0x10010430
#define SDO2_RPB2       0x10020434
#define SDO2_RPB5       0x10050440
#define SDO2_RPB6       0x10060444
#define SDO2_RPB8       0x1008044C
#define SDO2_RPB11      0x100B0458
#define SDO2_RPB13      0x100D0460

/** Serial Data In (SDIx) pin codes **/
#define SDI1_RPA1       0x01010084
#define SDI1_RPB1       0x11010284
#define SDI1_RPB5       0x11050184
#define SDI1_RPB8       0x11080484
#define SDI1_RPB11      0x110B0384
/********************************/
#define SDI2_RPA2       0x01020090
#define SDI2_RPA4       0x01040290
#define SDI2_RPB2       0x11020490
#define SDI2_RPB6       0x11060190
#define SDI2_RPB13      0x110D0390


/******************************************************************************/
/*---------------------------------Bit Masks----------------------------------*/
/******************************************************************************/

/** PORTA module **/

/* ANSELA register */
#define PIO_ANSA0_MASK      (1 << 0)
#define PIO_ANSA1_MASK      (1 << 1)

/* TRISA register */
#define PIO_TRISA0_MASK     (1 << 0)
#define PIO_TRISA1_MASK     (1 << 1)
#define PIO_TRISA2_MASK     (1 << 2)
#define PIO_TRISA3_MASK     (1 << 3)
#define PIO_TRISA4_MASK     (1 << 4)

/* PORTA register */
#define PIO_PORTA0_MASK     (1 << 0)
#define PIO_PORTA1_MASK     (1 << 1)
#define PIO_PORTA2_MASK     (1 << 2)
#define PIO_PORTA3_MASK     (1 << 3)
#define PIO_PORTA4_MASK     (1 << 4)

/* LATA register */
#define PIO_LATA0_MASK      (1 << 0)
#define PIO_LATA1_MASK      (1 << 1)
#define PIO_LATA2_MASK      (1 << 2)
#define PIO_LATA3_MASK      (1 << 3)
#define PIO_LATA4_MASK      (1 << 4)

/* ODCA register */
#define PIO_ODCA0_MASK      (1 << 0)
#define PIO_ODCA1_MASK      (1 << 1)
#define PIO_ODCA2_MASK      (1 << 2)
#define PIO_ODCA3_MASK      (1 << 3)
#define PIO_ODCA4_MASK      (1 << 4)

/* CNPUA register */
#define PIO_CNPUA0_MASK     (1 << 0)
#define PIO_CNPUA1_MASK     (1 << 1)
#define PIO_CNPUA2_MASK     (1 << 2)
#define PIO_CNPUA3_MASK     (1 << 3)
#define PIO_CNPUA4_MASK     (1 << 4)

/* CNPDA register */
#define PIO_CNPDA0_MASK     (1 << 0)
#define PIO_CNPDA1_MASK     (1 << 1)
#define PIO_CNPDA2_MASK     (1 << 2)
#define PIO_CNPDA3_MASK     (1 << 3)
#define PIO_CNPDA4_MASK     (1 << 4)

/* CNCONA register */
#define PIO_SIDL_MASK       (1 << 13)
#define PIO_ON_MASK         (1 << 15)

/* CNENA register */
#define PIO_CNIEA0_MASK     (1 << 0)
#define PIO_CNIEA1_MASK     (1 << 1)
#define PIO_CNIEA2_MASK     (1 << 2)
#define PIO_CNIEA3_MASK     (1 << 3)
#define PIO_CNIEA4_MASK     (1 << 4)

/* CNSTATA register */
#define PIO_CNSTATA0_MASK   (1 << 0)
#define PIO_CNSTATA1_MASK   (1 << 1)
#define PIO_CNSTATA2_MASK   (1 << 2)
#define PIO_CNSTATA3_MASK   (1 << 3)
#define PIO_CNSTATA4_MASK   (1 << 4)

/** PORTB module **/

/* ANSELB register */
#define PIO_ANSB0_MASK      (1 << 0)
#define PIO_ANSB1_MASK      (1 << 1)
#define PIO_ANSB2_MASK      (1 << 2)
#define PIO_ANSB3_MASK      (1 << 3)
#define PIO_ANSB12_MASK     (1 << 12)
#define PIO_ANSB13_MASK     (1 << 13)
#define PIO_ANSB14_MASK     (1 << 14)
#define PIO_ANSB15_MASK     (1 << 15)

/* TRISB register */
#define PIO_TRISB0_MASK     (1 << 0)
#define PIO_TRISB1_MASK     (1 << 1)
#define PIO_TRISB2_MASK     (1 << 2)
#define PIO_TRISB3_MASK     (1 << 3)
#define PIO_TRISB4_MASK     (1 << 4)
#define PIO_TRISB5_MASK     (1 << 5)
#define PIO_TRISB6_MASK     (1 << 6)
#define PIO_TRISB7_MASK     (1 << 7)
#define PIO_TRISB8_MASK     (1 << 8)
#define PIO_TRISB9_MASK     (1 << 9)
#define PIO_TRISB10_MASK    (1 << 10)
#define PIO_TRISB11_MASK    (1 << 11)
#define PIO_TRISB12_MASK    (1 << 12)
#define PIO_TRISB13_MASK    (1 << 13)
#define PIO_TRISB14_MASK    (1 << 14)
#define PIO_TRISB15_MASK    (1 << 15)

/* PORTB register */
#define PIO_PORTB0_MASK     (1 << 0)
#define PIO_PORTB1_MASK     (1 << 1)
#define PIO_PORTB2_MASK     (1 << 2)
#define PIO_PORTB3_MASK     (1 << 3)
#define PIO_PORTB4_MASK     (1 << 4)
#define PIO_PORTB5_MASK     (1 << 5)
#define PIO_PORTB6_MASK     (1 << 6)
#define PIO_PORTB7_MASK     (1 << 7)
#define PIO_PORTB8_MASK     (1 << 8)
#define PIO_PORTB9_MASK     (1 << 9)
#define PIO_PORTB10_MASK    (1 << 10)
#define PIO_PORTB11_MASK    (1 << 11)
#define PIO_PORTB12_MASK    (1 << 12)
#define PIO_PORTB13_MASK    (1 << 13)
#define PIO_PORTB14_MASK    (1 << 14)
#define PIO_PORTB15_MASK    (1 << 15)

/* LATB register */
#define PIO_LATB0_MASK      (1 << 0)
#define PIO_LATB1_MASK      (1 << 1)
#define PIO_LATB2_MASK      (1 << 2)
#define PIO_LATB3_MASK      (1 << 3)
#define PIO_LATB4_MASK      (1 << 4)
#define PIO_LATB5_MASK      (1 << 5)
#define PIO_LATB6_MASK      (1 << 6)
#define PIO_LATB7_MASK      (1 << 7)
#define PIO_LATB8_MASK      (1 << 8)
#define PIO_LATB9_MASK      (1 << 9)
#define PIO_LATB10_MASK     (1 << 10)
#define PIO_LATB11_MASK     (1 << 11)
#define PIO_LATB12_MASK     (1 << 12)
#define PIO_LATB13_MASK     (1 << 13)
#define PIO_LATB14_MASK     (1 << 14)
#define PIO_LATB15_MASK     (1 << 15)

/* ODCB register */
#define PIO_ODCB0_MASK      (1 << 0)
#define PIO_ODCB1_MASK      (1 << 1)
#define PIO_ODCB2_MASK      (1 << 2)
#define PIO_ODCB3_MASK      (1 << 3)
#define PIO_ODCB4_MASK      (1 << 4)
#define PIO_ODCB5_MASK      (1 << 5)
#define PIO_ODCB6_MASK      (1 << 6)
#define PIO_ODCB7_MASK      (1 << 7)
#define PIO_ODCB8_MASK      (1 << 8)
#define PIO_ODCB9_MASK      (1 << 9)
#define PIO_ODCB10_MASK     (1 << 10)
#define PIO_ODCB11_MASK     (1 << 11)
#define PIO_ODCB12_MASK     (1 << 12)
#define PIO_ODCB13_MASK     (1 << 13)
#define PIO_ODCB14_MASK     (1 << 14)
#define PIO_ODCB15_MASK     (1 << 15)

/* CNPUB register */
#define PIO_CNPUB0_MASK     (1 << 0)
#define PIO_CNPUB1_MASK     (1 << 1)
#define PIO_CNPUB2_MASK     (1 << 2)
#define PIO_CNPUB3_MASK     (1 << 3)
#define PIO_CNPUB4_MASK     (1 << 4)
#define PIO_CNPUB5_MASK     (1 << 5)
#define PIO_CNPUB6_MASK     (1 << 6)
#define PIO_CNPUB7_MASK     (1 << 7)
#define PIO_CNPUB8_MASK     (1 << 8)
#define PIO_CNPUB9_MASK     (1 << 9)
#define PIO_CNPUB10_MASK    (1 << 10)
#define PIO_CNPUB11_MASK    (1 << 11)
#define PIO_CNPUB12_MASK    (1 << 12)
#define PIO_CNPUB13_MASK    (1 << 13)
#define PIO_CNPUB14_MASK    (1 << 14)
#define PIO_CNPUB15_MASK    (1 << 15)

/* CNPDB register */
#define PIO_CNPDB0_MASK     (1 << 0)
#define PIO_CNPDB1_MASK     (1 << 1)
#define PIO_CNPDB2_MASK     (1 << 2)
#define PIO_CNPDB3_MASK     (1 << 3)
#define PIO_CNPDB4_MASK     (1 << 4)
#define PIO_CNPDB5_MASK     (1 << 5)
#define PIO_CNPDB6_MASK     (1 << 6)
#define PIO_CNPDB7_MASK     (1 << 7)
#define PIO_CNPDB8_MASK     (1 << 8)
#define PIO_CNPDB9_MASK     (1 << 9)
#define PIO_CNPDB10_MASK    (1 << 10)
#define PIO_CNPDB11_MASK    (1 << 11)
#define PIO_CNPDB12_MASK    (1 << 12)
#define PIO_CNPDB13_MASK    (1 << 13)
#define PIO_CNPDB14_MASK    (1 << 14)
#define PIO_CNPDB15_MASK    (1 << 15)

/* CNCONB register */
// #define PIO_SIDL_MASK    (1 << 13)
// #define PIO_ON_MASK      (1 << 15)

/* CNENB register */
#define PIO_CNIEB0_MASK     (1 << 0)
#define PIO_CNIEB1_MASK     (1 << 1)
#define PIO_CNIEB2_MASK     (1 << 2)
#define PIO_CNIEB3_MASK     (1 << 3)
#define PIO_CNIEB4_MASK     (1 << 4)
#define PIO_CNIEB5_MASK     (1 << 5)
#define PIO_CNIEB6_MASK     (1 << 6)
#define PIO_CNIEB7_MASK     (1 << 7)
#define PIO_CNIEB8_MASK     (1 << 8)
#define PIO_CNIEB9_MASK     (1 << 9)
#define PIO_CNIEB10_MASK    (1 << 10)
#define PIO_CNIEB11_MASK    (1 << 11)
#define PIO_CNIEB12_MASK    (1 << 12)
#define PIO_CNIEB13_MASK    (1 << 13)
#define PIO_CNIEB14_MASK    (1 << 14)
#define PIO_CNIEB15_MASK    (1 << 15)

/* CNSTATB register */
#define PIO_CNSTATB0_MASK   (1 << 0)
#define PIO_CNSTATB1_MASK   (1 << 1)
#define PIO_CNSTATB2_MASK   (1 << 2)
#define PIO_CNSTATB3_MASK   (1 << 3)
#define PIO_CNSTATB4_MASK   (1 << 4)
#define PIO_CNSTATB5_MASK   (1 << 5)
#define PIO_CNSTATB6_MASK   (1 << 6)
#define PIO_CNSTATB7_MASK   (1 << 7)
#define PIO_CNSTATB8_MASK   (1 << 8)
#define PIO_CNSTATB9_MASK   (1 << 9)
#define PIO_CNSTATB10_MASK  (1 << 10)
#define PIO_CNSTATB11_MASK  (1 << 11)
#define PIO_CNSTATB12_MASK  (1 << 12)
#define PIO_CNSTATB13_MASK  (1 << 13)
#define PIO_CNSTATB14_MASK  (1 << 14)
#define PIO_CNSTATB15_MASK  (1 << 15)

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** PORTx registers **/
typedef struct {
    Sfr_t       PIOxANSEL;
    Sfr_t       PIOxTRIS;
    Sfr_t       PIOxPORT;
    Sfr_t       PIOxLAT;
    Sfr_t       PIOxODC;
    Sfr_t       PIOxCNPU;
    Sfr_t       PIOxCNPD;
    Sfr_t       PIOxCNCON;
    Sfr_t       PIOxCNEN;
    Sfr_t       PIOxCNSTAT;
} volatile PioSfr_t;


/** PPS registers **/
typedef struct {
    uint32_t    INT1;
    uint32_t    INT2;
    uint32_t    INT3;
    uint32_t    INT4;
    Rsrvd_t     r1[1];
    uint32_t    T2CK;
    uint32_t    T3CK;
    uint32_t    T4CK;
    uint32_t    T5CK;
    uint32_t    IC1;
    uint32_t    IC2;
    uint32_t    IC3;
    uint32_t    IC4;
    uint32_t    IC5;
    Rsrvd_t     r2[3];
    uint32_t    OCFA;
    uint32_t    OCFB;
    uint32_t    U1RX;
    uint32_t    U1CTS;
    uint32_t    U2RX;
    uint32_t    U2CTS;
    Rsrvd_t     r3[9];
    uint32_t    SDI1;
    uint32_t    SS1;
    Rsrvd_t     r4[1];
    uint32_t    SDI2;
    uint32_t    SS2;
    Rsrvd_t     r5[8];
    uint32_t    REFCLKI;
} PpsInput_t;

typedef struct {
    uint32_t    RPA0;
    uint32_t    RPA1;
    uint32_t    RPA2;
    uint32_t    RPA3;
    uint32_t    RPA4;
    Rsrvd_t     r1[6];
    uint32_t    RPB0;
    uint32_t    RPB1;
    uint32_t    RPB2;
    uint32_t    RPB3;
    uint32_t    RPB4;
    uint32_t    RPB5;
    uint32_t    RPB6;
    uint32_t    RPB7;
    uint32_t    RPB8;
    uint32_t    RPB9;
    uint32_t    RPB10;
    uint32_t    RPB11;
    Rsrvd_t     r2[1];
    uint32_t    RPB13;
    uint32_t    RPB14;
    uint32_t    RPB15;
} PpsOutput_t;

typedef struct {
    PpsInput_t  PPSxIN;
    Rsrvd_t     r1[17];
    PpsOutput_t PPSxOUT;              
} volatile PpsSfr_t;


#endif	/* PIO_SFR_H */