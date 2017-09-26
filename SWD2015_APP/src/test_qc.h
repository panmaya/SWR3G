/*
 * test_qc.h
 *
 * Created: 7/6/2016 3:19:47 PM
 *  Author: Panmaya
 */ 


#ifndef APT_MODEQC_H_
#define APT_MODEQC_H_

#define HELLO_PRINT_QC(_myDate,_myTime)		do {\
	adl_rtcTime_t Time;\
	adl_rtcGetTime(&Time);\
	vPrintf("\r\n*****************************************************\r\n");\
	vPrintf("\r\n*************   TEST MODE APPLICATION    ************\r\n");\
	vPrintf("\r\n*****************************************************\r\n");\
	vPrintf("-- %s\r\n",adl_InitApplicationName);\
	vPrintf("-- BOARD %s built %s : %s\r\n", BOARD_NAME, _myDate, _myTime);\
	vPrintf("-- %s\r\n",adl_InitCompanyName);\
	vPrintf("-- VERSION %s\r\n",adl_InitApplicationVersion);\
	vPrintf("-- TODAY: %02d/%02d/%04d",Time.Day,Time.Month,Time.Year);\
	vPrintf("\r\n*****************************************************\r\n");\
}while(0)


void test_qc(void *pvParameters);

#endif /* APT_MODEQC_H_ */