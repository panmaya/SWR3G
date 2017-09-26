/*
 * gsm_live.c
 *
 * Created: 2016-10-21 2:37:54 PM
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

extern bool aptTracking_SoundCmd_Error(void);

static void gsm_wakeup_100ms(void) {
	static bool	power_is_off = false;
	static uint16_t wakeup_timing_delay = 0;
	
	if(!ioport_get_pin_level(PIN_GSM_DWL)){
		//GSM firmware upgrade ; never reset GSM module
		wakeup_timing_delay = 0;
	}
	
	if((!ioport_get_pin_level(PIN_SIMPRESS))
	&& arc_netsys.bSimReady ){
		arc_netsys.bSimReady		= false;
		arc_netsys.bGprsAttached	= false;
		arc_netsys.bPdpActivated	= false;
		arc_netsys.b3GRegistered	= false;
		arc_netsys.bNetworkRoaming  = false;
		arc_netsys.bGsmRegistered	= false;
	}
	
	if(power_is_off) {
		wakeup_timing_delay++;
		if(wakeup_timing_delay > _100mSecTick_Second(5)) {//5 Seconds
			power_is_off = false;
			wakeup_timing_delay = 0;
			hw_ctrl_gsm_Reset(false);
		}
	}
	else 
	{
		static uint16_t noresponse_count = 0;
		static uint16_t noip_count = 0;
		static uint16_t timeout_count = 0;
		static uint16_t spond_count = 0;
		
		//if(aptTracking_SoundCmd_Error()) {
		if(0) {
			spond_count++;
			if(spond_count > _100mSecTick_Second(60)) {//1 Minutes
				power_is_off = true;
				spond_count = 0;
				vPutsTrace(30,"GSM Play Err - Restart Module");
			}
		}else spond_count = 0;
		
		if(arc_netdev_noresponse) {
			noresponse_count++;
			if(noresponse_count > _100mSecTick_Second(1*60)) {//1 Minutes
				power_is_off = true;
				noresponse_count = 0;
				vPutsTrace(30,"GSM No Response - Restart Module");
			}
		}else noresponse_count = 0;
			
		if(!arc_netsys.bPdpActivated) {
			noip_count++;
			if(noip_count > _100mSecTick_Second(2*60)) {//2 Minutes
				power_is_off = true;
				noip_count = 0;
				vPutsTrace(30,"PPP No IP - Restart Module");
			}
		}else noip_count = 0;
			
		if(aptGateway_serverTimeout()) {
			timeout_count++;
			if(timeout_count > _100mSecTick_Second(10*60)) {//10 Minutes
				power_is_off = true;
				timeout_count = 0;
				vPutsTrace(30,"TCP Timeout - Restart Module");
			}
		}else timeout_count = 0;
		
		if(power_is_off == true){
			wakeup_timing_delay = 0;
			hw_ctrl_gsm_Reset(true);
		}
	}
}


void aptGsmWakeup_task(void * pvParameters) {
	static adl_ioDefs_t iodef[1] = {(ADL_IO_GPIO|ADL_IO_DIR_OUT|LED_GSM)};
	static s32 handle = -1;
	static uint16_t led_delay_counter = 0;
	static bool led_is_on = false;
	
	static uint32_t * pWdCount = NULL;
		
	pWdCount = (uint32_t*)pvParameters;
	
	handle = adl_ioSubscribe( 1, iodef, 0, 0, 0);
	
	for(;;){
		
		*pWdCount = 0;//Clear watchdog
		
		gsm_wakeup_100ms();
		
		vTaskDelay(100);
		
		if(apt_startup_mode == APT_STARTUP_FOR_QC_MODE){
			continue;
		}
		
		led_delay_counter = led_delay_counter >= 1500 ? 1500 : led_delay_counter + 100;
		
		/*** LED : GSM ****/
		if(arc_netsys.bGsmRegistered) {
			if(arc_netsys.b3GRegistered) {
				if((led_is_on==false)&&(led_delay_counter >= 200)) {
					adl_ioWriteSingle (handle, iodef ,FALSE);// LED ON
					led_is_on = true;
					led_delay_counter = 0;
				}
				else if((led_is_on==true)&&(led_delay_counter >= 1500)) {
					adl_ioWriteSingle (handle, iodef ,TRUE);// LED OFF
					led_is_on = false;
					led_delay_counter = 0;
				}
			}
			else {
				if((led_is_on==false)&&(led_delay_counter >= 600)) {
					adl_ioWriteSingle (handle, iodef ,FALSE);// LED ON
					led_is_on = true;
					led_delay_counter = 0;
				}
				else if((led_is_on==true)&&(led_delay_counter >= 600)) {
					adl_ioWriteSingle (handle, iodef ,TRUE);// LED OFF
					led_is_on = false;
					led_delay_counter = 0;
				}
			}
		}
		else {
			adl_ioWriteSingle (handle, iodef ,TRUE);// LED OFF
			led_is_on = false;
			led_delay_counter = 0;
		}
	}
}