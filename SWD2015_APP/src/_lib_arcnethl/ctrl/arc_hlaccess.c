/*
 * arc_hlaccess.c
 *
 * Created: 2016-10-27 2:04:22 PM
 *  Author: Panmaya
 */ 
#include <adl_global.h>
#include "arc_hlaccess.h"

/* The Mutex instance used for access control */
static xSemaphoreHandle ARC_HLCtrl_Mutex = NULL;

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Task                                       */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
void arc_hlCtrl_AccessTask(void *pvParameters) {
	arc_hlctrl_para_t * para = pvParameters;
	bool bKeepTaskRunning = TRUE;
	
	if(!ARC_HLCtrl_Mutex) ARC_HLCtrl_Mutex = xSemaphoreCreateMutex();
	if(!ARC_HLCtrl_Mutex) {
		vPuts("\r\nerror !! hlCtrl mutex create failed \r\n");
		configASSERT(false);
	}

	for(;;) {
		
		if(para->wd) *para->wd = 0;//Clear watchdog
		
		bKeepTaskRunning = TRUE;

		if( xSemaphoreTake( ARC_HLCtrl_Mutex, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ){
			
			bKeepTaskRunning = para->func(para->id);
			
			xSemaphoreGive( ARC_HLCtrl_Mutex );
			
		}
			
		if(!bKeepTaskRunning) {
			vTaskDelete(NULL);
			para->delc(para->id);
		}
			
		vTaskDelay(20);
	}
}