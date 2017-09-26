
#include "adl_global.h"
#include "apt_hahd_gps.h"
#include "apt_tracking.h"

static bool hdha_gps_update = FALSE;
static bool hdha_gps_gps_true = FALSE;
static float  hdha_gps_use_ha = 100;
static float  hdha_gps_use_hd = 100;
static float hdha_gps_data_speed[HDHA_GPS_ARR];
static float hdha_gps_data_hdop[HDHA_GPS_ARR];
static float hdha_gps_data_angle[HDHA_GPS_ARR];
static float hdha_gps_data_speed_buffer_befor = 0.0;
static float hdha_gps_data_speed_buffer_after = 0.0;
static u8 hdha_gps_data_sat[HDHA_GPS_ARR];
static u16 hdha_gps_rule_delay_check = 0;
static u16 hdha_gps_rule_hdop_check = 0;
static int hahd_gps_return = 0;
static s8 hdha_gps_rule_123_sec = 1;

static s8 sec_speed_hdha_check = 0;
static int hdha_sec_speed = 0;

static u16 hdha_gps_angle_dif(float w_A1,float w_A2,u16 w_a_dif) {
	s8 w_A1_Q = 0;
	if(w_A1 < 90)w_A1_Q = 1;
	else if(w_A1 < 180)w_A1_Q = 2;
	else if(w_A1 < 270)w_A1_Q = 3;
	else if(w_A1 < 360)w_A1_Q = 4;
	else w_A1_Q =100;
	
	if((w_A1_Q == 2) || (w_A1_Q == 3)) {
		if((w_A1 + w_a_dif) < w_A2)return 1;
		else if((w_A1 - w_a_dif) > w_A2)return 1;
		else return 0;
	}
	else {
		w_A1 += 180;
		if(w_A1 > 360)w_A1 -= 360;
		w_A2 += 180;
		if(w_A2 > 360)w_A2 -= 360;
		if((w_A1 + w_a_dif) < w_A2)return 1;
		else if((w_A1 - w_a_dif) > w_A2)return 1;
		else return 0;
	}
	return 0;
}


static void hahd_gps_set_delay(u16 w_delay) {
	if(w_delay > hdha_gps_rule_delay_check) hdha_gps_rule_delay_check = w_delay;
}


static void hdha_gps_1SecInterval(void) {
	static u8 w_data_update = 0;
	static u8 w_clear_hdha_count = 0;
	ascii w_display[100] = {""};
	
	if(vPrintTraceGet(24)) {
		snprintf(w_display,100,"UP:%d/%d,DL:%d,SP:%.1f,%.1f,%.1f,%.1f,%.1f,"
		,w_data_update,HDHA_GPS_RULE_DATA_LOSS,hdha_gps_rule_delay_check
		,hdha_gps_data_speed[4],hdha_gps_data_speed[3],hdha_gps_data_speed[2],hdha_gps_data_speed[1],hdha_gps_data_speed[0]);
		
		vPrintfTrace((24,w_display));
		
		snprintf(w_display,100,"HD:%.1f,%.1f,%.1f,%.1f,%.1f,SAT:%d,%d,%d,%d,%d,AG:%.1f,%.1f,%.1f,%.1f,%.1f"
		,hdha_gps_data_hdop[4],hdha_gps_data_hdop[3],hdha_gps_data_hdop[2],hdha_gps_data_hdop[1],hdha_gps_data_hdop[0]
		,hdha_gps_data_sat[4],hdha_gps_data_sat[3],hdha_gps_data_sat[2],hdha_gps_data_sat[1],hdha_gps_data_sat[0]
		,hdha_gps_data_angle[4],hdha_gps_data_angle[3],hdha_gps_data_angle[2],hdha_gps_data_angle[1],hdha_gps_data_angle[0]);
		
		vPrintfTrace((24,w_display));
	}
	
	if(hdha_gps_update == TRUE) {
		hdha_gps_update = FALSE;
		w_data_update = 0;
	}
	else {
      if(w_data_update >=HDHA_GPS_RULE_DATA_LOSS) w_data_update=HDHA_GPS_RULE_DATA_LOSS;
	  else{w_data_update++;}
    }
	
	if(hdha_gps_rule_delay_check>0)hdha_gps_rule_delay_check--;
	if(w_clear_hdha_count>0)w_clear_hdha_count--;
	else hahd_gps_return = 0;
	
	//RULE
	if(hdha_gps_gps_true == FALSE) { 
		hahd_gps_set_delay(HDHA_GPS_RULE_DELAY_1_SEC * 10);
		hdha_gps_rule_hdop_check = 0;
		vPrintfTrace((24,"[HDHA_GPS]:GPS FALSE[10]"));
	}
	else if(w_data_update >= HDHA_GPS_RULE_DATA_LOSS) {
		hahd_gps_set_delay(HDHA_GPS_RULE_DELAY_1_SEC * 10);
		hdha_gps_rule_hdop_check = 0;
		vPrintfTrace((24,"[HDHA_GPS]:GPS NOT UP DATE[10]"));
	}
	else if(hdha_gps_rule_delay_check > 0) {
		vPrintfTrace((24,"[HDHA_GPS]:DELAY"));
	}
	else if((hdha_gps_data_speed[0] - hdha_gps_data_speed[1])>=25) {
		//Speed Over 25km/Sec
		hahd_gps_set_delay(HDHA_GPS_RULE_DELAY_1_SEC * 10);
		hdha_gps_rule_hdop_check = 0;
		vPrintfTrace((24,"[HDHA_GPS]:Speed Over > 25:Sec[10]"));
	}
	else if((hdha_gps_data_hdop[0] >= HDHA_GPS_RULE_HDOP_USE)
	||(hdha_gps_data_hdop[1] >= HDHA_GPS_RULE_HDOP_USE)
	||(hdha_gps_data_hdop[2] >= HDHA_GPS_RULE_HDOP_USE)
	||(hdha_gps_data_hdop[3] >= HDHA_GPS_RULE_HDOP_USE)
	||(hdha_gps_data_hdop[4] >= HDHA_GPS_RULE_HDOP_USE)) {
		//HDOP
		hdha_gps_rule_hdop_check = 0;
		vPrintfTrace((24,"[HDHA_GPS]:GPS HDOP Error[>3]"));
    }
	else if(hdha_gps_rule_hdop_check < HDHA_GPS_RULE_HDOP_COUNT) {
		hdha_gps_rule_hdop_check++;
		if(vPrintTraceGet(24)) {
			snprintf(w_display,100,"[HDHA_GPS]:GPS HDOP[<3] Delay %d/%d",hdha_gps_rule_hdop_check,HDHA_GPS_RULE_HDOP_COUNT);
			vPrintfTrace((24,w_display));
		}
	}
	else if((hdha_gps_data_sat[0] < HDHA_GPS_RULE_SAT_USE)
	||(hdha_gps_data_sat[1] < HDHA_GPS_RULE_SAT_USE)
	||(hdha_gps_data_sat[2] < HDHA_GPS_RULE_SAT_USE)
	||(hdha_gps_data_sat[3] < HDHA_GPS_RULE_SAT_USE)
	||(hdha_gps_data_sat[4] < HDHA_GPS_RULE_SAT_USE)) {
		//SAT
		vPrintfTrace((24,"[HDHA_GPS]:GPS SAT ERROR[<5]"));
	}
	else if(hdha_gps_angle_dif(hdha_gps_data_angle[0],hdha_gps_data_angle[1],HDHA_GPS_RULE_ANGLE_USE)) {
		//Angle
		vPrintfTrace((24,"[HDHA_GPS]:GPS Angle_01 ERROR[>15]"));
	}
	else if(hdha_gps_angle_dif(hdha_gps_data_angle[1],hdha_gps_data_angle[2],HDHA_GPS_RULE_ANGLE_USE)) {
		vPrintfTrace((24,"[HDHA_GPS]:GPS Angle_12 ERROR[>15]"));
    }
    else if(hdha_gps_angle_dif(hdha_gps_data_angle[2],hdha_gps_data_angle[3],HDHA_GPS_RULE_ANGLE_USE)) {
		vPrintfTrace((24,"[HDHA_GPS]:GPS Angle_23 ERROR[>15]"));
	}
	else if(hdha_gps_angle_dif(hdha_gps_data_angle[3],hdha_gps_data_angle[4],HDHA_GPS_RULE_ANGLE_USE)) {
		vPrintfTrace((24,"[HDHA_GPS]:GPS Angle_34 ERROR[>15]"));
	}
	else if((hdha_gps_data_speed[2+(hdha_gps_rule_123_sec-1)] - hdha_gps_data_speed[1]) >= hdha_gps_use_hd) {
		//HD
		hahd_gps_set_delay(HDHA_GPS_RULE_DELAY_1_SEC * 20);
		vPrintfTrace((24,"[HDHA_GPS]:HD active"));
		//sound_onoff(2);
		hahd_gps_return = 1;
		sec_speed_hdha_check = 1;
		hdha_gps_data_speed_buffer_befor = hdha_gps_data_speed[2+(hdha_gps_rule_123_sec-1)];
		hdha_gps_data_speed_buffer_after = hdha_gps_data_speed[1];
		w_clear_hdha_count = HDHA_GPS_RULE_HDHA_CLEAR;
		
		if(vPrintTraceGet(24)) {
			snprintf(w_display,100,"[HDHA_GPS]>Sec:%d,GPS:%d,HD:%.2f,HA:%.2f,SP:%.1f,hdop:%.1f,SAT:%d,Angle:%.1f"
			,hdha_gps_rule_123_sec	,hdha_gps_gps_true	,hdha_gps_use_hd ,hdha_gps_use_ha
			,hdha_gps_data_speed[0] ,hdha_gps_data_hdop[0] ,hdha_gps_data_sat[0] ,hdha_gps_data_angle[0]);
			vPrintfTrace((24,w_display));
			vPrintfTrace((24,"[HDHA_GPS]:HD active"));
			
			snprintf(w_display,100,"UP:%d/%d,DL:%d,SP:%.1f,%.1f,%.1f,%.1f,%.1f,"
			,w_data_update,HDHA_GPS_RULE_DATA_LOSS,hdha_gps_rule_delay_check
			,hdha_gps_data_speed[4],hdha_gps_data_speed[3],hdha_gps_data_speed[2],hdha_gps_data_speed[1],hdha_gps_data_speed[0]);
			vPrintfTrace((24,w_display));
			
			snprintf(w_display,100,"HD:%.1f,%.1f,%.1f,%.1f,%.1f,SAT:%d,%d,%d,%d,%d,AG:%.1f,%.1f,%.1f,%.1f,%.1f"
			,hdha_gps_data_hdop[4],hdha_gps_data_hdop[3],hdha_gps_data_hdop[2],hdha_gps_data_hdop[1],hdha_gps_data_hdop[0]
			,hdha_gps_data_sat[4],hdha_gps_data_sat[3],hdha_gps_data_sat[2],hdha_gps_data_sat[1],hdha_gps_data_sat[0]
			,hdha_gps_data_angle[4],hdha_gps_data_angle[3],hdha_gps_data_angle[2],hdha_gps_data_angle[1],hdha_gps_data_angle[0]);
			vPrintfTrace((24,w_display));
		}
	  
    }
	else if((hdha_gps_data_speed[1] - hdha_gps_data_speed[2+(hdha_gps_rule_123_sec-1)]) >= hdha_gps_use_ha)	{
		//HA
		hahd_gps_set_delay(HDHA_GPS_RULE_DELAY_1_SEC * 20);
		vPrintfTrace((24,"[HDHA_GPS]:HA active"));
		//sound_onoff(1);
		hahd_gps_return = 2;
		sec_speed_hdha_check = 2;
		hdha_gps_data_speed_buffer_befor = hdha_gps_data_speed[2+(hdha_gps_rule_123_sec-1)];
		hdha_gps_data_speed_buffer_after = hdha_gps_data_speed[1];
		w_clear_hdha_count = HDHA_GPS_RULE_HDHA_CLEAR;
		
		if(vPrintTraceGet(24)) {
			snprintf(w_display,100,"[HDHA_GPS]>Sec:%d,GPS:%d,HD:%.2f,HA:%.2f,SP:%.1f,hdop:%.1f,SAT:%d,Angle:%.1f"
			,hdha_gps_rule_123_sec	,hdha_gps_gps_true	,hdha_gps_use_hd	,hdha_gps_use_ha
			,hdha_gps_data_speed[0]	,hdha_gps_data_hdop[0]	,hdha_gps_data_sat[0]	,hdha_gps_data_angle[0]);
			vPrintfTrace((24,w_display));
			vPrintfTrace((24, "[HDHA_GPS]:HA active"));
		
			snprintf(w_display,100,"UP:%d/%d,DL:%d,SP:%.1f,%.1f,%.1f,%.1f,%.1f,"
			,w_data_update,HDHA_GPS_RULE_DATA_LOSS,hdha_gps_rule_delay_check
			,hdha_gps_data_speed[4],hdha_gps_data_speed[3],hdha_gps_data_speed[2],hdha_gps_data_speed[1],hdha_gps_data_speed[0]);
			vPrintfTrace((24,w_display));
		
			snprintf(w_display,100,"HD:%.1f,%.1f,%.1f,%.1f,%.1f,SAT:%d,%d,%d,%d,%d,AG:%.1f,%.1f,%.1f,%.1f,%.1f"
			,hdha_gps_data_hdop[4],hdha_gps_data_hdop[3],hdha_gps_data_hdop[2],hdha_gps_data_hdop[1],hdha_gps_data_hdop[0]
			,hdha_gps_data_sat[4],hdha_gps_data_sat[3],hdha_gps_data_sat[2],hdha_gps_data_sat[1],hdha_gps_data_sat[0]
			,hdha_gps_data_angle[4],hdha_gps_data_angle[3],hdha_gps_data_angle[2],hdha_gps_data_angle[1],hdha_gps_data_angle[0]);
			vPrintfTrace((24,w_display));
		}
    }
}

static s8 hdha_gps_sec_speed_status(int *sec_speed) {
	static int w_speed_old = 0;
	
	vPrintfTrace((24,"[SEC] Speed Oid=%d New=%d HD=%d HA=%d",
	w_speed_old,(int)hdha_gps_data_speed[1],(int)hdha_gps_use_hd,(int)hdha_gps_use_ha));
	
	switch(sec_speed_hdha_check) {
		case 1:
		if((w_speed_old - hdha_gps_data_speed[1]) >= hdha_gps_use_hd) *sec_speed = hdha_gps_data_speed[1];
		else *sec_speed = w_speed_old - (hdha_gps_use_hd);
		sec_speed_hdha_check = 0;
		vPrintfTrace((24,"[SEC] Speed HD= %d",*sec_speed));
		break;
		case 2:
		if((hdha_gps_data_speed[1] - w_speed_old) >= hdha_gps_use_ha) *sec_speed = hdha_gps_data_speed[1];
		else *sec_speed = w_speed_old + (hdha_gps_use_ha);
		sec_speed_hdha_check = 0;
		vPrintfTrace((24,"[SEC] Speed HA= %d",*sec_speed));
		break;
		default:
		if((hdha_gps_data_speed[1] - w_speed_old) >= (hdha_gps_use_ha)) {
			*sec_speed = w_speed_old + (hdha_gps_use_ha-1) ;
			vPrintfTrace((24,"[SEC] Speed up= %d",*sec_speed));
		}
		else if((w_speed_old - hdha_gps_data_speed[1]) >= hdha_gps_use_hd) {
			*sec_speed = w_speed_old - (hdha_gps_use_hd-1);
			vPrintfTrace((24,"[SEC] Speed down= %d",*sec_speed));
		}
		else {
			*sec_speed = hdha_gps_data_speed[1];
			vPrintfTrace((24,"[SEC] Speed OK= %d",*sec_speed));
		}
		break;
	}
	w_speed_old = *sec_speed;
	return OK;
}

s8 aptHdhaGps_state(bool gps_true,float gps_speed,float gps_hdop,u8 gps_sat,float gps_angle,int g_ha,int g_hd,s8 sec) {
	u8 w_i;
	ascii w_display[100];
	hdha_gps_gps_true = gps_true;
	hdha_gps_use_ha = (float)g_ha;
	hdha_gps_use_hd = (float)g_hd;
	
	for(w_i = (HDHA_GPS_ARR-1) ; w_i > 0 ; w_i--) {
		hdha_gps_data_speed[w_i] = hdha_gps_data_speed[w_i-1];
		hdha_gps_data_hdop[w_i]  = hdha_gps_data_hdop[w_i-1];
		hdha_gps_data_angle[w_i] = hdha_gps_data_angle[w_i-1];
		hdha_gps_data_sat[w_i]   = hdha_gps_data_sat[w_i-1];
	}
	hdha_gps_data_speed[0] = gps_speed;
	hdha_gps_data_hdop[0]  = gps_hdop;
	hdha_gps_data_angle[0]  = gps_angle;
	hdha_gps_data_sat[0]   = gps_sat;
	hdha_gps_rule_123_sec = sec;
	hdha_gps_update = TRUE;
	
	if(vPrintTraceGet(24)) {
		snprintf(w_display,100,"[HDHA_GPS]>Sec:%d,GPS_FIG:%d,HD:%.2f,HA:%.2f,SP:%.1f,hdop:%.1f,SAT:%d,Angle:%.1f"
		,hdha_gps_rule_123_sec	,hdha_gps_gps_true	,hdha_gps_use_hd	,hdha_gps_use_ha
		,hdha_gps_data_speed[0]	,hdha_gps_data_hdop[0]	,hdha_gps_data_sat[0]	,hdha_gps_data_angle[0]);
		vPrintfTrace((24,w_display));
	}
	
	//Filter speed
	if(hdha_gps_data_speed[2]<hdha_gps_data_speed[1]) {//data speed error high
		if((hdha_gps_data_speed[1]-hdha_gps_data_speed[0])>=HDHA_GPS_FULTER_SPEED_HIGH) {
			if(vPrintTraceGet(24)) {
				snprintf(w_display,100,"[HDHA_GPS]Filter Speed : %.1f,%.1f,%.1f,%.1f,%.1f",
				hdha_gps_data_speed[4],hdha_gps_data_speed[3],hdha_gps_data_speed[2],hdha_gps_data_speed[1],hdha_gps_data_speed[0]);
				vPrintfTrace((24,w_display));
			}
			
			hdha_gps_data_speed[1] = (hdha_gps_data_speed[0]-hdha_gps_data_speed[2])/2;
			hdha_gps_data_speed[1] += hdha_gps_data_speed[2];
			
			if(vPrintTraceGet(24)) {
				snprintf(w_display,100,"[HDHA_GPS]Filter speed :High,%.1f,%.1f,%.1f,%.1f,%.1f",
				hdha_gps_data_speed[4],hdha_gps_data_speed[3],hdha_gps_data_speed[2],hdha_gps_data_speed[1],hdha_gps_data_speed[0]);
				vPrintfTrace((24,w_display));
			}
		}
	}
	else if(hdha_gps_data_speed[2]>hdha_gps_data_speed[1]) { //data speed error low
		if((hdha_gps_data_speed[0]-hdha_gps_data_speed[1])>=HDHA_GPS_FULTER_SPEED_HIGH) {
			if(vPrintTraceGet(24)) {
				snprintf(w_display,100,"[HDHA_GPS]Filter Speed : %.1f,%.1f,%.1f,%.1f,%.1f",
				hdha_gps_data_speed[4],hdha_gps_data_speed[3],hdha_gps_data_speed[2],hdha_gps_data_speed[1],hdha_gps_data_speed[0]);
				vPrintfTrace((24,w_display));
			}
			
			hdha_gps_data_speed[1] = (hdha_gps_data_speed[2]-hdha_gps_data_speed[0])/2;
			hdha_gps_data_speed[1] += hdha_gps_data_speed[0];
			
			if(vPrintTraceGet(24)) {
				snprintf(w_display,100,"[HDHA_GPS]Filter speed :Low,%.1f,%.1f,%.1f,%.1f,%.1f",
				hdha_gps_data_speed[4],hdha_gps_data_speed[3],hdha_gps_data_speed[2],hdha_gps_data_speed[1],hdha_gps_data_speed[0]);
				vPrintfTrace((24,w_display));
			}
		}
	}
	
	if(hahd_gps_return == 1) {
		hahd_gps_return = 0;
		return 1;
	}
	else if(hahd_gps_return == 2){
		hahd_gps_return = 0;
		return 2;
	}
		
	return 0;
}

s8 aptHdhaGps_speed_buffer(float *speed_before,float *speed_after) {
  *speed_before = hdha_gps_data_speed_buffer_befor;
  *speed_after = hdha_gps_data_speed_buffer_after;
  return OK;
}

int aptHahdGps_sec_speed_get(void) {
	return hdha_sec_speed;
}

s8 aptHahdGps_sec_speed_update(void) { 
	return hdha_gps_sec_speed_status(&hdha_sec_speed);
}

void aptHahdGps_task(void * pvParameters) {
	static uint32_t counter_mSec1 = 0;
	static uint32_t * pWdCount = NULL;
	
	pWdCount = (uint32_t*)pvParameters;
	
	while(!aptTracking_task_completed()){
		*pWdCount = 0;//Clear watchdog
		vTaskDelay(10);
	}
	
	for(;;) {
		
		counter_mSec1 += (*pWdCount);
		
		*pWdCount = 0;//Clear watchdog
		
		if(counter_mSec1 >= (100UL*HDHA_GPS_TMR)) {
			counter_mSec1 = 0;
			hdha_gps_1SecInterval();
		}

		vTaskDelay(100);
	}
}