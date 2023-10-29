#include "Osc.h"

/** Local OSC and DRC base address configure for SFR access**/
static OscSfr_t *const oscSfr = &OSC_MODULE;
static CfgSfr_t *const cfgSfr = &CFG_MODULE;      

/** Local POSC variable **/
static uint32_t poscFreq;

/** Local sub-functions **/
static uint32_t GetPllMultDiv(float pllFactor);
static uint32_t GetFrcDiv(float divFactor);
static uint32_t GetPbDiv(float divFactor);


/*  
 *  Configures oscillator source to given frequency
 *  If exact frequency not possible, closest one is configured
 */
extern bool OSC_ConfigOsc(OscConfig_t oscConfig)
{    
    /* Max. system frequency protection */
    if( oscConfig.sysFreq > OSC_SYSCLK_MAX )
    {
        oscConfig.sysFreq = OSC_SYSCLK_MAX;
    }
    
    /* SYSCLK zero check */
    if( oscConfig.sysFreq == 0 )
    {
        return false;
    }
    
    /* PBCLK zero check */
    if( oscConfig.pbFreq == 0 )
    {
        oscConfig.pbFreq = oscConfig.pbFreq;
    }
    
    /* NOTE: pllOutFreq = (inFreq * PLLMULT) / (FPLLIDIV * PLLDIV) */
    
    uint32_t pllCode = 0;
    uint32_t frcCode = 0;
    
    if( oscConfig.oscSource == OSC_COSC_FRCPLL )
    {
        /* Return bit-field values for PLLDIV and PLLMULT configure */
        pllCode = GetPllMultDiv((float)oscConfig.sysFreq / OSC_FRC_FREQ);
    }
    else if( oscConfig.oscSource == OSC_COSC_POSCPLL )
    {
        /* If crystal is used and frequency user-defined */
        if( OSC_XTAL_FREQ != 0 )
        {
            /* Return bit-field values for PLLDIV and PLLMULT configure */
            pllCode = GetPllMultDiv((float)oscConfig.sysFreq / OSC_XTAL_FREQ);
        }
    }
    else if( oscConfig.oscSource == OSC_COSC_FRCDIV )
    {
        /* Return bit-field value for FRCDIV configure */
        frcCode = GetFrcDiv((float)OSC_FRC_FREQ / oscConfig.sysFreq);
    }
    /* SOSC, LPRC, FRC or POSC */
    else
    {
        /* Fixed setting in hardware */
    }
    
    /* Return bit-field value for PBDIV configure */
    OscPbDiv_t pbDiv = GetPbDiv((float)oscConfig.sysFreq / oscConfig.pbFreq);
    OscPbDiv_t oldPbDiv = (oscSfr->OSCxCON.W & OSC_PBDIV_MASK) >> OSC_PBDIV_POS;
    
    bool isPbDiv = false;
    /* If new setting is to be written */
    if( pbDiv != oldPbDiv )
    {
        isPbDiv = true;
    }
    
    bool isClkSwtch = false;
    /* If clock switching is enabled at programming */
    if( !(cfgSfr->DEVxCFG1.W & CFG_FCKSM1_MASK) )
    {
        isClkSwtch = true;
    }
    
    /* Unlock access for CFG register */
    volatile uint32_t intrStatus = CFG_UnlockSystemAccess();
    
    uint8_t pllMult = (pllCode >> 8) & 0x07;
    uint8_t pllDiv = (pllCode >> 0) & 0x07;
    uint8_t frcDiv = frcCode & 0x07;
    
    /* Clock switch sequence and PLL configuration (clock switch code always
     * executes if FCKM enabled however hardware automatically terminates
     * second part of the switch if FRC source is selected */
    if( isClkSwtch )
    {
        /* Switch to FRC first, then to PLL (if enabled) */
        oscSfr->OSCxCON.CLR = OSC_NOSC_MASK;
        oscSfr->OSCxCON.SET = OSC_COSC_FRC << OSC_NOSC_POS;
        oscSfr->OSCxCON.SET = OSC_OSWEN_MASK;

        /* Wait until clock switch to FRC is complete */
        while( oscSfr->OSCxCON.W & OSC_OSWEN_MASK );
        
        oscSfr->OSCxCON.CLR = OSC_PLLMULT_MASK | OSC_PLLODIV_MASK | OSC_NOSC_MASK | OSC_FRCDIV_MASK;
        oscSfr->OSCxCON.SET = (pllMult << OSC_PLLMULT_POS) |
                              (pllDiv << OSC_PLLODIV_POS) |
                              (oscConfig.oscSource << OSC_NOSC_POS) |
                              (frcDiv << OSC_FRCDIV_POS);
        oscSfr->OSCxCON.SET = OSC_OSWEN_MASK;

        /* Wait until clock switch to new source complete */
        while( oscSfr->OSCxCON.W & OSC_OSWEN_MASK );
    }
    /* In case clock switching is disabled but PLL source was set at device
     * programming only configure PLL */
    else
    {
        oscSfr->OSCxCON.CLR = OSC_PLLMULT_MASK | OSC_PLLODIV_MASK | OSC_FRCDIV_MASK;
        oscSfr->OSCxCON.SET = (pllMult << OSC_PLLMULT_POS) |
                              (pllDiv << OSC_PLLODIV_POS) |
                              (frcDiv << OSC_FRCDIV_POS);
    }
    
    /* PBCLK division configuration */
    if( isPbDiv )
    {
        oscSfr->OSCxCON.CLR = OSC_PBDIV_MASK;
        
        /* Wait until PBDIV can be written */
        while( !(oscSfr->OSCxCON.W & OSC_PBDIVRDY_MASK) );
        
        oscSfr->OSCxCON.SET = pbDiv << OSC_PBDIV_POS;
    }
    
    /* SOSC Enabled */
    if( oscConfig.oscSource == OSC_COSC_SOSC )
    {
        oscSfr->OSCxCON.SET = OSC_SOSCEN_MASK;
        
        /* Wait until SOSC stable */
        while( !(oscSfr->OSCxCON.W & OSC_SOSCRDY_MASK) );
    }
    else
    {
        oscSfr->OSCxCON.CLR = OSC_SOSCEN_MASK;
    }
    
    /* Lock access for CFG register */
    CFG_LockSystemAccess(intrStatus);
    
    return true;
}


/*  
 *  Returns the system clock frequency (SYSCLK)
 */
extern uint32_t OSC_GetSysFreq(void)
{
    uint32_t sysFreq;
    
    /* Read existing settings from OSCCON and DEVCFG2 */
    OscClkSource_t oscSource = (oscSfr->OSCxCON.W & OSC_COSC_MASK) >> OSC_COSC_POS;
    OscPllOutDiv_t pllOutDiv = (oscSfr->OSCxCON.W & OSC_PLLODIV_MASK) >> OSC_PLLODIV_POS;
    CfgPllInDiv_t pllInDiv = (cfgSfr->DEVxCFG2.W & CFG_FPLLIDIV_MASK) >> CFG_FPLLIDIV_POS;
    OscPllMult_t pllMult = (oscSfr->OSCxCON.W & OSC_PLLMULT_MASK) >> OSC_PLLMULT_POS;
    OscFrcDiv_t frcDiv = (oscSfr->OSCxCON.W & OSC_FRCDIV_MASK) >> OSC_FRCDIV_POS;
            
    /* Internal fast RC oscillator or primary oscillator (XT, HS or EC) with PLL */
    if( (oscSource == OSC_COSC_FRCPLL) || (oscSource == OSC_COSC_POSCPLL) )
    {
        uint32_t inDiv;
        
        /* PLL Input Division setting in DEVCFG2 register */
        if( pllInDiv == CFG_FPLLIDIV_12 )
        {
            inDiv = 12;
        }
        else if( pllInDiv == CFG_FPLLIDIV_10 )
        {
            inDiv = 10;
        }
        else
        {
            inDiv = pllInDiv + 1;
        }
        
        uint32_t mult;
        
        /* PLL Multiplication setting in OSCCON register */
        if( pllMult == OSC_PLLMULT_24 )
        {
            mult = 24;
        }
        else
        {
            mult = pllMult + 15;
        }
        
        uint32_t outDiv;
        
        /* PLL Output Division setting in OSCCON register */
        if( pllOutDiv == OSC_PLLODIV_256 )
        {
            outDiv = 256;
        }
        else
        {
            outDiv = (1 << pllOutDiv);
        }
        
        /* Internal FRC source */
        if( oscSource == OSC_COSC_FRCPLL )
        {
            sysFreq = (uint32_t)((OSC_FRC_FREQ * mult) / (inDiv * outDiv));
        }
        /* External POSC source */
        else
        {
            sysFreq = (uint32_t)((poscFreq * mult) / (inDiv * outDiv));
        }
    }
    /* Primary oscillator (XT, HS or EC) */
    else if( oscSource == OSC_COSC_POSC )
    {
        sysFreq = OSC_XTAL_FREQ;
    }
    /* Secondary oscillator */
    else if( oscSource == OSC_COSC_SOSC )
    {
        sysFreq = OSC_SOSC_FREQ;
    }
    /* Internal low-power RC oscillator */
    else if( oscSource == OSC_COSC_LPRC )
    {
        sysFreq = OSC_LPRC_FREQ;
    }
    /* Internal fast RC oscillator */
    else if( oscSource == OSC_COSC_FRC )
    {
        sysFreq = OSC_FRC_FREQ;
    }
    /* Internal fast RC oscillator (divided by 16) */
    else if( oscSource == OSC_COSC_FRCDIV16 )
    {
        sysFreq = (uint32_t)(OSC_FRC_FREQ / 16);
    }
    /* Internal fast RC oscillator (divided by N) */
    else
    {
        uint32_t div;
        
        /* FRC Division setting in OSCCON register */
        if( frcDiv == OSC_FRCDIV_256 )
        {
            div = 256;
        }
        else
        {
            div = (1 << frcDiv);
        }
        
        sysFreq = (uint32_t)(OSC_FRC_FREQ / div);
    }
    
    return sysFreq;
}


/*
 *  Returns the frequency of the peripheral bus (PBCLK)
 */
extern uint32_t OSC_GetPbFreq(void)
{
    OscPbDiv_t pbDiv = (oscSfr->OSCxCON.W & OSC_PBDIV_MASK) >> OSC_PBDIV_POS;
    
    uint32_t sysFreq = OSC_GetSysFreq();
    
    return (uint32_t)(sysFreq / (1 << pbDiv));
}


/*
 *  Returns PLLOMULT and PLLDIV based on PLL multiplication factor requested
 */
static uint32_t GetPllMultDiv(float pllFactor)
{
    /* NOTE: pllFactor = newFreq / currentFreq = (1 / FPLLIDIV) * (PLLMULT / PLLDIV) */
    
    CfgPllInDiv_t pllInDiv = (cfgSfr->DEVxCFG2.W & CFG_FPLLIDIV_MASK) >> CFG_FPLLIDIV_POS;
    
    uint8_t pllDiv;
    /* PLL Input Division setting in DEVCFG2 register */
    if( pllInDiv == CFG_FPLLIDIV_12 )
    {
        pllDiv = 12;
    }
    else if( pllInDiv == CFG_FPLLIDIV_10 )
    {
        pllDiv = 10;
    }
    else
    {
        pllDiv = (uint8_t)pllInDiv + 1;
    }
    
    /* PLLMULT / PLLDIV = pllFactor * FPLLIDIV  */
    float pllMultDiv = pllFactor * pllDiv;
    
    uint32_t retVal = 0;
    
    /* Min. output PLL factor is 15/256 */
    if( pllMultDiv < 0.06 )
    {
        retVal = ((0 << 8) | 7);
    }
    /* Max. output PLL factor is 24/1 */
    else if( pllMultDiv >= 24.0 )
    {
        retVal = ((7 << 8) | 0);
    }
    /* Other factors */
    else
    {
        /* Operating with integers most of the time*/
        uint32_t inputFactor = (uint32_t)(pllMultDiv * 1000000);    // 10^6 since float precision is 6 digits
        
        /* PLL output multiply and divide possible values */
        uint32_t pllDiv[] = {1,2,4,8,16,32,64,256};
        uint32_t pllMult[] = {15,16,17,18,19,20,21,24};
        
        /* All possible factors inside LUT */
        uint32_t pllLut[64] = {0};
        
        /* Create LUT - divide each multiplicand with each divisor */
        for(uint8_t k = 0; k < 8; k++)
        {
            for(uint8_t l = 0; l < 8; l++)
            {
                /* Multiplicands indexed in order low to high, divisors vice versa */
                pllLut[(k*8)+l] = (pllMult[l] * 1000000) / pllDiv[7-k];
            }
        }
        
        uint8_t i, j;
        
        /* Find two closest PLL factors to actual one */
        for(i = 0; i < 8; i++)
        {
            for(j = 0; j < 8; j++)
            {
                if( pllLut[(i*8)+j] >= inputFactor )
                {
                    /* End both loops */
                    goto endLoop;
                }
            }
        }

        endLoop:;

        /* NOTE: This function searches for two divisors, since the closest is
         * required to be found, which means it is possible to also have two
         * divisors, but this is the case only when both multiplicands are
         *  15 and 24 (e.g. if "input" is between 15/1 and 24/2) */
        
        uint32_t multVal[2], divdVal[2];    // Actual integer multiplicands and divisors
        uint8_t divdIndx[2], multIndx[2];   // Indexes for actual multiplicands and divisors

        bool isTwoMult;
        
        /* In case of two divisors */
        if( j == 0 )
        {
            isTwoMult = true;
            
            multVal[0] = pllMult[7];
            multVal[1] = pllMult[0];
            divdVal[0] = pllDiv[7-i+1];
            divdVal[1] = pllDiv[7-i];

            multIndx[0] = 7;
            multIndx[1] = 0;
            divdIndx[0] = 7-i+1;
            divdIndx[1] = 7-i;
        }
        /* Other cases with one divisors */
        else
        {
            isTwoMult = false;
            
            multVal[0] = pllMult[j];
            multVal[1] = pllMult[j];    // Non-valid multiplicand
            divdVal[0] = pllDiv[7-i+1];
            divdVal[1] = pllDiv[7-i];

            multIndx[0] = j;
            multIndx[1] = j;            // This one not valid
            divdIndx[0] = 7-i+1;
            divdIndx[1] = 7-i;
        }

        
        float res[2];       // Stores two possible PLL factors
        
        /* Each multiplicand by each divisor */
        if( isTwoMult )
        {
            res[0] = ((float)multVal[0]) / divdVal[0];
            res[1] = ((float)multVal[1]) / divdVal[1];
        }
        /* One multiplicand by two divisors */
        else
        {
            res[0] = ((float)multVal[0]) / divdVal[0];
            res[1] = ((float)multVal[0]) / divdVal[1];
        }


        float absRes[2];    // Stores two possible absolute PLL factors
        
        /* Find absolute of both possible PLL factors and actual factor */
        
        absRes[0] = res[0] - pllMultDiv;
        if(absRes[0] < 0)
        {
            absRes[0] *= (-1.0);
        }
        
        absRes[1] = res[1] - pllMultDiv;
        if(absRes[1] < 0)
        {
            absRes[1] *= (-1.0);
        }

        /* Lesser value is the closest, therfore indexes bound to that PLL factor are returned */
        if( absRes[0] <= absRes[1] )
        {
            retVal = (multIndx[0] << 8) | divdIndx[0];
        }
        else
        {
            retVal = (multIndx[1] << 8) | divdIndx[1];
        }
    }
    
    /* HIGH BYTE for PLL MULT and LOW BYTE for PLL DIV indexes */
    return retVal;
}


/*
 *  Returns FRCDIV base on requested FRC division factor
 */
static uint32_t GetFrcDiv(float divFactor)
{
    uint32_t frcDiv[] = {1,2,4,8,16,32,64,256};

    uint32_t retVal = 0;
    
    /* Min. output DIV factor is 1 */
    if( divFactor <= 1.0 )
    {
        retVal = 1;
    }
    /* Max. output DIV factor is 256 */
    else if( divFactor >= 256.0)
    {
        retVal = 7;
    }
    /* Other factors */
    else
    {
        uint8_t i;
        /* Find two closest DIV factors to actual one */
        for(i = 0; i < 8; i++)
        {
            if( frcDiv[i] > divFactor )
            {
                break;
            }
        }

        float absRes[2];

        /* Find absolute of both possible DIV factors and actual factor */

        absRes[0] = frcDiv[i] - divFactor;
        if(absRes[0] < 0.0)
        {
            absRes[0] *= (-1.0);
        }

        absRes[1] = frcDiv[i-1] - divFactor;
        if(absRes[1] < 0.0)
        {
            absRes[1] *= (-1.0);
        }

        float retVal = 0;

        /* Lesser value is the closest, therfore index bound to that DIV factor are returned */
        if( absRes[0] <= absRes[1] )
        {
            retVal = i;
        }
        else
        {
            retVal = i-1;
        }
    }
    
    /* FRC DIV index */
    return retVal;
}


/*
 *  Returns PBDIV for the requested PBCLK division factor
 */
static uint32_t GetPbDiv(float divFactor)
{
    uint32_t pbDiv[] = {1,2,4,8};

    uint32_t retVal = 0;
    
    /* Min. output DIV factor is 1 */
    if( divFactor <= 1.0 )
    {
        retVal = 0;
    }
    /* Max. output DIV factor is 8 */
    else if( divFactor >= 8.0)
    {
        retVal = 3;
    }
    /* Other factors */
    else
    {
        uint8_t i;
        /* Find two closest DIV factors to actual one */
        for(i = 0; i < 4; i++)
        {
            if( pbDiv[i] >= divFactor )
            {
                break;
            }
        }

        float absRes[2];

        /* Find absolute of both possible DIV factors and actual factor */

        absRes[0] = pbDiv[i] - divFactor;
        if(absRes[0] < 0.0)
        {
            absRes[0] *= (-1.0);
        }

        absRes[1] = pbDiv[i-1] - divFactor;
        if(absRes[1] < 0.0)
        {
            absRes[1] *= (-1.0);
        }

        /* Lesser value is the closest, therfore index bound to that DIV factor are returned */
        if( absRes[0] <= absRes[1] )
        {
            retVal = i;
        }
        else
        {
            retVal = i-1;
        }
    }
    
    /* PB DIV index */
    return retVal;
}