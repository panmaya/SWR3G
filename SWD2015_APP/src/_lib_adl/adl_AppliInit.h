/*
 * adl_AppliInit.h
 *
 * Created: 3/10/2557 17:29:55
 *  Author: pan
 */ 


/**********************************************************************************************************************/
/*  adl_AppliInit.h   -  Copyright Sierra Wireless Inc. (c) 2010													  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_AppliInit.h
  @brief ADL Application Entry Points public interface header file
  @ingroup adl_AppliInit_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_AppliInit_public_IFS ADL Application Entry Points Interface Specification
  @endif

  @section adl_AppliInit_public_IFS_OVERVIEW Overview
  This document describes the public Application Entry Points interface of the ADL software element.

  @version 02.00

  @author SUB Infosys AT Maintenance

  @section adl_AppliInit_public_IFS_HISTORY History
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
      <td>2007/11/05</td>	  
      <td>Creation</td>
      <td>DPO</td>
  </tr>
  <tr>
      <td>01.01</td>
      <td>N/A</td>
      <td>2008/01/15</td>	  
      <td>Minor Update:<br>
          History added.<br>
          Multitasking lock removed.<br>
          New @link adl_AppliInit_public_IFS_Naming Name & Version @endlink interfaces definition.
      </td>
      <td>DPO</td>
  </tr>
  <tr>
      <td>01.02</td>
      <td>N/A</td>	 
      <td>2009/07/03</td> 	  
      <td>Minor Update:<br>
          Add @link adl_AppliInit_public_IFS_IrqPrio FW interrupt priorities @endlink change.
      </td>
      <td>FDU</td>
  </tr>
  <tr>
      <td>01.03</td>
      <td>N/A</td>	 
      <td>2010/12/06</td> 	  
      <td>Minor Update:<br>
          Remove all details on Real Time Enhancement feature (commercial feature removal).<br>
          Removal in #adl_InitIRQLowLevelStackSize, #adl_InitIRQHighLevelStackSize description.
      </td>
      <td>FDU</td>
  </tr>
  <tr>
      <td>01.04</td>
      <td>N/A</td>	  
      <td>2010/12/23</td>	  
      <td>Minor Update:<br>
          Add macro for stack size declaration: #ADL_DECLARE_CALL_STACK, #ADL_DECLARE_LOWIRQ_STACK, #ADL_DECLARE_HIGHIRQ_STACK.
      </td>
      <td>FDU</td>
  </tr>
  <tr>
      <td>01.05</td>
      <td>N/A</td>	  
      <td>2010/12/23</td>	  
      <td>Minor Update:<br>
          Well format the compilation flag paragraph.<br>
          Set to deprecated the Interrupt priority change feature.
      </td>
      <td>FDU</td>
  </tr>
  <tr>
      <td>01.06</td>
      <td>N/A</td>	
      <td>2012/04/20</td>  	  
      <td>Minor Update:<br>
          Removed reference to RTE.
      </td>
      <td>SUB Infosys AT Maintenance</td>
  </tr>
  <tr>
      <td>01.07</td>
      <td>N/A</td>	 
      <td>2012/06/11</td> 	  
      <td>Minor Update:<br>
          Remove the Interrupt priority change feature.
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

  @section adl_AppliInit_public_IFS_INTRODUCTION Introduction

  @subsection adl_AppliInit_public_IFS_REMINDER Reminder

  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.

  @subsection adl_AppliInit_public_IFS_RELATED_DOCUMENTS Related documents references

  Here are the documents referenced in this interface specification.
  -# @anchor DocRef1 <a href="Notes://domino-arc/C1256B52003391E9/C12570D1004A006DC1256A94004F97ED/40423885831E1EDCC1257321003F9043">[WM_DEV_OAT_FCS_055] Applicative tasks services</a>

  @subsection adl_AppliInit_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary

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
  @defgroup adl_AppliInit_public_IFS ADL Application Entry Points Interface Specification
  @else
  @addtogroup adl_AppliInit_public_IFS
  @{
  @endif

  @section adl_AppliInit_public_IFS_ftr Features

  ADL supplies Application Entry Points interface to allow applications to define the generic settings of the application tasks and contexts.

  The application will have to define its entry points settings using the #adl_InitTasks table. Each line of this table represents a task,
  which is characterized by the folloing parameters:
  - the task entry point, called at the Wireless CPU boot time, in the priority order
  - the task call stack size
  - the task priority level
  - the task name

  If the application wishes to use the IRQ service, it will also have to define the call stack sizes for it
  low level (#adl_InitIRQLowLevelStackSize) and high level (#adl_InitIRQHighLevelStackSize) interrupt handlers.

  Moreover, some operation related to the initialization are available:
  - A <b>Init type check</b> function (adl_InitGetType()) to retrieve at any time the Wireless CPU initialization type.

  @section adl_AppliInit_public_IFS_sample Code Sample

  The code sample below illustrates a nominal use case of the ADL Application Entry Points public interface

  @code
// Application tasks declaration table
const adl_InitTasks_t adl_InitTasks [] =
{
    { MyFirstEntryPoint,  1024, "MYTASK1", 3 },
    { MySecondEntryPoint, 1024, "MYTASK2", 2 },
    { MyThirdEntryPoint,  1024, "MYTASK3", 1 },
    { 0, 0, 0, 0 }
};

// Low level handlers execution context call stack size
const u32 adl_InitIRQLowLevelStackSize = 1024;

// High level handlers execution context call stack size
const u32 adl_InitIRQHighLevelStackSize = 1024;
  @endcode

  @if GLOBAL_IFS
  @}
  @endif
  */



#ifndef __adl_AppliInit_H__
#define __adl_AppliInit_H__

#ifdef __CC_ARM
#pragma import(setlocale)
#endif


/**
  @defgroup adl_AppliInit_public_IFS_TaskTable Tasks declaration
  @ingroup adl_AppliInit_public_IFS

  Interfaces dedicated to the application tasks declaration
  */

/**
  * @addtogroup adl_AppliInit_public_IFS_TaskTable
  * @{
  */

/**
  @brief Tasks declaration structure

  @par Description:
  Open AT application's tasks declaration structure, used to format the #adl_InitTasks table.
 */
typedef struct _adl_InitTasks_t
{
    /**
      @brief Task entry point

      @par Description:
      Task initialization handler, which aims to be called each time the Wireless CPU boots, as soon as the application
      is started with the <b>AT+WOPEN=1</b> command.

      @note A task entry point function is NOT like a standard "C" main function. The task does not end when returns. An Open AT application is stopped only if the <b>AT+WOPEN=0</b> command is used. Such a call-back function is only the application entry point, and has to subscribe to some services and events to go further. In addition the whole software is protected by a watchdog mechanism, the application shall not use infinite loops and loops having a too long duration, the Wireless CPU will reset due to the watchdog hardware security (please refer to Hardware Security: Watchdog Protection for more information).
      */
    void ( * EntryPoint ) ( void * );

    /**
      @brief Task call stack size

      @par Description:
      Used to provide to the system the required call stack size (in bytes) for the current task. A call stack is the Open AT RAM area which contains the local variables and return addresses for function calls. Call stack sizes are deduced from the total available RAM size for the Open AT application.

      @note The GCC compiler and GNU Newlib (standard C library) implementation require more stack size than ARM compilers. If the GCC compiler is used, the Open AT application has to be declared with greater stack sizes.

      @note Call stack sizes shall be declared with some extra bytes margin. It is not recommended to try to reckon exactly the required call stack size of each task.

      @note @{ If the total call stack sizes (including the tasks ones & the interrupt contexts ones) is too large, the Firmware will refuse to launch the application, and the application launch status will be set to 9 (Bad memory configuration)\n
              (cf. <code><b>AT+WOPEN=7</b></code> description in AT Commands Interface Guide for more information) @}

	  @note Stack memory is limited to 64 kBytes and if allocated above 64 kBytes correct behavior is not guaranteed.
      */
    u32 StackSize;

    /**
      @brief Task name

      @par Description:
      Task identification string, used for debug purpose with Traces & Errors services.
      */
    const ascii * Name;

    /**
      @brief Task relative priority level

      @par Description:
      Task priority level, relatively to the other tasks declared in the table. The higher is the number,
      the higher is the priority level. Priorities values declared in the table should be from 1 to the
      tasks count. This priority  determines the order in which the events are notified
      to the several tasks when several ones receive information at the same time.

      @note @{ All the priorities declared in the table have to be different (two tasks can not have the same priority level).\n
               If there is an error in the priorities declaration, the Firmware will refuse to launch the application, and the application launch status will be set to 17 (Bad priority value)\n
              (cf. <code><b>AT+WOPEN=7</b></code> description in AT Commands Interface Guide for more information) @}
      */
    u8 Priority;
	
} adl_InitTasks_t;



/**
  @brief Tasks definition table

  @par Description:
  Mandatory tasks definition table to be provided by the application. For more information on each task's parameters, please refer to
  the #adl_InitTasks_t description. Each line of this table allows to intialize one task. To let the system know how many tasks are required,
  all the elements of the last line of this table have to be set to 0.\n
  Task entry points declared in the table will be called on Wireless CPU boot, in the priority order
  (the highest priority level is called first).\n

  @note @{ At least one task shall be declared in this table. If no tasks are declared in the table, the Firmware will refuse to launch the application, and the application launch status will be set to 16 (No task declared)\n
           (cf. <code><b>AT+WOPEN=7</b></code> description in AT Commands Interface Guide for more information) @}

  @note @{ There is a maximum limit to the number of tasks which shall be declared in this table (Please refer to the Resources chapter for more information).\n
           If more tasks than the authorized maximum are declared in the table, the Firmware will refuse to launch the application, and the application launch status will be set to 5 (Too many tasks)\n
           (cf. <code><b>AT+WOPEN=7</b></code> description in AT Commands Interface Guide for more information) @}

  @warning @{ Since ADL processing is running in the first application's task context, this one has always to be declared with the highest priority level,
              otherwise the Firmware will refuse to launch the application, and the application launch status will be set to 11 (Application binary init failure)\n
              (cf. <code><b>AT+WOPEN=7</b></code> description in AT Commands Interface Guide for more information) @}
 */
extern const adl_InitTasks_t adl_InitTasks [];



/**
  * @}
  */



/**
  @defgroup adl_AppliInit_public_IFS_IRQStacks Interrupt handlers call stack sizes declaration.
  @ingroup adl_AppliInit_public_IFS

  Interfaces dedicated to the interrupt handlers call stack sizes declaration
  */

/**
  * @addtogroup adl_AppliInit_public_IFS_IRQStacks
  * @{
  */

/**
  @brief Low level interrupt handler call stack size

  @par Description:
  Call stack size (in bytes) of the Low level interrupt handler execution context.
  If the application whishes to handle interrupts (cf. IRQ service chapter & Execution context service chapter), it has also to define the required contexts (low level and/or high level) call stack sizes.

  @note This definition is optional if the application does not plan to use the IRQ service.
 */
//extern const u32 adl_InitIRQLowLevelStackSize;

/**
  @brief High level interrupt handler call stack size

  @par Description:
  Call stack size (in bytes) of the High level interrupt handler execution context.
  If the application whishes to handle interrupts (cf. IRQ service chapter & Execution context service chapter), it has also to define the required contexts (low level and/or high level) call stack sizes.

  @note This definition is optional if the application does not plan to use the IRQ service, or just low level interrupt handlers.
 */
//extern const u32 adl_InitIRQHighLevelStackSize;

/**
  * @}
  */



/**
  @defgroup adl_AppliInit_public_IFS_InitType Initialization information
  @ingroup adl_AppliInit_public_IFS

  Initialization information related interfaces.
  */

/**
  * @addtogroup adl_AppliInit_public_IFS_InitType
  * @{
  */

/**
  @brief Wireless CPU power-on or reset reason

  @par Description:
  Details the reason of the Wireless CPU boot.
 */
typedef enum _adl_InitType_e
{
    ADL_INIT_POWER_ON,              ///< Normal power-on
    ADL_INIT_REBOOT_FROM_EXCEPTION, ///< Reboot after an exception
    ADL_INIT_DOWNLOAD_SUCCESS,      ///< Reboot after a successful install process (cf. adl_adInstall() API)
    ADL_INIT_DOWNLOAD_ERROR,        ///< Reboot after an error in install process (cf. adl_adInstall() API)
    ADL_INIT_RTC                    ///< Power-on due to an RTC alarm (cf. the <b>AT+CALA</b> command documentation for more information)
} adl_InitType_e;



/**
  @brief Initialization type retrieve function

  @par Description:
  Returns the last Wireless CPU power-on or reset reason.

  @return       The Wireless CPU reset reason. Please refer to #adl_InitType_e for more information.

  @section adl_InitGetType_sample Code Sample

  This example demonstrates how to use the function adl_InitGetType() in a nominal case.

  @code
  // Anywhere in the application code, to retrieve init type.
  adl_InitType_e InitType = adl_InitGetType();
  @endcode
 */
adl_InitType_e adl_InitGetType ( void );


/**
  * @}
  */


/**
  @defgroup adl_AppliInit_public_IFS_Naming Miscellaneous name & version information
  @ingroup adl_AppliInit_public_IFS

  @section adl_AppliInit_public_IFS_Naming_Overview Name & version related interfaces.
  The constants defined below allows the application to define some
  information readable by the Sierra Wireless Firmware. These constants definitions
  are optional, and automatically considered as empty strings if not provided
  by the application.

  @section adl_AppliInit_public_IFS_Naming_Samples  Code sample

  @code
// Application name definition
const ascii adl_InitApplicationName[] = "My Application";

// Company name definition
const ascii adl_InitCompanyName[] = "My Company";

// Application version definition
const ascii adl_InitApplicationVersion[] = "v1.0.0";
  @endcode
  */

/**
  * @addtogroup adl_AppliInit_public_IFS_Naming
  * @{
  */

/**
  @brief Application name

  @par Description:
  This constant string should be defined by the application in order
  to provide a name readable by the Sierra Wireless Firmware.
 */
extern const ascii adl_InitApplicationName[];



/**
  @brief Company name

  @par Description:
  This constant string should be defined by the application, in order
  to provide a company name readable by the Sierra Wireless Firmware.
 */
extern const ascii adl_InitCompanyName[];


/**
  @brief Application version

  @par Description:
  This constant string should be defined by the application in order
  to provide a version readable by the Sierra Wireless Firmware.
 */
extern const ascii adl_InitApplicationVersion[];


/**
  * @}
  */


/**
  @defgroup adl_AppliInit_public_IFS_Deprecated Deprecated entry points interfaces
  @ingroup adl_AppliInit_public_IFS

  Former ways to handle the application entry point configuration.
  */

/**
  * @addtogroup adl_AppliInit_public_IFS_Deprecated
  * @{
  */

/**
  @brief Single task application entry point

  @deprecated   Former way to declare the entry point of a single task application, kept for ascendant compatibility. If this function is used, the #adl_InitTasks table has not to be declared.

  @param[in]    adlInitType     Wireless CPU initialization type (please refer to #adl_InitType_e description for more information)
 */
//void adl_main ( adl_InitType_e adlInitType );



/**
  @brief Single task application stack size

  @deprecated   Former way to declare the stack size of a single task application, kept for ascendant compatibility. If this contant is used, the #adl_InitTasks table has not to be declared.
 */
//extern const u16 wm_apmCustomStackSize;



/**
  @brief Low level interrupt handler call stack size

  @deprecated   Former way to declare the stack size of the low level interrupt execution context, kept for ascendant compatibility.
 */
//#define wm_apmIRQLowLevelStackSize adl_InitIRQLowLevelStackSize

/**
  @brief High level interrupt handler call stack size

  @deprecated   Former way to declare the stack size of the high level interrupt execution context, kept for ascendant compatibility.
 */
//#define wm_apmIRQHighLevelStackSize adl_InitIRQHighLevelStackSize

/**
  * @}
  */


/**
  @defgroup adl_AppliInit_public_IFS_StackSize Stack sizes macro
  @ingroup adl_AppliInit_public_IFS

  @section adl_AppliInit_public_IFS_StackSize_Overview Stack sizes macros.
  The constants defined below allows the application to define the stack sizes.

  */

/**
  * @addtogroup adl_AppliInit_public_IFS_StackSize
  * @{
  */

/**
  @brief Application stack size Macro

  @par Description:
  This macro declares the right #wm_apmCustomStackSize value according to the compilers.\n
  The GCC compiler and GNU Newlib (standard C library) implementation require more stack size than ARM compilers.\n
  If the GCC compiler is used, the allocation has to be declared with greater stack sizes (the #X parameter is then multiplied by 3).
 */
//#ifdef __GNU_GCC__
#ifdef __GNUC__
//#define ADL_DECLARE_CALL_STACK(X)   const u16 wm_apmCustomStackSize = X*3
#else /* #ifndef __GNU_GCC__ */
#define ADL_DECLARE_CALL_STACK(X)   const u16 wm_apmCustomStackSize = X
#endif /* #ifndef __GNU_GCC__ */

/**
  @brief Low level interrupt handler call stack size Macro

  @par Description:
  This macro declares the right #adl_InitIRQLowLevelStackSize value according to the compilers.\n
  The GCC compiler and GNU Newlib (standard C library) implementation require more stack size than ARM compilers.\n
  If the GCC compiler is used, the allocation has to be declared with greater stack sizes (the #X parameter is then multiplied by 3).
 */
//#ifdef __GNU_GCC__
#ifdef __GNUC__
//#define ADL_DECLARE_LOWIRQ_STACK(X) const u32 adl_InitIRQLowLevelStackSize = X*3
#else /* #ifndef __GNU_GCC__ */
#define ADL_DECLARE_LOWIRQ_STACK(X) const u32 adl_InitIRQLowLevelStackSize = X
#endif /* #ifndef __GNU_GCC__ */

/**
  @brief High level interrupt handler call stack size Macro

  @par Description:
  This macro declares the right #adl_InitIRQHighLevelStackSize value according to the compilers.\n
  The GCC compiler and GNU Newlib (standard C library) implementation require more stack size than ARM compilers.\n
  If the GCC compiler is used, the allocation has to be declared with greater stack sizes (the #X parameter is then multiplied by 3).
 */
//#ifdef __GNU_GCC__
#ifdef __GNUC__
//#define ADL_DECLARE_HIGHIRQ_STACK(X) const u32 adl_InitIRQHighLevelStackSize = X*3
#else /* #ifndef __GNU_GCC__ */
#define ADL_DECLARE_HIGHIRQ_STACK(X) const u32 adl_InitIRQHighLevelStackSize = X
#endif /* #ifndef __GNU_GCC__ */
/**
  * @}
  */

/**
  @defgroup adl_AppliInit_public_IFS_CompilationFlag Compilation flag
  @ingroup adl_AppliInit_public_IFS

  @section adl_AppliInit_public_IFS_CompilationFlag_Overview Compilation flag for GDB

  */

/**
  * @addtogroup adl_AppliInit_public_IFS_CompilationFlag
  * @{
  */

/**
  @brief Compilation flag definition

  @par Description:
  This enum shall be used for the #adl_CompilationMode variable to define the compilation flag

 */
typedef enum
{
  ADL_COMPILATION_MODE_UNKNOWN = 0x0DEADBEF,     ///< Unknown compilation mode
  ADL_COMPILATION_MODE_DEBUG   = 0x0CAFEBAB,     ///< Debug compilation mode
  ADL_COMPILATION_MODE_RELEASE = 0x0C0FEEEE      ///< Release compilation mode
}adl_CompilationMode_e;

/**
  @brief Is the Open AT compiled with Debug or Release mode (or unknown)

  @par Description:
  this global variable is used to define the compilation flag
 */
//extern const adl_CompilationMode_e adl_CompilationMode;

/**
  * @}
  */

// Don't use semihosting SWI for RVDS (only defined in application)
#ifndef __OAT_LIBRARY_BUILD__
#ifdef __ARMCC_VERSION
#pragma import(__use_no_semihosting_swi)
#endif
#endif


#endif

