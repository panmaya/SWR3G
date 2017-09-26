/*
 * gps_live.h
 *
 * Created: 2016-10-21 2:38:40 PM
 *  Author: Panmaya
 */ 


#ifndef APT_GPS_DATA_WAKEUP_H_
#define APT_GPS_DATA_WAKEUP_H_

extern gps_pvtInfo_t	aptGpsWakeup_PvtRawInfo;

void aptGpsWakeup_task(void * pvParameters);

#endif /* APT_GPS_DATA_WAKEUP_H_ */