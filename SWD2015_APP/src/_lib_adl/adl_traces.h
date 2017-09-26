/*
 * adl_traces.h
 *
 * Created: 31/10/2557 10:25:43
 *  Author: pan
 */ 


#ifndef ADL_TRACES_H_
#define ADL_TRACES_H_

/**********************************************************************************************************************/
/*  adl_traces.h   -  Copyright Sierra Wireless Inc. (c) 2010														  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_traces.h
  @brief ADL Debug Traces service public interface header file
  @ingroup adl_traces_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_traces_public_IFS ADL Debug Traces service Interface Specification
  @endif
  
  @section adl_traces_public_IFS_OVERVIEW Overview
  This document describes the public Debug Traces service interface of the ADL software element.
  
  @version 02.00

  @author SUB Infosys AT Maintenance

  @section adl_traces_public_IFS_HISTORY History
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
  
  @section adl_traces_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_traces_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_traces_public_IFS_RELATED_DOCUMENTS Related documents references
  
  Here are the documents referenced in this interface specification.
  -# @anchor DocRef1 <a href="Notes://domino-arc/C1256B52003391E9/C12570D1004A006DC1256A94004F97ED/152992295B9717ABC1257321003FEE8A">[WM_DEV_OAT_FCS_059] Debug Facilities services</a>
  
  @subsection adl_traces_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
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
  @defgroup adl_traces_public_IFS ADL Debug Traces service Interface Specification
  @else
  @addtogroup adl_traces_public_IFS
  @{
  @endif
  
  @section adl_traces_public_IFS_ftr Features
  
  This service allows to display debug trace strings on Developer Studio.
  The different ways to embed these trace strings in an Open AT® application depends 
  on the selected configuration in the used Open AT IDE (see below).
  For more information about the Developer Studio, the configurations and the Integrated Development Environment, please refer to the Tools Manual.
  
  The defined operations are:
  - <b>Trace</b> function & macros (adl_trcPrint(), TRACE() & FULL_TRACE()) to print the required trace string
  - <b>Dump</b> function & macros (adl_trcDump(), DUMP() & FULL_DUMP()) to dump the required buffer content
  
  @section adl_traces_public_IFS_sample Code Sample
  
  The code sample below illustrates a nominal use case of the ADL Debug Traces service public interface (error cases are not handled)
  
  @code
u8 MyInt = 12;
ascii * MyString = "hello";

// Print a debug trace for current context on level 1
TRACE (( 1, "My Sample Trace: %d", MyInt ));

// Dump a buffer content for current context on level 2
DUMP ( 2, MyString, strlen ( MyString ) );
  @endcode
  
  @if GLOBAL_IFS
  @}
  @endif
  */



#ifndef __adl_traces_H__
#define __adl_traces_H__



/**
  @defgroup adl_traces_public_IFS_Config Build configuration macros
  @ingroup adl_traces_public_IFS
  
  According to the chosen build configuration in the IDE, following macros will be defined or not,
  allowing the user to embed none, part or all of the debug traces information in its final application.
  
  @section adl_traces_public_IFS_Config_Debug Debug Configuration
  When the Debug configuration is selected in the IDE,
  the <b>__DEBUG_APP__</b> compilation flag is defined, and also the TRACE() & DUMP() macros.\n
  Traces & dumps declared with these macros will be embedded at compilation time.\n\n
  In this Debug configuration, the FULL_TRACE() and FULL_DUMP() macros are ignored
  (even if these ones are used in the application source code, they will neither be compiled, nor displayed on Developer Studio at runtime).
  
  @section adl_traces_public_IFS_Config_FullDebug Full Debug Configuration
  When the Full Debug configuration is selected in the IDE,
  both the <b>__DEBUG_APP__</b> and <b>__DEBUG_FULL__</b> compilation flags are defined, and also the TRACE(), FULL_TRACE(), DUMP() & FULL_DUMP() macros.\n
  Traces & dumps declared with these macros will be embedded at compilation time.
  
  @section adl_traces_public_IFS_Config_Release Release Configuration
  When the Release configuration is selected in the IDE,
  neither the <b>__DEBUG_APP__</b> nor <b>__DEBUG_FULL__</b> compilation flags are defined\n
  The TRACE(), FULL_TRACE(), DUMP() and FULL_DUMP() macros are ignored
  (even if these ones are used in the application source code, they will neither be compiled, nor displayed on Developer Studio at runtime).
  */

/**
  * @addtogroup adl_traces_public_IFS_Config
  * @{
  */

// Within Remote Task Environment, always define Debug Flag
#ifdef __REMOTETASKS__
#ifndef __DEBUG_APP__
#define __DEBUG_APP__
#endif
#endif

#define __DEBUG_APP__
#ifdef __DEBUG_APP__
// With the __DEBUG_APP__ flag, define TRACE and DUMP macros

#define TRACE(_X_)          adl_trcPrint _X_
#define DUMP(_lvl_,_P_,_L_) adl_trcDump  ( _lvl_, ( u8 * ) _P_, _L_ )


#else
// Without the __DEBUG_APP__ flag, TRACE and DUMP macros are not defined

/**
  @brief Trace macro
  
  @par Description:
  This macro is a shortcut to the adl_trcPrint() function. 
  Traces declared with this macro are only embedded in the application 
  if it is compiled with in the Debug or Full Debug configuration, but not in the Release configuration.
 */
#define TRACE(_X_)

/**
  @brief Dump macro
  
  @par Description:
  This macro is a shortcut to the adl_trcDump() function. 
  Dumps declared with this macro are only embedded in the application 
  if it is compiled with in the Debug or Full Debug configuration, but not in the Release configuration.
 */
#define DUMP(_lvl_,_P_,_L_)

#endif



#ifdef __DEBUG_FULL__
// With the __DEBUG_FULL__ flag, define FULL_TRACE and FULL_DUMP macros

#define FULL_TRACE(_X_)          adl_trcPrint _X_
#define FULL_DUMP(_lvl_,_P_,_L_) adl_trcDump  ( _lvl_, ( u8 * ) _P_, _L_ )


#else
// Without the __DEBUG_FULL__ flag, FULL_TRACE and FULL_DUMP macros

/**
  @brief Full Trace macro
  
  @par Description:
  This macro is a shortcut to the adl_trcPrint() function. 
  Traces declared with this macro are only embedded in the application 
  if it is compiled with in Full Debug configuration, but not in the Debug or Release configuration.
 */
#define FULL_TRACE(_X_)

/**
  @brief Full Dump macro
  
  @par Description:
  This macro is a shortcut to the adl_trcDump() function. 
  Dumps declared with this macro are only embedded in the application 
  if it is compiled with in Full Debug configuration, but not in the Debug or Release configuration.
 */
#define FULL_DUMP(_lvl_,_P_,_L_)

#endif

/**
  * @}
  */

/**
  @brief Trace channel

  @par Description:
  This enumeration lists the available trace channel, which one can find in Developer Studio.
 */
typedef enum _adl_trcChannel_e
{
    _OAT01 = ( 1<<8),	///< OAT01 channel
    _OAT02 = ( 2<<8),	///< OAT02 channel
    _OAT03 = ( 3<<8),	///< OAT03 channel
    _OAT04 = ( 4<<8),	///< OAT04 channel
    _OAT05 = ( 5<<8),	///< OAT05 channel
    _OAT06 = ( 6<<8),	///< OAT06 channel
    _OAT07 = ( 7<<8),	///< OAT07 channel
    _OAT08 = ( 8<<8),	///< OAT08 channel
    _OAT09 = ( 9<<8),	///< OAT09 channel
    _OAT10 = (10<<8),	///< OAT10 channel
    _OAT11 = (11<<8),	///< OAT11 channel
    _OAT12 = (12<<8),	///< OAT12 channel
    _OAT13 = (13<<8),	///< OAT13 channel
    _OAT14 = (14<<8),	///< OAT14 channel
    _OAT15 = (15<<8),	///< OAT15 channel
    _OAT16 = (16<<8),	///< OAT16 channel
    _OAT17 = (17<<8),	///< OAT17 channel
    _OAT18 = (18<<8),	///< OAT18 channel
    _OAT19 = (19<<8),	///< OAT19 channel
    _OAT20 = (20<<8),	///< OAT20 channel
    _OAT21 = (21<<8),	///< OAT21 channel
    _OAT22 = (22<<8),	///< OAT22 channel
    _OAT23 = (23<<8),	///< OAT23 channel
    _OAT24 = (24<<8),	///< OAT24 channel
    _OAT25 = (25<<8),	///< OAT25 channel
    _OAT26 = (26<<8),	///< OAT26 channel
    _OAT27 = (27<<8),	///< OAT27 channel
    _OAT28 = (28<<8),	///< OAT28 channel
    _OAT29 = (29<<8),	///< OAT29 channel
    _OAT30 = (30<<8),	///< OAT30 channel
    _OAT31 = (31<<8),	///< OAT31 channel
    _OAT32 = (32<<8)	///< OAT32 channel
} adl_trcChannel_e;

/**
  @brief Max size of a trace element name (including NULL termination) (must be equal to STRING_SIZE_MAX in remote.ho)
 */
#define ADL_TRC_SIZE_MAX 7 


/**
  @brief Trace channel confiuration structure
  
 */
typedef struct {
    char name[ADL_TRC_SIZE_MAX];	///< name of the trace element
    u32 level_mask;	///< 32-bits mask, each (1<<i) indicates that traces are enabled on level i
} adl_trcCfg_t;


/**
  @defgroup adl_traces_public_IFS_op2 Trace and Dump functions
  @ingroup adl_traces_public_IFS
  */

/**
  * @addtogroup adl_traces_public_IFS_op2
  * @{
  */

/**
  @brief Trace function
  
  @par Description:
  This function displays the required debug trace on the provided trace level.
  If no channel is given in the first parameter, the trace will be displayed in the Developer Studio Tool, according to the current context:
  - for tasks: on the trace element name defined in the tasks declaration table (cf. Application Initialization service)
  - for Low Level Interrupt handlers: on the "LLH" trace element
  - for High Level Interrupt handlers: on the "HLH" trace element
  Otherwise it will be displayed in the given channel (@ref adl_trcChannel_e)
  
  @par
  In addition to the trace information, a Wireless CPU local timestamp is also displayed in the tool.
  
  @par Example 1

  @code
u8 I = 123;
TRACE (( _OAT01 | 1, "Value of I: %d", I ));
  @endcode

At runtime, this will display the following string on the _OAT01 channel, level 1 on the Developer Studio Tool:
  @code
Value of I: 123
  @endcode
  
    @par Example 2
  
    @code
  u8 I = 123;
  TRACE ((  2, "Value of I: %d", I ));
    @endcode
  
  At runtime, this will display the following string on the _ADL channel, level 2 on the Developer Studio Tool:
    @code
  Value of I: 123
    @endcode
    
  @param[in]        level		       Trace level on which the information has to be sent. This is composed of two parts:
                                                  - Bits[7:0] which gives the level in Dev Studio. Valid range is <b>1 - 32</b>.
                                                  - Other bits to choose in @ref adl_trcChannel_e enumeration
  @param[in]        strFormat	String to be displayed, using a standard C "sprintf" format.
  @param[in]        ...			Additional arguments to be dynamically inserted in the provided constant string.
  
  @note     @{ Direct use of the adl_trcPrint() function is not recommended.
               The TRACE() & FULL_TRACE() macros should be used instead, to take benefit of the build configurations features @}
  @note     '\%s' character, normally used to insert strings, is not supported by the trace function.
  @note     The trace display should be limited to 255 bytes. If the trace string is longer, it will be truncated.
 */
s32 adl_trcPrint ( u32 Level, const ascii *strFormat, ... );

/**
  @brief Dump function
  
  @par Description:
  This function dumps the required buffer content on the provided trace level.
  The dump will be displayed in Developer Studio, according to the current context:
  - for tasks: on the trace element name defined in the tasks declaration table (cf. Application Initialization service)
  - for Low Level Interrupt handlers: on the "LLH" trace element
  - for High Level Interrupt handlers: on the "HLH" trace element
  
  @par
  In addition to the dump data, a Wireless CPU local timestamp is also displayed in the tool.\n\n
  Since a display line maximum length is 255 bytes, if the display length is greater than 80 (each byte is displayed on 3 ascii characters),
  the dump will be segmented on several lines. Each 80 bytes truncated line will end with the "..." characters sequence.
  
  @par Example 1

  @code
u8 * Buffer = "\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9";
DUMP ( 1, Buffer, 10 );
  @endcode

At runtime, this will display the following string on the level 1 in Developer Studio:
  @code
00 01 02 03 04 05 06 07 08 09
  @endcode

  @par Example 2
  
  @code
u8 Buffer [ 200 ], i;
for ( i = 0 ; i < 200 ; i++ ) Buffer [ i ] = i;
DUMP ( 1, Buffer, 200 );
  @endcode

At runtime, this will display the following three lines on the level 1 in Developer Studio:
  @code
00 01 02 03 04 05 06 07 08 09 0A [bytes from 0B to 4D] 4E 4F...
50 51 52 53 54 55 56 57 58 59 5A [bytes from 5B to 9D] 9E 9F...
A0 A1 A2 A3 A4 A5 A6 A7 [bytes from A8 to C4] C5 C6 C7
  @endcode
  
  @param[in]        level		       Trace level on which the information has to be sent. This is composed of two parts:
                                                  - Bits[7:0] which gives the level in Dev Studio. Valid range is <b>1 - 32</b>.
                                                  - Other bits to choose in @ref adl_rtcChannel_e enumeration
  @param[in]        DumpBuffer	Buffer address to be dumped
  @param[in]        DumpLength	Number of bytes to be displayed at required address.
  
  @note     @{ Direct use of the adl_trcDump() function is not recommended.
               The DUMP() & FULL_DUMP() macros should be used instead, to take benefit of the build configurations features @}
 */
void adl_trcDump ( u32 level, u8 * DumpBuffer, u16 DumpLength );

/**
  @brief Configure parameters (name, level mask) of a given OAT trace channel
  
  @par Description:
  
  
  @param[in]        channel   Trace channel on which to send the required trace
  @param[in]        cfg         Trace channel configuration to apply
  @param[in]        Save      If set to TRUE, this configuration will be saved in NVRAM
  
  @retval   OK      On success
                ERROR    Otherwise
 */
//s32 adl_trcSetCfg ( adl_trcChannel_e channel, adl_trcCfg_t * Cfg, bool Save );

/**
  @brief Get parameters (name, level mask) of a given OAT trace channel
  
  @par Description:
  
  
  @param[in]        channel       Trace channel on which to send the required trace
  @param[out]      cfg              Trace channel configuration to get
  
  @retval   OK      On success
                ERROR    Otherwise
 */
//s32 adl_trcGetCfg ( adl_trcChannel_e channel, adl_trcCfg_t * Cfg );

/**
  * @}
  */

u32 adl_trcLevGet ( void );

void adl_trcLevSet ( u32 level );


#endif


#endif /* ADL_TRACES_H_ */