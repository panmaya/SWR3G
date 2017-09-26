/*
 * app_freertos.c
 *
 * Created: 26/11/2557 11:45:43
 *  Author: pan
 */
#include <adl_global.h>

/*
 * FreeRTOS hook (or callback) functions that are defined in this file.
 */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle pxTask,signed char *pcTaskName);
void vApplicationTickHook(void);

static volatile uint32_t os_IdleCount = 0;
static volatile uint32_t os_stackOver = 0;
static volatile uint32_t os_allocFailed = 0;
static volatile uint32_t os_assert = 0;

#define OS_MAX_TICK_COUNT	1200000000


uint32_t os_getIdleHook(void) {
	uint32_t ret = os_IdleCount;
	os_IdleCount = 0;
	return ret;
}

uint32_t os_stackOverFlow(void) {
	return os_stackOver;
}
uint32_t os_memAllocFailed(void) {
	return os_allocFailed;
}
uint32_t os_assertFailed(void) {
	return os_assert;
}

void vAssertCalled( uint32_t ulLine, const char *pcFile )
{
	/* The following two variables are just to ensure the parameters are not
	optimized away and therefore unavailable when viewed in the debugger. */
	volatile uint32_t ulLineNumber = ulLine, ulSetNonZeroInDebuggerToReturn = 1;
	volatile const char * const pcFileName = pcFile;

	taskENTER_CRITICAL();
	
	os_assert  = 1;
	
	printf("\r\nAssert Failed %s line %ld",pcFileName,ulLineNumber);
	
	while( ulSetNonZeroInDebuggerToReturn == 0 )
	{
		/* If you want to set out of this function in the debugger to see the
		assert() location then set ulSetNonZeroInDebuggerToReturn to a non-zero
		value. */
	}
	taskEXIT_CRITICAL();

	( void ) pcFileName;
	( void ) ulLineNumber;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	
	os_allocFailed = 1;
	taskENTER_CRITICAL();
	puts("\r\n MemAlloc Failed\r\n");
	taskEXIT_CRITICAL();
	taskDISABLE_INTERRUPTS();
	for (;;) {
	}
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	volatile signed char *name = pcTaskName;
	volatile xTaskHandle *pxT = pxTask;
	
	os_stackOver = (int)pxT;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskENTER_CRITICAL();
	printf("\r\n Stack OverFlow @ 0x%x, %s\r\n",(int)pxT,(const char*)pcTaskName);
	taskEXIT_CRITICAL();
	taskDISABLE_INTERRUPTS();
	for(;;) {}
}


/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{

	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
	os_IdleCount = os_IdleCount > OS_MAX_TICK_COUNT ? OS_MAX_TICK_COUNT : os_IdleCount + 1;
}

/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}

