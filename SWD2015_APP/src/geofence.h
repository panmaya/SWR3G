/*
 * geofence.h
 *
 *  Created on: Jun 28, 2016
 *      Author: Panmaya
 */



#ifndef GEOFENCE_H_
#define GEOFENCE_H_


#define GEOFENCE_MAX_TABLE_ID		120

typedef struct GEOFENCE_T {
	u16 geo_id;
	u8 idx_point;
	float lat;
	float lon;
	u8 radius_meter;
	u8 geo_type;
	u8 alarm_type;
	u8 data_cond;
	u8 data_value;
	u8 data_cond_ex;
	u8 data_value_ex;
	u8 action_sound;
	u8 sound_cond;
	u8 speed_cond;
	u8 speed_value_before_warning;
	u8 speed_value_exceed_warninig;
	u8 ovs_alert_send_time;
}geofence_t;

s8 geofence_decode_raw_config( const char *buffer, u16 size, geofence_t dst[GEOFENCE_MAX_TABLE_ID] , u8 *version);

#endif
