/*
 * adl_memory.c
 *
 * Created: 3/3/2558 13:52:41
 *  Author: pan
 */ 
#include <adl_global.h>
#include <FreeRTOSConfig.h>


#if (!WM_TYPES_MALLOC_C_LIB)
#define adl_memalloc	pvPortMalloc
#define adl_memfree		vPortFree
#else
#define adl_memalloc	malloc
#define adl_memfree		free
#endif


void * _adl_memGet(uint32_t size, const char * file, uint32_t line) {
	return (void *)adl_memalloc(size);
}

void _adl_memRelease ( void ** Ptr ) {
	adl_memfree(*Ptr);
	*Ptr = NULL;
}



bool adl_memStat ( uint32_t *o_pu32_HeapSize, uint32_t *o_pu32_HeapFree, uint32_t *o_pu32_HeapContig) {
	*o_pu32_HeapSize = configTOTAL_HEAP_SIZE;
	*o_pu32_HeapFree = xPortGetFreeHeapSize();
	return true;
}