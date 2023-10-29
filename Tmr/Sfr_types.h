#ifndef SFR_TYPES_H
#define	SFR_TYPES_H

/** Standard libs **/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*---------------------------------Macros-------------------------------------*/
/******************************************************************************/

/* NOTE: These aren't used for SFR types but are used everywhere else, where SFR
 *       types are used as well
 */

#ifndef INLINE
#define INLINE  inline __attribute__ ((always_inline))
#endif

#ifndef bool
#define bool  bool
#endif

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** Used on protected regions of memory **/
typedef const uint32_t      Rsrvd_t;

/** Basic SFR type with atomic access **/
typedef struct {
    uint32_t    W;
    uint32_t    CLR;
    uint32_t    SET;
    uint32_t    INV;
} Sfr_t;

typedef struct {
    uint32_t    W;
} Word_t;

#endif	/* SFR_TYPES_H */