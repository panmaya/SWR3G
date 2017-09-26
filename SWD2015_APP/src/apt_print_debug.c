/*
 * print_debug.c
 *
 * Created: 2016-10-21 3:07:12 PM
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
#include "apt_debug_port.h"

void aptPrintDbg_task(void * pvParameters) {
	static bool sync_success = FALSE;
	static uint32_t * pWdCount = NULL;

	pWdCount = (uint32_t*)pvParameters;
	
	for (;;) {
		
		*pWdCount = 0;//Clear watchdog
		
		if(!sync_success) {
			sync_success = aptTracking_CCLK_SyncTime();
			if(sync_success)vPuts("\r\nSync RTC\r\n");
		}
		
		//arc_atcmd_exe();
		
		vPrint();
		
		vTaskDelay(1);
	}
}