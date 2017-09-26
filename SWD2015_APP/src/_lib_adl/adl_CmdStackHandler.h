/*
 * adl_CmdStackHandler.h
 *
 * Created: 4/10/2557 10:38:55
 *  Author: pan
 */ 


/**********************************************************************************************************************/
/*  adl_CmdStackHandler.h   -  Copyright Sierra Wireless Inc. (c) 2010                                                */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_CmdStackHandler.h
  @brief ADL AT Command Sending Service public interface header file
  @ingroup adl_CmdStackHandler_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_CmdStackHandler_public_IFS ADL AT Command Sending Service Interface Specification
  @endif

  @section adl_CmdStackHandler_public_IFS_OVERVIEW Overview
  This document describes the public AT Command Sending Service interface of the ADL software element.

  @version 02.00

  @author SUB Infosys AT Maintenance

  @section adl_CmdStackHandler_public_IFS_HISTORY History
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
      <td>2007/11/15</td>	 
      <td>Creation - OS V6 update</td>
      <td>FDN</td>
  </tr>
  <tr>
      <td>01.01</td>
      <td>N/A</td>	 
      <td>2007/12/06</td>	  
      <td>Minor Update:<br>
          History added.<br>
          New adl_atResponse_t::IsTerminal input field.
      </td>
      <td>DPO</td>
  </tr>
  <tr>
      <td>01.02</td>
      <td>N/A</td>	  
      <td>2011/04/27</td>	 
      <td>Minor Update:<br>
          A note added for adl_atCmdCreate().
      </td>
      <td>SUB Infosys AT Maintenance</td>
  </tr>
  <tr>
      <td>01.03</td>
      <td>N/A</td>	  
      <td>2011/06/16</td>	  
      <td>Minor Update:<br>
          A note added for adl_atCmdCreate().
      </td>
      <td>SUB Infosys AT Maintenance</td>
  </tr>
  <tr>
      <td>01.04</td>
      <td>N/A</td>	 
      <td>2011/02/23</td>	  
      <td>Minor Update:<br>
          A note added for adl_atCmdCreate().
      </td>
      <td>SUB Infosys AT Maintenance</td>
  </tr>
  <tr>
      <td>01.05</td>
      <td>N/A</td>		  
      <td>2011/02/23</td>	  
      <td>Minor Update:<br>
          A note added for adl_atCmdSend() and adl_atCmdSendExt().
      </td>
      <td>SUB Infosys AT Maintenance</td>
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

  @section adl_CmdStackHandler_public_IFS_INTRODUCTION Introduction

  @subsection adl_CmdStackHandler_public_IFS_REMINDER Reminder

  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.

  @subsection adl_CmdStackHandler_public_IFS_RELATED_DOCUMENTS Related documents references

  Here are the documents referenced in this interface specification.
  @anchor ref1 [1] <A HREF="http://domino-arc/ged/gedysdoc.nsf/c12570d1004a006dc1256a94004f97ed/1cfb385b24e3235dc1257330004c5548?OpenDocument"> FCS AT Commands Service </A> \n

  @subsection adl_CmdStackHandler_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary

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
  @defgroup adl_CmdStackHandler_public_IFS ADL AT Command Sending Service Interface Specification
  @else
  @addtogroup adl_CmdStackHandler_public_IFS
  @{
  @endif

  @section adl_CmdStackHandler_public_IFS_ftr Features

  The following functions allow to send a command on the required port and allows the subscription to several responses and intermediates responses with one associated callback function, so that when any of the responses or intermediates responses we subscribe to will be received by the ADL parser, the callback function will be executed.

  The defined operations are:
  - adl_atCmdCreate() function to send a command on the required port and allow the subscription to several responses and intermediates responses with one associated callback function, so that when any of the responses or intermediates responses we subscribe to will be received by the ADL parser, the callback function will be executed.
  - adl_atCmdSend() same function as adl_atCmdCreate() without the rspflag argument and instead sending the command to the Open AT internal port.
  - adl_atCmdSendExt() same function as adl_atCmdCreate() without the rspflag argument and instead the port argument plus a Notification Identifier and a Context.
  - adl_atCmdSendText() function to allow to provide a running "Text Mode" command on a specific port (e.g. "AT+CMGW") with the required text. This function has to be used as soon as the prompt response ("> ") comes in the response handler provided on adl_atCmdCreate() / adl_atCmdSend() / adl_atCmdSendExt() function call.

  - <b>Note1</b>: Now adl_atCmdSendExt() (with a NI parameter different from 0) finds out if the command has been subscribed. If the command has been subscribed the handler is called otherwise the command is executed (as it is when called with adl_atCmdSend() or adl_atCmdCreate()). If the command has multiple subscription the last handler subscribed is called. In order for any other handler to be called the last handler has to resend the command with adl_atCmdSendExt() API and the NI parameter provided so that the penultimate handler will be called and so on ...
  - <b>Note2</b>: For any multiply subscribed command sent by an external application on one of the Wireless CPU®s ports all handlers were called at the same time. Now there is a change of behaviour where only the last subscribed handler is called (by resending the command using adl_atCmdSendExt() API and the provided NI the penultimate handler is called and so on ...).
  - <b>Note3</b>: If any Inner AT Command (as decribed in section 2.5 Inner AT Commands Configuration of ADL UGD) is subscribed its handler has to resend the command with adl_atCmdSendExt() API and the NI parameter provided so that ADL internal handler is called. Otherwise as explained in section 2.5 of ADL UGD it may affect ADL correct behaviour.
  - <b>Note4</b>: If a command is only once subscribed. Sending this command will call the handler. If the handler resends the command with adl_atCmdSendExt() API and the NI parameter provided the command will be sent for execution. Likewise if a command is multiply subscribed. Sending this command with adl_atCmdSendExt() API and the NI parameter provided will call the last handler if at some point (after re sending the command with adl_atCmdSendExt() API and the NI parameter provided) the first handler is called re sending the command with adl_atCmdSendExt() API and the NI parameter provided will sends the command for execution.
  - <b>Note5</b>: If the required port is not opened, the functions return an error(ADL_RET_ERR_PARAM). In the USB case, the cable must be plugged and the enumeration with the host has to succeed before proceed to one of these operations.
  
@section adl_CmdStackHandler_public_IFS_sample Code Sample

  This example demonstrates how to use the AT Command Sending service in a nominal case (error cases not handled) with a Wireless CPU.

  Complete examples using the AT Command service are also available on the SDK.

  - Example 1:

  @code
    // ati responses callback function
    s16 ATI_Response_Handler(adl_atResponse_t *paras)
    {
       TRACE((1, "Reponse handled"));
       TRACE((1, paras->StrData));
       return FALSE;
    }

    // function 1
    void function1(adl_InitType_e adlInitType)
    {
        // We send ati and subscribe to its responses
        adl_atCmdSend("ati",
                      (adl_atRspHandler_t)ATI_Response_Handler,
                      "*",
                      NULL);
    }
  @endcode

  - Example 2:

  @code
    // at+bbb responses handler function
    bool B_RspHandler ( adl_atResponse_t * paras )
    {
        TRACE (( 1, "In B_RspHandler - printing out response" ));
        // the return value is TRUE to print out responses
        return TRUE;
    }
    // at+aaa command handler function
    void A_CmdHandler(adl_atCmdPreParser_t * paras )
    {
        TRACE (( 1, "In A_CmdHandler - sending AT+BBB cmd" ));
        // sending at+bbb command with adl_atCmdSendExt() and provided NI
        // at+bbb is subscribed so command handler B_CmdHandler() is to be called
        adl_atCmdSendExt( "at+bbb", paras->Port, paras->NI, NULL, B_RspHandler, "*", NULL );
    }

    // ati responses handler function
    bool C_RspHandler ( adl_atResponse_t * paras )
    {
        TRACE (( 1, "In C_RspHandler - transferring response" ));
        // ati responses are handled and transfered to the previous responses handler subscribes with the same NI
        adl_atSendResponseSpe ( ADL_AT_PORT_TYPE ( paras->Dest, paras->Type ), paras->StrData, paras->NI );
        return FALSE;
    }
    // at+bbb command handler function
    void B_CmdHandler(adl_atCmdPreParser_t * paras )
    {
        TRACE (( 1, "In B_CmdHandler - sending ATI cmd" ));
        // sending ati command with adl_atCmdSendExt() and provided NI
        // ati is not subscribed hence the AT command is sent for execution
        adl_atCmdSendExt( "ati", TRUE, paras->NI, NULL, C_RspHandler, "*", NULL );
    }

    void adl_main ( adl_InitType_e InitType )
    {
        TRACE (( 1, "Embedded Application : Main" ));

        // at+aaa is subscribed with A_CmdHandler command handler
        adl_atCmdSubscribe("AT+AAA",A_CmdHandler,ADL_CMD_TYPE_ACT);
        // at+bbb is subscribed with B_CmdHandler command handler
        adl_atCmdSubscribe("AT+BBB",B_CmdHandler,ADL_CMD_TYPE_ACT);
    }
  @endcode

  - Example 3:

  @code
    // ati responses handler function
    bool ATI_RspHandler2 ( adl_atResponse_t * paras )
    {
        TRACE (( 1, "In ATI_RspHandler2 - printing out response" ));
        // ati responses are handled
        // the return value is TRUE to print out responses
        return TRUE;
    }
    // ati command handler function
    void ATI_CmdHandler1(adl_atCmdPreParser_t * paras )
    {
        TRACE (( 1, "In ATI_CmdHandler1 - re-sending AT cmd" ));
        // This handler is the last subscribed so the first called
        // sending ati command with adl_atCmdSendExt() and provided NI
        // ati is again subscribed so next command handler ATI_CmdHandler2() is to be called
        adl_atCmdSendExt( paras->StrData, paras->Port, paras->NI, NULL, ATI_RspHandler2, "*", NULL );
    }

    // ati responses handler function
    bool ATI_RspHandler3 ( adl_atResponse_t * paras )
    {
        TRACE (( 1, "In ATI_RspHandler3 - transferring response" ));
        // ati responses are handled and transfered to the previous responses handler subscribes with the same NI
        adl_atSendResponseSpe ( ADL_AT_PORT_TYPE ( paras->Dest, paras->Type ), paras->StrData, paras->NI );
        return FALSE;
    }
    // ati command handler function
    void ATI_CmdHandler2(adl_atCmdPreParser_t * paras )
    {
        TRACE (( 1, "In ATI_CmdHandler2 - sending AT cmd for execution (no more handlers)" ));
        // sending ati command with adl_atCmdSendExt() and provided NI
        // ati is not subscribed anymore (both subscribed handler have been called) hence the AT command is sent for execution
        adl_atCmdSendExt( paras->StrData, paras->Port, paras->NI, NULL, ATI_RspHandler3, "*", NULL );
    }

    void adl_main ( adl_InitType_e InitType )
    {
        TRACE (( 1, "Embedded Application : Main" ));

        // ati is subscribed twice
        // - first with ATI_CmdHandler2 command handler
        // - then  with ATI_CmdHandler1 command handler
        adl_atCmdSubscribe("ati",ATI_CmdHandler2,ADL_CMD_TYPE_ACT);
        adl_atCmdSubscribe("ati",ATI_CmdHandler1,ADL_CMD_TYPE_ACT);
    }
  @endcode

  @if GLOBAL_IFS
  @}
  @endif
  */

#ifndef __adl_CmdStackHandler_H__
#define __adl_CmdStackHandler_H__

/**
  @defgroup adl_CmdStackHandler_public_IFS_Definitions Types and constants definition
  @ingroup adl_CmdStackHandler_public_IFS

  */

/**
  * @addtogroup adl_CmdStackHandler_public_IFS_Definitions
  * @{
  */

/**
  @brief  Body for response message type

  @par Description:
  This structure contains information about AT command response
 */
typedef struct _adl_atResponse_t
{
    /**
      @brief RspID

      @par Description:
      Detected standard response ID if the received response is a standard one.
      */
#ifndef __REMOTETASKS__
  adl_strID_e           RspID;
#else
  u8                    RspID;
#endif
  /**
    @brief Dest

    @par Description:
    Port on which the command has been executed ; it is also the destination port where the response will be forwarded if the handler returns TRUE.
    */
#ifndef __REMOTETASKS__
  adl_atPort_e          Dest;
#else
  u8                    Dest;
#endif


    /**
      @brief Response length

      @par Description:
      Response string buffer length.
      */
    u16 StrLength;


    /**
      @brief Context

      @par Description:
      A context holding information gathered at the time the command is sent (if provided)
      */
    void *Contxt;


    /**
      @brief Terminal response flag

      @par Description:
      A boolean flag indicating if the received response is the terminal one (TRUE)
      or an intermediate one (FALSE)
      */
    bool IsTerminal;


    /**
      @brief Notification Identifier

      @par Description:
      This parameter is to hold the Notification Identifier provided by the command initiating the response
      */
    u8 NI;


    /**
      @brief Type of the response

      @par Description:
      Type of the response
      */
    u8 Type;


    u8 Pad[1];  ///< Reserved for future use


    /**
      @brief Response adress

      @par Description:
      Response string buffer address.
      */
    ascii StrData[1];
} adl_atResponse_t;


s8 adl_CmdStackHandler_init(void);


/**
  * @}
  */

/**
  @defgroup adl_CmdStackHandler_public_IFS_cb1 AT response handler
  @ingroup adl_CmdStackHandler_public_IFS

  */

/**
  * @addtogroup adl_CmdStackHandler_public_IFS_cb1
  * @{
  */

/**
  @brief Callback function definition

  @par Description:
  such a call-back function has to be supplied to ADL through the adl_atCmdCreate() / adl_atCmdSend() / adl_atCmdSendExt() interface in order to process AT response subscribed.

  @param[in]        Params    Contains information about AT response (refer to #adl_atResponse_t for more information)

  @return           The return value of the callback function has to be TRUE if the response string has to be sent to the provided port, FALSE otherwise

 */
typedef bool (*adl_atRspHandler_t)(adl_atResponse_t * Params);

/**
  * @}
  */

/**
  @defgroup adl_CmdStackHandler_public_IFS_op1 AT command Sending
  @ingroup adl_CmdStackHandler_public_IFS

  These function allows to subscribe or unsubscribe to the AT commands.
  */

/**
  * @addtogroup adl_CmdStackHandler_public_IFS_op1
  * @{
  */

/**
  @brief ADL_NI_LAUNCH to enable searching handler process

  @par Description:
  If ADL_NI_LAUNCH is provided in API adl_atCmdSendExt() searching handler process will be launched :
  If the command is subscribed the handler will be called
  Else the command will be executed.
 */
#define ADL_NI_LAUNCH   0xFE

/**
  @brief Send AT command


  @par Description:
  Add command to the required port command stack, in order to be executed as soon as this port is ready

  @param[in]        atstr     The string (name) of the command we want to send. Since this service only handles AT commands, this string has to begin by the "AT" characters.
  @param[in]        rspflag   This parameter is composed of the unsubscribed responses destination flag, and the port where to send the command.
                              The flag & destination combination has to be done with the following macro :
                              ADL_AT_PORT_TYPE ( _port, _flag )
                              - The _port argument has to be a defined value of the adl_atPort_e type, and this required port has to be available (cf. the AT/FCM port Service). If this port is not available, or if it is a GSM or GPRS based one, the command will not be executed.
                              - The _flag argument has to be one of the values defined below:
                                  - If set to TRUE: the responses and intermediate responses of the sent command that are not subscribed (ie. not listed in the adl_atCmdCreate() function arguments) will be sent on the required port.
                                  - If set to FALSE they will not be sent to the external application.
                              - If the ADL_AT_PORT_TYPE macro is not used, by default the command will be sent to the Open AT® virtual port.
  @param[in]        rsphdl    The response handler of the callback function associated to the command.
  @param[in]        ...       A list of strings of the response to subscribed to. This list has to be terminated by NULL.

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler
  - <b>ADL_RET_ERR_PARAM</b>              if an error occurred
  - <b>ADL_RET_ERR_UNKNOWN_HDL</b>        if the _port argument corresponds to a port which is closed

  @note         Arguments rsphdl can be set to NULL to only send the command. The list of subscribed responses is ignored when rsphdl is NULL.
  @note         If the _port parameter is set to ADL_PORT_NONE the command will be sent on ADL_PORT_OPEN_AT_VIRTUAL_BASE port.
  @note         ATQ commands should not be used with adl_atCmdCreate() / adl_atCmdSend() / adl_atCmdSendExt() API but instead adl_atCmdSetQuietMode() API is to be used.
  @note         The response will not be received in case the UART settings are changed by giving AT+IPR or AT+ICF command through adl_atCmdCreate, even with rspflag set as TRUE.
  @note         ADL_PORT_NONE and ADL_PORT_OPEN_AT_VIRTUAL_BASE should not be used with the AT commands related to Serial Port (like ATE0, AT&C1, +IPR). Instead the actual physical port needs to be specified.

 */
s8 adl_atCmdCreate ( const ascii *atstr, u16 rspflag, adl_atRspHandler_t rsphdl, ... );

/**
  @brief Send AT command


  @par Description:
  Add command to internal defaut port command stack, in order to be executed as soon as this port is ready

  @param[in]        atstr     The string (name) of the command we want to send. Since this service only handles AT commands, this string has to begin by the "AT" characters.
  @param[in]        rsphdl    The response handler of the callback function associated to the command.
  @param[in]        ...       A list of strings of the response to subscribed to. This list has to be terminated by NULL.

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler
  - <b>ADL_RET_ERR_PARAM</b>              if an error occurred.

  @note         Argument rsphdl can be set to NULL to only send the command. The list of subscribed responses is ignored when rsphdl is NULL.

 */
//s8 adl_atCmdSend ( ascii *atstr, adl_atRspHandler_t rsphdl, ... );

/**
  @brief Send AT command with 2 added arguments compare to adl_atCmdCreate() / adl_atCmdSend() : a NI (Notification Identifier) and a Context


  @par Description:
  Add command to the required port command stack, in order to be executed as soon as this port is ready

  @param[in]        atstr     The string (name) of the command we want to send. Since this service only handles AT commands, this string has to begin by the "AT" characters.
  @param[in]        port      The required port on which the command will be executed.
  @param[in]        NI        This parameter is to hold the Notification Identifier
                              The NI parameter can have the following values :
                              - 0 (default value) : the command is directly send for execution (as when using #adl_atCmdCreate or #adl_atCmdSend)
                              - #ADL_NI_LAUNCH : the searching handler process is launched :
                                  - If the command is subscribed the handler will be called
                                  - Else the command will be executed.
                              - any para->NI provided by the handler (if called inside a handler)
  @param[in]        Contxt    Context made to hold information gathered at the time the command was sent
  @param[in]        rsphdl    The response handler of the callback function associated to the command. (see Note below)
  @param[in]        ...       A list of strings of the response to subscribed to. This list has to be terminated by NULL. (see Note below)

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler
  - <b>ADL_RET_ERR_PARAM</b>              if an error occurred.

  @note         Argument rsphdl can be set to NULL to only send the command. The list of subscribed responses is ignored when rsphdl is NULL.
  @note         Unsolicited service has to be subscribed beforehand in order to get unsolicited responses forwarded in AT command export mechanism.

 */
//s8 adl_atCmdSendExt ( ascii *atstr, adl_atPort_e port, u16 NI, void* Contxt, adl_atRspHandler_t rsphdl, ... );

/**
  @brief Send Text AT command


  @par Description:
  Sends text for a running text command

  @param[in]        Port      Port on which is currently running the "Text Mode" command, waiting for some text input.
  @param[in]        Text      Text to be provided to the running "Text Mode" command on the required port. If the text does not end with a ‘Ctrl-Z’ character (0x1A code), the function will add it automatically.

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler
  - <b>ADL_RET_ERR_PARAM</b>              if an error occurred.

 */
//s8 adl_atCmdSendText ( adl_port_e Port, ascii * Text );


/**
  * @}
  */

#endif
