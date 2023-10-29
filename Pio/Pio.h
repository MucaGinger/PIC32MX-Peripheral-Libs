#ifndef PIO_H
#define	PIO_H


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

/** Standard libs **/
#include <stdint.h>

/** Custom libs **/
#include "Pio_sfr.h"
#include "Cfg.h"
#include "Ic.h"


/******************************************************************************/
/*---------------------------------Macros-------------------------------------*/
/******************************************************************************/

/** Used for reading pin code **/
#define PIO_PIN_POS(pCode)  ( (pCode >> 16) & 0xFF )
#define PIO_PIN_DIR(pCode)  ( (pCode >> 24) & 0x01 )
#define PIO_PIN_MOD(pCode)  ( (pCode >> 28) & 0x0F )


/********************User-defined interrupt vector priority********************/

/* NOTE: IPL = 0 means interrupt disabled. ISR_IPL level must equal ICX_IPL */
/* NOTE: Vectors of same priority and sub-priority are services in their
 *       natural order */
/* NOTE: All CN register sets (for all PIO modules) operate under same priority
 *       and correspond to same interrupt vector (one ISR for all modules,
 *       however each flag for each module - one handler for each module) */

/* User-defined (sub)priority levels (IPL: 0-7, ISL: 0-3) */
#define CN_ISR_IPL    IPL1SOFT
#define CN_ICX_IPL    1
#define CN_ICX_ISL    0

/******************************************************************************/


/******************************************************************************/
/*----------------------------Enumeration Types-------------------------------*/
/******************************************************************************/

typedef enum {
    PIO_DIR_OUTPUT = 0,
    PIO_DIR_INPUT = 1
} PioPinDirect_t;

typedef enum {
    PIO_TYPE_DIGITAL = 0,
    PIO_TYPE_ANALOG = 1
} PioPinType_t;

typedef enum {
    PIO_DRIVER_NORMAL = 0,
    PIO_DRIVER_OPENDRAIN = 1
} PioPinDriver_t;

typedef enum {
    PIO_POS_0 = 0,
    PIO_POS_1 = 1,
    PIO_POS_2 = 2,
    PIO_POS_3 = 3,
    PIO_POS_4 = 4,
    PIO_POS_5 = 5,
    PIO_POS_6 = 6,
    PIO_POS_7 = 7,
    PIO_POS_8 = 8,
    PIO_POS_9 = 9,
    PIO_POS_10 = 10,
    PIO_POS_11 = 11,
    PIO_POS_12 = 12,
    PIO_POS_13 = 13,
    PIO_POS_14 = 14,
    PIO_POS_15 = 15
} PioPinPos_t;

typedef enum {
    PIO_CN_NONE = 0,
    PIO_CN_PULLDOWN = 1,
    PIO_CN_PULLUP = 2,
    PIO_CN_PULLUPDOWN = 3
} PioPullType_t;


/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/* Structure for reading pin info from pin code */
typedef struct {
    PioSfr_t           *pinMod;
    PioPinDirect_t      pinDir;
    PioPinPos_t         pinPos;
} PinInfo_t;


/******************************************************************************/
/*-----------------------------Function Prototypes----------------------------*/
/******************************************************************************/

/* Other PIO configuration functions */
bool PIO_ConfigPpsSfr(const uint32_t pinCode);
bool PIO_ReleasePpsSfr(const uint32_t pinCode);
bool PIO_ConfigInputChange(const uint32_t pinCode, PioPullType_t pullType);
bool PIO_SetIsrHandler(const uint32_t pinCode, volatile void (*isrHandler)(void));

/* Pin configuration functions */
INLINE void PIO_ConfigPpsPin(const uint32_t pinCode, PioPinType_t pinType);
INLINE void PIO_ConfigGpioPin(const uint32_t pinCode, PioPinType_t pinType, PioPinDirect_t pinDir);
INLINE void PIO_ConfigGpioPinDir(const uint32_t pinCode, PioPinDirect_t pinDir);
INLINE void PIO_ConfigGpioPinType(const uint32_t pinCode, PioPinType_t pinType);
INLINE void PIO_ConfigGpioPinPull(const uint32_t pinCode, PioPullType_t pullType);
INLINE void PIO_ConfigPinDriver(const uint32_t pinCode, PioPinDriver_t pinDriver);

/* Pin macro code identification functions */
INLINE PinInfo_t PIO_ReadPinCode(const uint32_t pinCode);
INLINE PioSfr_t *const PIO_ReadPinModule(const uint32_t pinCode);
INLINE PioPinDirect_t PIO_ReadPinDirection(const uint32_t pinCode);
INLINE PioPinPos_t PIO_ReadPinPosition(const uint32_t pinCode);

/* Modify pin state functions */
INLINE void PIO_ClearPin(const uint32_t pinCode);
INLINE void PIO_SetPin(const uint32_t pinCode);
INLINE void PIO_TogglePin(const uint32_t pinCode);

/* Read (input) pin state function */
INLINE uint32_t PIO_ReadPin(const uint32_t pinCode);

/******************************************************************************/
/*-----------------------------Function In-lines------------------------------*/
/******************************************************************************/

/*
 *  Configures PPS pin as digital/analog
 */
INLINE void PIO_ConfigPpsPin(const uint32_t pinCode, PioPinType_t pinType)
{
    PioSfr_t *pioSfr = (PioSfr_t *)( (uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) );
     
    pioSfr->PIOxANSEL.CLR = 1 << PIO_PIN_POS(pinCode);
    pioSfr->PIOxANSEL.SET = pinType << PIO_PIN_POS(pinCode);
    pioSfr->PIOxTRIS.CLR = 1 << PIO_PIN_POS(pinCode);
    pioSfr->PIOxTRIS.SET = PIO_PIN_DIR(pinCode) << PIO_PIN_POS(pinCode);
}


/*
 *  Configures GPIO pin as in/out and digital/analog
 * 
 *  NOTE: This function is provided due to the fact that PinDir() and PinType()
 *        functions are commonly used together
 */
INLINE void PIO_ConfigGpioPin(const uint32_t pinCode, PioPinType_t pinType, PioPinDirect_t pinDir)
{
    PioSfr_t *pioSfr = (PioSfr_t *)( (uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) );
     
    pioSfr->PIOxANSEL.CLR = 1 << PIO_PIN_POS(pinCode);
    pioSfr->PIOxANSEL.SET = pinType << PIO_PIN_POS(pinCode);
    pioSfr->PIOxTRIS.CLR = 1 << PIO_PIN_POS(pinCode);
    pioSfr->PIOxTRIS.SET = pinDir << PIO_PIN_POS(pinCode);
}


/*
 *  Configures GPIO pin as in/out
 */
INLINE void PIO_ConfigGpioPinDir(const uint32_t pinCode, PioPinDirect_t pinDir)
{
    /* Set PIO base address for SFR access */
    PioSfr_t *pioSfr = (PioSfr_t *)( (uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) );
    
    pioSfr->PIOxTRIS.CLR = 1 << PIO_PIN_POS(pinCode);
    pioSfr->PIOxTRIS.SET = pinDir << PIO_PIN_POS(pinCode);
}


/*
 *  Configures GPIO pin as digital/analog
 */
INLINE void PIO_ConfigGpioPinType(const uint32_t pinCode, PioPinType_t pinType)
{
    PioSfr_t *pioSfr = (PioSfr_t *)( (uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) );
    
    pioSfr->PIOxANSEL.CLR = 1 << PIO_PIN_POS(pinCode);
    pioSfr->PIOxANSEL.SET = pinType << PIO_PIN_POS(pinCode);
}


/*
 *  Configures GPIO input with pull-up/down/both or no-pull resistor
 */
INLINE void PIO_ConfigGpioPinPull(const uint32_t pinCode, PioPullType_t pullType)
{
    /* Set PIO base address for SFR access */
    PioSfr_t *pioSfr = (PioSfr_t *)( (uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) );
    
    PioPinPos_t pinPos = PIO_ReadPinPosition(pinCode);
    
    /* Configure only if pin is input */
    if( PIO_ReadPinDirection(pinCode) == PIO_DIR_INPUT )
    {
        /* Pull-down */
        if( pullType == PIO_CN_PULLDOWN )
        {
            pioSfr->PIOxCNPD.SET = 1 << pinPos;
            pioSfr->PIOxCNPU.CLR = 1 << pinPos;
        }
        /* Pull-up */
        else if( pullType == PIO_CN_PULLUP )
        {
            pioSfr->PIOxCNPD.CLR = 1 << pinPos;
            pioSfr->PIOxCNPU.SET = 1 << pinPos;
        }
        /* Pull-both */
        else if( pullType == PIO_CN_PULLUPDOWN )
        {
            pioSfr->PIOxCNPD.SET = 1 << pinPos;
            pioSfr->PIOxCNPU.SET = 1 << pinPos;
        }
        /* No-pull */
        else
        {
            pioSfr->PIOxCNPD.CLR = 1 << pinPos;
            pioSfr->PIOxCNPU.CLR = 1 << pinPos;
        }
    }
}


/*
 *  Configures GPIO or PPS pin as Open-Drain/normal (digital) output
 */
INLINE void PIO_ConfigPinDriver(const uint32_t pinCode, PioPinDriver_t pinDriver)
{
    PioSfr_t *pioSfr = (PioSfr_t *)( (uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) );
    
    pioSfr->PIOxODC.CLR = 1 << PIO_PIN_POS(pinCode);
    pioSfr->PIOxODC.SET = pinDriver << PIO_PIN_POS(pinCode);
}


/*
 *  Returns information about pin module, direction and position
 */
INLINE PinInfo_t PIO_ReadPinCode(const uint32_t pinCode)
{
    PinInfo_t pinInfo;
    
    /* Set PIO base address and modify SFRs */
    pinInfo.pinMod = (PioSfr_t *)( (uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) );
    pinInfo.pinDir = PIO_PIN_DIR(pinCode);
    pinInfo.pinPos = PIO_PIN_POS(pinCode);
    
    return pinInfo;
}


/*
 *  Returns PIO base address of a given pin code (GPIO or PPS pin)
 */
INLINE PioSfr_t *const PIO_ReadPinModule(const uint32_t pinCode)
{
    return (PioSfr_t *)( (uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) );
}


/*
 *  Returns pin direction of a given pin code (PPS pin only)
 */
INLINE PioPinDirect_t PIO_ReadPinDirection(const uint32_t pinCode)
{
    return PIO_PIN_DIR(pinCode);
}


/*
 *  Returns pin position of a given pin code (GPIO or PPS pin)
 */
INLINE PioPinPos_t PIO_ReadPinPosition(const uint32_t pinCode)
{
    return PIO_PIN_POS(pinCode);
}


/*
 *  Clears pin state for a given pin code
 */
INLINE void PIO_ClearPin(const uint32_t pinCode)
{
    /* LATxCLR address = PIOA base address + module offset + register offset */
    *( (uint32_t *)((uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) + 0x34) ) = 1 << PIO_PIN_POS(pinCode);
}


/*
 *  Sets pin state for a given pin code
 */
INLINE void PIO_SetPin(const uint32_t pinCode)
{
    /* LATxSET address = PIOA base address + module offset + register offset */
    *( (uint32_t *)((uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) + 0x38) ) = 1 << PIO_PIN_POS(pinCode);
}


/*
 *  Toggles pin state for a given pin code
 */
INLINE void PIO_TogglePin(const uint32_t pinCode)
{
    /* LATxINV address = PIOA base address + module offset + register offset */
    *( (uint32_t *)((uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) + 0x3C) ) = 1 << PIO_PIN_POS(pinCode);
}


/*
 *  Reads input pin state for a given pin code
 */
INLINE uint32_t PIO_ReadPin(const uint32_t pinCode)
{
    /* ((PORTx address = PIOA base address + module offset + register offset) >> pinPos) & 0x01 */
    return ( *((uint32_t *)((uint8_t *)&PIOA_MODULE + (PIO_PIN_MOD(pinCode) << 8) + 0x20)) >> PIO_PIN_POS(pinCode) ) & 0x01 ;
}


#endif	/* PIO_H */