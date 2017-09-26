/*
 * adl_sim.h
 *
 * Created: 4/10/2557 10:43:15
 *  Author: pan
 */ 

/**********************************************************************************************************************/
/*  adl_sim.h   -  Copyright Sierra Wireless Inc. (c) 2010															  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_sim.h
  @brief ADL SIM Service public interface header file
  @ingroup adl_sim_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_sim_public_IFS ADL SIM Service Interface Specification
  @endif
  
  @section adl_sim_public_IFS_OVERVIEW Overview
  This document describes the public SIM interface of the ADL software element.

  @version 02.00

  @author SUB Infosys AT Maintenance

  @section adl_sim_public_IFS_HISTORY History
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
      <td>2008/05/13</td>	  
      <td>Creation</td>
      <td>FDN</td>
  </tr>
  <tr>
      <td>01.01</td>
      <td>N/A</td>
      <td>2008/08/24</td>	  
      <td>Minor Update:<br>
	      News APIs adl_simEnterPUK and adl_simRemAttempt<br>
	      SIM net lock state detection.
      </td>
      <td>JRA</td>
  </tr>
  <tr>
      <td>01.02</td>
      <td>N/A</td>
      <td>2012/02/27</td>	  
      <td>Minor Update:<br>
	      Description updated for ADL_SIM_EVENT_PIN_WAIT event.
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
  
  @section adl_sim_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_sim_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_sim_public_IFS_RELATED_DOCUMENTS Related documents references
  
  Here are the documents referenced in this interface specification.
  @anchor ref1 [1] <> X </A> \n
  
  @subsection adl_sim_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
  - <b>N.A.</b>\n
    Not appliable
  - <b>N.U.</b>\n
    Not used
  - <b>IFS</b>\n
    Interface Specification
  - <b>SE</b>\n
    Software Element
  - <b>PIN</b>\n
    Personnal Identification Number
  - <b>PUK</b>\n
    Personnal Unblocking Code
  */

/**
  @ifnot GLOBAL_IFS
  @defgroup adl_sim_public_IFS ADL SIM Service Interface Specification
  @else
  @addtogroup adl_sim_public_IFS
  @{
  @endif
  
  @section adl_sim_public_IFS_ftr Features
  
  ADL provides this service to handle SIM and PIN code related events.
  
  <br>The defined operations are:
  - #adl_simSubscribe
  - #adl_simUnsubscribe
  - #adl_simGetState
  - #adl_simEnterPIN
  - #adl_simEnterPUK
  - #adl_simRemAttempt
  
  @if GLOBAL_IFS
  @}
  @endif
  */

#ifndef __ADL_SIM_H__
#define __ADL_SIM_H__

/**
  @defgroup adl_sim_public_IFS_Definitions Types and constants definition
  @ingroup adl_sim_public_IFS
  
  */

/**
  * @addtogroup adl_sim_public_IFS_Definitions
  * @{
  */

/**
  @brief SIM event handler callback defintion
  @par Description:
  such a call-back function has to be supplied to ADL through the #adl_simSubscribe API in order to process SIM event reception.
  @param[in]   Event	SIM event
 */
typedef void (* adl_simHdlr_f)(u8 Event);

/**
  @brief enum for SIM states
  @par Description:
  enum for #adl_simGetState API return code
 */
typedef enum
{
    ADL_SIM_STATE_INIT,         ///< Service init state (PIN state not known yet)
    ADL_SIM_STATE_REMOVED,      ///< SIM removed
    ADL_SIM_STATE_INSERTED,     ///< SIM inserted (PIN state not known yet)
    ADL_SIM_STATE_FULL_INIT,    ///< SIM Full Init done
    ADL_SIM_STATE_PIN_ERROR,    ///< SIM error state
    ADL_SIM_STATE_PIN_OK,       ///< PIN code OK, waiting for full init
    ADL_SIM_STATE_PIN_WAIT,     ///< SIM inserted, PIN code not entered yet
    ADL_SIM_STATE_PUK_WAIT,     ///< PIN locked, PUK is required
    ADL_SIM_STATE_PUK_ERROR,    ///< Wrong PUK input
    ADL_SIM_STATE_FAILURE,      ///< PUK locked, SIM unavailable
    ADL_SIM_STATE_NET_LOCK,     ///< Network locked, phone to SIM


    ADL_SIM_STATE_LAST		///< Always last state
} adl_simState_e;

/**
  @brief enum for SIM events
  @par Description:
  enum for #adl_simHdlr_f handler
 */
typedef enum
{
    ADL_SIM_EVENT_REMOVED = 1,  ///< SIM removed event
    ADL_SIM_EVENT_INSERTED,     ///< SIM inserted event
    ADL_SIM_EVENT_FULL_INIT,    ///< SIM Full Init done event 
    ADL_SIM_EVENT_PIN_ERROR,    ///< SIM error event
    ADL_SIM_EVENT_PIN_OK,       ///< PIN code OK event
    ADL_SIM_EVENT_PIN_WAIT,     ///< If the argument PinCode is set to NULL ( In case adl_simSubscribe is not called directly from adl_main (or main_task), this event will not be received.)
 								///< On the last three events, the service is waiting for either the external application or the adl_simEnterPIN API to enter the PIN code.
    ADL_SIM_EVENT_PUK_WAIT,     ///< PUK wait event
    ADL_SIM_EVENT_PUK_ERROR,    ///< Wrong PUK input event
    ADL_SIM_EVENT_FAILURE,      ///< PUK locked event
    ADL_SIM_EVENT_NET_LOCK,     ///< Network locked event, phone to SIM

    ADL_SIM_EVENT_LAST			///< Always last Event
} adl_simEvent_e;



/**
  @brief SIM remaining attempts structure
  
  @par Description:
  This structure allows the application to handle SIM PIN and PUK remaining attempts.
 */

typedef struct
{
    /**
      @brief PIN remaining attempts
      
      @par Description:
      Contains PIN remaining attempts before lock PIN.
      */
    s8         PinRemaining;
    
    /**
      @brief PUK remaining attempts
      
      @par Description:
      Contains PUK remaining attempts before lock PUK.
      */ 
    s8         PukRemaining;
} adl_simRem_e;

/**
  * @}
  */

/**
  @defgroup adl_sim_public_IFS_op1 SIM API access functions
  @ingroup adl_sim_public_IFS
  */

/**
  * @addtogroup adl_sim_public_IFS_op1
  * @{
  */

/** 
 * @brief SIM service subscription function
 * @par Description: This function subscribes to the SIM service, in order to receive SIM and PIN code related events. This will allow to enter PIN code (if provided) if necessary.
 * @param   SimHandler 		SIM handler defined using the following type: (see #adl_simHdlr_f).For events received by this handler: (see #adl_simEvent_e).
 * @param   PinCode 		PinCode
 * @retval 	  	ADL_RET_ERR_SERVICE_LOCKED 				when called from a low level interruption handler(the function is forbidden in this context).
 * @retval 		ADL_RET_ERR_ALREADY_SUBSCRIBED 				 if the service was already subscribed with the same handler.
 * @retval 		ADL_RET_ERR_PARAM 				 if the function was called with a null handler.
 * @retval 		OK 				Operation successful.
 */
s32 adl_simSubscribe ( adl_simHdlr_f SimHandler, ascii * PinCode );

/** 
 * @brief SIM service unsubscription function
 * @par Description: This function unsubscribes from SIM service. The provided handler will not receive SIM events any more.
 * @param [in]  Handler 		Call back Handler (see #adl_simHdlr_f)
 * @retval 		OK 				Operation successful
 * @retval 	  	ADL_RET_ERR_SERVICE_LOCKED when called from a low level interruption handler
 */
//s32 adl_simUnsubscribe ( adl_simHdlr_f Handler );

/** 
 * @brief Get SIM state function
 * @par Description: This function gets the current SIM service state.
 * @retval 		state 			depending on the #adl_simState_e enum
 */
adl_simState_e adl_simGetState ( void );

/** 
 * @brief SIM service enter PIN code function
 * @par Description: The adl_simEnterPIN interface enables the user to enter the Pin Code of the inserted SIM.
 * @param [in]  PinCode 		the PIN code to be entered
 * @retval 		OK 				the PIN code has been correctly processed
 * @retval 	  	ADL_RET_ERR_PARAM 	the Pin Code is not informed
 * @retval 	  	ADL_RET_ERR_BAD_STATE 	the SIM is not waiting for any Pin Code to be entered
 * @retval 	  	ERROR  			otherwise
 */
//s32 adl_simEnterPIN ( ascii * PinCode );


/**
 * @brief SIM service enter PUK code function
  @par Description: This function allows to enter PUK code and a new PIN code.
  
  @param[in]        PukCode                SIM PUK code.
  @param[in]        NewPinCode             New SIM PIN code.
 * @retval          OK                     ADL try the given PUK code
 * @retval          ADL_RET_ERR_PARAM      if PukCode or NewPinCode is not informed
 * @retval          ADL_RET_ERR_BAD_STATE  if the SIM is not waiting for PIN or PUK, and nothing entered yet from ext          
  @note     ERROR values are defined in adl_error.h.
 */
//s32 adl_simEnterPUK ( ascii * PukCode , ascii * NewPinCode );


/**
 * @brief Get remaining attempts on PIN and PUK codes
  @par Description: 
  This function allows to get the number of remaining attempts on PIN and PUK codes.
 * @retval          adl_simRem_e              PIN and PUK remaining attempts  
 */
//adl_simRem_e adl_simRemAttempt ( void );

/**
  * @}
  */
 
#endif
