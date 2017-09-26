/**********************************************************************************************************************/
/* adl_port.h   -  Copyright Sierra Wireless Inc. (c) 2010															  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/****************************************************************************/
/* File    :   adl_port.h                                                   */
/*                                                                          */
/* Scope   :   Port service definitions                                     */
/*                                                                          */
/****************************************************************************/


/**
  @file adl_port.h
  @brief ADL AT/FCM IO Ports Service public interface header file
  @ingroup adl_port_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_port_public_IFS ADL AT/FCM IO Ports Service Interface Specification
  @endif
  
  @section adl_port_public_IFS_OVERVIEW Overview
  This document describes the public AT/FCM IO Ports Service interface of the ADL software element.
  
  @version 02.00
  
  @author SUB Infosys AT Maintenance
  
  @section adl_port_public_IFS_HISTORY History
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
      <td>01.01</td>
      <td>N/A</td>	  
      <td>2011/04/05</td>	 
      <td>Minor Update:<br>
          Update the adl_portIsAvailable description for the ADL_PORT_GPRS_BASE.
      </td>
      <td>INF</td>
  </tr>
  <tr>
      <td>01.02/03</td>
      <td>N/A</td>	  
      <td>2011/04/18</td>	  
      <td>Minor Update:<br>
          Removed the ADL_PORT_RDMS_VIRTUAL_BASE value from the adl_port_e enum.
      </td>
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
  
  @section adl_port_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_port_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_port_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
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
  @defgroup adl_port_public_IFS ADL AT/FCM IO Ports Service Interface Specification
  @else
  @addtogroup adl_port_public_IFS
  @{
  @endif
  
  @section adl_port_public_IFS_ftr Features
  
  ADL applications may use this service to be informed about the product AT/FCM IO ports states

  The defined operations are:
  - An <b>adl_portSubscribe()</b> function to subscribe to the AT/FCM IO Ports Service
  - An <b>adl_portUnsubscribe()</b> function to unsubscribe from the AT/FCM IO Ports Service
  - An <b>adl_portIsAvailable()</b> function to check if the required port is currently opened or not.
  - An <b>adl_portSendPriorityDataExt()</b> function to send the data with priority to specified port.
  - An <b>adl_portGetSignalState()</b> function to get the required port signal state.
  - An <b>adl_portStartSignalPolling()</b> function to start a polling process on a required port signal.
  - An <b>adl_portStopSignalPolling()</b> function to stop a polling process on a required port signal.
  
  @if GLOBAL_IFS
  @}
  @endif
  */

#ifndef __ADL_PORT_H__
#define __ADL_PORT_H__


/**
  @defgroup adl_port_public_IFS_Definitions Types and constants definition
  @ingroup adl_port_public_IFS

  */


/**
  * @addtogroup adl_port_public_IFS_Definitions
  * @{
  */

/**
  @brief Port max handles number

  @par Description:
  Port max handles number
 */
#define ADL_PORT_HANDLE_MAX     0x7F

/**
  @brief Signal change event base

  @par Description:
  Signal change event base
 */
#define ADL_PORT_EVENT_SIGNAL_CHANGE_BASE   ADL_PORT_EVENT_CTS_STATE_CHANGE

/**
  @brief Ports identifiers

  @par Description:
  Ports identifiers.\n
  AT Commands and FCM services can be used to send and receive AT Commands or data blocks, to or from one of the product ports. These ports are linked either to product physical serial ports (as UART1 / UART2 / USB ports), or virtual ports (as Open AT virtual AT port, GSM CSD call data port, GPRS session data port or Bluetooth virtual ports).\n
  AT/FCM IO Ports are identified by the type below:
 */
typedef enum
{
    ADL_PORT_NONE,                            ///< not usable
    ADL_PORT_UART1,                           ///< Product physical UART 1. Please refer to the AT+WMFM command documentation to know how to open/close this product port
    ADL_PORT_UART2,                           ///< Product physical UART 2. Please refer to the AT+WMFM command documentation to know how to open/close this product port
    ADL_PORT_USB,                             ///< Product physical USB. Please refer to the AT+WMFM command documentation to know how to open/close this product port
    ADL_PORT_UART3,                           ///< Product physical UART 3. Please refer to the AT+WMFM command documentation to know how to open/close this product port


    ADL_PORT_UART1_VIRTUAL_BASE       = 0x10, ///< Base ID for 27.010 protocol logical channels on UART 1. Please refer to AT+CMUX command & 27.010 protocol documentation to know how to open/close such a logical channel
    ADL_PORT_UART2_VIRTUAL_BASE       = 0x20, ///< Base ID for 27.010 protocol logical channels on UART 2. Please refer to AT+CMUX command & 27.010 protocol documentation to know how to open/close such a logical channel
    ADL_PORT_USB_VIRTUAL_BASE         = 0x30, ///< Base ID for 27.010 protocol logical channels on USB link (reserved for future products
    ADL_PORT_UART3_VIRTUAL_BASE       = 0x40, ///< Base ID for 27.010 protocol logical channels on UART 3. Please refer to AT+CMUX command & 27.010 protocol documentation to know how to open/close such a logical channel
    ADL_PORT_GSM_BASE                 = 0x50, ///< Virtual Port ID for GSM CSD data call flow. ONLY USABLE WITH THE FCM SERVICE. Please note that this port will be considered as always available (no OPEN/CLOSE events for this port; #adl_portIsAvailable function will always return TRUE)
    ADL_PORT_GPRS_BASE                = 0x60, ///< Virtual Port ID for GPRS data session flow. ONLY USABLE WITH THE FCM SERVICE. Please note that this port will be considered as always available (no OPEN/CLOSE events for this port; #adl_portIsAvailable function will always return TRUE) if the GPRS feature is supported on the current product.
    ADL_PORT_RDMS_SERVER_VIRTUAL_BASE = 0x71, ///< Virtual Port ID for IDS service supporting the flow of messages exchanged with the server (only internal use).
    ADL_PORT_OPEN_AT_VIRTUAL_BASE     = 0x80  ///< Base ID for AT commands contexts dedicated to Open AT applications. ONLY USABLE WITH THE AT COMMANDS SERVICE. This port is always available, and is opened immediately at the product’s start-up. This is the default port where are executed the AT commands sent by the AT Command service
} adl_port_e;


/**
  @brief Port service events

  @par Description:
  Port service events.
 */
typedef enum
{
    ADL_PORT_EVENT_OPENED,              ///< Informs the ADL application that the specified <b>Port</b> is now opened. According to its type, it may now be used with either AT Commands service or FCM service.
    ADL_PORT_EVENT_CLOSED,              ///< Informs the ADL application that the specified <b>Port</b> is now closed. It is not usable anymore with neither AT Commands service nor FCM service.
    ADL_PORT_EVENT_RTS_STATE_CHANGE,    ///< Informs the ADL application that the specified <b>Port</b> RTS signal state has changed to the new <b>State</b> value (0/1). This event will be received by all subscribers which have started a polling process on the specified <b>Port</b> RTS signal with the #adl_portStartSignalPolling function. The handler <b>Port</b> parameter uses the #adl_port_e type described above. The handler <b>State</b> parameter is set only for the #ADL_PORT_EVENT_XXX_STATE_CHANGE events.
    ADL_PORT_EVENT_DTR_STATE_CHANGE     ///< Informs the ADL application that the specified <b>Port</b> DTR signal state has changed to the new <b>State</b> value (0/1). This event will be received by all subscribers which have started a polling process on the specified <b>Port</b> DTR signal with the #adl_portStartSignalPolling function.
} adl_portEvent_e;

/**
  @brief Port serial signals

  @par Description:
  Port serial signals
 */
typedef enum
{
    ADL_PORT_SIGNAL_RTS,    ///< Required port RTS input signal: physical pin in case of a physical port (UARTX), emulated logical signal in case of a 27.010 logical port.
    ADL_PORT_SIGNAL_DTR,    ///< Required port DTR input signal: physical pin in case of a physical port (UARTX), emulated logical signal in case of a 27.010 logical port
    ADL_PORT_SIGNAL_LAST    ///< Internal use only
} adl_portSignal_e;







/**
  @brief CTS state change
  @deprecated depreciated ports events constants
 */
#define ADL_PORT_EVENT_CTS_STATE_CHANGE    ADL_PORT_EVENT_RTS_STATE_CHANGE

/**
  @brief DSR state change
  @deprecated depreciated ports events constants
 */
 #define ADL_PORT_EVENT_DSR_STATE_CHANGE    ADL_PORT_EVENT_DTR_STATE_CHANGE
 
/**
  @brief CTS port signal constant
  @deprecated depreciated port signal constants
 */
#define ADL_PORT_SIGNAL_CTS    ADL_PORT_SIGNAL_RTS
/**
  @brief DSR port signal constant
  @deprecated depreciated port signal constants
 */
 #define ADL_PORT_SIGNAL_DSR    ADL_PORT_SIGNAL_DTR



/**
  * @}
  */

/**
  @defgroup adl_port_public_IFS_cb1 AT/FCM IO Ports events handler
  @ingroup adl_port_public_IFS

  */

/**
  * @addtogroup adl_port_public_IFS_cb1
  * @{
  */

/**
  @par Description:
  Port handler

  @param[in]        Event      Event, please refer to the #adl_portEvent_e description
  @param[in]        Port       Port on which the event happens 
  @param[in]        State      Port DTR signal
 */
typedef void ( * adl_portHdlr_f ) ( adl_portEvent_e Event, adl_port_e Port, u8 State );
/**
  * @}
  */


/**
  @defgroup adl_port_public_IFS_op1 AT/FCM IO Ports resources control
  @ingroup adl_port_public_IFS

  ADL applications may use this service to be informed about the product AT/FCM IO ports states.
  */

/**
  * @addtogroup adl_port_public_IFS_op1
  * @{
  */

/**
  @par Description:
  This function subscribes to the AT/FCM IO Ports service in order to receive specific ports related events.

  @param[in]        PortHandler            Port related events handler supplied by the application. Please refer to #adl_portHdlr_f type definition for more information.

  @return   Returned value description
  - <b>Positive or NULL</b>                handle on success, to be used in further Port service function calls. A confirmation event will then be received in the service handler
  - <b>#ADL_RET_ERR_PARAM</b>              on parameter error.
  - <b>#ADL_RET_ERR_NO_MORE_HANDLES</b>    if there is no more free handles (the service is able to process up 127 subscriptions)
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>     if the function was called from a low level Interrupt handler (the function is forbidden in this context).

  @note     ERROR values are defined in adl_error.h.
 */
//s8 adl_portSubscribe ( adl_portHdlr_f PortHandler );


/**
  @par Description:
  This function unsubscribes from the AT/FCM IO Ports service. The related handler will not receive ports related events any more. If a signal polling process was started only for this handle, it will be automaticaly stopped.

  @param[in]        Handle            Handle previously returned by the #adl_portSubscribe function.

  @return   Returned value description
  - <b>OK</b>                         on success
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>        if the provided handle is unknown.
  - <b>#ADL_RET_ERR_NOT_SUBSCRIBED</b>     if the service is not subscribed.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>     if the function was called from a low level Interrupt handler (the function is forbidden in this context).

  @note     ERROR values are defined in adl_error.h.
 */
//s8 adl_portUnsubscribe ( u8 Handle );


/**
  @par Description:
  This function checks if the required port is currently opened or not.

  @param[in]        Port            Port from which to require the current state.

  @return   Returned value description
  - <b>TRUE</b>                     if the port is currently opened;
  - <b>FALSE</b>                    if the port is closed, or if it does not exists.
  
  @note The function will always return TRUE on the #ADL_PORT_GSM_BASE port
  @note The function will always return TRUE on the #ADL_PORT_GPRS_BASE port.
  */
//bool adl_portIsAvailable ( adl_port_e Port );

/**
  @par Description:
  This function sends the data with priority to specified port.

  @param[in]        Buffer        Data buffer to send
  @param[in]        Length        Data buffer Length
  @param[in]        Port          Required port
 */
//void adl_portSendPriorityDataExt ( const u8 * Buffer, u8 Length, adl_port_e Port );


/**
  @par Description:
  This function returns the required port signal state.

  @param[in]        Port            Port from which to require the current signal state. Only physical output related ports (UARTX & USB ones, used as physical ports, or with the 27.010 protocol) may be used with this function.
  @param[in]        Signal          Signal from which to query the current state. Please refer to #adl_portSignal_e definition

  @return   Returned value description
  - <b>0/1</b>                           signal state (0/1) on success
  - <b>#ADL_RET_ERR_PARAM</b>            on parameter error.
  - <b>#ADL_RET_ERR_BAD_STATE</b>        if the required port is not opened.

  @note     ERROR values are defined in adl_error.h.
 */
// s8 adl_portGetSignalState ( adl_port_e Port, adl_portSignal_e Signal );


/**
  @par Description:
  This function starts a polling process on a required port signal for the provided subscribed handle.\n
  Only one polling process can run at a time. A polling process is defined on one port, for one or several of this port's signals.\n
  It means that this function may be called several times on the same port in order to monitor several signals; the polling time interval is set up by the first function call (polling tme parameters are ignored or further calls). If the function is called several times on the same port & signal, additional calls will be ignored. \n
  Once a polling process is started on a port's signal, this one is monitored: each time this signal state changes, a #ADL_PORT_EVENT_XXX_STATE_CHANGE event is sent to all the handlers which have required a polling process on it.\n
  Whatever is the number of requested signals and subscribers to this port polling process, a single cyclic timer will be internally used for this one.

  @param[in]        Handle                Handle previously returned by the #adl_portSubscribe function.
  @param[in]        Port                  Port on which to run the polling process. Only physical output related ports (UARTX & USB ones, used as physical ports, or with the 27.010 protocol) may be used with this function
  @param[in]        Signal                Signal to monitor while the polling process. See the adl_portGetSignalState function for information about the available signals.
  @param[in]        PollingTimerType      PollingTimerValue parameter value's unit. The allowed values are defined: #ADL_TMR_TYPE_100MS (PollingTimerValue is in 100 ms steps) or #ADL_TMR_TYPE_TICK (PollingTimerValue is in 18.5 ms tick steps). This parameter is ignored on additional function calls on the same port.
  @param[in]        PollingTimerValue     Polling time interval (uses the PollingTimerType parameter's value unit). This parameter is ignored on additional function calls on the same port.

  @return   Returned value description
  - <b>OK</b>                               on success
  - <b>#ADL_RET_ERR_PARAM</b>               on parameter error.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>         if the provided handle is unknown.
  - <b>#ADL_RET_ERR_NOT_SUBSCRIBED</b>      if the service is not subscribed.
  - <b>#ADL_RET_ERR_BAD_STATE</b>           if the required port is not opened.
  - <b>#ADL_RET_ERR_ALREADY_SUBSCRIBED</b>  if a polling process is already running on another port.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>      if the function was called from a low level Interrupt handler (the function is forbidden in this context).

  @note     ERROR values are defined in adl_error.h.
 */
//s8 adl_portStartSignalPolling ( u8 Handle, adl_port_e Port, adl_portSignal_e Signal, u8 PollingTimerType, u32 PollingTimerValue );

/**
  @par Description:
  This function stops a running polling process on a required port signal for the provided subscribed handle. \n
The associated handler will not receive the #ADL_PORT_EVENT_XXX_STATE_CHANGE events related to this signal port anymore.\n
The internal polling process cyclic timer will be stopped as soon as the last subscriber to the current running polling process has call this function.

  @param[in]        Handle                Handle previously returned by the #adl_portSubscribe function.
  @param[in]        Port                  Port on which the polling process to stop is running.
  @param[in]        Signal                Signal on which the polling process to stop is running.

  @return   Returned value description
  - <b>OK</b>                               on success
  - <b>#ADL_RET_ERR_PARAM</b>               on parameter error.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL</b>         if the provided handle is unknown.
  - <b>#ADL_RET_ERR_NOT_SUBSCRIBED</b>      if the service is not subscribed.
  - <b>#ADL_RET_ERR_BAD_STATE</b>           if the required port is not opened.
  - <b>#ADL_RET_ERR_BAD_HDL</b>             if there is no running polling process for this Handle / Port / Signal combination.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>      if the function was called from a low level Interrupt handler (the function is forbidden in this context).

  @note     ERROR values are defined in adl_error.h.
 */
//s8 adl_portStopSignalPolling ( u8 Handle, adl_port_e Port, adl_portSignal_e Signal );


/**
  @par Description:
  This function returns the maximum number of physical ports.

  @return   Returned value description
  - <b> >=0 </b>                            max port number 

  */
//u8 adl_portGetPhyPortMaxNb (void);




/**
  @brief Provide max physical port 

  @par Description:
  Provides the maximum number of physical port. 
 */
#define ADL_PORT_MAX    3//adl_portGetPhyPortMaxNb()


/**
  @brief Extract port physical base

  @par Description:
  Extracts the physical port identifier part of the provided "_port". E.g. if used on a 27.010 virtual port identifier based on the UART 2, this macro will return #ADL_PORT_UART2
 */
#define ADL_PORT_GET_PHYSICAL_BASE(_port)   ( ( (_port) & 0xF0 )?  (_port) >>4  : (_port))

/**
  @brief Check condition : is the provided event a signal change one ?

  @par Description:
  Returns TRUE if the event "_event" is a signal change one, FALSE otherwise.
 */
#define ADL_PORT_IS_A_SIGNAL_CHANGE_EVENT(_event)   ( ( _event >= ADL_PORT_EVENT_SIGNAL_CHANGE_BASE ) && ( _event < ( ADL_PORT_EVENT_SIGNAL_CHANGE_BASE + ADL_PORT_SIGNAL_LAST ) ) )

/**
  @brief Check condition : is the provided port a physical output based one ?

  @par Description:
  Returns TRUE if the provided "_port" is a physical output based one (E.g. UART1, UART2 or 27.010 logical ports), FALSE otherwise.
 */
#define ADL_PORT_IS_A_PHYSICAL_PORT(_port)  ( ADL_PORT_GET_PHYSICAL_BASE ( _port ) <= ADL_PORT_MAX )

/**
  @brief Check condition : is the provided port an "FCM able" one ?

  @par Description:
  Returns TRUE if the provided "_port" is able to handle the FCM service (i.e. all ports except the Open AT virtual base ones), FALSE otherwise.
 */
#define ADL_PORT_IS_AN_FCM_PORT(_port)  ( ADL_PORT_GET_PHYSICAL_BASE ( _port ) <= ADL_PORT_GET_PHYSICAL_BASE ( ADL_PORT_GPRS_BASE ) )

/**
  @brief Check condition : is the provided port an "AT commands able" one ?

  @par Description:
  Returns TRUE if the provided "_port" is able to handle AT commands services (i.e. all ports except the GSM & GPRS virtual base ones), FALSE otherwise.
 */
 #define ADL_PORT_IS_AN_AT_PORT(_port)   ( ADL_PORT_IS_A_PHYSICAL_PORT( _port ) || ( ADL_PORT_GET_PHYSICAL_BASE ( _port ) == ADL_PORT_GET_PHYSICAL_BASE ( ADL_PORT_OPEN_AT_VIRTUAL_BASE ) ) )


/**
  * @}
  */
#endif // __ADL_PORT_H__
