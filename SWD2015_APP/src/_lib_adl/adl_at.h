/*
 * adl_at.h
 *
 * Created: 3/10/2557 17:38:06
 *  Author: pan
 */ 


/**********************************************************************************************************************/
/*  adl_at.h   -  Copyright Sierra Wireless Inc. (c) 2010															  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/***************************************************************************/
/*  File       : adl_at.h                                                  */
/*-------------------------------------------------------------------------*/
/*  Object     : ADL definitions of AT structures                          */
/*                                                                         */
/*
 * --------------------------------------------------------------------------
 *  Date     | Author | Revision       | Description
 * ----------+--------+----------------+-------------------------------------
 *  15.09.05 | DPO    | 1.8            | *   Resolution for 30309: Single po
 *           |        |                | rt unsolicited responses support   
 * ----------+--------+----------------+-------------------------------------
 *  24.03.05 | DPO    | 1.7            | * FCM interface updated with new su
 *           |        |                | pported flows                      
 * ----------+--------+----------------+-------------------------------------
 *  07.03.05 | DPO    | 1.6            | *  Resolution for 27871: ADL AT com
 *           |        |                | mands service re-implementation (ne
 *           |        |                | w V24 architecture)                
 * ----------+--------+----------------+-------------------------------------
 *  16.02.05 | DPO    | 1.5            | * New Dest field for Intermediates 
 *           |        |                | & Responses structure              
 * ----------+--------+----------------+-------------------------------------
 *  13.11.03 | dpo    | 1.4            | * Add port identifiers structure   
 * ----------+--------+----------------+-------------------------------------
 *  25.08.03 | dpo    | 1.3            | * Response and Unsolicited structur
 *           |        |                | es updated                         
 * ----------+--------+----------------+-------------------------------------
 *  30.04.03 | dpo    | 1.2            | * Add new line at end of file      
 * ----------+--------+----------------+-------------------------------------
 *  08.08.02 | fdn    | 1.0            | Initial revision.                  
 * ----------+--------+----------------+-------------------------------------
*/

#ifndef __ADL_AT_H__
#define __ADL_AT_H__


/* ---------------- */
/* Serial Ports IDs */
/* ---------------- */

// Use port service type
typedef adl_port_e  adl_atPort_e;

// Deprecated constants
#define ADL_AT_UART1    ADL_PORT_UART1
#define ADL_AT_UART2    ADL_PORT_UART2
#define ADL_AT_USB      ADL_PORT_USB
#define ADL_AT_UART3    ADL_PORT_UART3


#define ADL_TYPE_NONE   0x0000

/****************************************************************************/
/*  Body structures assosciated to AT Messages                              */
/****************************************************************************/
/* ------------------------------------ */
/* Body for unsolicited message type :  */
/* ------------------------------------ */
typedef struct
{
#ifndef __REMOTETASKS__
  adl_strID_e           RspID;
  adl_atPort_e          Dest;
#else
  u8                    RspID;
  u8                    Dest;
#endif
  u16                   StrLength;
  bool                  RiPulse;        ///< Indicate if RI V24 signal must be pulsed
  u8                    Pad[3];         ///< not used
  ascii                 StrData[1];
} adl_atUnsolicited_t;


#endif // __ADL_AT_H__
