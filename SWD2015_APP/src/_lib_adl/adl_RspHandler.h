/*
 * adl_RspHandler.h
 *
 * Created: 23/9/2557 17:25:51
 *  Author: pan
 */ 


#ifndef ADL_RSPHANDLER_H_
#define ADL_RSPHANDLER_H_


/**********************************************************************************************************************/
/*  adl_RspHandler.h   -  Copyright Sierra Wireless Inc. (c) 2010													  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_RspHandler.h
  @brief ADL AT Response Sending Service public interface header file
  @ingroup adl_RspHandler_public_IFS
  */
/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_RspHandler_public_IFS ADL AT Response Sending Service Interface Specification
  @endif

  @section adl_RspHandler_public_IFS_OVERVIEW Overview
  This document describes the public AT Response Sending Service interface of the ADL software element.

  @section adl_RspHandler_public_IFS_INTRODUCTION Introduction

  @subsection adl_RspHandler_public_IFS_REMINDER Reminder

  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.

  @subsection adl_CmdHandler_public_IFS_RELATED_DOCUMENTS Related documents references

  Here are the documents referenced in this interface specification.
  @anchor ref1 [1] <A HREF="http://domino-arc/ged/gedysdoc.nsf/c12570d1004a006dc1256a94004f97ed/1cfb385b24e3235dc1257330004c5548?OpenDocument"> FCS AT Commands Service </A> \n

  @subsection adl_RspHandler_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary

  - <b>N.A.</b>\n
    Not appliable
  - <b>N.U.</b>\n
    Not used
  - <b>IFS</b>\n
    Interface Specification
  - <b>SE</b>\n
    Software Element
  */

/**
  @ifnot GLOBAL_IFS
  @defgroup adl_RspHandler_public_IFS ADL AT Response Sending Service Interface Specification
  @else
  @addtogroup adl_RspHandler_public_IFS
  @{
  @endif

  @section adl_RspHandler_public_IFS_ftr Features

    ADL AT responses sending.


  The defined operations are:
  - adl_atSendResponse() sending of the text provided as a response with the type provided to the port provided
  - adl_atSendResponseSpe() with the NI provided the command associated is found if it had subscribed to the response provided the response handler is called else the response is sent to the port provided
  - adl_atSendStdResponse() sending of the standard response provided as a response with the type provided to the port provided
  - adl_atSendStdResponseSpe() with the NI provided the command associated is found if it had subscribed to the standard response provided the response handler is called else the standard response is sent to the port provided
  - adl_atSendStdResponseExt() sending of standard response with an argument provided as a response with the type provided to the port provided
  - adl_atSendStdResponseExtSpe() with the NI provided the command associated is found if it had subscribed to the standard response with an argument provided the response handler is called else the standard response with an argument is sent to the port provided
  - adl_atSendStdResponseExtStr() sending of standard response with a string argument provided as a response with the type provided to the port provided
  - adl_atSendUnsoResponse() sending of an unsolicited response with a string argument provided as a reponse with the port provided and RI flag provided

    @note:
  - adl_atSendResponseSpe(), adl_atSendStdResponseSpe(), adl_atSendStdResponseExtSpe() are to be used with adl_atCmdSendExt() function
  - adl_atCmdSendExt() stacks command when call in a command handler to resend the command whereas adl_atSendResponseSpe(), adl_atSendStdResponseSpe(), adl_atSendStdResponseExtSpe() unstacks the command and call the appropriate response handler (if any)




  @if GLOBAL_IFS
  @}
  @endif
  */

//#ifndef __adl_RspHandler_H__
//#define __adl_RspHandler_H__

/**
  @defgroup adl_RspHandler_public_IFS_Definitions Types and constants definition
  @ingroup adl_RspHandler_public_IFS

  */

/**
  * @addtogroup adl_RspHandler_public_IFS_Definitions
  * @{
  */


/**
  @brief Response type

  @par Description:
  The text/ID associated to this type will be sent as a standard or terminal response
  Terminal response (have to ends an incoming AT command).
  A destination port has to be specified.
  Sending such a response will flush all previously buffered unsolicited responses on the required port.
 */
#define     ADL_AT_RSP	0x01

/**
  @brief Unsollicited Response type

  @par Description:
  The text/ID associated to this type will be sent as an unsollicited response
  Unsolicited response (text to be displayed out of a currently running command process).
  For the required port (if any) or for each currently opened port (if the ADL_AT_PORT_TYPE macro is not used), if an AT command is currently running (ie. the command was sent by the external application, but this command answer has not be sent back yet), any unsolicited response will automatically be buffered, until a terminal response is sent on this port.
 */
#define     ADL_AT_UNS	0x02

/**
  @brief Intermediate Response type

  @par Description:
  The text/ID associated to this type will be sent as an intermediate response
  Intermediate response (text to display while an incoming AT command is running).
  A destination port has to be specified.
  Sending such a response will just display the required text, without flushing all previously buffered unsolicited responses on the required port.
 */
#define     ADL_AT_INT	0x04

/**
  @brief Macro to specify Response destination port and type

  @par Description:
  The _port argument has to be a defined value of the adl_port_e type, and this required port has to be available (cf. the AT/FCM port Service) ; sending a response on an Open AT® the GSM or GPRS based port will have no effects).

  @note:
  - With the ADL_AT_UNS type value, if the ADL_AT_PORT_TYPE macro is not used, the unsolicited response will be broadcasted on all currently opened ports.
  - If the ADL_AT_PORT_TYPE macro is not used with the ADL_AT_RSP & ADL_AT_INT types, responses will be by default sent on the UART 1 port. If this port is not opened, responses will not be displayed.
 */
#define ADL_AT_PORT_TYPE(_port_,_type_) ( u16 ) ( _port_ << 8 | _type_ )

/**
  @brief Additional Port parameter to adl_atSendResponse() API

  @par Description:
  Additional Port parameter definition for response sending function adl_atSendResponse()
 */
//#define adl_atSendResponsePort(_t,_p,_r) adl_atSendResponse(ADL_AT_PORT_TYPE(_p,_t),_r)

/**
  @brief Additional Port parameter to adl_atSendStdResponse() API

  @par Description:
  Additional Port parameter definition for response sending function adl_atSendStdResponse()
 */
//#define adl_atSendStdResponsePort(_t,_p,_r) adl_atSendStdResponse(ADL_AT_PORT_TYPE(_p,_t),_r)

/**
  @brief Additional Port parameter to adl_atSendStdResponseExt() API

  @par Description:
  Additional Port parameter definition for response sending function adl_atSendStdResponseExt()
 */
//#define adl_atSendStdResponseExtPort(_t,_p,_r,_a) adl_atSendStdResponseExt(ADL_AT_PORT_TYPE(_p,_t),_r,_a)


/**
  * @}
  */

/**
  @defgroup adl_RspHandler_public_IFS_op1 AT response Sending
  @ingroup adl_RspHandler_public_IFS

  These function allow to send AT responses.
  */

/**
  * @addtogroup adl_RspHandler_public_IFS_op1
  * @{
  */

/**
  @brief AT response send function

  @par Description:
  This function sends the provided text to any external application connected to the required port, as a response, an unsolicited response or an intermediate response, according to the requested type.
  @param[in]        Type     This parameter is composed of the response type, and the destination port where to send the response. The type & destination combination has to be done with the following macro: ADL_AT_PORT_TYPE ( _port, _type )
  @param[in]        Text     The string of the response

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler

 */
s32 adl_atSendResponse ( u16 Type, const ascii *Text );

/**
  @brief AT response send function with a NI

  @par Description:
  This function sends the provided text as a response, an unsolicited response or an intermediate response, according to the requested type.
  With the NI provided the associated command is found.
  If the command had subscribed to this reponse then the response handler is called.
  Else the response is sent to the port provided.

  @param[in]        Type     This parameter is composed of the response type, and the destination port where to send the response. The type & destination combination has to be done with the following macro: ADL_AT_PORT_TYPE ( _port, _type )
  @param[in]        Text     The string of the response
  @param[in]        NI       Notification Identifier to find the associate command

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler

  @note         Unsolicited service has to be subscribed beforehand in order to get unsolicited responses forwarded in AT command export mechanism.

 */
//s32 adl_atSendResponseSpe ( u16 Type, ascii *Text, u16 NI );

/**
  @brief Standard AT response send function

  @par Description:
  This function sends the provided standard response to the required port, as a response, an unsolicited response or an intermediate response, according to the requested type.

  @param[in]        Type     This parameter is composed of the response type, and the destination port where to send the response. The type & destination combination has to be done with the following macro: ADL_AT_PORT_TYPE ( _port, _type )
  @param[in]        RspID    the ID of the response

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler

 */
//s32 adl_atSendStdResponse ( u16 Type, adl_strID_e RspID );

/**
  @brief Standard AT response send function with a NI

  @par Description:
  This function sends the provided standard response as a response, an unsolicited response or an intermediate response, according to the requested type.
  With the NI provided the associated command is found.
  If the command had subscribed to this standard response then the response handler is called.
  Else the standard response is sent to the port provided.

  @param[in]        Type     This parameter is composed of the response type, and the destination port where to send the response. The type & destination combination has to be done with the following macro: ADL_AT_PORT_TYPE ( _port, _type )
  @param[in]        RspID    the ID of the response
  @param[in]        NI       Notification Identifier to find the associate command

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler

 */
//s32 adl_atSendStdResponseSpe ( u16 Type, adl_strID_e RspID, u16 NI );

/**
  @brief Standard Extended AT response send function

  @par Description:
  This function sends the provided standard response with an argument to the required port, as a response, an unsolicited response or an intermediate response, according to the requested type.

  @param[in]        Type     This parameter is composed of the response type, and the destination port where to send the response. The type & destination combination has to be done with the following macro: ADL_AT_PORT_TYPE ( _port, _type )
  @param[in]        RspID    the ID of the response
  @param[in]        arg      Standard response argument (being a u32)

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler

 */
//s32 adl_atSendStdResponseExt ( u16 Type, adl_strID_e RspID, u32 arg );

/**
  @brief Standard Extended AT response send function with a NI

  @par Description:
  This function sends the provided standard response with an argument as a response, an unsolicited response or an intermediate response, according to the requested type.
  With the NI provided the associated command is found.
  If the command had subscribed to this standard response with an argument then the response handler is called.
  Else the standard response with an argument is sent to the port provided.

  @param[in]        Type     This parameter is composed of the response type, and the destination port where to send the response. The type & destination combination has to be done with the following macro: ADL_AT_PORT_TYPE ( _port, _type )
  @param[in]        RspID    the ID of the response
  @param[in]        arg      Standard response argument (being a u32)
  @param[in]        NI       Notification Identifier to find the associate command

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler

 */
//s32 adl_atSendStdResponseExtSpe ( u16 Type, adl_strID_e RspID, u32 arg, u16 NI );

/**
  @brief Standard Extended AT response send function with string argument

  @par Description:
  This function sends the provided standard response with an argument to the required port, as a response, an unsolicited response or an intermediate response, according to the requested type.

  @param[in]        Type     This parameter is composed of the response type, and the destination port where to send the response. The type & destination combination has to be done with the following macro: ADL_AT_PORT_TYPE ( _port, _type )
  @param[in]        RspID    the ID of the response
  @param[in]        arg      Standard response argument (being a string)

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler

 */
//s32 adl_atSendStdResponseExtStr ( u16 Type, adl_strID_e RspID, ascii* arg );

/**
  @brief Unsolicited response send function with string argument

  @par Description:
  sends the text provided to the required port as an unsolicited response with the RIpulse flag to allows to generate a RI pulse. Refer to "+WRIM" AT command in the "AT command interface guide" to get more information about RI signal

  @param[in]        Port     The destination port where to send the response
  @param[in]        Text     The text to be sent. Please note that this is exactly the text string to be displayed on the required port (ie. all carriage return & line feed characters ("\r\n" in C language) have to be sent by the application itself).
  @param[in]        RIpulse      RI pulse flag, if TRUE, RI signal is pulsed.

  @note The RI pulse generation behaviour depends on "+WRIM" AT command parameter :
        - if <b>mode</b> parameter of "+WRIM" AT command is set to 0, RI signal can not be pulse by adl_atSendUnsoResponse().
        - RI pulse duration depends on <b>pulse_width</b> parameter of "+WRIM" AT command.

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler

 */
//s32 adl_atSendUnsoResponse ( adl_port_e Port, ascii *Text, bool RIpulse );


/**
  * @}
  */

//#endif

#endif /* ADL_RSPHANDLER_H_ */