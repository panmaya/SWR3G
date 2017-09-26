/*
 * apt_tracking.h
 *
 *  Created on: 29 Á.¤. 2555
 *      Author: Pan
 */

#ifndef APT_TRACKING_H_
#define APT_TRACKING_H_

#include "misc_config.h"
#include "xm0110_compat.h"

#include "buffer_gpbm.h"

typedef struct _APT_TRACKING_RUNTIME_T {
	struct {
		u8		hour;
		u8		minute;
		u8		second;
		u8		day;
		u8		month;
		u16		year;
	}tmRTC;
	
	struct {
		u16 ch1;
		u16 ch2;
	}adcIN;
	
	#if(0) //GPI format
	+------------+-----------+------+------+------+----+
	|   SEC_IO   |  RFID_IO  | PTO3 | PTO2 | PTO1 | IG |
	|------------+-----------+------+------+------+----+
	| XXXX XXXX  |   XXXX    |   X  |   X  |  X   |  X |
	+------------+-----------+------+------+------+----+
	#endif
	u16 digGPI;				/*16Inputs  Maximum*//*In16 ,In15 ,In14 ,......,In1 */
	
	u16	digGPO;				/*16Outputs Maximum*//*Out16,Out15,Out14,......,Out1*/
	
	struct {
		float	hDop;
		double	lat;
		double	lon;
		u16		speed;
		u16		course;
		u8		fixDimnt;
		u8		satUsed;
		u8		hour;
		u8		minute;
		u8		second;
		u8		day;
		u8		month;
		u16		year;
	}GPS;
	
	struct {
		u8	Event;
		u16	R_Val;
		u16	S_Val;
		s8	Trk3id;
	}Status;
	
	u32	TotalDistance;/*Meter Unit*/
	u8	TempSensor[4];
	u16 HarshSpeed;
	u16 RFID;
	u32	TripDistance; /*Meter Unit*/
	u32	TripTime; /*Meter Unit*/
	u16 IGON_Counter;
	u16 IDLE_Counter;
	u16 OVSP_Counter;
	u16 PTO_Counter;
	
	u16 MAIN_Voltage;
	u16 BATT_Voltage;
	u16 Fuel_Voltage;
	u16 RPM_Frequency;
	
	u16 MAXSpeedPerMin;
	u16 OVSPCountPerMin;
	u16 HACountPerMin;
	u16 HDCountPerMin;
	
	u16 DriveTimePerDay;
	u32 DistancePerDay;

}aptTracking_runtime_t;

#define APT_TRACKING_DRIVER_ID_LEN		32

typedef struct _APT_TRACKING_DRIVER_T {
	bool isLogin;
	char id[APT_TRACKING_DRIVER_ID_LEN + 1];
	uint32_t IDActiveMinute;
	uint32_t RoundRestMinute;
	uint32_t RoundWorkMinute;
	uint32_t TotalWorkMinute;
}aptTracking_driver_t;

extern aptTracking_runtime_t aptTracking_Runtime;
extern uint16_t aptTracking_tempsensor_timeout;
extern uint16_t aptTracking_rfid_timeout;
extern arc_bootld_init_t aptTracking_bootInfo;
extern aptTracking_driver_t aptTracking_Driver[GPBM_LICENSE_TRK3_MAX_ID];

void aptTracking_task(void * pvParameters);
bool aptTracking_task_completed(void);
void aptTracking_backward_runtime(u8 delay);
void aptTracking_save_idle_LatLon(void);
void aptTracking_load_idle_LatLon(void);
void aptTracking_print_secdata(void);
bool aptTracking_CCLK_SyncTime(void);
bool aptTracking_RuntimeGps_Update(gps_pvtInfo_t *info);

#endif /* APT_TRACKING_H_ */
