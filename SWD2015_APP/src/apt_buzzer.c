/*
 * sound_buzzer.c
 *
 * Created: 2016-10-21 2:31:10 PM
 *  Author: Panmaya
 */ 
#include "adl_global.h"
#include "misc_config.h"
#include "apt_tracking.h"
#include "swd_io.h"
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


/* The Mutex instance used for access control */
static xSemaphoreHandle sound_hSem = NULL;

typedef struct APT_BUZZER_T {
	enum {
		APT_BUZZER_NONE,
		APT_BUZZER_CLRBUF,
		APT_BUZZER_ERROR,
		APT_BUZZER_ALERT,
		APT_BUZZER_OVER,
		APT_BUZZER_QC,
	}theme;
	uint16_t rhythm;
}sound_buzzer_t;

static sound_buzzer_t sound_buzzer;
static s32	hBuzzer = -1;

static void Buzzer_Handler(s32 audioHandle, adl_audioEvents_e Event) {}

void aptBuzzer_task(void * pvParameters) {
	
	static uint32_t * pWdCount = NULL;
		
	pWdCount = (uint32_t*)pvParameters;
	
	sound_hSem = xSemaphoreCreateMutex();
	
	if(!sound_hSem) {
		vPutsTrace(0,"Buzzer Task Sem Error");
		vTaskSuspend(NULL);
	}
	
	hBuzzer = adl_audioSubscribe ( ADL_AUDIO_BUZZER,Buzzer_Handler, ADL_AUDIO_RESOURCE_OPTION_FORBID_PREEMPTION);
	
	for (;;) {
		
		*pWdCount = 0;//Clear watchdog
		
		/****  BUZZER : *****/
		
		switch(sound_buzzer.theme) {
		case APT_BUZZER_NONE:
			vTaskDelay(100);
			break;
		case APT_BUZZER_CLRBUF:
			for(u8 iCount=1;iCount<=5;iCount++) {
				adl_audioTonePlay(hBuzzer,2700, -5, 0, 0, (4*50) );
				if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(1);
				vTaskDelay(90);
				adl_audioStop(hBuzzer);
				if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(0);
				vTaskDelay(90);
			}
			sound_buzzer.theme = APT_BUZZER_NONE;
			xSemaphoreGive( sound_hSem );
			break;
		case APT_BUZZER_ALERT:
			for(u8 iCount=1;iCount<=sound_buzzer.rhythm;iCount++) {
				adl_audioTonePlay(hBuzzer,2700, -5, 0, 0, (4*50) );
				if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(1);
				vTaskDelay(200);
				adl_audioStop(hBuzzer);
				if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(0);
				vTaskDelay(200);
			}
			sound_buzzer.theme = APT_BUZZER_NONE;
			xSemaphoreGive( sound_hSem );
			break;
		case APT_BUZZER_ERROR:
			adl_audioTonePlay(hBuzzer,2700, -5, 0, 0, (4*50) );
			if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(1);
			vTaskDelay(2000);
			adl_audioStop(hBuzzer);
			if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(0);
			sound_buzzer.theme = APT_BUZZER_NONE;
			xSemaphoreGive( sound_hSem );
			break;
		case APT_BUZZER_OVER:
			adl_audioTonePlay(hBuzzer,2700, -5, 0, 0, (4*50) );
			if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(1);
			vTaskDelay(1000);
			adl_audioStop(hBuzzer);
			if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(0);
			sound_buzzer.theme = APT_BUZZER_NONE;
			xSemaphoreGive( sound_hSem );
			break;
		case APT_BUZZER_QC:
			for(u8 iCount=1;iCount<=3;iCount++) {
				adl_audioTonePlay(hBuzzer,2700, -5, 0, 0, (4*50) );
				vTaskDelay(90);
				adl_audioStop(hBuzzer);
				vTaskDelay(90);
			}
			sound_buzzer.theme = APT_BUZZER_NONE;
			xSemaphoreGive( sound_hSem );
		break;
		default:
			break;
		}
	}
}

void aptBuzzer_alert (u8 loop) {
	if(!sound_hSem) return;
	if( xSemaphoreTake( sound_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		sound_buzzer.rhythm = loop;
		sound_buzzer.theme = APT_BUZZER_ALERT;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
	}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.
}

void aptBuzzer_qc(void) {
	if(!sound_hSem) return;
	if( xSemaphoreTake( sound_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		sound_buzzer.rhythm = 0;
		sound_buzzer.theme = APT_BUZZER_QC;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
		}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.

	}

void aptBuzzer_clrbuff(void) {
	if(!sound_hSem) return;
	if( xSemaphoreTake( sound_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		sound_buzzer.rhythm = 0;
		sound_buzzer.theme = APT_BUZZER_CLRBUF;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
	}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.

}

void aptBuzzer_error(void) {
	if(!sound_hSem) return;
	if( xSemaphoreTake( sound_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		sound_buzzer.rhythm = 0;
		sound_buzzer.theme = APT_BUZZER_ERROR;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
	}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.
}

void aptBuzzer_ovspeed(void) {
	if(!sound_hSem) return;
	if( xSemaphoreTake( sound_hSem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		// We were able to obtain the semaphore and can now access the shared resource.
		// ...
		sound_buzzer.rhythm = 0;
		sound_buzzer.theme = APT_BUZZER_OVER;
		// We have finished accessing the shared resource.  Release the semaphore.
		//xSemaphoreGive( sound_hSem );
	}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.
}