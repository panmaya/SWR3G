/*
 * adl_rtc.c
 *
 * Created: 29/1/2558 9:59:26
 *  Author: pan
 */ 

#include "adl_global.h"


static uint32_t ul_time = 0;
static bool ext_tick_rtc = false;
static uint16_t ms_tick = 0;

static uint32_t ul_hour = 0, ul_minute = 0, ul_second = 0;
static uint32_t ul_year = 0, ul_month = 0, ul_day = 0, ul_week = 0;
static uint8_t day_of_month[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

/**
 * \brief Current time returned is packed into a DWORD value.
 *
 * The bit field is as follows:
 *
 * bit31:25  Year from 1980 (0..127)
 *
 * bit24:21  Month (1..12)
 *
 * bit20:16  Day in month(1..31)
 *
 * bit15:11  Hour (0..23)
 *
 * bit10:5   Minute (0..59)
 *
 * bit4:0    Second (0..59)
 *
 * \return Current time.
 */


/**
 * \brief Calculate week from year, month, day.
 */
static uint32_t ADL_RTC_CALC_Week(uint32_t year, uint32_t month, uint32_t day)
{
	uint32_t week;

	if (month == 1 || month == 2) {
		month += 12;
		--year;
	}

	week = (day + 2 * month + 3 * (month + 1) / 5 + year +
			year / 4 - year / 100 + year / 400) % 7;

	++week;

	return week;
}


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

s32 adl_rtcGetTime ( adl_rtcTime_t * rtc ) {
	static uint32_t hour, minute, second;
	static uint32_t year, month, day, week;
		
	rtc_get_time(RTC, &hour, &minute, &second);
	rtc_get_date(RTC, &year, &month, &day, &week);

	rtc->Year = (uint16_t)year;
	rtc->Month = (uint8_t)month;
	rtc->Day = (uint8_t)day;
	rtc->Hour = (uint8_t)hour;
	rtc->Minute = (uint8_t)minute;
	rtc->Second  = (uint8_t)second;

	return OK;
}

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
s32 adl_rtcSetTime ( adl_rtcTime_t * TimeStructure ){
	uint32_t hour, minute, second;
	uint32_t year, month, day, week;
	s32 ret = 0;
	
	day = TimeStructure->Day;
	month = TimeStructure->Month;
	year = TimeStructure->Year;
	hour = TimeStructure->Hour;
	minute = TimeStructure->Minute;
	second = TimeStructure->Second;
	week = ADL_RTC_CALC_Week(year,month,day);
	
	
	//ensure calendar and clock running
	Clr_bits(RTC->RTC_CR,/*RTC_CR_UPDCAL|*/RTC_CR_UPDTIM);

	//wait at least a second since last update
	while ((RTC->RTC_SR & RTC_SR_SEC) == RTC_SR_SEC_NO_SECEVENT);

	//stop calendar and clock
	Set_bits(RTC->RTC_CR,/*RTC_CR_UPDCAL|*/RTC_CR_UPDTIM);

	//wait for go ahead to update
	while ((RTC->RTC_SR & RTC_SR_ACKUPD) == RTC_SR_ACKUPD_FREERUN);
	
	ret += rtc_set_time(RTC, hour, minute, second);
	
	//ensure calendar and clock running
	Clr_bits(RTC->RTC_CR,RTC_CR_UPDCAL/*|RTC_CR_UPDTIM*/);

	//wait at least a second since last update
	while ((RTC->RTC_SR & RTC_SR_SEC) == RTC_SR_SEC_NO_SECEVENT);

	//stop calendar and clock
	Set_bits(RTC->RTC_CR,RTC_CR_UPDCAL/*|RTC_CR_UPDTIM*/);

	//wait for go ahead to update
	while ((RTC->RTC_SR & RTC_SR_ACKUPD) == RTC_SR_ACKUPD_FREERUN);
	
	ret += rtc_set_date(RTC, year, month, day, week);
		
	if(ret)return -1;
	return 0;
}

