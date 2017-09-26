/*
 * apt_gps_data.h
 *
 * Created: 26/3/2558 17:51:25
 *  Author: pan
 */ 

#include "xm0110_compat.h"

#ifndef APT_GPS_DATA_H_
#define APT_GPS_DATA_H_

void aptGpsData_task(void * pvParameters);
bool aptGpsData_getError(void);
uint16_t aptGpsData_getInfo(gps_pvtInfo_t *);



#endif /* APT_GPS_DATA_H_ */