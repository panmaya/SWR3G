/*
 * hdha_ode.c
 *
 * Created: 4/11/2558 11:32:52
 *  Author: Pan
 */ 

#include "adl_global.h"
#include "hdha_odo.h"

double hdha_odo_calibrate(uint16_t pto_rpm, double current_speed, uint16_t oversp_cfg) {
	double hdhacal_value;
	//int int_current_speed = 0;
	double counter_value;
	static int int_last_speed1 = 0, int_last_speed2 = 0;

	//int_current_speed = (int)current_speed;
	
	if ((abs(int_last_speed1 - current_speed)<3)&&(abs(int_last_speed2-int_last_speed1)<3)
	&&((current_speed + 0) > (55) )&&((int_last_speed1 + 0) > (55) )
	&&(pto_rpm>10) && (current_speed >= 20)&& (current_speed < 160) && (oversp_cfg > 20) ) {
		counter_value = pto_rpm;
		hdhacal_value = (double) current_speed/counter_value;
		return hdhacal_value;
	}
	
	int_last_speed2 = int_last_speed1;
	int_last_speed1 = current_speed;
	
	
	return 0;
}