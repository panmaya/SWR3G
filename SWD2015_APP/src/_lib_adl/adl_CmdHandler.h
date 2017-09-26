/*
 * adl_CmdHandler.h
 *
 * Created: 23/9/2557 16:09:00
 *  Author: pan
 */ 


#ifndef ADL_CMDHANDLER_H_
#define ADL_CMDHANDLER_H_

/**********************************************************************************************************************/
/*  adl_CmdHandler.h   -  Copyright Sierra Wireless Inc. (c) 2010													  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_CmdHandler.h
  @brief ADL AT Command Subscription/Unsubscription Service public interface header file
  @ingroup adl_CmdHandler_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_CmdHandler_public_IFS ADL AT Command Subscription/Unsubscription Service Interface Specification
  @endif
  
  @section adl_CmdHandler_public_IFS_OVERVIEW Overview
  This document describes the public AT Command Subscription/Unsubscription Service interface of the ADL software element.
  
  @section adl_CmdHandler_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_CmdHandler_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_CmdHandler_public_IFS_RELATED_DOCUMENTS Related documents references
  
  Here are the documents referenced in this interface specification.
  @anchor ref1 [1] <A HREF="http://domino-arc/ged/gedysdoc.nsf/c12570d1004a006dc1256a94004f97ed/1cfb385b24e3235dc1257330004c5548?OpenDocument"> FCS AT Commands Service </A> \n
  
  @subsection adl_CmdHandler_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
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
  @defgroup adl_CmdHandler_public_IFS ADL AT Command Subscription/Unsubscription Service Interface Specification
  @else
  @addtogroup adl_CmdHandler_public_IFS
  @{
  @endif
  
  @section adl_CmdHandler_public_IFS_ftr Features
  
  An ADL application may subscribes to an AT command string, in order to receive events each time either an external application sends this AT command on one of the Wireless CPU®s ports or this AT command is sent with adl_atCmdSendExt API (and an appropriate NI parameter).
  Once the application has subscribed to a command, it will have to unsubscribe to stop the callback function being executed every time this command is sent either by an external application or with adl_atCmdSendExt API.\n
  
  Multiple subscriptions: An application subscribes to a command with a handler (handler1) and subscribes then to the same command with another handler (handler2). Every time this command is sent either by the external application or with adl_atCmdSendExt API the last subscribed handler (handler2) will be called. Handler1 will only be called if handler2 resends the subscribed command with adl_atCmdSendExt API and the provided NI.
  
  <b>Important note about incoming concatenated command:</b> \n
  ADL is able to recognize and process concatenated commands coming from external applications (Please refer to AT Commands Interface Guide (document [2]) for more information on concatenated commands syntax).\n

  In this case, this port enters a specific concatenation processing mode, which will end as soon as the last command replies OK, or if one of the used command replies an ERROR code. During this specific mode, all other external command requests will be refused on this port: any external application connected on this port will receive a "+CME ERROR: 515" code if it tries to send another command. The embedded application can continue using this port for its specific processes, but it has to be careful to send one (at least one, and only one) terminal response for each subscribed command.\n

  If a subscribed command is used in a concatenated command string, the corresponding handler will be notified as if the command was used alone.\n

  In order to handle properly the concatenation mechanism, each subscribed command has to finally answer with a single terminal response (ADL_STR_OK, ADL_STR_ERROR or other ones), otherwise the port will stay in concatenation processing mode, refusing all internal and external commands on this one.\n

  
  The defined operations are:
  - A adl_atCmdSubscribeExt() function to subscribe to a command with providing a Context.
  - A adl_atCmdSubscribe() function to subscribe to a command without providing a Context.
  - A adl_atCmdUnSubscribe() function to unsubscribe to a command.
  
  @section adl_CmdHandler_public_IFS_sample Code Sample
  
  This example demonstrates how to use the AT Command Subscription/Unsubscriptions service in a nominal case (error cases not handled) with a Wireless CPU.
  
  Complete examples using the AT Command service are also available on the SDK.
  
  @code


    // ati callback function 
    void ATI_Handler(adl_atCmdPreParser_t *paras)
    {
        // we send a terminal response
        adl_atSendStdResponsePort(ADL_AT_RSP, paras->Port, ADL_STR_OK);
    }

    // function 2
    void function2(adl_InitType_e adlInitType)
    {
        // We unsubscribe the command ;
        adl_atCmdUnSubscribe("ati", 
                            (adl_atCmdHandler_t)ATI_Handler);
    }

    // function 1
    void function1(adl_InitType_e adlInitType)
    {
        // Subscribe to the 'ati’ command.
        adl_atCmdSubscribe("ati", 
                            (adl_atCmdHandler_t)ATI_Handler, 
                            ADL_CMD_TYPE_ACT);
    }

  @endcode
  
  @if GLOBAL_IFS
  @}
  @endif
  */

//#ifndef __adl_CmdHandler_H__
//#define __adl_CmdHandler_H__


/**
  @defgroup adl_CmdHandler_public_IFS_Definitions Types and constants definition
  @ingroup adl_CmdHandler_public_IFS
  
  */

/**
  * @addtogroup adl_CmdHandler_public_IFS_Definitions
  * @{
  */


/**
  @brief Setting command type
  
  @par Description:
  'AT+cmd=x, y'is allowed.
  The execution of the callback function also depends on whether the number of argument is valid or not.
  Information about number of arguments is combined with a bitwise 'OR' : ADL_CMD_TYPE_PARA | 0xXY , where X which defines maximum argument number for incoming command and Y which defines minimum argument number for incoming command.

 */
#define ADL_CMD_TYPE_PARA   0x0100

/**
  @brief Test command type
  
  @par Description:
  'AT+cmd=?' is allowed.
 */
#define ADL_CMD_TYPE_TEST   0x0200

/**
  @brief reading command type
  
  @par Description:
  'AT+cmd?' is allowed.
 */
#define ADL_CMD_TYPE_READ   0x0400

/**
  @brief Action command type
  
  @par Description:
  'AT+cmd' is allowed.
 */
#define ADL_CMD_TYPE_ACT   0x0800

/**
  @brief Root command type
  
  @par Description:
  All commands starting with the subscribed string are allowed but without the ending character ";" which is parsed for concatenated commands mode. The handler will only receive the whole AT string (no parameters detection). 
  For example, if the "at-" string is subscribed, all "at-cmd1", "at-cmd2", etc. strings will be received by the handler,however the only string "at-" is not received.
  
 */
#define ADL_CMD_TYPE_ROOT   0x1000

/**
  @brief Root Extended command type
  
  @par Description:
  All commands starting with the subscribed string are allowed even with the ending character ";" this means that such a command will not be usable in a concatenated AT commands string. The handler will only receive the whole AT string (no parameters detection). 
  For example, if the "at-" string is subscribed, all "at-cmd1", "at-cmd2", etc. strings will be received by the handler,however the only string "at-" is not received.

  @note  IN THIS CURRENT RELEASE #ADL_CMD_TYPE_ROOT_EXT IS BEHAVING LIKE #ADL_CMD_TYPE_ROOT
  
 */
#define ADL_CMD_TYPE_ROOT_EXT   0x2000


/** 
 * @brief Basic required subscription port affected
 * @note  IN THIS CURRENT RELEASE #ADL_CMD_SUBSCRIPTION_ONLY_EXTERNAL_PORT IS THE ONLY VALID CHOICE
 * 
 */
typedef enum
{
    ADL_CMD_SUBSCRIPTION_ONLY_EXTERNAL_PORT,    ///< The subscription is only concerning command received on the external port
    ADL_CMD_SUBSCRIPTION_ALL_PORTS              ///< The subscription is concerning command received on all ports
} adl_atCmdSubscriptionPort_e;

/**
  @brief  Body for command message type
  
  @par Description:
  This structure contains information about AT command
 */
typedef struct _adl_atCmdPreParser_t
{
    /**
      @brief Type
      
      @par Description:
      Incoming command type (will be one of the required ones at subscription time), detected by the ADL pre-processing..
      */
    u16 Type; 
    
    
    /**
      @brief Number of parameters
      
      @par Description:
      Non NULL parameters number (if Type is ADL_CMD_TYPE_PARA), or 0 (with other type values).    
      */        
    u8 NbPara;  
    
    
    /**
      @brief Port
      
      @par Description:
      Port on which the command was sent by the external application.
      */   
//#ifndef __REMOTETASKS__     
//    adl_atPort_e    Port;       
//#else
    u8              Port;
//#endif

   
    /**
      @brief List of parameters
      
      @par Description:
      Only if Type is ADL_CMD_TYPE_PARA. Each parameter may be accessed by the #ADL_GET_PARAM(_p,_i) macro
      If a string parameter is provided (eg. AT+MYCMD="string"), the quotes will be removed from the returned string (eg. ADL_GET_PARAM(para,0) will return "string" (without quotes) in this case). 
      If a parameter is not provided (eg. AT+MYCMD), the matching list element will be set to NULL (eg. ADL_GET_PARAM(para,0) will return NULL in this case).

      */        
    wm_lst_t ParaList; 
    
    
    /**
      @brief Incoming command length
      
      @par Description:
      Incoming command string buffer length.
      */        
    u16 StrLength; 
    
    
    /**
      @brief Notification Identifier
      
      @par Description:
      This parameter is to hold the Notification Identifier provided by the command handler.

      */        
    u16 NI;      
    
    
    /**
      @brief Context
      
      @par Description:
      A context holding information gathered at the time the command is subscribed (if provided)
      */        
    void *Contxt;      
    
    
    /**
      @brief Incoming command adress
      
      @par Description:
      Incoming command string buffer address. 
      If the incoming command from the external application is containing useless spaces (" ") or semi-colon (";") characters, those will automatically be removed from the command string (e.g. if an external application sends "AT+MY CMD;" string, the command handler will receive "AT+MYCMD").
      */        
    ascii StrData[1];      

} adl_atCmdPreParser_t;

/**
  @brief Get parameter macro
  
  @par Description:
  Macro to get the requested parameter.
  
  @param[in]        _P_  command handler parameter ( refer to #adl_atCmdPreParser_t structure about pointer to use )
  @param[in]        _i_  parameter index from 0 to NbPara ( refer to #adl_atCmdPreParser_t structure for more information about NbPara )
  
 */
#define ADL_GET_PARAM(_P_,_i_)  ( ( ascii * ) wm_lstGetItem ( _P_->ParaList, _i_ ) )

/**
  * @}
  */

/**
  @defgroup adl_CmdHandler_public_IFS_cb1 AT command handler
  @ingroup adl_CmdHandler_public_IFS
  
  */

/**
  * @addtogroup adl_CmdHandler_public_IFS_cb1
  * @{
  */

/**
  @brief Command handler callback defintion
  
  @par Description:
  such a call-back function has to be supplied to ADL through the adl_atCmdSubscribe() interface in order to process AT command subscribed.
  
  @param[in]        Params    Contains information about AT response (refer to #adl_atCmdPreParser_t for more information)
  
  @note         The command handler has the responsability to send unsollicited/intermediate reponses and at least one terminal response.
  
 */
typedef void (*adl_atCmdHandler_t)(adl_atCmdPreParser_t * Params );

/**
  * @}
  */

/**
  @defgroup adl_CmdHandler_public_IFS_op1 AT command Subscription/Unsubscription
  @ingroup adl_CmdHandler_public_IFS
  
  These function allows to subscribe or unsubscribe to the AT commands.
  */

/**
  * @addtogroup adl_CmdHandler_public_IFS_op1
  * @{
  */

/**
  @brief AT command subscribtion without Context

  
  @par Description:
  This function subscribes to a specific command with an associated callback function, so that next time the required command is sent exclusively by an external application, the callback function will be executed.
  
  @param[in]        Cmdstr     The string (name) of the command we want to subscribe to. Since this service only handles AT commands, this string has to begin by the "AT" characters.
  @param[in]        Cmdhdl     The handler of the callback function associated to the command. (refer to #adl_atCmdHandler_t for more information about callback function)
  @param[in]        Cmdopt     This flag combines with a bitwise 'OR' ('|' in C language) the following information :
                               - #ADL_CMD_TYPE_PARA
                               - #ADL_CMD_TYPE_TEST
                               - #ADL_CMD_TYPE_READ
                               - #ADL_CMD_TYPE_ACT
                               - #ADL_CMD_TYPE_ROOT
                               - #ADL_CMD_TYPE_ROOT_EXT
  <br><br><b>Note</b>:
  If #ADL_CMD_TYPE_ROOT_EXT is associated with others it has priority and therefore the command can not be recognized as a concatenated one.
  @note  IN THIS CURRENT RELEASE #ADL_CMD_TYPE_ROOT_EXT IS BEHAVING LIKE #ADL_CMD_TYPE_ROOT
  
  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler
  - <b>ERROR</b>                          if an error occurred.  

 */
s16 adl_atCmdSubscribe ( const ascii *Cmdstr, adl_atCmdHandler_t Cmdhdl, u16 Cmdopt );
bool adl_atCmdSubscribeExecute( const ascii *str, adl_atPort_e port);

/**
  @brief AT command subscribtion with Context
  
  @par Description:
  This function subscribes to a specific command with an associated callback function, so that next time the required command is sent by an external application or on all ports (depending on the Cmdport parameter), the callback function will be executed.
  
  @param[in]        Cmdstr     The string (name) of the command we want to subscribe to. Since this service only handles AT commands, this string has to begin by the "AT" characters.
  @param[in]        Cmdhdl     The handler of the callback function associated to the command. (refer to #adl_atCmdHandler_t for more information about callback function)
  @param[in]        Cmdopt     This flag combines with a bitwise 'OR' ('|' in C language) the following information :
                               - #ADL_CMD_TYPE_PARA
                               - #ADL_CMD_TYPE_TEST
                               - #ADL_CMD_TYPE_READ
                               - #ADL_CMD_TYPE_ACT
                               - #ADL_CMD_TYPE_ROOT
                               - #ADL_CMD_TYPE_ROOT_EXT
  
  <br><br><b>Note</b>:
  If #ADL_CMD_TYPE_ROOT_EXT is associated with others it has priority and therefore the command can not be recognized as a concatenated one.
  @note  IN THIS CURRENT RELEASE #ADL_CMD_TYPE_ROOT_EXT IS BEHAVING LIKE #ADL_CMD_TYPE_ROOT
  
  @param[in]        Contxt     Context made to hold information gathered at the time the command is subscribed
  @param[in]        Cmdport    Port on which the command is subscribed (type of to #adl_atCmdSubscriptionPort_e)
  								- #ADL_CMD_SUBSCRIPTION_ONLY_EXTERNAL_PORT
                                - #ADL_CMD_SUBSCRIPTION_ALL_PORTS
  @note  IN THIS CURRENT RELEASE #ADL_CMD_SUBSCRIPTION_ONLY_EXTERNAL_PORT IS THE ONLY VALID CHOICE

  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler
  - <b>ERROR</b>                          if an error occurred.  

 */
//s16 adl_atCmdSubscribeExt ( ascii *Cmdstr, adl_atCmdHandler_t Cmdhdl, u16 Cmdopt, void * Contxt, adl_atCmdSubscriptionPort_e Cmdport);

/**
  @brief AT command unsubscription
  
  @par Description:
  This function unsubscribes from a command and its handler.
  
  @param[in]        Cmdstr     The string (name) of the command we want to unsubscribe from.
  @param[in]        Cmdhdl     The handler of the callback function associated to the command.
  
  @return       Returned value description
  - <b>OK</b>                             on success
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler
  - <b>ERROR</b>                          otherwise 

 */
//s16 adl_atCmdUnSubscribe ( ascii *Cmdstr, adl_atCmdHandler_t Cmdhdl );

/**
  @brief Set Quiet mode.
  
  @par Description:
  This function allows to set Quiet mode. In this mode, terminal response are not send.
  This function have the same behaviour than ATQ command behaviour.
  
  @param[in]        IsQuiet     Quiet mode setting : 
                                - <b>TRUE :</b> Quiet mode is activated
                                - <b>FALSE :</b> Quiet mode is deactivated. Default value.

 */
//void adl_atCmdSetQuietMode ( bool IsQuiet );

/**
  * @}
  */

//#endif




#endif /* ADL_CMDHANDLER_H_ */