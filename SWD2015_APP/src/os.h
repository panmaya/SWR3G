/*
 * app_freertos.h
 *
 * Created: 26/11/2557 11:46:01
 *  Author: pan
 */ 


#ifndef APP_FREERTOS_H_
#define APP_FREERTOS_H_

void vAssertCalled( uint32_t ulLine, const char *pcFile );

uint32_t os_getIdleHook(void);
uint32_t os_stackOverFlow(void);
uint32_t os_memAllocFailed(void);
uint32_t os_assertFailed(void);


#endif /* APP_FREERTOS_H_ */