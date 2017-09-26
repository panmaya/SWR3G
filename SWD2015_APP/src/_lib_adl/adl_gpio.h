/*
 * adl_gpio.h
 *
 * Created: 16/2/2558 16:26:20
 *  Author: pan
 */ 


#ifndef ADL_GPIO_H_
#define ADL_GPIO_H_

/**********************************************************************************************************************/
/*  adl_gpio.h   -  Copyright Sierra Wireless Inc. (c) 2010															  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_gpio.h
  @brief ADL public interface header file
  @ingroup adl_gpio_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_gpio_public_IFS ADL GPIO Interface Specification
  @endif
  
  @section adl_gpio_public_IFS_OVERVIEW Overview
  This document describes the public interface of the ADL software element.
  
  @section adl_gpio_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_gpio_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_gpio_public_IFS_RELATED_DOCUMENTS Related documents references
  
  @anchor ref1 [1] <A HREF="Notes://domino-arc/C1256B52003391E9/C1256F9300391ADBC12570AD0039A948/754B9334007C6523C125721F0055CC86"> WM_DEV_OAT_FCS_034 / IO Management </A> \n
  
  @subsection adl_gpio_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
  - <b>N.A.</b>\n
    Not appliable
  - <b>N.U.</b>\n
    Not used
  - <b>FCS</b>\n
    Functional Specification
  - <b>IFS</b>\n
    Interface Specification
  - <b>SE</b>\n
    Software Element
  - <b>IO</b>\n
    Input Output
  - <b>GPIO</b>\n
    General Purpose Input Output
  */

/**
  @ifnot GLOBAL_IFS
  @defgroup adl_gpio_public_IFS ADL GPIO Service Interface Specification
  @else
  @addtogroup adl_gpio_public_IFS
  @{
  @endif
  
  @section adl_gpio_public_IFS_ftr Features
  
  The ADL supplies interface to handle GPIO operations.
  
  The defined operations are:
  - A function adl_ioGetCapabilitiesList() to retrieve a list of GPIO capablities informations.
  - A function adl_ioSubscribe() to set the reserved GPIO parameters
  - A function adl_ioUnsubscribe() to un-subscribes from a previously allocated GPIO handle
  - A function adl_ioEventSubscribe() to provide ADL with a call-back for GPIO related events
  - A function adl_ioEventUnsubscribe() to unsubscribe from the GPIO events notification
  - A function adl_ioSetDirection() to allow the direction of one or more previously allocated GPIO to be modified
  - A function adl_ioRead() to allow several GPIOs to be read from a previously allocated handle
  - A function adl_ioReadSingle() to allow one GPIO to be read from a previously allocated handle
  - A function adl_ioWrite() to write on several GPIOs from a previously allocated handle
  - A function adl_ioWriteSingle() to allow one GPIO to be written from a previously allocated handle
  
  
  @section adl_gpio_public_IFS_sample Code Sample
  
  This example demonstrates how to use the GPIO service in a nominal case (error cases not handled) with a Wireless CPU.
  
  Complete examples using the GPIO service are also available on the SDK (generic Telemetry sample, generic Drivers library sample).
  
  @code
  
    // Global variables & constants
    
    // Subscription data
    #define GPIO_COUNT1 2
    #define GPIO_COUNT2 1
    
    u32 My_Gpio_Label1 [ GPIO_COUNT1 ] = { 1 , 2 };
    u32 My_Gpio_Label2 [ GPIO_COUNT2 ] = { 3 };
    
    adl_ioDefs_t MyGpioConfig1 [ GPIO_COUNT1 ] =
        { ADL_IO_GPIO | 1 | ADL_IO_DIR_OUT | ADL_IO_LEV_LOW ,
          ADL_IO_GPIO | 2 | ADL_IO_DIR_IN };
    adl_ioDefs_t MyGpioConfig2 [ GPIO_COUNT2 ] =
        { ADL_IO_GPIO | 3 | ADL_IO_DIR_IN };
    
    // Gpio Event Handle
    s32 MyGpioEventHandle;
    
    // Gpio Handles
    s32 MyGpioHandle1, MyGpioHandle2;
    
    // GPIO event handler
    void MyGpioEventHandler ( s32 GpioHandle, adl_ioEvent_e Event, u32 Size, void * Param )
    {
        // Check event
        switch ( Event )
        {
            case ADL_IO_EVENT_INPUT_CHANGED :
            {
                u32 My_Loop;
                // The subscribed input has changed
                for ( My_Loop = 0 ; My_Loop < Size ; My_Loop++)
                {
                    if (( ADL_IO_TYPE_MSK & ((adl_ioDefs_t *)Param)[ My_Loop ] ) && ADL_IO_GPO )
                    {
                        TRACE (( 1, "GPO %d new value: %d", 
                        ( ((adl_ioDefs_t *)Param)[ My_Loop ] ) & ADL_IO_NUM_MSK ,
                        ( ( ((adl_ioDefs_t *)Param)[ My_Loop ] ) & ADL_IO_LEV_MSK ) & ADL_IO_LEV_HIGH ));
                    }
                    else
                    {
                        TRACE (( 1, "GPIO %d new value: %d", 
                        ( ((adl_ioDefs_t *)Param)[ My_Loop ] ) & ADL_IO_NUM_MSK ,
                        ( ( ((adl_ioDefs_t *)Param)[ My_Loop ] ) & ADL_IO_LEV_MSK ) & ADL_IO_LEV_HIGH ));
                    }
                }
            }
            break;
        }
    }
    
    ...
    
    // Somewhere in the application code, used as an event handler
    void MyFunction ( void )
    {
        // Local variables
        s32 ReadValue;
        adl_ioDefs_t Gpio_to_write1 = ADL_IO_GPIO | My_Gpio_Label1 [ 0 ] ;
        adl_ioDefs_t Gpio_to_read1 = ADL_IO_GPIO | My_Gpio_Label1 [ 1 ] ;
        adl_ioDefs_t Gpio_to_read2 = ADL_IO_GPIO | My_Gpio_Label2 [ 0 ] ;
    
        // Subscribe to the GPIO event service
        MyGpioEventHandle = adl_ioEventSubscribe ( MyGpioEventHandler );

        // Subscribe to the GPIO service (One handle without polling,
        // one with a 100ms polling process)
        MyGpioHandle1 = adl_ioSubscribe ( GPIO_COUNT1, MyGpioConfig1, 0, 0, 0 );
        MyGpioHandle2 = adl_ioSubscribe ( GPIO_COUNT2, MyGpioConfig2,
        ADL_TMR_TYPE_100MS, 1, MyGpioEventHandle );

        // Set output
        adl_ioWriteSingle ( MyGpioHandle1, &Gpio_to_write1 , TRUE );

        // Read inputs
        ReadValue = adl_ioReadSingle (MyGpioHandle1, &Gpio_to_read1 );
        ReadValue = adl_ioReadSingle (MyGpioHandle2, &Gpio_to_read2 );
    
        // Unsubscribe from the GPIO services
        adl_ioUnsubscribe ( MyGpioHandle1 );
        adl_ioUnsubscribe ( MyGpioHandle2 );
    
        // Unsubscribe from the GPIO event service
        adl_ioEventUnsubscribe ( MyGpioEventHandle );
    }



  @endcode
  
  @if GLOBAL_IFS
  @}
  @endif
  */



#ifndef __adl_gpio_H__
#define __adl_gpio_H__



/**
  @defgroup adl_gpio_public_IFS_Definitions Types and constants definition
  @ingroup adl_gpio_public_IFS
  
  ADL GPIO types and constants description.
  */

/**
  * @addtogroup adl_gpio_public_IFS_Definitions
  * @{
  */



/** 
 * @brief Definition for the GPIO label.
 
 * @par This is a bits field:
 *  - b0-b15 are use to identify the io
 *      - see #adl_ioLabel_e
 *  - b16-b31 usage depends of the command
 *      - see #adl_ioLevel_e
 *      - see #adl_ioDir_e
 *      - see #adl_ioStatus_e
 *      - see #adl_ioCap_e
 *      - see #adl_ioError_e
 */
typedef s32 adl_ioDefs_t;



/** 
 * @brief Label field definition (b0-b15 of #adl_ioDefs_t). Each IO is identified by a number and a type.
 * @see #adl_ioDefs_t for others fields
 * @note b14-b15 are reserved
 */
#if(0)
typedef enum _adl_ioLabel_e
{
    ADL_IO_NUM_MSK     = 0xFFF,                           ///< <b>Number field (b0-b11; 0->4095)</b>
    ADL_IO_TYPE_POS    = 12,
    ADL_IO_TYPE_MSK    = (3UL<<ADL_IO_TYPE_POS),            ///< <b>Type field (b12-b13):</b>
    ADL_IO_GPI         = (1UL<<ADL_IO_TYPE_POS),            ///< - To identify a GPI
    ADL_IO_GPO         = (2UL<<ADL_IO_TYPE_POS),            ///< - To identify a GPO
    ADL_IO_GPIO        = (3UL<<ADL_IO_TYPE_POS),            ///< - To identify a GPIO (GPO + GPI)
    ADL_IO_LABEL_MSK   = ADL_IO_NUM_MSK | ADL_IO_TYPE_MSK   ///< Mask including #ADL_IO_NUM_MSK and #ADL_IO_TYPE_MSK
} adl_ioLabel_e;
#else
typedef enum _adl_ioLabel_e
{
	ADL_IO_NUM_MSK     = 0x000000FF,            
	ADL_IO_TYPE_POS    = 0x00000100,			
	ADL_IO_TYPE_MSK    = 0x00000800,            
	ADL_IO_GPI         = 0x00000200,						///< - To identify a GPI
	ADL_IO_GPO         = 0x00000400,						///< - To identify a GPO
	ADL_IO_GPIO        = 0x00000800,						///< - To identify a GPIO (GPO + GPI)
	ADL_IO_LABEL_MSK   = ADL_IO_NUM_MSK | ADL_IO_TYPE_MSK   ///< Mask including #ADL_IO_NUM_MSK and #ADL_IO_TYPE_MSK
} adl_ioLabel_e;
#endif


/** 
 * @brief Level field definition (b16 of #adl_ioDefs_t).
 * @see #adl_ioDefs_t for others fields
 */
#if(0)
typedef enum _adl_ioLevel_e
{
    ADL_IO_LEV_POS     = 16,
    ADL_IO_LEV_MSK     = (1UL<<ADL_IO_LEV_POS),    ///< <b>Level field: The level of the GPIO:</b>
    ADL_IO_LEV_HIGH    = (1UL<<ADL_IO_LEV_POS),    ///< - High Level
    ADL_IO_LEV_LOW     = (0UL<<ADL_IO_LEV_POS)     ///< - Low Level
} adl_ioLevel_e;
#else
typedef enum _adl_ioLevel_e
{
	ADL_IO_LEV_POS     = 0x00001000,
	ADL_IO_LEV_MSK     = 0x00002000,    ///< <b>Level field: The level of the GPIO:</b>
	ADL_IO_LEV_HIGH    = 0x00002000,    ///< - High Level
	ADL_IO_LEV_LOW     = 0x00001000     ///< - Low Level
} adl_ioLevel_e;
#endif

/** 
 * @brief Direction field definition (b17-b18 of #adl_ioDefs_t).
 * @see #adl_ioDefs_t for others fields
 */
#if(0)
typedef enum _adl_ioDir_e
{
    ADL_IO_DIR_POS     = 17,
    ADL_IO_DIR_MSK     = (3UL<<ADL_IO_DIR_POS),    ///< <b>Dir field: The direction of the GPIO:</b>
    ADL_IO_DIR_OUT     = (0UL<<ADL_IO_DIR_POS),    ///< - Set as an Output
    ADL_IO_DIR_IN      = (1UL<<ADL_IO_DIR_POS),    ///< - Set as an Input
    ADL_IO_DIR_TRI     = (2UL<<ADL_IO_DIR_POS)     ///< - Set as a Tristate
} adl_ioDir_e;
#else
typedef enum _adl_ioDir_e
{
	ADL_IO_DIR_POS     = 0x00004000,
	ADL_IO_DIR_MSK     = 0x00020000,    ///< <b>Dir field: The direction of the GPIO:</b>
	ADL_IO_DIR_OUT     = 0x00004000,    ///< - Set as an Output
	ADL_IO_DIR_IN      = 0x00008000,    ///< - Set as an Input
	ADL_IO_DIR_TRI     = 0x00010000     ///< - Set as a Tristate
} adl_ioDir_e;
#endif

/**
 * @brief Error field definition (b28-b31 of #adl_ioDefs_t), it is only an output.
 * @see #adl_ioDefs_t for others fields
 */
#if(0)
typedef enum _adl_ioError_e
{
    ADL_IO_ERR_POS     = 28,
    ADL_IO_ERR_MSK     = 0,//(7UL<<ADL_IO_ERR_POS),    ///< <b>Error cause (b28-b30):</b>
    ADL_IO_ERR         = (0UL<<ADL_IO_ERR_POS),    ///< - Unidentified error
    ADL_IO_ERR_UNKWN   = (1UL<<ADL_IO_ERR_POS),    ///< - Unknown GPIO
    ADL_IO_ERR_USED    = (2UL<<ADL_IO_ERR_POS),    ///< - Already used
    ADL_IO_ERR_BADDIR  = (3UL<<ADL_IO_ERR_POS),    ///< - Bad direction
    ADL_IO_ERR_NIH     = (4UL<<ADL_IO_ERR_POS),    ///< - GPIO is not in the handle
    ADL_IO_GERR_POS    = 0,//31,
    ADL_IO_GERR_MSK    = 0,//(s32)(1UL<<ADL_IO_GERR_POS),   ///< <b>General error field (b31):</b>
    ADL_IO_GNOERR      = 0,//(0UL<<ADL_IO_GERR_POS),        ///< - No Error (b28-b30 are unsignificant)
    ADL_IO_GERR        = 0,//(s32)(1UL<<ADL_IO_GERR_POS)    ///< - Error during the treatment (see b28-b30 for the cause)
} adl_ioError_e;
#else
typedef enum _adl_ioError_e
{
	ADL_IO_ERR_POS     = 0x02000000,
	ADL_IO_ERR_MSK     = 0x40000000,
	ADL_IO_ERR         = 0x02000000,    ///< - Unidentified error
	ADL_IO_ERR_UNKWN   = 0x04000000,    ///< - Unknown GPIO
	ADL_IO_ERR_USED    = 0x08000000,    ///< - Already used
	ADL_IO_ERR_BADDIR  = 0x10000000,    ///< - Bad direction
	ADL_IO_ERR_NIH     = 0x20000000,    ///< - GPIO is not in the handle
	ADL_IO_GERR_POS    = 0,//31,
	ADL_IO_GERR_MSK    = 0,//(s32)(1UL<<ADL_IO_GERR_POS),   ///< <b>General error field (b31):</b>
	ADL_IO_GNOERR      = 0,//(0UL<<ADL_IO_GERR_POS),        ///< - No Error (b28-b30 are unsignificant)
	ADL_IO_GERR        = 0,//(s32)(1UL<<ADL_IO_GERR_POS)    ///< - Error during the treatment (see b28-b30 for the cause)
} adl_ioError_e;
#endif


/** 
 * @brief Capabilities field definition (b21-b22 of #adl_ioDefs_t), it is only an output.
 * @see #adl_ioDefs_t for others fields
 */
#if(0)
typedef enum _adl_ioCap_e
{
    ADL_IO_CAP_POS     = 21,
    ADL_IO_CAP_MSK     = (3UL<<ADL_IO_CAP_POS),    ///< <b>Capabilities field: Specials capabilities:</b>
    ADL_IO_CAP_OR      = (1UL<<ADL_IO_CAP_POS),    ///< - Ouput is readable,
    ADL_IO_CAP_IW      = (2UL<<ADL_IO_CAP_POS)     ///< - Input is writable.
} adl_ioCap_e;
#else
typedef enum _adl_ioCap_e
{
	ADL_IO_CAP_POS     = 0x00200000,
	ADL_IO_CAP_MSK     = 0x01000000,    ///< <b>Capabilities field: Specials capabilities:</b>
	ADL_IO_CAP_OR      = 0x00400000,    ///< - Ouput is readable,
	ADL_IO_CAP_IW      = 0x00800000     ///< - Input is writable.
} adl_ioCap_e;
#endif

/** 
 * @brief Status field definition (b19-b20 of #adl_ioDefs_t), it is only an output.
 * @see #adl_ioDefs_t for others fields
 */
#if(0)
typedef enum _adl_ioStatus_e
{
    ADL_IO_STATUS_POS  = 19,
    ADL_IO_STATUS_MSK  = (3UL<<ADL_IO_STATUS_POS), ///< <b>Status field: To get the status of GPIOs</b>
    ADL_IO_STATUS_USED = (1UL<<ADL_IO_STATUS_POS), ///< - The IO is used by a task.
    ADL_IO_STATUS_FREE = (0UL<<ADL_IO_STATUS_POS)  ///< - The IO is available
} adl_ioStatus_e;
#else
typedef enum _adl_ioStatus_e
{
	ADL_IO_STATUS_POS  = 0x00040000,
	ADL_IO_STATUS_MSK  = 0x00100000, ///< <b>Status field: To get the status of GPIOs</b>
	ADL_IO_STATUS_USED = 0x00080000, ///< - The IO is used by a task.
	ADL_IO_STATUS_FREE = 0x00040000  ///< - The IO is available
} adl_ioStatus_e;
#endif


/** 
 * @brief Information about io capabilities.
 */
typedef struct _adl_ioCap_t
{
    u32 NbGpio;   ///< The number of GPIO managed by ADL
    u32 NbGpo;    ///< The number of GPO managed by ADL
    u32 NbGpi;    ///< The number of GPI managed by ADL
    
    u32 Pad;      ///< Internal use only
} adl_ioCap_t;



/**
  @brief Wireless CPU GPIO Event
  
  @par Description:
  This type describe the GPIO events received.
  
**/
typedef enum _adl_ioEvent_e
{
    ADL_IO_EVENT_INPUT_CHANGED = 2  ///<  One or several of the subscribed inputs have changed. This event will be received only if a polling process is required at GPIO subscription time.
} adl_ioEvent_e;



/**
  * @}
  */



/**
  @defgroup adl_gpio_public_IFS_cb1 Handler call-back type
  @ingroup adl_gpio_public_IFS
  
  ADL GPIO handler call-back details.
  */

/**
  * @addtogroup adl_gpio_public_IFS_cb1
  * @{
  */

/**
  @brief GPIO event handler
  
  @par Description:
  Such a call-back function has to be provided to ADL through the adl_ioEventSubscribe() interface, in order to receive GPIO related events.
  
  @param[in]            GpioHandle  Read GPIO handle for the #ADL_IO_EVENT_INPUT_CHANGED event.
  @param[in]            Event       \link adl_ioHdlr_f() Event\endlink is the received event identifier; other parameters use depends on the event type.
  @param[in]            Size        Number of items (read inputs) in the \link adl_ioHdlr_f() Param\endlink table.
  @param[in]            Param       Read values tables (using #adl_ioDefs_t * type) for the #ADL_IO_EVENT_INPUT_CHANGED event.\n
                                    - Outputs available for each array element:
                                        - the GPIO label (see #adl_ioLabel_e).
                                        - the GPIO level (see #adl_ioLevel_e).
                                        - the GPIO error information (see #adl_ioError_e).

 */
//typedef void ( * adl_ioHdlr_f ) ( s32 GpioHandle, adl_ioEvent_e Event, u32 Size, void * Param );



/**
  * @}
  */



/**
  @defgroup adl_gpio_public_IFS_op1 Event controls
  @ingroup adl_gpio_public_IFS
  
  ADL provides GPIO event functions to handle GPIO events.
  */

/**
  * @addtogroup adl_gpio_public_IFS_op1
  * @{
  */



/**
  @brief Subscribe to GPIO events
  
  @par Description:
  This function allows the Open AT<sup>®</sup> application to provide ADL with a call-back for GPIO related events.

  
  @param[in]        GpioEventHandler    GPIO event handler
  
  @return   Returned value description
            - A positive or null value on success : GPIO event handle value to be used on further GPIO API functions calls.
            - A negative error value otherwise
                    - <b>#ADL_RET_ERR_PARAM</b>   if one parameter has an incorrect value
                    - <b>#ADL_RET_ERR_SERVICE_LOCKED</b> if called from a low level interruption handler
  
  @note         In order to set-up an automatic GPIO polling process, the adl_ioEventSubscribe() function has to be called before the adl_ioSubscribe().
 */
//s32 adl_ioEventSubscribe ( adl_ioHdlr_f GpioEventHandler );



/**
  @brief Unsubscribe to GPIO events
  
  @par Description:
  This function allows the Open AT<sup>®</sup> application to unsubscribe from the GPIO events notification.

  
  @param[in]    GpioEventHandle    Handle previously returned by the adl_ioEventSubscribe() function
  
  @return   Returned value description
            - A <b>#OK</b> on success.
            - A negative error value otherwise:
                    - <b>#ADL_RET_ERR_UNKNOWN_HDL</b> if the handle is unknown.
                    - <b>#ADL_RET_ERR_BAD_STATE</b> if a polling process is currently running with this event handle.
                    - <b>#ADL_RET_ERR_SERVICE_LOCKED</b> if the function was called from a low level interruption handler (the function is forbidden in this context).
  
 */
//s32 adl_ioEventUnsubscribe ( s32 GpioEventHandle );



/**
  * @}
  */



/**
  @defgroup adl_gpio_public_IFS_op2 Control operations
  @ingroup adl_gpio_public_IFS
  
  ADL provides GPIO functions to control GPIOs.
  */

/**
  * @addtogroup adl_gpio_public_IFS_op2
  * @{
  */



/**
  @brief Subscribe to GPIOs
  
  @par Description:
  This function subscribes to some GPIOs. For subscribed inputs, a polling system can be configured in order to notify a previously subscribed GPIO event handler with an #ADL_IO_EVENT_INPUT_CHANGED event.

  
  @param[in]        GpioNb              Size of the \link adl_ioSubscribe() GpioConfig\endlink array.
  @param[in,out]    GpioConfig          GPIO subscription configuration array, which contains \link adl_ioSubscribe() GpioNb\endlink elements. For each element, the #adl_ioDefs_t structure members have to be configured.\n
                                        - Inputs to set for each array element:
                                            - the label of the GPIO to subscribe (see #adl_ioLabel_e).
                                            - the GPIO direction ( see #adl_ioDir_e ).
                                            - the GPIO level, only if the GPIO is an output (see #adl_ioLevel_e).
                                        - Outputs available for each array element:
                                            - the GPIO error information (see #adl_ioError_e).
  @param[in]        PollingTimerType    Type of the polling timer (if required). If no polling is requested, this parameter is ignored.defined values are:
                                        - <b>#ADL_TMR_TYPE_100MS</b>    100 ms granularity timer
                                        - <b>#ADL_TMR_TYPE_TICK</b>     18.5 ms tick granularity timer
  @param[in]        PollingTime         If some GPIO are allocated as inputs, this parameter represents the time interval between two GPIO polling operations (unit is dependent on the \link adl_ioSubscribe() PollingTimerType\endlink value). Please note that each required polling process uses one of the available ADL timers (Reminder: up to 32 timers can be simultaneously subscribed). If no polling is requested, this parameter has to be 0.
  @param[in]        GpioEventHandle     GPIO event handle (previously returned by adl_ioEventSubscribe() function). Associated event handler will receive an #ADL_IO_EVENT_INPUT_CHANGED event each time one of the subscribed inputs state has changed. If no polling is requested, this parameter is ignored.
  
  @return   Returned value description
            - A positive or null value on success: GPIO handle to be used on further GPIO API functions calls.
            - A negative error value otherwise (<b>No GPIO is reserved</b>):
                    - <b>#ADL_RET_ERR_PARAM</b>   if one parameter has an incorrect value.
                    - <b>#ADL_RET_ERR_DONE</b> refers to the field #adl_ioError_e for more informations.
                    - <b>#ADL_RET_ERR_NO_MORE_TIMERS</b> if there is no timer available to start the polling process required by application.
                    - <b>#ADL_RET_ERR_NO_MORE_HANDLES</b> if no more GPIO handles are available.
                    - <b>#ADL_RET_ERR_SERVICE_LOCKED</b> if the function was called from a low level interruption handler (the function is forbidden in this context).
  
  @note         In order to set-up an automatic GPIO polling process, the adl_ioEventSubscribe() function has to be called before the adl_ioSubscribe().
 */
s32 adl_ioSubscribe ( u32 GpioNb, adl_ioDefs_t * GpioConfig, u8 PollingTimerType, u32 PollingTime, s32 GpioEventHandle );



/**
  @brief Unsubscribe to GPIOs
  
  @par Description:
  This function un-subscribes from a previously allocated GPIO handle.
  
  @param[in]    GpioHandle      Handle previously returned by adl_ioSubscribe() function.
  
  @return   Returned value description
            - A <b>#OK</b> on success.
            - A negative error value otherwise:
                    - <b>#ADL_RET_ERR_UNKNOWN_HDL</b> if the handle is unknown.
                    - <b>#ADL_RET_ERR_SERVICE_LOCKED</b> if the function was called from a low level interruption handler (the function is forbidden in this context).
 */
//s32 adl_ioUnsubscribe ( s32 GpioHandle );



/**
  @brief Set GPIOs direction
  
  @par Description:
  This function allows the direction of one or more previously allocated GPIO to be modified.
  
  @param[in]        GpioHandle      Handle previously returned by adl_ioSubscribe() function.
  @param[in]        GpioNb          Size of the \link adl_ioSetDirection() GpioDir\endlink array.
  @param[in,out]    GpioDir         GPIO direction configuration structure array (using the #adl_ioDefs_t * type).\n
                                    - Inputs to set for each array element:
                                        - the label of the GPIO to modify (see #adl_ioLabel_e).
                                        - the new GPIO direction ( see #adl_ioDir_e).
                                    - Outputs available for each array element:
                                        - the GPIO error information (see #adl_ioError_e).

  @return   Returned value description
            - A <b>#OK</b> on success.
            - A negative error value otherwise:
                    - <b>#ADL_RET_ERR_PARAM</b> if one parameter has an incorrect value.
                    - <b>#ADL_RET_ERR_DONE</b> refers to the field #adl_ioError_e for more informations for each GPIO. If the error information is #ADL_IO_GNOERR, the process has been completed with success for this GPIO.
                    - <b>#ADL_RET_ERR_UNKNOWN_HDL</b> if the handle is unknown.
                    - <b>#ADL_RET_ERR_SERVICE_LOCKED</b> if the function was called from a low level interruption handler (the function is forbidden in this context).
 */
//s32 adl_ioSetDirection ( s32 GpioHandle, u32 GpioNb, adl_ioDefs_t * GpioDir );



/**
  * @}
  */



/**
  @defgroup adl_gpio_public_IFS_op3 Capabilities operations
  @ingroup adl_gpio_public_IFS
  
  ADL provides GPIO functions to get GPIO capabilities.
  */

/**
  * @addtogroup adl_gpio_public_IFS_op3
  * @{
  */



/**
  @brief Get Wireless CPU GPIO capabilities
  
  @par Description:
  This function returns the WCPU GPIO capabilities list. For each <b>hardware available GPIO</b>, the WCPU shall add an item in the GPIO capabilities list. An GPIO is hardware available when it is not used by any feature.\n
  
  @note         Only the nunmber of hardware available GPIO is returned (see \link adl_ioGetCapabilitiesList() GpioNb\). It is possible to know the number of GPIO not hardware available with \link adl_ioGetCapabilitiesList() GpioTypeNb\ structure.
  
  @warning      The returned \link adl_ioGetCapabilitiesList() GpioTab\endlink array must be released by the customer application when the information is not useful any more.
  
  @param[out]   GpioNb      Number of GPIO treated, it is the size of \link adl_ioGetCapabilitiesList() GpioTab\endlink array.
  @param[out]   GpioTab     Returned a pointer to a list containing GPIO capablities informations (using #adl_ioDefs_t ** type).\n
                            - Outputs available for each array element:
                                - the GPIO label (see #adl_ioLabel_e).
                                - the GPIO tri-state capability (see #adl_ioDir_e). Input and output capabilities are already indicated in the GPIO label part. 
                                - the GPIO capabilities (see #adl_ioCap_e).
                                - the GPIO status (see #adl_ioStatus_e)
  @param[out]   GpioTypeNb  Returned the number of each GPIO, GPO and GPI (hardware available or not). \link adl_ioGetCapabilitiesList() GpioTypeNb\endlink is an optional parameter, not used if set to NULL.
  
  @return   Returned value description
            - A <b>#OK</b> on success.
            - A negative error value otherwise:
                    - <b>#ADL_RET_ERR_PARAM</b> if one parameter has an incorrect value.
                    
                    
  @section adl_gpio_public_IFS_CapabilitiesList_sample Code Sample
  
  This example demonstrates how to use the function adl_ioGetCapabilitiesList() in a nominal case (error cases not handled).
  
  Complete examples using the GPIO service are also available on the SDK (generic Telemetry sample, generic Drivers library sample).
  
  @code
    void my_ioGetCapabilitiesList ()
    {
        u32 My_Loop;
        ascii * My_Message = adl_memGet ( 100 );
        u32 My_GpioNb;
        adl_ioDefs_t * My_GpioTab = NULL;
        adl_ioCap_t GpioTypeNb;
        
        adl_ioGetCapabilitiesList ( &My_GpioNb , &My_GpioTab , &GpioTypeNb );
        
        wm_sprintf ( My_Message , "\r\nRessources : %d GPIO, %d GPI and %d GPO \r\n" , GpioTypeNb.NbGpio , GpioTypeNb.NbGpi , GpioTypeNb.NbGpo );
        adl_atSendResponse ( ADL_AT_UNS, My_Message );
        
        adl_atSendResponse ( ADL_AT_UNS, "\r\nList of GPIO :\r\n" );
        
        for ( My_Loop = 0 ; My_Loop < My_GpioNb ; My_Loop++ )
        {
            switch ( My_GpioTab [ My_Loop ] & ADL_IO_TYPE_MSK )
            {
                case ADL_IO_GPI :
                    wm_sprintf ( My_Message, "GPI %d \r\n", ( My_GpioTab [ My_Loop ] & ADL_IO_NUM_MSK ) );
                    break;
                case ADL_IO_GPIO :
                    wm_sprintf ( My_Message, "GPIO %d \r\n", ( My_GpioTab [ My_Loop ] & ADL_IO_NUM_MSK ) );
                    break;
                case ADL_IO_GPO :
                    wm_sprintf ( My_Message, "GPO %d \r\n", ( My_GpioTab [ My_Loop ] & ADL_IO_NUM_MSK ) );
                    break;
            }
            adl_atSendResponse ( ADL_AT_UNS, My_Message );
            
            ... // customer treatment
            
        }
        
        adl_memRelease ( My_Message );
        
        // My_GpioTab must be released by the customer application
        adl_memRelease ( My_GpioTab );
    }


  @endcode
 */
//s32 adl_ioGetCapabilitiesList ( u32 * GpioNb , adl_ioDefs_t ** GpioTab , adl_ioCap_t * GpioTypeNb );



/**
  * @}
  */



/**
  @defgroup adl_gpio_public_IFS_op4 Read/write operations
  @ingroup adl_gpio_public_IFS
  
  ADL provides GPIO functions to handle GPIO read/write operations.
  */



/**
  * @addtogroup adl_gpio_public_IFS_op4
  * @{
  */



/**
  @brief Read GPIOs
  
  @par Description:
  This function allows several GPIOs to be read from a previously allocated handle.
  
  @param[in]        GpioHandle      Handle previously returned by adl_ioSubscribe() function.
  @param[in]        GpioNb          Size of the \link adl_ioRead() GpioRead\endlink array.
  @param[in,out]    GpioRead        GPIO read structure array (using the #adl_ioDefs_t * type).\n
                                    - Inputs to set for each array element:
                                        - the label of the GPIO to read (see #adl_ioLabel_e).
                                    - Outputs available for each array element:
                                        - the GPIO level value (see #adl_ioLevel_e).
                                        - the GPIO error information (see #adl_ioError_e).
  
  @return   Returned value description
            - A <b>#OK</b> on success (read values are updated in the GpioArray parameter).
            - A negative error value otherwise:
                    - <b>#ADL_RET_ERR_PARAM</b> if one parameter has an incorrect value.
                    - <b>#ADL_RET_ERR_DONE</b> refers to the field #adl_ioError_e for more informations.  If the error information is #ADL_IO_GNOERR, the process has been completed with success for this GPIO.
                    - <b>#ADL_RET_ERR_UNKNOWN_HDL</b> if the handle is unknown.
 */
//s32 adl_ioRead ( s32 GpioHandle, u32 GpioNb, adl_ioDefs_t * GpioRead );



/**
  @brief Read a single GPIO
  
  @deprecated This function is kept in the GPIO service interface for ascendant compatibility only, but should not be used anymore by any application. It shall be removed from future GPIO service interface.
  
  @par Description:
  This function allows one GPIO to be read from a previously allocated handle.
  
  @param[in]        GpioHandle      Handle previously returned by adl_ioSubscribe() function.
  @param[in,out]    Gpio            GPIO read structure, only one element (using the #adl_ioDefs_t * type).\n
                                    - Input value to set:
                                        - the label of the GPIO to read (see #adl_ioLabel_e).
                                    - Outputs available :
                                        - the GPIO level value (see #adl_ioLevel_e).
                                        - the GPIO error information (see #adl_ioError_e).
  
  @return   Returned value description
            - GPIO read value on success (1 for a high level or 0 for a low level),
            - A negative error value otherwise
                    - <b>#ADL_RET_ERR_PARAM</b> if one parameter has an incorrect value.
                    - <b>#ADL_RET_ERR_UNKNOWN_HDL</b> if the handle is unknown
                    - <b>#ADL_RET_ERR_DONE</b> refers to the field #adl_ioError_e for more informations.  If the error information is #ADL_IO_GNOERR, the process has been completed with success for this GPIO.
 */
s32 adl_ioReadSingle ( s32 GpioHandle, adl_ioDefs_t * Gpio );
u32 adl_ioReadFreq ( s32 GpioHandle, adl_ioDefs_t * Gpio );


/**
  @brief Write GPIOs
  
  @par Description:
  This function writes on several GPIOs from a previously allocated handle.
  
  @param[in]        GpioHandle  Handle previously returned by adl_ioSubscribe() function.
  @param[in]        GpioNb      Size of the \link adl_ioWrite() GpioWrite\endlink array.
  
  @param[in,out]    GpioWrite       GPIO write structure array (using the #adl_ioDefs_t * type).\n
                                    - Inputs to set for each array element:
                                        - the label of the GPIO to write (see #adl_ioLabel_e).
                                        - the new GPIO level (see #adl_ioLevel_e).
                                    - Outputs available for each array element:
                                        - the GPIO error information (see #adl_ioError_e).
  
  @return   Returned value description
            - A <b>#OK</b> on success.
            - A negative error value otherwise:
                    - <b>#ADL_RET_ERR_PARAM</b> if one parameter has an incorrect value.
                    - <b>#ADL_RET_ERR_DONE</b> refers to the field #adl_ioError_e for more informations.  If the error information is #ADL_IO_GNOERR, the process has been completed with success for this GPIO.
                    - <b>#ADL_RET_ERR_UNKNOWN_HDL</b> if the handle is unknown.
 */
//s32 adl_ioWrite ( s32 GpioHandle, u32 GpioNb, adl_ioDefs_t * GpioWrite );



/**
  @brief Write a single GPIO
  
  @deprecated This function is kept in the GPIO service interface for ascendant compatibility only, but should not be used anymore by any application. It shall be removed from future GPIO service interface.
  
  @par Description:
  This function allows one GPIO to be written from a previously allocated handle.
  
  @param[in]        GpioHandle  Handle previously returned by adl_ioSubscribe() function.
  @param[in,out]    Gpio        GPIO write structure, only one element(using the #adl_ioDefs_t * type).\n
                                    - Input value to set :
                                        - the label of the GPIO to write (see #adl_ioLabel_e).
                                    - Outputs available :
                                        - the GPIO error information (see #adl_ioError_e).
  @param[in]        State       Value to be set on the output:
                                    - #TRUE for a high level.
                                    - #FALSE for a low level.
  
  @return   Returned value description
            - A <b>#OK</b> on success.
            - A negative error value otherwise:
                    - <b>#ADL_RET_ERR_PARAM</b> if one parameter has an incorrect value.
                    - <b>#ADL_RET_ERR_UNKNOWN_HDL</b> if the handle is unknown.
                    - <b>#ADL_RET_ERR_DONE</b> refers to the field #adl_ioError_e for more informations.  If the error information is #ADL_IO_GNOERR, the process has been completed with success for this GPIO.
 */
s32 adl_ioWriteSingle ( s32 GpioHandle, adl_ioDefs_t * Gpio, bool State );



/**
  * @}
  */

void update_adl_io_1ms(void);


#endif




#endif /* ADL_GPIO_H_ */