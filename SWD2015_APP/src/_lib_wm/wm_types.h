

/**********************************************************************************************************************/
/*  wm_types.h   -  Copyright Sierra Wireless Inc. (c) 2010                                                           */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/

/**
 * @file wm_types.h
 * @brief Sierra Wireless standard types and Sierra Wireless standard macros definition
 *
 * This file defines standard integer types which should be used
 * throughout all Sierra Wireless source files to ensure both platform and
 * compiler portability: the ANSI C89 standard does not define such
 * portable types, but although C99 finally does and both GCC and ARMCC
 * make them available in the C99 standard "stdint.h" include file,
 * compilers such as Microsoft's or many source verification tools
 * still use or need their own type definitions.
 *
 * This file and "wm_macros.h" should supersede the previous "stdcomp.h", which overrides
 * the Borland C++ include file or the RogueWave C++ library include file,
 * and the previous "wm_types.ht" which basically had the same contents
 * as "stdcomp.h" but was intended to be delivered to Open MMI layers
 * (and had a non C standard name).
 */

#ifndef WM_TYPES_H
#define WM_TYPES_H

#include <compiler.h>
/****************************************************************************/
/*               AT LEAST ANSI/ISO C90 COMPLIANCE IS REQUIRED               */
/****************************************************************************/
#include <stddef.h>
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#endif


/****************************************************************************/
/*                     SIERRA WIRELESS INTEGER TYPES                        */
/****************************************************************************/

/* Modern ARM compilers and GCC */
#if (defined(__ARMCC_VERSION)&&(__ARMCC_VERSION >= 120000)) || defined(__GNUC__)

#include <stdint.h>

typedef uint8_t   u8;
typedef int8_t    s8;
typedef uint16_t  u16;
typedef int16_t   s16;
typedef uint32_t  u32;
typedef int32_t   s32;
typedef uint64_t  u64;
typedef int64_t   s64;

#elif defined(_MSC_VER) /* Microsoft C (Visual C++) */


typedef unsigned _int8          u8;
typedef          _int8          s8;
typedef unsigned _int16         u16;
typedef          _int16         s16;
typedef unsigned _int32         u32;
typedef          _int32         s32;
typedef unsigned _int64         u64;
typedef          _int64         s64;

#elif defined(S_SPLINT_S) /* Secure programming lint */

/* Splint natively knows about ISO standard int types */
#define u8  uint8_t
#define s8  int8_t
#define u16 uint16_t
#define s16 int16_t
#define u32 uint32_t
#define s32 int32_t
#define u64 uint64_t
#define s64 int64_t

#else /* other old ARM compilers (SDT/Jumpstart/Norcroft) and unknown stuff */

typedef unsigned char          u8;
typedef signed   char          s8;
typedef unsigned short     int u16;
typedef          short     int s16;
typedef unsigned long      int u32;
typedef          long      int s32;
typedef unsigned long long int u64;
typedef          long long int s64;

#endif

/****************************************************************************/
/*                       BOOLEAN RELATED DEFINITIONS                        */
/****************************************************************************/

/* The bool type is a standard type in C++,
 * note that ISO C99 has introduced a new boolean type for C, "_Bool" */
#ifndef __cplusplus

/* Note that in C++, sizeof(bool) == 1 */
#if __STDC_VERSION__ < 199901L
typedef unsigned char bool;
#endif

#ifndef TRUE
#define TRUE   1
#endif
#ifndef FALSE
#define FALSE  0
#endif
#if __STDC_VERSION__ < 199901L
#define true   TRUE
#define false  FALSE
#endif

#endif

/****************************************************************************/
/*               OTHER SIERRA WIRELESS SPECIFIC TYPES                       */
/****************************************************************************/

typedef char ascii;      /* For ASCIIZ strings */
typedef uint32_t Primitive; /* For messages IDs   */



/****************************************************************************/

/* Methuselah's corner:
 * still supporting definitions once required by non-ANSI compilers.
 * Many source files still use them */
#ifndef CONST
#define  CONST       const
#endif
#ifndef VOID
#define  VOID        void
#endif

/****************************************************************************/

/* OK and ERROR are return codes used by many software elements */
#ifndef OK
#define  OK          0
#endif
#ifndef ERROR
#define  ERROR       -1
#endif

/****************************************************************************/
/* 
 * Standardized Operation Status
 */

typedef enum
{
   CH_STATUS_NOT_SUPPORTED  = -5L,
   CH_STATUS_ALREADY_USED   = -4L,
   CH_STATUS_INVALID_CMD    = -3L,
   CH_STATUS_HANDLE_ERR     = -2L,
   CH_STATUS_ERROR          = -1L,         // operation failed     (sync/async)
   CH_STATUS_NORMAL         = 0,           // operation succeeded  (sync)
   CH_STATUS_PENDING        = 0x7FFFFFFEL, // operation is pending (async)
   CH_STATUS_ALIGN          = 0x7FFFFFFFL
} eChStatus_t;




/****************************************************************************/

#define WM_TYPES_MALLOC_C_LIB		0

#endif /* WM_TYPES_H not defined */