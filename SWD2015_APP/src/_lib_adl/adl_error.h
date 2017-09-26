/*
 * adl_error.h
 *
 * Created: 24/9/2557 17:46:42
 *  Author: pan
 */ 

/**********************************************************************************************************************/
/*  adl_error.h   -  Copyright Sierra Wireless Inc. (c) 2010														  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_error.h
  @brief ADL Error service public interface header file
  @ingroup adl_error_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_error_public_IFS ADL Error service Interface Specification
  @endif
  
  @section adl_error_public_IFS_OVERVIEW Overview
  This document describes the public Error service interface of the ADL software element.
  
  @section adl_error_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_error_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_error_public_IFS_RELATED_DOCUMENTS Related documents references
  
  Here are the documents referenced in this interface specification.
  -# @anchor DocRef1 <a href="Notes://domino-arc/C1256B52003391E9/C12570D1004A006DC1256A94004F97ED/152992295B9717ABC1257321003FEE8A">[WM_DEV_OAT_FCS_059] Debug Facilities services</a>
  
  @subsection adl_error_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
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
  @defgroup adl_error_public_IFS ADL Error service Interface Specification
  @else
  @addtogroup adl_error_public_IFS
  @{
  @endif
  
  @section adl_error_public_IFS_ftr Features
  
  ADL supplies Error service interface to allow the application to cause & intercept fatal errors, and also to retrieve stored back-trace logs.
  
  The defined operations are:
  - A <b>subscription</b> function (adl_errSubscribe()) to register an error event handler
  - An <b>unsubscription</b> function (adl_errUnsubscribe()) to cancel this event handler registration
  - An <b>error handler</b> callback (#adl_errHdlr_f) to be notified each time a fatal error occurs
  - An <b>error request</b> function (adl_errHalt()) to cause a fatal error
  - A <b>cleaning</b> function (adl_errEraseAllBacktraces()) to clean the back-traces storage area
  - An <b>analysis status</b> function (adl_errGetAnalysisState()) to retrieve the current back-trace analysis status
  - An <b>analysis start</b> function (adl_errStartBacktraceAnalysis()) to start the back-trace analysis
  - A <b>retrieve</b> function (adl_errRetrieveNextBacktrace()) to retrieve the next back-trace buffer for the current analysis
  
  @section adl_error_public_IFS_sample Code Sample
  
  The code sample below illustrates a nominal use case of the ADL Error service public interface (error cases are not handled)
  
  @code
// Error Event handler
bool MyErrorHandler ( u16 ErrorID, ascii * ErrorStr )
{
    // Nothing to do but accept the reset
    return TRUE;
}

// Error string
const ascii * MyErrorString = "Application Generated Error";

// Error launch function
void MyFunction1 ( void )
{
    // Subscribe to error service
    adl_errSubscribe ( MyErrorHandler );
    
    // Cause an error
    adl_errHalt ( ADL_ERR_LEVEL_APP + 1, MyErrorString );
}

// Error service unsubscription function
void MyFunction2 ( void )
{
    // Unsubscribe from error service
    adl_errUnsubscribe ( MyErrorHandler );
}

// Backtraces analysis event handler
u8 * MyAnalysisFunction ( void )
{
    // Start analysis
    s8 AnalysisHandle = adl_errStartBacktraceAnalysis();
    
    // Get state
    adl_errAnalysisState_e State = adl_errGetAnalysisState();
    
    // Retrieve next backtrace size
    u8 * Buffer = NULL;
    u32 Size = adl_errRetrieveNextBacktrace ( AnalysisHandle, Buffer, 0 );
    
    // Retrieve next backtrace buffer
    Buffer = adl_memGet ( Size );
    adl_errRetrieveNextBacktrace ( AnalysisHandle, Buffer, Size );
    
    // Erase all backtraces
    adl_errEraseAllBacktraces();
    
    // Return backtrace buffer
    return Buffer;
}
  @endcode
  
  @if GLOBAL_IFS
  @}
  @endif
  */
#ifndef ADL_ERROR_H_
#define ADL_ERROR_H_

/**
  @defgroup adl_error_public_IFS_Definitions ADL standard error codes
  @ingroup adl_error_public_IFS
  
  Generic error codes used in all the ADL services APIs
  */

/**
  * @addtogroup adl_error_public_IFS_Definitions
  * @{
  */


#define ADL_RET_ERR_PARAM                   -2	///< Parameter error
#define ADL_RET_ERR_UNKNOWN_HDL             -3	///< Unknown handler, handle or resource error
#define ADL_RET_ERR_ALREADY_SUBSCRIBED      -4	///< Already subscribed service or resource error
#define ADL_RET_ERR_NOT_SUBSCRIBED          -5	///< Not subscribed service error
#define ADL_RET_ERR_FATAL                   -6	///< Fatal error
#define ADL_RET_ERR_BAD_HDL                 -7	///< Bad handle or resource error
#define ADL_RET_ERR_BAD_STATE               -8	///< Bad state error
#define ADL_RET_ERR_PIN_KO                  -9	///< Bad PIN state error
#define ADL_RET_ERR_NO_MORE_HANDLES         -10	///< The service subscription maximum capacity is reached
#define ADL_RET_ERR_DONE                    -11	///< The required iterative process is now terminated
#define ADL_RET_ERR_OVERFLOW                -12	///< The required operation has exceeded the function capabilities
#define ADL_RET_ERR_NOT_SUPPORTED           -13	///< An option, required by the function, is not enabled on the Wireless CPU®, the function is not supported in this configuration
#define ADL_RET_ERR_NO_MORE_TIMERS          -14	///< The function requires a timer subscription, but no more timer resources are available
#define ADL_RET_ERR_NO_MORE_SEMAPHORES      -15	///< The function requires a semaphore allocation, but there are no more semaphores resources
#define ADL_RET_ERR_SERVICE_LOCKED          -16	///< The function was called from a low or high lewel interrupt handler (the function is forbidden in this case)

#define ADL_RET_ERR_SPECIFIC_BASE           -20	///< Beginning of specific errors ranges

/**
  * @}
  */



/**
  @defgroup adl_error_public_IFS_ADL_Errors ADL generated error events
  @ingroup adl_error_public_IFS
  */

/**
  * @addtogroup adl_error_public_IFS_ADL_Errors
  * @{
  */



/**
  @brief Standard ADL errors definition
  
  @par Description:
  This type lists the error identifiers which should be generated by ADL.
 */
typedef enum _adl_errInternalID_e
{
	ADL_ERR_LEVEL_MEM = 0x0010,		///< Base level for generated ADL memory errors
	
	ADL_ERR_MEM_GET = ADL_ERR_LEVEL_MEM,	///< The platform runs out of dynamic memory.
	ADL_ERR_MEM_RELEASE,					///< Internal error on dynamic memory release operation.
											///< @note Internal usage only. An application has no way to produce such an error.
	ADL_ERR_MEM_INVALID_CALL,				///< Invalid memory API call.
											///< @note Internal usage only. An application has no way to produce such an error.
	
	ADL_ERR_LEVEL_FLH = 0x0020,		///< Base level for generated ADL flash errors
	
	ADL_ERR_FLH_READ = ADL_ERR_LEVEL_FLH,	///< Internal error on flash object read operation.
											///< @note Internal usage only. An application has no way to produce such an error.
	ADL_ERR_FLH_DELETE,						///< Internal error on flash object delete operation.
											///< @note Internal usage only. An application has no way to produce such an error.
	
	ADL_ERR_LEVEL_APP = 0x0100		///< Base level for application generated errors
} adl_errInternalID_e;



/**
  * @}
  */



/**
  @defgroup adl_error_public_IFS_cb1 Error event handler
  @ingroup adl_error_public_IFS
  */

/**
  * @addtogroup adl_error_public_IFS_cb1
  * @{
  */

/**
  @brief Error event handler callback type
  
  @par Description:
  Such a call-back is called each time a fatal error is caused by the application or by ADL.\n
  Errors which should be generated by ADL are described in the #adl_errInternalID_e type.\n
  An error is described by an identifier and a string (associated text),
  that are sent as parameters to the adl_errHalt() function.\n
  If the error is processed and filtered the handler should return FALSE.
  The return value TRUE will cause the Wireless CPU to execute a fatal error reset
  with a backtrace.
  A backtrace is composed of the provided message, and a call stack dump taken at
  the function call time. It is readable by the Target Monitoring Tool
  (Please refer to the Tools Manual for more information).

  @param[in]        ErrorID		Error identifier, defined by the application or by ADL
  @param[in]        ErrorString	Error string, defined by the application or by ADL
  
  @retval       TRUE   If the handler decides to let the Wireless CPU reset
  @retval       FALSE  If the handler refuses to let the Wireless CPU reset
  
  @note  An error event handler is called in the same execution context than the code which has caused the error.
  @note  If the error handler returns FALSE, the back-trace log is not registered in the Wireless CPU non-volatile memory.
 */
typedef bool ( * adl_errHdlr_f ) ( u16 ErrorID, ascii * ErrorString );

/**
  * @}
  */



/**
  @defgroup adl_error_public_IFS_op1 Error events control
  @ingroup adl_error_public_IFS
  */

/**
  * @addtogroup adl_error_public_IFS_op1
  * @{
  */

/**
  @brief Error service subscription function
  
  @par Description:
  This function subscribes to error service and gives an error handler: 
  this allows the application to handle errors generated by ADL or by the adl_errHalt() function.
  Errors generated by the Open AT Firmware can not be handled by such an error handler.
  
  @param[in]        ErrorHandler	Error event handler, defined using the #adl_errHdlr_f type.
  
  @retval       OK								On success
  @retval       ADL_RET_ERR_PARAM				On parameter error
  @retval		ADL_RET_ERR_ALREADY_SUBSCRIBED	If the service is already subscribed
  @retval		ADL_RET_ERR_SERVICE_LOCKED		If the function was called from a low level interrupt handler (the function is forbidden in this context)
 */
//s8 adl_errSubscribe ( adl_errHdlr_f ErrorHandler );

/**
  @brief Error service unsubscription function
  
  @par Description:
  This function unsubscribes from error service. 
  Errors generated by ADL or by the adl_errHalt() function will no more be handled by the error handler.
  
  @param[in]        ErrorHandler	Error event handler, defined using the #adl_errHdlr_f type, and previously provided to adl_errSubscribe() function.
  
  @retval       OK								On success
  @retval       ADL_RET_ERR_PARAM				On parameter error
  @retval		ADL_RET_ERR_UNKNOWN_HDL			If the provided handler is unknown
  @retval		ADL_RET_ERR_NOT_SUBSCRIBED		If the service is not subscribed
  @retval		ADL_RET_ERR_SERVICE_LOCKED		If the function was called from a low level interrupt handler (the function is forbidden in this context)
 */
//s8 adl_errUnsubscribe ( adl_errHdlr_f ErrorHandler );

/**
  @brief Error launch function
  
  @par Description:
  This function causes an error, defined by its ID and string.
  If an error handler is defined (using #adl_errHdlr_f type), it will be called, otherwise
   a Wireless CPU reset will occur.\n
  When the Wireless CPU resets (if there is no handler, or if this one returns TRUE), a back-trace log
  is registered in a non-volatile memory area, and also sent to the Target Monitoring Tool (if this one is running).\n
  Such a back-trace log contains:
  - the call stack dump when the error occurs
  - the provided error identifier & string
  - the context name which has caused the error, following the same behaviour than a trace display operation (please refer to the Debug Traces service for more information)
  
  @param[in]	ErrorID		Error identifier. Shall be at least equal to #ADL_ERR_LEVEL_APP (lower values are reserved for ADL internal error events)
  @param[in]	ErrorStr	Error string to be provided to the error handler, and to be stored in the resulting backtrace if a fatal error is required.
  
  @note @{ Please note that only the string address is stored in the backtrace,
           so this parameter has not to be a pointer on a RAM buffer, but a constant string pointer.
           Moreover, the string will only be correctly displayed if the current application is still
           present in the Wireless CPU flash memory.
           If the application is erased or modified, the string will not be 
           correctly displayed when retrieving the backtraces. @}
  
  @note Error identifiers below #ADL_ERR_LEVEL_APP are for internal purpose so the application should only use an identifier above #ADL_ERR_LEVEL_APP
  
  @note When the Wireless CPU reset is due to a fatal error, the init type parameter will be set to the ADL_INIT_REBOOT_FROM_EXCEPTION value (Please refer to the tasks initialization service for more information)
 */
//void adl_errHalt ( u16 ErrorID, const ascii * ErrorStr );

/**
  * @}
  */

/**
  @defgroup adl_error_public_IFS_op2 Stored errors analysis functions
  @ingroup adl_error_public_IFS
  */

/**
  * @addtogroup adl_error_public_IFS_op2
  * @{
  */

/**
  @brief Backtrace analysis process states
  
  @par Description:
  This type is used to enumerate the possible states of the backtraces analysis.
 */
typedef enum _adl_errAnalysisState_e
{
    ADL_ERR_ANALYSIS_STATE_IDLE,    ///< No running analysis
    ADL_ERR_ANALYSIS_STATE_RUNNING  ///< A backtrace analysis is running
} adl_errAnalysisState_e;

/**
  @brief Backtraces storage clean function
  
  @par Description:
  Backtraces (caused by the adl_errHalt() function, ADL or the Firmware) are stored 
  in the Wireless CPU non-volatile memory. A limited number of backtraces may be stored
  in memory (depending on each backtrace size, and other internal parameters 
  stored in the same storage place). The adl_errEraseAllBacktraces() function allows
  to free and re-initialize this storage place.
  
  @retval       OK							On success
  @retval		ADL_RET_ERR_SERVICE_LOCKED	If the function was called from a low level interrupt handler (the function is forbidden in this context)
 */
//s32 adl_errEraseAllBacktraces ( void );

/**
  @brief Backtraces analysis start function
  
  @par Description:
  In order to retrieve backtraces from the product memory, a backtrace analysis process 
  has to be started with the adl_errStartBacktraceAnalysis() function.
  
  @retval       Handle							A positive or null handle on success. This handle have to be used in the next adl_errRetrieveNextBacktrace() function call. It will be valid until this function returns a ADL_RET_ERR_DONE code.
  @retval		ADL_RET_ERR_ALREADY_SUBSCRIBED	If a backtrace analysis is already running.
  @retval		ERROR							If an unexpected error occurs.
  @retval		ADL_RET_ERR_SERVICE_LOCKED		If the function was called from a low level interrupt handler (the function is forbidden in this context)
  
  @note		Only one analysis may be running at a time. The adl_errStartBacktraceAnalysis() function will return the ADL_RET_ERR_ALREADY_SUBSCRIBED error code if it is called while an analysis is currently running.
 */
//s8 adl_errStartBacktraceAnalysis ( void );

/**
  @brief Backtraces analysis status function
  
  @par Description:
  This function may be used in order to know the current backtrace analysis process state.
  
  @return	The current analysis state, using the #adl_errAnalysisState_e type.
 */
//adl_errAnalysisState_e adl_errGetAnalysisState ( void );

/**
  @brief Backtraces analysis retrieve function
  
  @par Description:
  This function allows the application to retrieve the next backtrace buffer stored in the Wireless CPU memory.
  The backtrace analysis has to be started first with the adl_errStartBacktraceAnalysis() function.
  
  @param[in]		Handle			Backtrace analysis handle, returned by the adl_errStartBacktraceAnalysis() function.
  @param[in,out]	BacktraceBuffer	Buffer in which the next retrieved backtrace will be copied. This parameter can be set to NULL in order to know the next backtrace buffer required size.
  @param[in]		Size			Backtrace buffer size. If this size is not large enough, the ADL_RET_ERR_PARAM error code will be returned.
  
  @retval	OK							If the next stored backtrace was successfully copied in the buffer parameter.
  @retval	Size						The required size for next backtrace buffer if the buffer parameter is set to NULL.
  @retval	ADL_RET_ERR_PARAM			If the provided size parameter is not large enough.
  @retval	ADL_RET_ERR_NOT_SUBSCRIBED	If the adl_errStartBacktraceAnalysis() function was not called before.
  @retval	ADL_RET_ERR_UNKNOWN_HDL		If the provided handle parameter is invalid.
  @retval	ADL_RET_ERR_DONE			If the last backtrace buffer has already been retrieved. The Handle parameter will now be unsubscribed and not usable any more with the adl_errRetrieveNextBacktrace() function. A new analysis has to be started with the adl_errStartBacktraceAnalysis() function.
  @retval	ADL_RET_ERR_SERVICE_LOCKED	If the function was called from a low level interrupt handler (the function is forbidden in this context)
  
  @note	@{ Once retrieved, the backtrace buffers may be stored,
           in order to be sent (using the application’s protocol/bearer choice) to a remote server or PC.
           Once retrieved as a concatenated file on a PC, this one can be read using the 
           Target Monitoring Tool and the Serial Link Manager in order to decode the backtrace buffers.
           Please refer to the Tools Manual in order to know how to process these files. @}
  
  @note @{ If adl_errRetrieveNextBacktrace() is used, the application has to retrieve all next backtraces.
           Otherwise it is not possible to start an analysis again. There is no way to cancel a backtrace analysis;
           an analysis has always to be completed until all the backtraces are retrieved. @}
 */
//s32 adl_errRetrieveNextBacktrace ( u8 Handle, u8 * BacktraceBuffer, u16 Size );

/**
  * @}
  */






#endif /* ADL_ERROR_H_ */