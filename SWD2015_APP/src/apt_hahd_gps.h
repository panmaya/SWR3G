/*
 * hdha_gps.h
 *
 *  Created on: Apr 8, 2015
 *      Author: XPMUser
 */

#ifndef HDHA_GPS_H_
#define HDHA_GPS_H_

//#define HDHA_GPS_DEUBG
//#define HDHA_GPS_TMR					2               // 1 : 100ms
#define HDHA_GPS_TMR				10       // 1 : 100ms
#define HDHA_GPS_ARR				5
#define HDHA_GPS_FULTER_SPEED_HIGH  4
#define HDHA_GPS_RULE_DELAY_1_SEC   ((10/HDHA_GPS_TMR) * 1)
#define HDHA_GPS_RULE_HDHA_CLEAR	(3 * HDHA_GPS_RULE_DELAY_1_SEC)
#define HDHA_GPS_RULE_SAT_USE		5
#define HDHA_GPS_RULE_HDOP_USE		3
#define HDHA_GPS_RULE_HDOP_COUNT	(20 * HDHA_GPS_RULE_DELAY_1_SEC)
#define HDHA_GPS_RULE_ANGLE_USE		15
#define HDHA_GPS_RULE_DATA_LOSS		(HDHA_GPS_RULE_DELAY_1_SEC * 3)


s8 aptHdhaGps_state(bool gps_true,float gps_speed,float gps_hdop,u8 gps_sat,float gps_angle,int g_ha,int g_hd,s8 sec);
s8 aptHdhaGps_speed_buffer(float *speed_before,float *speed_after);
//sec data
s8 aptHahdGps_sec_speed_update(void);//Pan
int aptHahdGps_sec_speed_get(void);//Pan

void aptHahdGps_task(void * pvParameters);

#endif /* HDHA_GPS_H_ */
