/*
 * buffer.c
 *
 * Created: 2017-08-07 1:44:50 PM
 *  Author: Panmaya
 */ 
#include "adl_global.h"
#include "buffer.h"

static xSemaphoreHandle buffer_sem = NULL;
/*extern*/ uint8_t buffer_accessCentralStack[512];


void buffer_init(void) {
	
	buffer_sem = xSemaphoreCreateMutex();
		
	buffer_gpbm_create(buffer_sem);

	apt_buffer_cpe_create(buffer_sem);
		
	apt_buffer_cpl_create(buffer_sem);
	
}