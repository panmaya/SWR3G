/*
 * adl_str.h
 *
 * Created: 3/10/2557 17:39:57
 *  Author: pan
 */ 


/**********************************************************************************************************************/
/*  adl_str.h   -  Copyright Sierra Wireless Inc. (c) 2010															  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/***************************************************************************/
/*  File       : adl_str.h                                                 */
/*-------------------------------------------------------------------------*/
/*  Object     : ADL string service                                        */
/*                                                                         */
/***************************************************************************/


/**
  @file adl_str.h
  @brief ADL AT Strings Service public interface header file
  @ingroup adl_str_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_str_public_IFS ADL AT Strings Service Interface Specification
  @endif
  
  @section adl_str_public_IFS_OVERVIEW Overview
  This document describes the public AT Strings Service interface of the ADL software element.
  
  @version 02.00
  
  @author SUB Infosys AT Maintenance
  
  @section adl_str_public_IFS_HISTORY History
  <table>
  <tr>
      <td><b>Rev<br>XX.YY</b></td>
      <td><b>FH Rev<br>X</b></td>
      <td><b>Date<br>YYYY/MM/DD</b></td>	  
      <td><b>Updates</b></td>
      <td><b>Author</b></td>
  </tr>
  <tr>
      <td>01.00</td>
      <td>N/A</td>
      <td>2010/12/08</td>	  
      <td>Creation</td>
      <td>FDU</td>
  </tr>
  <tr>
      <td>02.00</td>
      <td>N/A</td>	  
      <td>2013/03/13</td>	  
      <td>Major Update:<br>
          Updated name and revision table as per records management guidelines.
	  </td>
      <td>SUB Infosys AT Maintenance</td>
  </tr>
  </table>
  
  @section adl_str_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_str_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_str_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
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
  @defgroup adl_str_public_IFS ADL AT Strings Service Interface Specification
  @else
  @addtogroup adl_str_public_IFS
  @{
  @endif
  
  @section adl_str_public_IFS_ftr Features
  
  This service provides APIs to process AT standard response strings.

  The defined operations are:
  - An <b>adl_strGetIDExt()</b> function to return the ID of the provided response string, with argument.
  - An <b>adl_strGetID()</b> function to returns the ID of the provided response string.
  - An <b>adl_strIsTerminalResponse()</b> function to check whether the provided response ID is a terminal one.
  - An <b>adl_strGetResponseExt()</b> function to provide a standard response string from its ID, with argument.
  - An <b>adl_strGetResponse()</b> function to provide a standard response string from its ID.

  
  @if GLOBAL_IFS
  @}
  @endif
  */

#ifndef __ADL_STR_H__
#define __ADL_STR_H__


/**
  @defgroup adl_str_public_IFS_Definitions Types and constants definition
  @ingroup adl_str_public_IFS

  */


/**
  * @addtogroup adl_str_public_IFS_Definitions
  * @{
  */

/**
  @brief String ID enum

  @par Description:
  String ID definition
 */
typedef enum
{
    ADL_STR_NO_STRING,  ///< Unknown string
    
    ADL_STR_OK,         ///< "OK"
    ADL_STR_BUSY,       ///< "BUSY"
    ADL_STR_NO_ANSWER,  ///< "NO ANSWER"
    ADL_STR_NO_CARRIER, ///< "NO CARRIER"
    ADL_STR_CONNECT,    ///< "CONNECT"
    ADL_STR_ERROR,      ///< "ERROR"
    ADL_STR_CME_ERROR,  ///< "+CME ERROR:"
    ADL_STR_CMS_ERROR,  ///< "+CMS ERROR:"
    ADL_STR_CPIN,       ///< "+CPIN:"
    ADL_STR_CPIN2,      ///< "+CPIN2:"

    ADL_STR_LAST_TERMINAL,      ///< All terminal responses are before this line

    ADL_STR_RING = ADL_STR_LAST_TERMINAL,   ///< "RING"
    ADL_STR_WIND,       ///< "+WIND:"
    ADL_STR_CRING,      ///< "+CRING:"
    ADL_STR_CPINC,      ///< "+CPINC:"
    ADL_STR_WSTR,       ///< "+WSTR:"
    ADL_STR_CMEE,       ///< "+CMEE:"
    ADL_STR_CREG,       ///< "+CREG:"
    ADL_STR_CGREG,      ///< "+CGREG:"
    ADL_STR_CRC,        ///< "+CRC:"
    ADL_STR_CGEV,       ///< "+CGEV:"
    ADL_STR_CGEREP,     ///< "+CGEREP:"
    
    ADL_STR_LAST        ///< Last string ID
} adl_strID_e;

/**
  @brief Argument type constants: ASCII
 */
#define ADL_STR_ARG_TYPE_ASCII  0
/**
  @brief Argument type constants: U32
 */
#define ADL_STR_ARG_TYPE_U32    1

/**
  * @}
  */
  
  
/**
  @defgroup adl_str_public_IFS_op1 AT Strings resources control
  @ingroup adl_str_public_IFS

  This service provides APIs to process AT standard response strings.
  */

/**
  * @addtogroup adl_str_public_IFS_op1
  * @{
  */

/**
  @par Description:
  This function returns the ID of the provided response string, with an optional argument and its type.

  @param[in]        rsp            String to parse to get the ID.
  @param[in]        arg            Parsed first argument; not used if set to NULL.
  @param[in]        argtype        Type of the parsed argument. If #argtype is #ADL_STR_ARG_TYPE_ASCII, #arg is an ascii * string ; if #argtype is #ADL_STR_ARG_TYPE_U32, #arg is an u32 * integer

  @return   Returned value description
  - <b>ADL_STR_NO_STRING</b>                     if the string is unknown
  - <b>Id</b>                                    Id of the string otherwise.
 */
//adl_strID_e adl_strGetIDExt ( ascii * rsp, void * arg, u8 * argtype );


/**
  @par Description:
  This function returns the ID of the provided response string.

  @param[in]        rsp            String to parse to get the ID.

  @return   Returned value description
  - <b>ADL_STR_NO_STRING</b>                     if the string is unknown
  - <b>Id</b>                                    Id of the string otherwise.
 */
//adl_strID_e adl_strGetID ( ascii * rsp );


/**
  @par Description:
  This function checks whether the provided response ID is a terminal one. A terminal response is the last response that a response handler will receive from a command.

  @param[in]        RspID            Response ID to check.

  @return   Returned value description
  - <b>TRUE</b>                      if the provided response ID is a terminal one.
  - <b>FALSE</b>                     otherwise.
 */
//bool adl_strIsTerminalResponse ( adl_strID_e RspID );


/**
  @par Description:
  This function provides a standard response string from its ID, with the provided argument.

  @param[in]        RspID            Response ID from which to get the string.
  @param[in]        arg              Response argument to copy in the response string. Depending on the response ID, this argument should be an u32 integer value, or an ascii * string.

  @return   Returned value description
  - Standard response string on success.
  - <b>NULL</b>                     if the ID does not exist.
  
  @warning The returned pointer memory is allocated by this function, but its ownership is transferred to the embedded application. This means that the embedded application will have to release the returned pointer.
 */
//ascii * adl_strGetResponseExt ( adl_strID_e RspID, u32 arg );


/**
  @par Description:
  This function provides the standard response string from its ID.

  @param[in]        RspID            Response ID from which to get the string.
  
  @return   Returned value description
  - Standard response string on success.
  - <b>NULL</b>                     if the ID does not exist.
  
  @warning The returned pointer memory is allocated by this function, but its ownership is transferred to the embedded application. This means that the embedded application will have to release the returned pointer.
 */
//ascii * adl_strGetResponse ( adl_strID_e RspID );

/**
  * @}
  */

#endif
