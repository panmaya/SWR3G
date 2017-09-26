/*
 * misc_config.h
 *
 *  Created on: 17 ¡.¾. 2555
 *      Author: Pan
 */

#ifndef MISC_CONFIG_H_
#define MISC_CONFIG_H_
#include <adl_global.h>
#include <limits.h>


#define GPBR_SLEEP_ID		GPBR0
#define GPBR_SOUND_ID		GPBR1
#define GPBR_FDISK_ID		GPBR2
#define GPBR_DFRAG_ID		GPBR3
#define GPBR_RECFG_ID		GPBR4
#define GPBR_CLRBF_ID		GPBR5
#define GPBR_RESET_ID		GPBR6



#define STRINGIFY(x)					#x
#define TOSTRING(x)						STRINGIFY(x)
#define AT								__FILE__ ":" TOSTRING(__LINE__)

#define SWD2015_FW_VERSION				302

#define _1SecTick_Second(_x)			(_x)// For 1 Second Loop
#define _100mSecTick_Second(_x)			(_x*10) //For 100 MilliSecond Loop
#define _1mSecTick_Second(_x)			(_x*1000) //For 1 MilliSecond Loop

#define DMS_SERVER_IP_DEFAULT			"202.149.98.139"
#define DMS_SERVER_PORT_DEFAULT			909

#define SWD_SERVER_DEFAULT_IP			"203.151.232.144"
#define SWD_SERVER_DEFAULT_PORT			700

#define APT_PARK_SAMPLING_DEFAULT		999   // Never use in SWD
#define APT_DRIVE_SAMPLING_DEFAULT		60
#define SWD_OVER_SPEED_DEFAULT			100
#define SWD_IDLE_SPEED_DEFAULT			7     // km/h
#define SWD_HA_DEFAULT_VAL				25
#define SWD_HD_DEFAULT_VAL				25
#define SWD_DEFAULT_EMAIL				"mail@noconfig.com"

#define VEXT_MIN_THRESHOLD				10000
#define VINT_MIN_THRESHOLD				3500
#define WAKEUP_TO_SEND_PERIOD			(12*20)//20 Minute


#define APT_CARD_ALERT_TIME			_1SecTick_Second(600)   //10 Minute

#define __OAT_API_VERSION__				1
#define __MCU_FW_PATH__					"0:SWD2015.bin"
#define __OTA_LOG_PATH__				"0:OTA.txt"
#define __BOOT_DIAG_PATH__				"0:BDX.txt"
#define __HTTP_DATA_PATH__				"0:ZONE.txt"
#define __FDISK_LOG_PATH__				"0:FDISK.txt"
#define __HTTP_DATA_MAXSIZE__			(2*1024)//(16*1024)
#define __MCU_FW_MAXSIZE__				(256*1024)

#define CPE_SERVER_CTX_ID				1
#define DMS_SERVER_CTX_ID				2

#define SWD_IO_CHANGE_IG				0
#define SWD_IO_CHANGE_PTO1				1
#define SWD_IO_CHANGE_PTO2				2
#define SWD_IO_CHANGE_PTO3				3

#define SWD_HTTP_URL_ZONE				"http://www.dtcgps.com/WEB2014/webservice/alert_zone_service.php?sim="

#define SWD_RFID_NO_CONNECT				0x3FF
#define SWD_TEMP_SS_NO_CONNECT			0xFF

// Please Note That We Keep Distance iIn 24 Bits Variable   //
// So Do Not Define DISTANCE_MAX_KEEP more than	16,777,215  //
//#define DISTANCE_MAX_KEEP				15000000  // 15,000Km (15,000,000 meters).
#define DISTANCE_MAX_KEEP				6000000  // 6,000Km (6,000,000 meters).
#define LIMIT_TRIP_DISTANCE				DISTANCE_MAX_KEEP

extern int16_t  ibuild_day, ibuild_month, ibuild_year;
extern uint8_t	uptime_dd, uptime_hh, uptime_mm, uptime_ss;

#define APT_STARTUP_INPROGRESS		0
#define APT_STARTUP_FOR_QC_MODE		1
#define APT_STARTUP_NORMAL			2

extern uint8_t	apt_startup_mode;
extern bool		battery_isOperating;
extern freertos_twi_if TWI1_hBus;
extern bool FRAM_TestPassed;
extern bool WDOG_TestPassed;
extern uint32_t	printTraceOn;
extern uint8_t all_task_created_nb;
extern uint32_t volatile wd_monitor[32];
extern xTaskHandle tassk_handle[32];
extern bool bLivingInIdleMode;


#define IS_A_GOOD_DATE(_day,_month,_year)			((_day)&&(_day<32)&&(_month)&&(_month<13)&&(_year)&&(_year>=ibuild_year))
#define IS_A_GOOD_LOC(_lat,_lon,_sat)				((abs(_lat)>0.1)&&(abs(_lon)>0.1)&&(_sat))

#define UP_COUNTER_OVERFLOW_LIMIT(_cnt,_cond,_max)	_cnt = _cond ? (_cnt>=_max) ? _cnt :_cnt + 1 : 0
#define UP_COUNTER_OVERFLOW_ZERO(_cnt,_cond,_max)	_cnt = _cond ? (_cnt>=_max) ? 0 :_cnt + 1 : 0
#define DOWN_COUNTER_ZERO_LIMIT(_cnt,_cond,_init)	_cnt = _cond ? (_cnt > 0) ? _cnt - 1 : 0 : _init


#define AUTO_TRACEON_CLOSE_TIME			_1mSecTick_Second(300)// 5 minute

typedef struct _ARC_BOOTLD_INIT_T_ {
	enum {
		FW_LOCAL,
		FW_DOTA,
	}fw_source;
	bool verifyOK;
	uint16_t pre_version;
	char sms_resp_numb[62];
}arc_bootld_init_t;

/* Struct to hold the accelerometer sensor data */
typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} lis3dhData_t;

//APT_TRACKING_LEGACY_EVENT_CODE
#define	DRIVING_LECACY_EVCODE 			0x10 /*GEAR-D*/
#define	RAPIDTURN_LECACY_EVCODE 		0x14
#define	ACCELERATE_LECACY_EVCODE		0x16
#define	DEACCELERATE_LECACY_EVCODE		0x17
#define	DINPUT_LECACY_EVCODE			0x20 /*ELECTRIC INPUT*/
#define	OVERSPEED_LECACY_EVCODE			0x30
#define	EXIT_OVERSPEED_LECACY_EVCODE	0x31
#define	PARKING_LECACY_EVCODE			0x40 /*GEAR-P*/
#define	IDLING_LECACY_EVCODE			0x41 /*GEAR-N*/
#define	IGOFF_LECACY_EVCODE				0x50
#define	ANTGPS_DISCON_LECACY_EVCODE		0x60
#define	ANTGPS_CONNT_LECACY_EVCODE		0x70
#define	ANTGPS_SHORT_LECACY_EVCODE		0x80
#define	STARTUP_LECACY_EVCODE			0x90
#define	CMDRSP_LECACY_EVCODE			0xA0
#define	TRIG2SEND_LECACY_EVCODE			0xA1
#define	GEOFENCE_LECACY_EVCODE			0xB0
#define GSENS_FW_LECACY_EVCODE			0xC0
#define GSENS_RV_LECACY_EVCODE			0xC1
#define	GSENS_HIT_FR_LECACY_EVCODE		0xC2
#define	GSENS_HIT_BK_LECACY_EVCODE		0xC3
#define	EXTEN_LECACY_EVCODE				0xD0
#define	TPMS_LECACY_EVCODE				0xD1
#define	ACK_LECACY_EVCODE				0xD2
#define	RESET_LECACY_EVCODE				0xD3

#endif /* MISC_CONFIG_H_ */
