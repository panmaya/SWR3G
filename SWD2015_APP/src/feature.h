/*
 * feature_tbl.h
 *
 *  Created on: May 20, 2013
 *      Author: PAN
 */

#ifndef FEATURE_TBL_H_
#define FEATURE_TBL_H_

/* SWD FEATURE */

#define	FEAT_DLAY_OVERSP			(1<< 0)
#define	FEAT_SMS_CUTSTART			(1<< 1)
#define	FEAT_OIL_SUPPORT			(1<< 2)
#define	FEAT_MSR_SUPPORT			(1<< 3)
#define	FEAT_RPM_BY_PTO2			(1<< 4)
#define	FEAT_AT_SEC					(1<< 5)
#define	FEAT_AT_RFID				(1<< 6)
#define	FEAT_AT_CARD				(1<< 7)
#define	FEAT_AT_TEMP				(1<< 8)
#define	FEAT_TEMP_4_SENSOR			(1<< 9)
#define	FEAT_HAHD_ODO				(1<<10)
#define	FEAT_HAHD_GPS				(1<<11)
#define	FEAT_PTO1_EVENT				(1<<12)
#define	FEAT_PTO2_EVENT				(1<<13)
#define	FEAT_PTO3_EVENT				(1<<14)
#define	FEAT_EXT_BUZZER				(1<<15)
#define	FEAT_NO_IDLE				(1<<16)
#define	FEAT_DISTANCE_NO_IG			(1<<17)
#define	FEAT_OPT_LIGHT_BY_PTO2		(1<<18)
#define	FEAT_OPT_LIGHT_BY_RPM		(1<<19)
#define	FEAT_GPS_HIBERNATED			(1<<20)
#define	FEAT_GEOFENCE_SOUND			(1<<21)

#define FEATURE_STANDARD_50X								FEAT_DLAY_OVERSP			|FEAT_SMS_CUTSTART			|FEAT_OIL_SUPPORT			|\
															FEAT_MSR_SUPPORT			|0/*FEAT_RPM_BY_PTO2*/		|FEAT_AT_SEC				|\
															FEAT_AT_RFID				|FEAT_AT_CARD				|FEAT_AT_TEMP				|\
															0/*FEAT_TEMP_4_SENSOR*/		|0/*FEAT_HAHD_ODO*/			|FEAT_HAHD_GPS				|\
															FEAT_PTO1_EVENT				|FEAT_PTO2_EVENT			|0/*FEAT_PTO3_EVENT*/			|\
															FEAT_EXT_BUZZER				|0/*FEAT_NO_IDLE*/			|0/*FEAT_DISTANCE_NO_IG*/		|\
															FEAT_OPT_LIGHT_BY_PTO2		|0/*FEAT_OPT_LIGHT_BY_RPM*/	|0/*FEAT_GPS_HIBERNATED*/		|0/*FEAT_GEOFENCE_SOUND*/
									
#define FEATURE_MOVELESS_MACHINE_NO_IDLE_52X				FEAT_DLAY_OVERSP			|FEAT_SMS_CUTSTART			|FEAT_OIL_SUPPORT			|\
															FEAT_MSR_SUPPORT			|FEAT_RPM_BY_PTO2			|FEAT_AT_SEC				|\
															FEAT_AT_RFID				|FEAT_AT_CARD				|FEAT_AT_TEMP				|\
															0/*FEAT_TEMP_4_SENSOR*/		|0/*FEAT_HAHD_ODO*/			|FEAT_HAHD_GPS				|\
															FEAT_PTO1_EVENT				|0/*FEAT_PTO2_EVENT*/			|0/*FEAT_PTO3_EVENT*/			|\
															FEAT_EXT_BUZZER				|FEAT_NO_IDLE				|0/*FEAT_DISTANCE_NO_IG*/		|\
															0/*FEAT_OPT_LIGHT_BY_PTO2*/	|FEAT_OPT_LIGHT_BY_RPM		|0/*FEAT_GPS_HIBERNATED*/		|0/*FEAT_GEOFENCE_SOUND*/
															
#define	FEATURE_HAHD_ODO_56X								FEAT_DLAY_OVERSP			|FEAT_SMS_CUTSTART			|FEAT_OIL_SUPPORT			|\
															FEAT_MSR_SUPPORT			|0/*FEAT_RPM_BY_PTO2*/		|FEAT_AT_SEC				|\
															FEAT_AT_RFID				|FEAT_AT_CARD				|FEAT_AT_TEMP				|\
															0/*FEAT_TEMP_4_SENSOR*/		|FEAT_HAHD_ODO				|0/*FEAT_HAHD_GPS*/			|\
															FEAT_PTO1_EVENT				|0/*FEAT_PTO2_EVENT*/			|0/*FEAT_PTO3_EVENT*/			|\
															FEAT_EXT_BUZZER				|0/*FEAT_NO_IDLE*/			|0/*FEAT_DISTANCE_NO_IG*/		|\
															0/*FEAT_OPT_LIGHT_BY_PTO2*/	|0/*FEAT_OPT_LIGHT_BY_RPM*/	|0/*FEAT_GPS_HIBERNATED*/		|0/*FEAT_GEOFENCE_SOUND*/
									
#define FEATURE_PTO3_A2D_READ_DOOR_SIGNAL_58X				FEAT_DLAY_OVERSP			|FEAT_SMS_CUTSTART			|0/*FEAT_OIL_SUPPORT*/			|\
															FEAT_MSR_SUPPORT			|0/*FEAT_RPM_BY_PTO2*/		|FEAT_AT_SEC				|\
															FEAT_AT_RFID				|FEAT_AT_CARD				|FEAT_AT_TEMP				|\
															0/*FEAT_TEMP_4_SENSOR*/		|0/*FEAT_HAHD_ODO*/			|FEAT_HAHD_GPS				|\
															FEAT_PTO1_EVENT				|FEAT_PTO2_EVENT			|FEAT_PTO3_EVENT			|\
															FEAT_EXT_BUZZER				|0/*FEAT_NO_IDLE*/			|0/*FEAT_DISTANCE_NO_IG*/		|\
															FEAT_OPT_LIGHT_BY_PTO2		|0/*FEAT_OPT_LIGHT_BY_RPM*/	|0/*FEAT_GPS_HIBERNATED*/		|0/*FEAT_GEOFENCE_SOUND*/
									
#define FEATURE_USING_4_HEAT_SENSORS_27X					FEAT_DLAY_OVERSP			|FEAT_SMS_CUTSTART			|FEAT_OIL_SUPPORT			|\
															FEAT_MSR_SUPPORT			|0/*FEAT_RPM_BY_PTO2*/		|FEAT_AT_SEC				|\
															FEAT_AT_RFID				|FEAT_AT_CARD				|FEAT_AT_TEMP				|\
															FEAT_TEMP_4_SENSOR			|0/*FEAT_HAHD_ODO*/			|FEAT_HAHD_GPS				|\
															FEAT_PTO1_EVENT				|FEAT_PTO2_EVENT			|0/*FEAT_PTO3_EVENT*/			|\
															FEAT_EXT_BUZZER				|0/*FEAT_NO_IDLE*/			|0/*FEAT_DISTANCE_NO_IG*/		|\
															FEAT_OPT_LIGHT_BY_PTO2		|0/*FEAT_OPT_LIGHT_BY_RPM*/	|0/*FEAT_GPS_HIBERNATED*/		|0/*FEAT_GEOFENCE_SOUND*/
									
#define FEATURE_NEVER_USE_IG_FOR_CAL_DISTANCE_24X			FEAT_DLAY_OVERSP			|FEAT_SMS_CUTSTART			|FEAT_OIL_SUPPORT			|\
															FEAT_MSR_SUPPORT			|0/*FEAT_RPM_BY_PTO2*/		|FEAT_AT_SEC				|\
															FEAT_AT_RFID				|FEAT_AT_CARD				|FEAT_AT_TEMP				|\
															0/*FEAT_TEMP_4_SENSOR*/		|0/*FEAT_HAHD_ODO*/			|FEAT_HAHD_GPS				|\
															FEAT_PTO1_EVENT				|FEAT_PTO2_EVENT			|0/*FEAT_PTO3_EVENT*/			|\
															FEAT_EXT_BUZZER				|0/*FEAT_NO_IDLE*/			|FEAT_DISTANCE_NO_IG		|\
															FEAT_OPT_LIGHT_BY_PTO2		|0/*FEAT_OPT_LIGHT_BY_RPM*/	|0/*FEAT_GPS_HIBERNATED*/		|0/*FEAT_GEOFENCE_SOUND*/
															
#define FEAT_BIKE_TRACKER									FEAT_DLAY_OVERSP			|FEAT_SMS_CUTSTART			|FEAT_OIL_SUPPORT			|\
															FEAT_MSR_SUPPORT			|0/*FEAT_RPM_BY_PTO2*/		|FEAT_AT_SEC				|\
															FEAT_AT_RFID				|FEAT_AT_CARD				|FEAT_AT_TEMP				|\
															0/*FEAT_TEMP_4_SENSOR*/		|0/*FEAT_HAHD_ODO*/			|FEAT_HAHD_GPS				|\
															FEAT_PTO1_EVENT				|FEAT_PTO2_EVENT			|0/*FEAT_PTO3_EVENT*/			|\
															FEAT_EXT_BUZZER				|0/*FEAT_NO_IDLE*/			|0/*FEAT_DISTANCE_NO_IG*/		|\
															FEAT_OPT_LIGHT_BY_PTO2		|0/*FEAT_OPT_LIGHT_BY_RPM*/	|FEAT_GPS_HIBERNATED		|0/*FEAT_GEOFENCE_SOUND*/
															
#define FEATURE_GEOFENCE_SOUND_4SCG							FEAT_DLAY_OVERSP			|FEAT_SMS_CUTSTART			|FEAT_OIL_SUPPORT			|\
															FEAT_MSR_SUPPORT			|0/*FEAT_RPM_BY_PTO2*/		|FEAT_AT_SEC				|\
															FEAT_AT_RFID				|FEAT_AT_CARD				|FEAT_AT_TEMP				|\
															0/*FEAT_TEMP_4_SENSOR*/		|0/*FEAT_HAHD_ODO*/			|FEAT_HAHD_GPS				|\
															FEAT_PTO1_EVENT				|FEAT_PTO2_EVENT			|0/*FEAT_PTO3_EVENT*/			|\
															FEAT_EXT_BUZZER				|0/*FEAT_NO_IDLE*/			|0/*FEAT_DISTANCE_NO_IG*/		|\
															FEAT_OPT_LIGHT_BY_PTO2		|0/*FEAT_OPT_LIGHT_BY_RPM*/	|0/*FEAT_GPS_HIBERNATED*/		|FEAT_GEOFENCE_SOUND


bool swd_feature_isEnable(uint64_t feature);
void swd_feature_Init(ascii * fwname);

#endif /* FEATURE_TBL_H_ */
