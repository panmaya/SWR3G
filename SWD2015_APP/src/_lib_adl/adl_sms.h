/*
 * adl_sms.h
 *
 * Created: 4/10/2557 10:45:47
 *  Author: pan
 */ 


/**********************************************************************************************************************/
/*  adl_sms.h   -  Copyright Sierra Wireless Inc. (c) 2010															  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_sms.h
  @brief ADL SMS Service public interface header file
  @ingroup adl_sms_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_sms_public_IFS ADL SMS Service Interface Specification
  @endif
  
  @section adl_sms_public_IFS_OVERVIEW Overview
  This document describes the public SMS Service interface of the ADL software element.
  
  @version 02.00

  @author SUB Infosys AT Maintenance

  @section adl_sms_public_IFS_HISTORY History
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
      <td>xxxx/xx/xx</td>	  
      <td>Creation</td>
      <td>XXXX</td>
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
  
  @section adl_sms_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_sms_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_sms_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
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
  @defgroup adl_sms_public_IFS ADL SMS Service Interface Specification
  @else
  @addtogroup adl_sms_public_IFS
  @{
  @endif
  
  @section adl_sms_public_IFS_ftr Features
  
  ADL provides a SMS service to filter, spy or process any reception or sending of SMS.
  
  The defined operations are:
  - A adl_smsSubscribe()
  - A adl_smsSubscribeExt()
  - A adl_smsSend()
  - A adl_smsUnsubscribe()
  
  @section adl_CmdHandler_public_IFS_sample Code Sample
  
  This example demonstrates how to use the SMS service in a nominal case (error cases not handled) with a Wireless CPU.
  
  Complete examples using the AT Command service are also available on the SDK.
  
  @code

    void SMS_ctrl_Handler(u8 Event, u16 Nb)
    {
        TRACE (( 1, "In SMS_ctrl_Handler" ));
    }

    s32 SMS_Handler(ascii *SmsTel, ascii *SmsTimeOrLength, ascii *SmsText)
    {
        s32 sms_tobe_fwd = ADL_SMS_FORWARD_INDICATION_AND_STORE;
        TRACE (( 1, "In SMS_Handler - 1 more SMS received" ));
        return sms_tobe_fwd;
    }

    // Somewhere in the application code, used as an event handler
    void MyFunction ( void )
    {
        // Local variables
        s8 sms_handle;        
        sms_handle = adl_smsSubscribeExt(SMS_Handler, SMS_ctrl_Handler, ADL_SMS_MODE_TEXT);        
    }

  @endcode
  @if GLOBAL_IFS
  @}
  @endif
  */


#ifndef __ADL_SMS_H__
#define __ADL_SMS_H__

/**
  @defgroup adl_sms_public_IFS_Definitions Types and constants definition
  @ingroup adl_sms_public_IFS
  */

/**
  * @addtogroup adl_sms_public_IFS_Definitions
  * @{
  */


/**
  @brief SMS handler defined response 
  
  @par Description: 
  CMTI indication is sent to external application.
  SMS is stored and sent to external application.
 */
#define ADL_SMS_FORWARD_INDICATION_AND_STORE 1
/**
  @brief SMS handler defined response 
  
  @par Description: 
  No indication is sent.
  SMS is deleted.
 */
#define ADL_SMS_FILTER_INDICATION_AND_DELETE 0
/**
  @brief SMS handler defined response 
  
  @par Description: 
  No indication is sent.
  SMS is stored and sent to external application.
 */
#define ADL_SMS_FILTER_INDICATION_AND_STORE -1

/**
  @brief SMS mode 
  
  @par Description: 
  Mode is PDU
 */
#define ADL_SMS_MODE_PDU 0x00

/**
  @brief SMS mode 
  
  @par Description: 
  Mode is Text
 */
#define ADL_SMS_MODE_TEXT 0x01

/**
  @brief SMS sending events 
  
  @par Description: 
  Sending is OK
 */
#define ADL_SMS_EVENT_SENDING_OK    0

/**
  @brief SMS sending events 
  
  @par Description: 
  Sending is NOK
 */
#define ADL_SMS_EVENT_SENDING_ERROR 1

/**
  @brief SMS sending events 
  
  @par Description: 
  Sending is OK with added informations
 */
#define ADL_SMS_EVENT_SENDING_MR    2

/**
  * @}
  */

/**
  @defgroup adl_sms_public_IFS_cb1 SMS handlers
  @ingroup adl_sms_public_IFS
  
  */

/**
  * @addtogroup adl_sms_public_IFS_cb1
  * @{
  */


/**
  @brief SMS handler definition
  
  @par Description:
  Process the received SMS.
  
  @param[in]  SmsTel  contains the originating telephone number of the SMS (in #ADL_SMS_MODE_TEXT mode), or NULL (in #ADL_SMS_MODE_PDU mode).   
  @param[in]  SmsTimeLength contains the SMS time stamp (in #ADL_SMS_MODE_TEXT mode), or the PDU length (in #ADL_SMS_MODE_PDU mode).
  @param[in]  SmsText contains the SMS text (in #ADL_SMS_MODE_TEXT mode), or the SMS PDU (in #ADL_SMS_MODE_PDU mode).

  @return 
  - TRUE    if the SMS must be forwarded to the external application (it is then stored in SIM memory) and the external application is notified by a "+CMTI" unsolicited indication.
  - FALSE   if the SMS should not be forwarded.
 */
typedef bool ( * adl_smsHdlr_f ) ( ascii * SmsTel, ascii * SmsTimeOrLength, ascii * SmsText );

/**
  @brief SMS handler definition (extended)
    
  @par Description:
  Process the received SMS.

  @param[in]  SmsTel  contains the originating telephone number of the SMS (in #ADL_SMS_MODE_TEXT mode), or NULL (in #ADL_SMS_MODE_PDU mode).   
  @param[in]  SmsTimeLength contains the SMS time stamp (in #ADL_SMS_MODE_TEXT mode), or the PDU length (in #ADL_SMS_MODE_PDU mode).
  @param[in]  SmsText contains the SMS text (in #ADL_SMS_MODE_TEXT mode), or the SMS PDU (in #ADL_SMS_MODE_PDU mode).
  
  @return 
  - #ADL_SMS_FORWARD_INDICATION_AND_STORE   if the SMS must be forwarded to the external application (it is then stored in SIM memory) and the external application is notified by a "+CMTI" unsolicited indication.
  - #ADL_SMS_FILTER_INDICATION_AND_DELETE   if the SMS should not be forwarded.
  - #ADL_SMS_FILTER_INDICATION_AND_STORE    if the SMS must be forwarded to the external application (it is then stored in SIM memory).
 */
//typedef s32 ( * adl_smsHdlrExt_f ) ( ascii * SmsTel, ascii * SmsTimeOrLength, ascii * SmsText );

/**
  @brief SMS control handler definition

  @par Description:
  Handle the event received.

  @param[in]  Event can be :
                -   #ADL_SMS_EVENT_SENDING_OK     the SMS was sent successfully.
                -   #ADL_SMS_EVENT_SENDING_ERROR  An error occurred during SMS sending
                -   #ADL_SMS_EVENT_SENDING_MR     the SMS was sent successfully (Note a #ADL_SMS_EVENT_SENDING_OK will follow)
  @param[in]  Nb    depending on parameter Event
                -   if Event is #ADL_SMS_EVENT_SENDING_OK         Nb parameter value is not relevant.
                -   if Event is #ADL_SMS_EVENT_SENDING_ERROR      Nb parameter contains the error number, according to "+CMS ERROR" value (cf. AT Commands Interface Guide).
                -   if Event is #ADL_SMS_EVENT_SENDING_MR         Nb parameter contains the sent Message Reference value.
 */
typedef void ( * adl_smsCtrlHdlr_f ) ( u8 Event, u16 Nb );

/**
  * @}
  */

/**
  @defgroup adl_sms_public_IFS_op1 SMS API functions
  @ingroup adl_sms_public_IFS
  
  These function allow SMS process
  */

/**
  * @addtogroup adl_sms_public_IFS_op1
  * @{
  */


/**
  @brief Subscribe to the SMS service 
  
  @par Description:
  This function subscribes to the SMS service in order to receive SMSs from the network.
  
  @param[in]    SmsHandler      SMS handler of type #adl_smsHdlr_f
  @param[in]    SmsCtrlHandler  SMS Control Handler of type #adl_smsCtrlHdlr_f
  @param[in]    Mode            Mode :
                                    - #ADL_SMS_MODE_PDU
                                    - #ADL_SMS_MODE_TEXT
  
  @return       Returned value description
  - Positive or null handle             on success
  - <b>ADL_RET_ERR_PARAM</b>            on parameter error
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>   if called from a low level interruption handler 
 */
s8 adl_smsSubscribe( adl_smsHdlr_f SmsHandler, adl_smsCtrlHdlr_f SmsCtrlHandler, u8 Mode );

/**
  @brief Subscribe to the SMS service (extended)
  
  @par Description:
  This function subscribes to the SMS service in order to receive SMSs from the network.
  
  @param[in]    SmsHandler      SMS handler of type #adl_smsHdlrExt_f
  @param[in]    SmsCtrlHandler  SMS Control Handler of type #adl_smsCtrlHdlr_f
  @param[in]    Mode            Mode :
                                    - #ADL_SMS_MODE_PDU
                                    - #ADL_SMS_MODE_TEXT
  
  @return       Returned value description
  - Positive or null handle             on success
  - <b>ADL_RET_ERR_PARAM</b>            on parameter error
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>   if called from a low level interruption handler 
 */
//s8 adl_smsSubscribeExt ( adl_smsHdlrExt_f SmsHandler, adl_smsCtrlHdlr_f SmsCtrlHandler, u8 Mode );

/**
  @brief Send a SMS
  
  @par Description:
  Send a SMS in text or PDU mode
  
  @param[in]    Handle      SMS service handle (returned by function #adl_smsSubscribe or #adl_smsSubscribeExt)
  @param[in]    SmsTel      Destination telephon number to send SMS
  @param[in]    SmsText     SMS in Text or PDU depending on Mode parameter
  @param[in]    Mode        Mode :
                                - #ADL_SMS_MODE_PDU
                                - #ADL_SMS_MODE_TEXT
  
  @return       Returned value description
  - <b>OK</b>                           on success
  - <b>ADL_RET_ERR_PARAM</b>            on parameter error
  - <b>ADL_RET_ERR_UNKNOWN_HDL</b>      if handle provided is unknown
  - <b>ADL_RET_ERR_BAD_STATE</b>        if service is not ready to send SMS
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>   if called from a low level interruption handler 
 */
s8 adl_smsSend( u8 Handle, ascii * SmsTel, ascii * SmsText, u8 Mode );

/**
  @brief Unsubscribe from the SMS service
  
  @par Description:
  Unsubscribe from the SMS service
  
  @param[in]    Handle      SMS service handle (returned by function #adl_smsSubscribe or #adl_smsSubscribeExt)
  
  @return       Returned value description
  - <b>OK</b>                           on success
  - <b>ADL_RET_ERR_NOT_SUBSCRIBED</b>   if the service is not subscribed   
  - <b>ADL_RET_ERR_UNKNOWN_HDL</b>      if handle provided is unknown
  - <b>ADL_RET_ERR_BAD_STATE</b>        if service is processing an SMS
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>   if called from a low level interruption handler 
 */
//s8 adl_smsUnsubscribe( u8 Handle );

/**
  * @}
  */

#endif
