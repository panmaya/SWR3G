/*
 * adl_UnSoHandler.h
 *
 * Created: 4/10/2557 10:56:58
 *  Author: pan
 */ 


/**********************************************************************************************************************/
/*  adl_UnSoHandler.h    -  Copyright Sierra Wireless Inc. (c) 2010													  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/***************************************************************************/
/*  File       : adl_UnSoHandler.h                                         */
/*-------------------------------------------------------------------------*/
/*  Object     : ADL functions dealing with unsolicited responses          */
/*                                                                         */
/*
 * --------------------------------------------------------------------------
 *  Date     | Author | Revision       | Description
 * ----------+--------+----------------+-------------------------------------
 *  30.04.03 | dpo    | 1.2            | * Add new line at end of file      
 * ----------+--------+----------------+-------------------------------------
 *  08.08.02 | fdn    | 1.0            | Initial revision.                  
 * ----------+--------+----------------+-------------------------------------
*/

#ifndef __ADL_ATUNSO_H__
#define __ADL_ATUNSO_H__

typedef bool (*adl_atUnSoHandler_t)(adl_atUnsolicited_t *);

/*{{SHUT_OUT_BODY_BEGIN}}*/
/***************************************************************************/
/*  Function   : adl_atUnSoSubscribe                                       */
/*-------------------------------------------------------------------------*/
/*  Object     : Subscribe to an unsolicited response                      */
/*                                                                         */
/*  Return     : OK otherwise ERROR if an error occurred.                  */
/*               ADL_RET_ERR_SERVICE_LOCKED if called from a low level     */
/*                                                    interruption handler */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  UnSostr           | x |   |   | unsolicited response string            */
/*--------------------+---+---+---+----------------------------------------*/
/*  UnSohdl           | x |   |   | unsolicited response handler           */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
s16 adl_atUnSoSubscribe(ascii *UnSostr, adl_atUnSoHandler_t UnSohdl);



/***************************************************************************/
/*  Function   : adl_atUnSoUnSubscribe                                     */
/*-------------------------------------------------------------------------*/
/*  Object     : Unsubscribe to an unsolicited response                    */
/*                                                                         */
/*  Return     : OK if the unsolicited reponse could be found,             */
/*               ADL_RET_ERR_SERVICE_LOCKED if called from a low level     */
/*                                                    interruption handler */
/*               ERROR otherwise.                                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  UnSostr           | x |   |   | unsolicited response string            */
/*--------------------+---+---+---+----------------------------------------*/
/*  UnSohdl           | x |   |   | unsolicited response handler           */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//s16 adl_atUnSoUnSubscribe(ascii *UnSostr, adl_atUnSoHandler_t UnSohdl);



/*{{SHUT_OUT_BODY_END}}*/

s8 adl_UnSoHandler_init(void);

#endif
