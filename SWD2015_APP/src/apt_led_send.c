/*
 * led_tcpsend.c
 *
 * Created: 2016-10-21 2:50:53 PM
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
#include "swd_io.h"

/* The Mutex instance used for access control */
static xSemaphoreHandle light_hSem = NULL;

typedef struct LIGHT_LED_T {
	enum {
		LIGHT_LED_NONE,
		LIGHT_LED_SEND_SET,
		LIGHT_LED_SEND_CLRBUF,
		LIGHT_LED_SEND_IGOFF,
		LIGHT_LED_SEND_SENDING,
		LIGHT_LED_SEND_SERVER,
	}theme;
	uint16_t rhythm;
}light_led_t;


static light_led_t light_led;


void aptLedSend_task(void * pvParameters) {
	
	static uint32_t * pWdCount = NULL;
	
	pWdCount = (uint32_t*)pvParameters;

	light_hSem = xSemaphoreCreateMutex();
	
	if(!light_hSem) {
		vPutsTrace(0,"Led Task Sem Error");
		vTaskSuspend(NULL);
	}
	adl_ioDefs_t iodef[1] = {(ADL_IO_GPIO|ADL_IO_DIR_OUT|ADL_IO_LEV_HIGH|LED_SEND)};
	s32 handle = adl_ioSubscribe( 1, iodef, 0, 0, 0);

	for(;;) {
		
		*pWdCount = 0;//Clear watchdog
		
		if(apt_startup_mode == APT_STARTUP_FOR_QC_MODE) {
			vTaskDelay(100);
			continue;
		}
		else {
			/****  LED  :  IG   *****/
			swdio_light_ig_set(swdio_get_ig_status());
			/****  LED  :  PTO  *****/
			swdio_light_pto_set(swdio_get_pto1_status());
		
			switch(light_led.theme){
				case LIGHT_LED_NONE:
				vTaskDelay(100);
				break;
				case LIGHT_LED_SEND_SET:
				adl_ioWriteSingle (handle, iodef ,light_led.rhythm);
				light_led.theme = LIGHT_LED_NONE;
				xSemaphoreGive( light_hSem );
				break;
				case LIGHT_LED_SEND_CLRBUF:
				for(u8 iCount=1;iCount<=5;iCount++) {
					adl_ioWriteSingle (handle, iodef ,TRUE);
					vTaskDelay(90);
					adl_ioWriteSingle (handle, iodef ,FALSE);
					vTaskDelay(90);
				}
				light_led.theme = LIGHT_LED_NONE;
				xSemaphoreGive( light_hSem );
				break;
				case LIGHT_LED_SEND_IGOFF:
				for(u8 iCount=1;iCount<=30;iCount++) {
					adl_ioWriteSingle (handle, iodef ,TRUE);
					vTaskDelay(36);
					adl_ioWriteSingle (handle, iodef ,FALSE);
					vTaskDelay(36);
				}
				light_led.theme = LIGHT_LED_NONE;
				xSemaphoreGive( light_hSem );
				break;
				case LIGHT_LED_SEND_SENDING:
				for(u8 iCount=1;iCount<=3;iCount++) {
					adl_ioWriteSingle (handle, iodef ,TRUE);
					vTaskDelay(200);
					adl_ioWriteSingle (handle, iodef ,FALSE);
					vTaskDelay(200);
				}
				light_led.theme = LIGHT_LED_NONE;
				xSemaphoreGive( light_hSem );
				break;
				case LIGHT_LED_SEND_SERVER:
				if(light_led.rhythm==1) {
					adl_ioWriteSingle (handle, iodef ,TRUE);
					vTaskDelay(1500);
					adl_ioWriteSingle (handle, iodef ,FALSE);
					vTaskDelay(1000);
				}
				else {
					adl_ioWriteSingle (handle, iodef ,TRUE);
					vTaskDelay(200);
					adl_ioWriteSingle (handle, iodef ,FALSE);
					vTaskDelay(600);
					adl_ioWriteSingle (handle, iodef ,TRUE);
					vTaskDelay(200);
					adl_ioWriteSingle (handle, iodef ,FALSE);
					vTaskDelay(1000);
				}
				light_led.theme = LIGHT_LED_NONE;
				xSemaphoreGive( light_hSem );
				break;
				default:
				break;
			}
		}
	}
}



void aptLedSend_set(bool status) {
	if(!light_hSem) return;
	if( xSemaphoreTake( light_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		light_led.rhythm = 0;
		light_led.theme = LIGHT_LED_SEND_SET;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
	}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.
}

void aptLedSend_igoff(void) {
	if(!light_hSem) return;
	if( xSemaphoreTake( light_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		light_led.rhythm = 0;
		light_led.theme = LIGHT_LED_SEND_IGOFF;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
	}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.
}

void aptLedSend_sending(void) {
	if(!light_hSem) return;
	if( xSemaphoreTake( light_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		light_led.rhythm = 0;
		light_led.theme = LIGHT_LED_SEND_SENDING;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
	}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.
}

void aptLedSend_server (bool status) {
	if(!light_hSem) return;
	if( xSemaphoreTake( light_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		light_led.rhythm = status;
		light_led.theme = LIGHT_LED_SEND_SERVER;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
	}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.
}

void aptLedSend_clrbuffer(void) {
	if(!light_hSem) return;
	if( xSemaphoreTake( light_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		light_led.rhythm = 0;
		light_led.theme = LIGHT_LED_SEND_CLRBUF;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
	}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.
}
