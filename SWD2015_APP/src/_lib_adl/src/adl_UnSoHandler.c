/*
 * adl_UnSoHandler.c
 *
 * Created: 8/10/2557 10:17:20
 *  Author: pan
 */ 
#include <adl_global.h>
#include "../adl_UnSoHandler.h"
#if(0)
typedef struct ADL_UNSOLICITED_ITEM_T {
	adl_atUnSoHandler_t UnSohdl;
	uint8_t length;
	char UnSostr[1];
}adl_unsohandler_item_t;

typedef struct ADL_UNSOLICITED_DAT_T {
	uint16_t size;
	uint8_t data[1];
}adl_unsolicited_dat_t;

static wm_lst_t ADL_UNSOLICITED_hList = NULL;
static wm_lst_t ADL_UNSOLICITED_DAT_hList = NULL;
#endif

#if(0)
static bool ADL_UNSOLICITED_CTRL_Handler ( adl_fcmEvent_e Event ) { return TRUE; }
static bool ADL_UNSOLICITED_DATA_Handler ( u16 DataSize, u8 * Data ) {
	adl_unsolicited_dat_t *pData = NULL;
	//if(freertos_usart1GetMode() == FREERTOS_USART1_MODE_DATA) return TRUE;
	pData = adl_memGet(sizeof(adl_unsolicited_dat_t) + DataSize);
	memcpy(pData->data,Data,DataSize);
	pData->size = DataSize;
	wm_lstAddItem(ADL_UNSOLICITED_DAT_hList,pData);
	return TRUE;
}
#endif

s16 adl_atUnSoSubscribe(ascii *UnSostr, adl_atUnSoHandler_t UnSohdl) {
	#if(0)
	adl_unsohandler_item_t *item = (adl_unsohandler_item_t*)adl_memGet(sizeof(adl_unsohandler_item_t)+strlen(UnSostr));
	wm_strcpy(item->UnSostr,UnSostr);
	item->UnSohdl = UnSohdl;
	item->length = strlen(UnSostr);
	wm_lstAddItem(ADL_UNSOLICITED_hList,item);
	#endif
	return OK;
}

#if(0)
static void ADL_UNSOLICITE_Task(void *pvParameters)
{
	for(;;) {
		if( xSemaphoreTake( xSemaphore_Usart1, ( portTickType ) 0 ) == pdTRUE )
		{
			// We were able to obtain the semaphore and can now access the shared resource.
			// ...
			adl_unsolicited_dat_t * pData = wm_lstGetItem(ADL_UNSOLICITED_DAT_hList,0);
			if(pData) {// found data
				uint8_t i = 0;
				adl_unsohandler_item_t * item = wm_lstGetItem(ADL_UNSOLICITED_hList,i);
				while(item) {
					char * pstr = strstr((char*)pData->data,item->UnSostr);
					if(pstr) {// found match data
						adl_atUnsolicited_t * pUns = adl_memGet(sizeof(adl_atUnsolicited_t) + pData->size);
						strncpy(pUns->StrData, (char*)pData->data, pData->size);
						pUns->StrData[ pData->size] = 0;
						pUns->StrLength =  pData->size;
						item->UnSohdl(pUns);//call back
						adl_memRelease(pUns);
					}
					i++;
					item = wm_lstGetItem(ADL_UNSOLICITED_hList,i);// get next
				}
				wm_lstDeleteItem(ADL_UNSOLICITED_DAT_hList,0);//delete data
			}
			// We have finished accessing the shared resource.  Release the semaphore.
			//xSemaphoreGive( xSemaphore );
			xSemaphoreGive( xSemaphore_Usart1 );
		}//else {// We could not obtain the semaphore and can therefore not access the shared resource safely.}
		vTaskDelay(1);
	}
}
#endif

s8 adl_UnSoHandler_init(void) {
	#if(0)
	if(ADL_UNSOLICITED_hList)return 1;//already subscribed
	ADL_UNSOLICITED_hList = wm_lstCreate(WM_LIST_NONE,NULL);
	ADL_UNSOLICITED_DAT_hList = wm_lstCreate(WM_LIST_NONE,NULL);
	freertos_usart1Subscribe (ADL_UNSOLICITED_CTRL_Handler, ADL_UNSOLICITED_DATA_Handler);
	/* Create task. */
	
	if(xSemaphore_Usart1 != NULL) {
		xTaskCreate( ADL_UNSOLICITE_Task,				/* The task that implements the command console. */
					(const int8_t *const)"UNS_TASK",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					(configMINIMAL_STACK_SIZE * 2),		/* The size of the stack allocated to the task. */
					NULL,								/* The parameter is used to pass the already configured UART port into the task. */
					(tskIDLE_PRIORITY + 1),				/* The priority allocated to the task. */
					NULL);								/* Used to store the handle to the created task - in this case the handle is not required. */
	}
	#endif
	return 0;
}