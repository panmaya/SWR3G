/*
 * gps_live.c
 *
 * Created: 2016-10-21 2:38:21 PM
 *  Author: Panmaya
 */ 
#include "adl_global.h"
#include "misc_config.h"
#include "apt_tracking.h"
#include "ini_config.h"
#include "buffer.h"
#include "trans_form.h"
#include "distance_calc.h"
#include "apt_gateway.h"
#include "feature.h"
#include "swd_event.h"
#include "_lib_gps/nmea_gps.h"
#include "_lib_gps/nmea.h"
#include "apt_swd.h"
#include "apt_gps_data.h"
#include "vprinter.h"

/*extern*/  gps_pvtInfo_t	aptGpsWakeup_PvtRawInfo;
static bool APT_GPS_WAKEUP_PvtAliving = false;


static void gps_wakeup_50ms(void) {
	static bool  gps_is_death = false;
	static uint16_t wakeup_timing_delay = 0;
	static u16		iCountPVT_NoResponse = 0;
		
	UP_COUNTER_OVERFLOW_LIMIT(iCountPVT_NoResponse,(aptTracking_RuntimeGps_Update(&aptGpsWakeup_PvtRawInfo)==FALSE),65535);
		
	//  GPS no response
	if(iCountPVT_NoResponse >=_100mSecTick_Second(20)) { // 10 seconds
		if(APT_GPS_WAKEUP_PvtAliving)vPutsTrace(30,"GPS un-update!!");
		APT_GPS_WAKEUP_PvtAliving = FALSE;
	}else APT_GPS_WAKEUP_PvtAliving = TRUE;
	
	/*GPS Monitor*/
	if(gps_is_death==false) {
		if(wakeup_timing_delay) wakeup_timing_delay--;
		else if((aptGpsData_getError())||(!hw_ctrl_gps_Wakeup())) {
			if(aptGpsData_getError())vPutsTrace(30,"GPS data err!!");
			if(!hw_ctrl_gps_Wakeup())vPutsTrace(30,"GPS not wakeup!!");
			hw_ctrl_gps_Reset(true);// Insert GPS reset pin
			hw_ctrl_gps_Enable(false);// Disable GPS Power
			hw_ctrl_gps_AntPower(false);// Disable GPS Antenna Power
			gps_is_death = true;
			wakeup_timing_delay = 0;
		}
	}
	else {
		wakeup_timing_delay++;
		if(hw_ctrl_gps_Wakeup()) {
			gps_is_death = false;
			wakeup_timing_delay = 160;//18000/*15 minute*/;
			vPutsTrace(30,"GPS wakeup OK");
			return;
		}
		
		switch(wakeup_timing_delay){
		case 60://Start Module after  3 seconds Stop
			hw_ctrl_gps_Reset(false);
			hw_ctrl_gps_Enable(true);
			hw_ctrl_gps_AntPower(true);
			ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);
			break;
		case 80://Set ON after 1 Second Module Start
		case 100://Try On after OFF 400 ms.
		case 120:
			ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_ON);
			break;
		case 92://Set OFF after ON 600ms.
		case 112:
		case 132:
			ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);
			break;
		case 160://Reset again after 1.4 seconds
			gps_is_death = false;
			vPutsTrace(30,"GPS try wakeup");
		default:
			break;
		}
	}
}

void aptGpsWakeup_task(void * pvParameters) {
	
	static adl_ioDefs_t iodef[1] = {(ADL_IO_GPIO|ADL_IO_DIR_OUT|LED_GPS)};
	static s32 handle = -1;
	static uint16_t led_delay_counter = 0;
	static bool led_is_on = false;
	
	static uint32_t * pWdCount = NULL;
		
	pWdCount = (uint32_t*)pvParameters;
	
	handle = adl_ioSubscribe( 1, iodef, 0, 0, 0);
	
	#if(0)
	ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_ON);
	*pWdCount = 0;//Clear watchdog
	vTaskDelay(500);
	*pWdCount = 0;//Clear watchdog
	ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);
	vTaskDelay(1500);
	*pWdCount = 0;//Clear watchdog
	if(!hw_ctrl_gps_Wakeup()) {
		ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_ON);
		vTaskDelay(500);
		*pWdCount = 0;//Clear watchdog
		ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);
	}else ioport_set_pin_level(LED_GPS, LED_ACTIVE_LEVEL);
	#endif
	
	for(;;){
		
		*pWdCount = 0;//Clear watchdog
		
		gps_wakeup_50ms();
		
		vTaskDelay(50);
		
		if(apt_startup_mode == APT_STARTUP_FOR_QC_MODE){continue;}
			
		led_delay_counter = led_delay_counter >= 1500 ? 1500 : led_delay_counter + 50;
		
		/*** LED : GPS ****/
		if(APT_GPS_WAKEUP_PvtAliving) {
			if((aptTracking_Runtime.GPS.fixDimnt >= GPS_FIX_3D)
			||(aptSwd_gps_fix_status() >= GPS_FIX_3D)) {
				if((led_is_on==false)&&(led_delay_counter >= 1500)){
					adl_ioWriteSingle (handle, iodef ,FALSE);//LED ON
					led_is_on = true;
					led_delay_counter = 0;
				}
			}
		}
		
		if((led_is_on==true)&&(led_delay_counter >= 1500)) {
			adl_ioWriteSingle (handle, iodef ,TRUE);//LED OFF
			led_is_on = false;
			led_delay_counter = 0;
		}
	}
}
