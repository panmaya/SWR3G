/**********************************************************************************************************************/
/*  adl_fcm.h    -  Copyright Sierra Wireless Inc. (c) 2010															  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/****************************************************************************/
/* File    :   adl_fcm.h                                                    */
/*                                                                          */
/* Scope   :   FCM common constants and fonctions                           */
/*                                                                          */
/****************************************************************************/

/**
  @file adl_fcm.h
  @brief ADL FCM Service public interface header file
  @ingroup adl_fcm_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_fcm_public_IFS ADL FCM Service Interface Specification
  @endif

  @section adl_fcm_public_IFS_OVERVIEW Overview
  This document describes the public FCM Service interface of the ADL software element.
  
  @version 02.00

  @author SUB Infosys AT Maintenance

  @section adl_fcm_public_IFS_HISTORY History
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

  @section adl_fcm_public_IFS_INTRODUCTION Introduction

  @subsection adl_fcm_public_IFS_REMINDER Reminder

  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.

  @subsection adl_fcm_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary


  - <b>FCM</b>\n
    Flow Control Manager
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
  @defgroup adl_fcm_public_IFS ADL FCM Service Interface Specification
  @else
  @addtogroup adl_fcm_public_IFS
  @{
  @endif

  @section adl_fcm_public_IFS_ftr Features

  ADL provides a FCM (Flow Control Manager) service to handle all FCM events, and to access to the data ports provided on the product.

  The defined operations are:
  - An <b>adl_fcmSubscribe()</b> function subscribe to the FCM service.
  - An <b>adl_fcmUnsubscribe()</b> function to unsubscribe from the FCM service.
  - An <b>adl_fcmGetStatus()</b> function to get buffer status for requested flow handle.
  - An <b>adl_fcmIsAvailable()</b> function to check if a flow is available for the FCM service.
  - An <b>adl_fcmSendData()</b> function to send a data packet on requested flow.
  - An <b>adl_fcmSendDataExt()</b> function to send a data packet on requested flow.
  - An <b>adl_fcmReleaseCredits()</b> function to release credits for requested flow handle.
  - An <b>adl_fcmSwitchV24State()</b> function to change serial state to AT mode or Data mode.
  - An <b>adl_fcmFlushV24TxData()</b> function to flush data already sent to V24 on requested flow.

  @if GLOBAL_IFS
  @}
  @endif
  */
  
  
#ifndef __ADL_FCM_H__
#define __ADL_FCM_H__


/**
  @defgroup adl_fcm_public_IFS_Definitions Types and constants definition
  @ingroup adl_fcm_public_IFS
  */

/**
  * @addtogroup adl_fcm_public_IFS_Definitions
  * @{
  */

 
/**
  @brief  Specific FCM Service Error Code
  
  @par Description:
  a GSM or GPRS subscription is made when the other one is already subscribed
 */
#define ADL_FCM_RET_ERROR_GSM_GPRS_ALREADY_OPENNED  ADL_RET_ERR_SPECIFIC_BASE
/**
  @brief  Specific FCM Service Error Code
  
  @par Description:
  the flow has no more credit to use
 */
#define ADL_FCM_RET_ERR_WAIT_RESUME                 ADL_RET_ERR_SPECIFIC_BASE - 1

/**
  @brief  Specific FCM Service Error Code
  
  @par Description:
  Success, but the last credit was used
 */
#define ADL_FCM_RET_OK_WAIT_RESUME                  OK + 1

/**
  @brief  Specific FCM Service Error Code
  
  @par Description:
  the requested flow and way buffer is empty
 */
#define ADL_FCM_RET_BUFFER_EMPTY                    OK + 2

/**
  @brief  Specific FCM Service Error Code
  
  @par Description:
  the requested flow and way buffer is not empty
 */
 #define ADL_FCM_RET_BUFFER_NOT_EMPTY                OK + 3

/**
  @brief  Flow types
  
  @par Description:
  Flow types : uses an u16 type for FCM specific flags. FCM flow IDs are the ones listed in the adl_port_e type
 */
typedef u16 adl_fcmFlow_e;


/**
  @brief  GSM flow definition
  @deprecated Former GSM flow definition, the #adl_port_e type members should now be used instead.
 */
#define ADL_FCM_FLOW_GSM_DATA   ADL_PORT_GSM_BASE
/**
  @brief  GPRS flow definition
  @deprecated Former GPRS flow definition, the #adl_port_e type members should now be used instead.
 */
#define ADL_FCM_FLOW_GPRS       ADL_PORT_GPRS_BASE
/**
  @brief  UART1 flow definition
  @deprecated Former UART1 flow definition, the #adl_port_e type members should now be used instead.
 */
#define ADL_FCM_FLOW_V24_UART1  ADL_PORT_UART1
/**
  @brief  UART2 flow definition
  @deprecated Former UART2 flow definition, the #adl_port_e type members should now be used instead.
 */
#define ADL_FCM_FLOW_V24_UART2  ADL_PORT_UART2
/**
  @brief  USB flow definition
  @deprecated Former USB flow definition, the #adl_port_e type members should now be used instead.
 */
 #define ADL_FCM_FLOW_V24_USB    ADL_PORT_USB
/**
  @brief  UART3 flow definition
  @deprecated Former UART3 flow definition, the #adl_port_e type members should now be used instead.
 */
#define ADL_FCM_FLOW_V24_UART3  ADL_PORT_UART3


/**
  @brief  Slave subscription
  
  @par Description:
  Flag for a FCM slave subscription 
 */
#define ADL_FCM_FLOW_SLAVE  0x8000

/**
  @brief  UART1 flow definition in master
  @deprecated Former UART1 flow definition, the #adl_port_e type members should now be used instead.
 */
#define ADL_FCM_FLOW_V24_MASTER ADL_FCM_FLOW_V24_UART1
/**
  @brief  UART1 flow definition in slave
  @deprecated Former UART1 flow definition, the #adl_port_e type members should now be used instead.
 */
#define ADL_FCM_FLOW_V24        ( ADL_FCM_FLOW_V24_UART1 | ADL_FCM_FLOW_SLAVE )

/**
  @brief  GPRS preferred on V24 Master subscription
  @deprecated not used anymore
 */
#define ADL_FCM_FLOW_GPRS_PREFERRED 0


/**
  @brief  Serial link AT state
  @par Description
  equivalent to "offline" modem state, DCD/DSR off
 */
#define ADL_FCM_V24_STATE_AT    0
/**
  @brief  Serial link Data state
  @par Description
  equivalent to "online connected" modem state, DCD/DSR on
 */
#define ADL_FCM_V24_STATE_DATA  1
/**
  @brief  Serial link offline state
  @par Description
  equivalent to "offline connected" modem state, DCD on, DSR off
 */
 #define ADL_FCM_V24_STATE_OFFLINE   2


/**
  @brief FCM Events

  @par Description:
  This enum specifies all FCM events
 */
typedef enum
{
    ADL_FCM_EVENT_FLOW_OPENNED,            ///< related to #adl_fcmSubscribe
    ADL_FCM_EVENT_FLOW_CLOSED,             ///< related to #adl_fcmUnsubscribe
    ADL_FCM_EVENT_V24_DATA_MODE,           ///< related to #adl_fcmSwitchV24State
    ADL_FCM_EVENT_V24_DATA_MODE_EXT,       ///< 
    ADL_FCM_EVENT_V24_AT_MODE,             ///< related to #adl_fcmSwitchV24State
    ADL_FCM_EVENT_V24_AT_MODE_EXT,         ///< 
    ADL_FCM_EVENT_RESUME,                  ///< related to #adl_fcmSendData and #adl_fcmSend
    ADL_FCM_EVENT_MEM_RELEASE,             ///< related to #adl_fcmSendData and #adl_fcmSendDataExt
    ADL_FCM_EVENT_V24_DATA_MODE_FROM_CALL, ///< 
    ADL_FCM_EVENT_V24_AT_MODE_FROM_CALL,   ///< 
    ADL_FCM_EVENT_V24_TX_DATA_FLUSHED      ///< related to #adl_fcmFlushV24TxData
} adl_fcmEvent_e;


/**
  @brief FCM flow ways

  @par Description:
  This enum specifies the direction (or way) from which the buffer status is requested.
 */
typedef enum
{
    ADL_FCM_WAY_FROM_EMBEDDED,      ///< from embedded to application
    ADL_FCM_WAY_TO_EMBEDDED         ///< from application to embedded
} adl_fcmWay_e;



/**
  @brief FCM block type 

  @par Description:
  This structure specifies the Data block buffer. \
  The block must be dynamically allocated and filled by the application, before sending it to the function. The allocation size has to be sizeof ( #adl_fcmDataBlock_t ) + DataLength, where DataLength is the value to be set in the DataLength field of the structure. Maximum data packet size depends on the subscribed flow :On serial link based flows : 2000 bytes. On GSM data flow : no limitation (memory allocation size). On GPRS flow : 1500 bytes.
 */
typedef struct
{
    u16 Reserved1[4];   ///< Internal use only
    u32 Reserved3;      ///< Internal use only
    u16 DataLength;     ///< Data length
    u16 Reserved2[5];   ///< Internal use only
    u8  Data[1];        ///< Data to send
} adl_fcmDataBlock_t;


/**
  * @}
  */

  
/**
  @defgroup adl_fcm_public_IFS_cb1 FCM events handler
  @ingroup adl_fcm_public_IFS

  */

/**
  * @addtogroup adl_fcm_public_IFS_cb1
  * @{
  */

/**
  @par Description:
   FCM control events handler

  @param[in]        Event      This is the received event identifier (refer to #adl_fcmEvent_e for more information about the different events).
  
  @note The FCM control events are defined below (All handlers related to the concerned flow (master and slaves) will be notified together with these events).
 */
typedef bool ( * adl_fcmCtrlHdlr_f ) ( adl_fcmEvent_e Event );


/**
  @par Description:
   FCM data events handler. This handler receives data blocks from the associated flow.\n
   Once the data block is processed, the handler must return <b>TRUE</b> to release the credit, or <b>FALSE</b> if the credit must not be released. In this case, all credits will be released next time the handler will return <b>TRUE</b>.\n
   On all flows, all subscribed data handlers (master and slaves) are notified with a data event, and the credit will be released only if all handlers return <b>TRUE</b>: each handler should return <b>TRUE</b> as default value.\n
   If a credit is not released on the data block reception, it will be released next time the data handler will return <b>TRUE</b>. The #adl_fcmReleaseCredits should also be used to release credits outside the data handler.\n
   Maximum size of each data packets to be received by the data handlers depends on the flow type:\n
  - On serial link flows (UART physical & logical based ports): 120 bytes\n
  - On GSM CSD data port: 270 bytes\n
  - On GPRS port: 1500 bytes\n
  If data size to be received by the Open AT application exceeds this maximum packet size, data will be segmented by the Flow Control Manager, which will call several times the Data Handlers with the segmented packets.\n
  Please note that on GPRS flow, whole IP packets will always be received by the Open AT application.

  @param[in]        DataSize     This is the received Data size.
  @param[in]        Data         This is the received Data buffer.
 */
typedef bool ( * adl_fcmDataHdlr_f ) ( u16 DataSize, u8 * Data );


/**
  * @}
  */

/**
  @defgroup adl_fcm_public_IFS_op1 FCM resources control
  @ingroup adl_fcm_public_IFS

  These function allows to allocate or release the a FCM flow
  */

/**
  * @addtogroup adl_fcm_public_IFS_op1
  * @{
  */

/**
  @par Description:
  This function subscribes to the FCM service, opening the requested port and setting the control and data handlers. The subscription will be effective only when the control event handler has received the #ADL_FCM_EVENT_FLOW_OPENED event. \n
  Each port may be subscribed only one time.\n
  Additional subscriptions may be done, using the #ADL_FCM_FLOW_SLAVE flag (see below). Slave subscribed handles will be able to send and receive data on/from the flow, but will know some limitations:\n
  - For serial-line flows (UART physical and logical based ports), only the main handle will be
able to switch the Serial Link state between AT & Data mode\n
  - If the main handle unsubscribe from the flow, all slave handles will also be unsubscribed.\n
  

  @param[in]        Flow            The allowed values are the available ports of the #adl_port_e type. Only ports with the
FCM capability may be used with this service (ie. all ports except the #ADL_PORT_OPEN_AT_VIRTUAL_BASE and not SPP #ADL_PORT_BLUETOOTH_VIRTUAL_BASE based ones).
  @param[in]        CtrlHandler     FCM control events handler
  @param[in]        DataHandler     FCM data events handler

  @return   Returned value description
  - <b>Positive or NULL </b>                            if allocation succeed, Flow handle on success to be used on further FCM API functions calls.
  - <b>#ADL_RET_ERR_PARAM</b>                           if one parameter has an incorrect value.
  - <b>#ADL_RET_ERR_ALREADY_SUBSCRIBED</b>              if the ressource is already subscribed.
  - <b>#ADL_RET_ERR_NOT_SUBSCRIBED</b>                  if master is not subscribed.
  - <b>#ADL_FCM_RET_ERROR_GSM_GPRS_ALREADY_OPENNED</b>  if GSM or GPRS flow is already opened
  - <b>#ADL_RET_ERR_BAD_STATE</b>                       if the required port is not available
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>                  if called from a low level interruption handler

  @note     ERROR values are defined in adl_error.h.
  
  @warning @{ For serial-link related flows (UART physical and logical based ports), the corresponding port has to be opened first with the AT+WMFM command (for physical ports), or with the 27.010 protocol driver on the external application side (for logical ports), otherwise the subscription will fail. See AT Commands Interface Guide for more information.\n
By default, only the UART1 physical port is opened.\n
A specific port state may be known using the ADL AT/FCM port service.}@

  @note @{ Please note that the #adl_fcmFlow_e type is the same than the #adl_port_e one, except the fact that it may handle some additional FCM specific flags (see below). Previous versions FCM flows identifiers have been kept for ascendant compatibility. However, these constants should be considered as deprecated, and the #adl_port_e type members should now be
used instead.\n
#define ADL_FCM_FLOW_V24_UART1 ADL_PORT_UART1\n
#define ADL_FCM_FLOW_V24_UART2 ADL_PORT_UART2\n
#define ADL_FCM_FLOW_V24_USB ADL_PORT_USB\n
#define ADL_FCM_FLOW_V24_UART3 ADL_PORT_UART3\n
#define ADL_FCM_FLOW_GSM_DATA ADL_PORT_GSM_BASE\n
#define ADL_FCM_FLOW_GPRS ADL_PORT_GPRS_BASE\n
To perform a slave subscription (see above), a bit-wise or has to be done with the flow ID and the #ADL_FCM_FLOW_SLAVE flag ; for example:
adl_fcmSubscribe (ADL_PORT_UART1 | ADL_FCM_FLOW_SLAVE, MyCtrlHandler, MyDataHandler ); }@

  @note When "7 bits" mode is enabled on a v24 serial link, in data mode, payload data is located on the 7 least significant bits (LSB) of every byte.
  
  @note @{ When a serial link is in data mode, if the external application sends the sequence "1s delay ; +++ ; 1s delay", this serial link is switched to AT mode, and corresponding handler is notified by the #ADL_FCM_EVENT_V24_AT_MODE_EXT event.\n
  Application can emulate the sequence "1s delay; +++;1s delay" behaviour with #adl_fcmSwitchV24State API and #ADL_FCM_EVENT_V24_STATE_OFFLINE parameter.Then the behaviour depends on the returned value:
    If it is TRUE, all this flow remaining handlers are also notified with this event. The main handle can not be un-subscribed in this state.
	If it is FALSE, this flow remaining handlers are not notified with this event, and this serial link is switched back immediately to data mode.
    In the first case, after the ADL_FCM_EVENT_V24_AT_MODE_EXT event, the main handle subscriber should switch the serial link to data mode with the adl_fcmSwitchV24State API, or wait for the ADL_FCM_EVENT_V24_DATA_MODE_EXT event. This one will come when the external application sends the "ATO" command: the serial link is switched to data mode, and then all V24 clients are notified.@}

  @note When a GSM data call is released from the remote part, the GSM flow will automatically be unsubscribed (the ADL_FCM_EVENT_FLOW_CLOSED event will be received by all the flow subscribers).
  @note When a GPRS session is released, or when a GSM data call is released from the embedded module side (with the adl_callHangUp function), the corresponding GSM or GPRS flow have to be unsubscribed. These flows will have to be subscribed again before starting up a new GSM data call, or a new GPRS session.
  @note For serial link flows, the serial line parameters (speed, character framing, etc...) must not be modified while the flow is in data state. In order to change these parameters' value, the concerned flow has to be first switched back in AT mode with the adl_fcmSwitchV24State API. Once the parameters changed, the flow may be switched again to data mode, using the same API.
  @note PORT service should be subscribed if the FCM service is being used on USB PORT.
  When the USB cable is unplugged, the FCM service automatically gets unsubscribed and correspondingly the FCM control event handler is notified by the ADL_FCM_EVENT_FLOW_CLOSED event.
  @note For USB port, FCM services can be used only when the USB cable is connected.

  @note The sum of sending and receiving credits is 6.
  @note To perform a GSM data call, the GSM flow should be open first. Only when the flow opened event (ADL_FCM_EVENT_FLOW_OPENED) is received, then a data call can be done or answered.
 */
s8 adl_fcmSubscribe ( adl_fcmFlow_e Flow, adl_fcmCtrlHdlr_f CtrlHandler, adl_fcmDataHdlr_f DataHandler );


/**
  @par Description:
  This function unsubscribes from a previously subscribed FCM service, closing the previously opened flows. The unsubscription will be effective only when the control event handler has received the #ADL_FCM_EVENT_FLOW_CLOSED event.\n
  If slave handles were subscribed, as soon as the master one unsubscribes from the flow, all the slave one will also be unsubscribed.


  @param[in]        Handle              Handle returned by the #adl_fcmSubscribe function
  
  @return   Returned value description
  - <b>OK </b>                          on success. The Control handler will also receive a #ADL_FCM_EVENT_FLOW_CLOSED event
when flow is ready to process
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>     if the provided handle is incorrect.
  - <b>#ADL_RET_ERR_NOT_SUBSCRIBED</b>  if flow is already unsubscribed.
  - <b>#ADL_RET_ERR_BAD_STATE</b>       if the serial link is not in AT mode.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>  if the function was called from a low level interrupt handler (the function is forbidden in this context).

  @note     ERROR values are defined in adl_error.h.
 */
//s8 adl_fcmUnsubscribe ( u8 Handle );

/**
  @par Description:
  This function switches a serial link state to AT mode or to Data mode. The operation will be effective only when the control event handler has received an ADL_FCM_EVENT_V24_XXX_MODE event. Only the main handle subscriber can use this API.


  @param[in]        Handle              Handle returned by the #adl_fcmSubscribe function.
  @param[in]        State               Serial link state to switch to. Allowed values: #ADL_FCM_V24_STATE_AT( equivalent to "offline" modem state, DCD/DSR off ), #ADL_FCM_V24_STATE_DATA ( equivalent to "online connected" modem state, DCD/DSR on ), #ADL_FCM_V24_STATE_OFFLINE (equivalent to "offline connected" modem state, DCD on, DSR off )
  
  @return   Returned value description
  - <b>OK </b>                          on success. The Control handler will also receive a ADL_FCM_EVENT_V24_XXX_MODE event when the serial link state has changed
  - <b>#ADL_RET_ERR_PARAM</b>           if one parameter has an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>     if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_HDL</b>         if the handle is not the main flow one.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>  if the function was called from a low level interrupt handler (the function is forbidden in this context).

  @note     ERROR values are defined in adl_error.h.
 */
s8 adl_fcmSwitchV24State ( u8 Handle, u8 State );



/**
  @par Description:
  This function sends a data block on the requested flow. This API do not perform any processing on provided data block, which is sent directly on the flow.


  @param[in]        Handle               Handle returned by the #adl_fcmSubscribe function.
  @param[in]        DataBlock            Data block buffer to write, please refer to #adl_fcmDataBlock_t description
  
  @return   Returned value description
  - <b>OK </b>                           on success. The Control handler will also receive a #ADL_FCM_EVENT_MEM_RELEASE event when the data block memory buffer will be released,
  - <b>#ADL_FCM_RET_OK_WAIT_RESUME</b>   on success, but the last credit was used. The Control handler will also receive a #ADL_FCM_EVENT_MEM_RELEASE event when the data block memory buffer will be released
  - <b>#ADL_RET_ERR_PARAM</b>            if one parameter has an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>      if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>        if the flow is not ready to send data.
  - <b>#ADL_FCM_RET_ERR_WAIT_RESUME</b>  if the flow has no more credit to use.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>  if the function was called from a low level interrupt handler (the function is forbidden in this context).

  @note     ERROR values are defined in adl_error.h.
  
  @note On ADL_FCM_RET_XXX_WAIT_RESUME returned value, the subscriber has to wait for an #ADL_FCM_EVENT_RESUME event on Control Handler to continue sending data.
  
  @note <b>Important remark:</b> The Data block will be released by the #adl_fcmSendDataExt API on <b>OK</b> and #ADL_FCM_RET_OK_WAIT_RESUME return values (the memory buffer will be effectively released once the #ADL_FCM_EVENT_MEM_RELEASE event will be received in the Control Handler). The application has to use only dynamic allocated buffers (with #adl_memGet function).
 */
//s8 adl_fcmSendDataExt ( u8 Handle, adl_fcmDataBlock_t * DataBlock );

/**
  @par Description:
  This function allows to send a data packet on requested flow


  @param[in]        Handle               Handle returned by the #adl_fcmSubscribe function.
  @param[in]        DataBlock            Data block buffer to write.
  @param[in]        DataLen              Data block buffer size.. Maximum data packet size depends on the subscribed flow. On serial link based flows: 2000 bytes. On GSM data flow: no limitation (memory allocation size). On GPRS flow: 1500 bytes.
  
  @return   Returned value description
  - <b>OK </b>                           on success. The Control handler will also receive a #ADL_FCM_EVENT_MEM_RELEASE event
when the data block memory buffer will be released.
  - <b>#ADL_FCM_RET_OK_WAIT_RESUME</b>   on success, but the last credit was used. The Control handler will also receive a #ADL_FCM_EVENT_MEM_RELEASE event when the data block memory buffer will be released
  - <b>#ADL_RET_ERR_PARAM</b>            if one parameter has an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>      if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>        if the flow is not ready to send data.
  - <b>#ADL_FCM_RET_ERR_WAIT_RESUME</b>  if the flow has no more credit to use.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>  if the function was called from a low level interrupt handler (the function is forbidden in this context).

  @note     ERROR values are defined in adl_error.h.
  
  @note On ADL_FCM_RET_XXX_WAIT_RESUME returned value, the subscriber has to wait for a #ADL_FCM_EVENT_RESUME event on Control Handler to continue sending data.
 */
s8 adl_fcmSendData ( u8 Handle, u8 * Data, u16 DataLen );


/**
  @par Description:
  This function releases some credits for requested flow handle.\n
  The slave subscribers should not use this API.


  @param[in]        Handle              Handle returned by the #adl_fcmSubscribe function
  @param[in]        NbCredits           Number of credits to release for this flow. If this number is higher than the number of
previously received data blocks, all credits are released. If an application wants to release all received credits at any time, it should call the #adl_fcmReleaseCredits API with #NbCredits parameter set to 0xFF.
  
  @return   Returned value description
  - <b>OK </b>                          on success
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>     if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_HDL</b>         if the handle provided is a slave one
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>  if the function was called from a low level interrupt handler (the function is forbidden in this context).

  @note     ERROR values are defined in adl_error.h.

  @note     When all the available credits are consumed then data flow can be resumed only after releasing at least 3 credits.
 */
//s8 adl_fcmReleaseCredits ( u8 Handle, u8 NbCredits );


/**
  @par Description:
  This function gets the buffer status for requested flow handle, in the requested way.


  @param[in]        Handle           Handle returned by the #adl_fcmSubscribe function.
  @param[in]        Way              Way from which get the buffer status, please refer to the #adl_fcmWay_e description.
  
  @return   Returned value description
  - <b>OK </b>                              on success
  - <b>#ADL_FCM_RET_BUFFER_EMPTY</b>        if the requested flow and way buffer is empty
  - <b>#ADL_FCM_RET_BUFFER_NOT_EMPTY</b>    if the requested flow and way buffer is not empty ; the Flow Control Manager is still processing data on this flow.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>         if the provided handle is unknown.
  - <b>#ADL_RET_ERR_PARAM</b>               if the way parameter value is out of range.

  @note     ERROR values are defined in adl_error.h.
 */
//s8 adl_fcmGetStatus ( u8 Handle, adl_fcmWay_e Way );


/**
  @par Description:
  This function checks if a flow is available for the FCM service


  @param[in]        Handle           Port from which to require the state.
  
  @return   Returned value description
  - <b>TRUE</b>                      if the port is ready to handle the FCM service
  - <b>FALSE</b>                     if it is not ready
  
  @note @{ All ports should be available for the FCM service, except:\n
  The Open AT virtual one, which can only be used for AT commands\n
  If the port is already used to handle a feature required by an external application through the AT
commands (+WLDB command, or a CSD/GPRS data session is already running)@}
 */
//bool adl_fcmIsAvailable ( adl_fcmFlow_e Flow );


/**
  @par Description:
  This function Flush the data already sent to V24 for requested flow handle.\n
  The slave subscribers should not use this API.


  @param[in]        Handle              Handle returned by the #adl_fcmSubscribe function
  
  @return   Returned value description
  - <b>OK </b>                          on success. The Control handler will also receive a #ADL_FCM_EVENT_V24_TX_DATA_FLUSHED event
  when flush is finished
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>     if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_HDL</b>         if the handle provided is a slave one
  - <b>#ADL_RET_ERR_BAD_STATE</b>       if the flow is not ready to send data.
  - <b>#ADL_RET_ERR_PARAM</b>           if one parameter has an incorrect value.

  @note     ERROR values are defined in adl_error.h.
 */
//s8 adl_fcmFlushV24TxData ( u8 Handle );

/**
  * @}
  */


#endif // __ADL_FCM_H__

