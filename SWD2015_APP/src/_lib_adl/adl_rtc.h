/*
 * adl_rtc.h
 *
 * Created: 22/1/2558 11:41:40
 *  Author: pan
 */ 


#ifndef ADL_RTC_H_
#define ADL_RTC_H_
/**********************************************************************************************************************/
/*  adl_rtc.h   -  Copyright Sierra Wireless Inc. (c) 2010															  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_rtc.h
  @brief ADL Real Time Clock Service public interface header file
  @ingroup adl_rtc_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_rtc_public_IFS ADL Real Time Clock Service Interface Specification
  @endif

  @section adl_rtc_public_IFS_OVERVIEW Overview
  This document describes the public Real Time Clock Service interface of the ADL software element.
  
  @version 02.00

  @author SUB Infosys AT Maintenance

  @section adl_rtc_public_IFS_HISTORY History
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

  @section adl_rtc_public_IFS_INTRODUCTION Introduction

  @subsection adl_rtc_public_IFS_REMINDER Reminder

  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.

  @subsection adl_rtc_public_IFS_RELATED_DOCUMENTS Related documents references

  Here are the documents referenced in this interface specification.
  @anchor ref1 [1] <A HREF="http://domino-arc/ged/gedysdoc.nsf/(IDs)/C1C597B14E6AF948C12572A70055A8B0?"> FCS RTC Service </A> \n

  @subsection adl_rtc_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary

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
  @defgroup adl_rtc_public_IFS ADL Real Time Clock Service Interface Specification
  @else
  @addtogroup adl_rtc_public_IFS
  @{
  @endif

  @section adl_rtc_public_IFS_ftr Features

  ADL provides a RTC service to access to the Wireless CPU®s inner RTC, and to process time related data.

  The defined operations are:
  - A adl_rtcGetTime()
  - A adl_rtcSetTime()
  - A adl_rtcConvertTime()
  - A adl_rtcDiffTime()

  @section adl_CmdHandler_public_IFS_sample Code Sample

  This example demonstrates how to use the Real Time Clock service in a nominal case (error cases not handled) with a Wireless CPU.

  Complete examples using the AT Command service are also available on the SDK.

  @code

    // Somewhere in the application code, used as an event handler
    void MyFunction ( void )
    {
        // Local variables
        adl_rtcTime_t Time1, Time2, Diff;
        adl_rtcTimeStamp_t Stamp1, Stamp2, DiffStamp;
        s32 Way;

        // Get time
        adl_rtcGetTime ( &Time1 );
        adl_rtcGetTime ( &Time2 );

        // Convert to time stamps
        adl_rtcConvertTime ( &Time1, &Stamp1, ADL_RTC_CONVERT_TO_TIMESTAMP );
        adl_rtcConvertTime ( &Time2, &Stamp2, ADL_RTC_CONVERT_TO_TIMESTAMP );

        // Reckon time difference
        Way = adl_rtcDiffTime ( &Stamp1, &Stamp2, &DiffStamp );

        // Convert the time difference from time stamps
        adl_rtcConvertTime ( &Diff, &DiffStamp, ADL_RTC_CONVERT_FROM_TIMESTAMP );

        // Set back the initial time
        adl_rtcSetTime ( &Time1 );
    }

  @endcode
  @if GLOBAL_IFS
  @}
  @endif
  */


#ifndef __ADL_RTC_H__
#define __ADL_RTC_H__


/**
  @defgroup adl_rtc_public_IFS_capa Capabilities registry informations
  @ingroup adl_rtc_public_IFS

  ADL provides informations to get the RTC Second Frac Part capabilities.\n \n
 * The following entry is defined in the registry:\n
 * <table>
 * <tr><td><b>Registry entry</b></td><td><b>Type</b></td><td><b>Description</b></td></tr>
 * <tr><td>rtc_PreScalerMaxValue</td><td>INTEGER</td><td>0: No second fractional part<br>xxx: Second fractional part resolution</td></tr>
 * </table>
 */

/**
  @defgroup adl_rtc_public_IFS_Definitions Types and constants definition
  @ingroup adl_rtc_public_IFS

  */

/**
  * @addtogroup adl_rtc_public_IFS_Definitions
  * @{
  */


/**
  @brief Seconds count in a day

  @par Description:
  Used in the #ADL_RTC_GET_TIMESTAMP_DAYS extraction

 */
#define ADL_RTC_DAY_SECONDS     ( 24 * ADL_RTC_HOUR_SECONDS )

/**
  @brief Seconds count in a hour

  @par Description:
  Used in the #ADL_RTC_GET_TIMESTAMP_HOURS extraction

 */
#define ADL_RTC_HOUR_SECONDS    ( 60 * ADL_RTC_MINUTE_SECONDS )

/**
  @brief Seconds count in a minute

  @par Description:
  Used in the #ADL_RTC_GET_TIMESTAMP_MINUTES extraction

 */
#define ADL_RTC_MINUTE_SECONDS  60

/**
  @brief µseconds count in a millisecond

  @par Description:
  Used in the #ADL_RTC_GET_TIMESTAMP_MS and #ADL_RTC_GET_TIMESTAMP_US extraction

 */
#define ADL_RTC_MS_US           1000


/**
  @brief RTC Time Structure

  @par Description:
  Holds a RTC time:
  @note : Weekday should be start from Sunday to Saturday i.e Sunday = 0 and Saturday =6
 */
typedef struct _adl_rtcTime_t
{
    //u32 Pad0;           ///< Not used
    //u32 Pad1;           ///< Not used
    u16 Year;           ///< Year (Four digits)
    u8  Month;          ///< Month (1-12)
    u8  Day;            ///< Day of Month [1..31]
   // u8  WeekDay;        ///< Day of Week [0..6]
    u8  Hour;           ///< Hour (0-23)
    u8  Minute;         ///< Minute (0-59)
    u8  Second;         ///< Second (0-59)
    //u32 SecondFracPart; ///< Second fractional part (0-MAX) The MAX value is available from the registry field rtc_PreScalerMaxValue. See panel "Capabilities registry informations".
    //u32 Pad2;           ///< Not used
} adl_rtcTime_t;

/**
  @brief Timestamp Structure

  @par Description:
  Used to perform arithmetic operations on time data:
 */
//typedef struct _adl_rtcTimeStamp_t
//{
//    u32 TimeStamp;      ///< Seconds elapsed since 1st January 1970
    //u32 SecondFracPart; ///< Second fractional part (0-MAX) The MAX value is available from the registry field rtc_PreScalerMaxValue. See panel "Capabilities registry informations".
//} adl_rtcTimeStamp_t;

/**
  @brief Conversion modes structure

  @par Description:
  This structure contains the available conversion modes
 */
//typedef enum _adl_rtcConvert_e
//{
 //   ADL_RTC_CONVERT_TO_TIMESTAMP,       ///< Conversion mode to TimeStamp
 //   ADL_RTC_CONVERT_FROM_TIMESTAMP,     ///< Conversion mode from TimeStamp
//} adl_rtcConvert_e;


/**
  * @}
  */

/**
  @defgroup adl_rtc_public_IFS_cb1 RTC macros
  @ingroup adl_rtc_public_IFS

  */

/**
  * @addtogroup adl_rtc_public_IFS_cb1
  * @{
  */

/**
  @brief Second fractional part step value (in µs) extraction macro

  @par Description:
  From the Capabilities get the second fractional part step value.

 */
//#define ADL_RTC_SECOND_FRACPART_STEP    adl_rtcGetSecondFracPartStep()

/**
  @brief Days number extraction macro

  @par Description:
  From a TimeStamp date extract the days number

  @param[in]        _t    TimeStamp date

 */
//#define ADL_RTC_GET_TIMESTAMP_DAYS(_t)      ( _t.TimeStamp / ADL_RTC_DAY_SECONDS )

/**
  @brief Hours number extraction macro

  @par Description:
  From a TimeStamp date extract the hours number

  @param[in]        _t    TimeStamp date

 */
//#define ADL_RTC_GET_TIMESTAMP_HOURS(_t)     ( ( _t.TimeStamp % ADL_RTC_DAY_SECONDS ) / ADL_RTC_HOUR_SECONDS )

/**
  @brief Minutes number extraction macro

  @par Description:
  From a TimeStamp date extract the minutes number

  @param[in]        _t    TimeStamp date

 */
//#define ADL_RTC_GET_TIMESTAMP_MINUTES(_t)   ( ( _t.TimeStamp % ADL_RTC_HOUR_SECONDS ) / ADL_RTC_MINUTE_SECONDS )

/**
  @brief Seconds number extraction macro

  @par Description:
  From a TimeStamp date extract the seconds number

  @param[in]        _t    TimeStamp date

 */
//#define ADL_RTC_GET_TIMESTAMP_SECONDS(_t)   ( _t.TimeStamp % ADL_RTC_MINUTE_SECONDS )

/**
  @brief Milliseconds number extraction macro

  @par Description:
  From a TimeStamp date extract the milliseconds number

  @param[in]        _t    TimeStamp date

 */
//#define ADL_RTC_GET_TIMESTAMP_MS(_t)        ( ((u32)( _t.SecondFracPart * ADL_RTC_SECOND_FRACPART_STEP )) / ADL_RTC_MS_US )

/**
  @brief µseconds number extraction macro

  @par Description:
  From a TimeStamp date extract the µseconds number

  @param[in]        _t    TimeStamp date

 */
//#define ADL_RTC_GET_TIMESTAMP_US(_t)        ( ((u32)( _t.SecondFracPart * ADL_RTC_SECOND_FRACPART_STEP )) % ADL_RTC_MS_US )


/**
  * @}
  */

/**
  @defgroup adl_rtc_public_IFS_op1 RTC API access functions
  @ingroup adl_rtc_public_IFS

  These function allow RTC usage
  */

/**
  * @addtogroup adl_rtc_public_IFS_op1
  * @{
  */

/**
  @brief Retrieves the second fractional part step from the Capabilities

  @par Description:
  Retrieves the second fractional part step (in µs), reading the rtc_PreScalerMaxValue register field

  @return The second fractional part step of the Wireless CPU, in µs

 */
//float adl_rtcGetSecondFracPartStep ( void );

/**
  @brief Retrieves the current RTC time

  @par Description:
  Retrieves the current RTC time into a #adl_rtcTime_t structure

  @param[out]   TimeStructure   RTC structure where to copy current time

  @return       Returned value description
  - <b>OK</b>                            on success
  - <b>ADL_RET_ERR_PARAM</b>             on parameter error
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>    if called from a low level interruption handler.

 */
s32 adl_rtcGetTime ( adl_rtcTime_t * TimeStructure );

/**
  @brief Set a RTC time

  @par Description:
  Set a RTC time from a #adl_rtcTime_t structure

  @param[in]   TimeStructure   RTC structure where to get current time

  @return       Returned value description
  - <b>OK</b>                            on success
  - <b>ADL_RET_ERR_PARAM</b>             on parameter error
  - <b>ADL_RET_ERR_SERVICE_LOCKED</b>    if called from a low level interruption handler.


  @note 1 : the input parameter can not be a constant since it is modified by the API
  @note 2 : when setting the rtc time SecondFracPart and WeekDay field are ignored

 */
s32 adl_rtcSetTime ( adl_rtcTime_t * TimeStructure );


/**
  @brief Converts time between RTC time structure and timestamp

  @par Description:
  This function is able to convert RTC time structure to timestamp structure, and timestamp structure to RTC time structure thanks to a 3rd agument precising the way of conversion

  @param[in,out]    TimeStructure   RTC structure where to get/set current time
  @param[in,out]    TimeStamp       Timestamp structure where to get/set current time
  @param[in]        Conversion      Conversion way :
                                    - #ADL_RTC_CONVERT_TO_TIMESTAMP
                                    - #ADL_RTC_CONVERT_FROM_TIMESTAMP

  @return       Returned value description
  - <b>OK</b>                   on success
  - <b>ERROR</b>                if conversion failed (internal error)
  - <b>ADL_RET_ERR_PARAM</b>    on parameter error

 */
//s32 adl_rtcConvertTime ( adl_rtcTime_t * TimeStructure, adl_rtcTimeStamp_t * TimeStamp, adl_rtcConvert_e Conversion );

/**
  @brief Reckons the difference between two timestamps

  @par Description:
  Reckons the difference between two timestamps

  @param[in]    TimeStamp1   First timestamp to compare
  @param[in]    TimeStamp2   Second timestamp to compare
  @param[out]   Result       Reckoned time difference

  @return       Returned value description
  - <b>1</b>    if TimeStamp1 is greater than TimeStamp2
  - <b>-1</b>   if TimeStamp2 is greater than TimeStamp1
  - <b>0</b>    if the provided TimeStamps are the same
  - <b>ADL_RET_ERR_PARAM</b> on parameter error
 */
//s32 adl_rtcDiffTime ( adl_rtcTimeStamp_t * TimeStamp1, adl_rtcTimeStamp_t * TimeStamp2, adl_rtcTimeStamp_t * Result );

/**
  * @}
  */

//void _adl_rtc_secFraction_update1ms(void);

#endif





#endif /* ADL_RTC_H_ */