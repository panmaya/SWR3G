/*
 * adl_fcm.c
 *
 * Created: 3/10/2557 17:15:27
 *  Author: pan
 */ 
#include <adl_global.h>
#include "../private/adl_fcm_Private.h"
#include "_drv_serial/uart/freertos_uart_serial.h"

#define ADL_FCM_DRV_STACK_SIZE		(1024)
static uint8_t ADL_FCM_DriverStack[ADL_FCM_DRV_STACK_SIZE];

//typedef struct _ADL_FCM_RCVBUFF_T {
//	uint16_t size;
//	uint8_t  data[ADL_FCM_BUFFER_SIZE+1];
//}ADL_FCM_rcvbuff_t;

//typedef struct _ADL_FCM_SUBSCRIBER_T {
static adl_fcmCtrlHdlr_f ADL_FCM_CtrlCbk = NULL;
static adl_fcmDataHdlr_f ADL_FCM_DataCbk = NULL;
//}adl_fcm_subsc_t;

/* The UART instance used for input and output. */
static freertos_uart_if ADL_FCM_hUsartIf = NULL;
/* The buffer provided to the UART driver to store incoming character in. */
//volatile ADL_FCM_rcvbuff_t ADL_FCM_RcvBuff = {0,{0}};
	
static uint8_t ADL_FCM_State = ADL_FCM_V24_STATE_AT;
	
	
/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         UART RX Task                           */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static void ADL_FCM_UART_rxTask(void *pvParameters) {
	static uint8_t input_string[ADL_FCM_BUFFER_SIZE +1];
	static portBASE_TYPE returned_value = 0;
	static const portTickType max_block_time_ticks = 500UL/*1000UL*/ / portTICK_RATE_MS;
	static uint8_t mode = ADL_FCM_V24_STATE_AT;
	
	UNUSED(pvParameters);
	
	ADL_FCM_CtrlCbk(ADL_FCM_EVENT_FLOW_OPENNED);
	
	for (;;) {
		if((mode == ADL_FCM_V24_STATE_AT)
		&&(ADL_FCM_State==ADL_FCM_V24_STATE_DATA)) {
			mode = ADL_FCM_V24_STATE_DATA;
			ADL_FCM_CtrlCbk(ADL_FCM_EVENT_V24_DATA_MODE);
		}
		else if((mode == ADL_FCM_V24_STATE_DATA)
		&&(ADL_FCM_State==ADL_FCM_V24_STATE_AT)) {
			mode = ADL_FCM_V24_STATE_AT;
			ADL_FCM_CtrlCbk(ADL_FCM_EVENT_V24_AT_MODE);
		}
		
		if(mode == ADL_FCM_V24_STATE_AT)continue;
		
		returned_value = freertos_uart_serial_read_packet(	ADL_FCM_hUsartIf,
															input_string,
															ADL_FCM_BUFFER_SIZE,
															max_block_time_ticks);
		
		if (returned_value > 0) {
			input_string[returned_value] = 0;
			ADL_FCM_DataCbk(returned_value,input_string);
		}
		vTaskDelay(10);
	}
}
	
	
static freertos_uart_if ADL_FCM_UART_CreateTask(void)
{
	//uint8_t * pHandle = NULL;
	freertos_uart_if freertos_uart;
	freertos_peripheral_options_t driver_options = {
		ADL_FCM_DriverStack,							/* The buffer used internally by the UART driver to store incoming characters. */
		ADL_FCM_DRV_STACK_SIZE,							/* The size of the buffer provided to the UART driver to store incoming characters. */
		ADL_FCM_TASK_INTERRUPT_PRIORITY,				/* The priority used by the UART interrupts. */
		UART_RS232,										/* Configure the UART for RS232 operation. */
		(WAIT_TX_COMPLETE | USE_TX_ACCESS_MUTEX)		/* Use access mutex on Tx (as more than one task transmits) but not Rx. Wait for a Tx to complete before returning from send functions. */
	};

	sam_uart_opt_t uart_settings;
	
	uart_settings.ul_mck = sysclk_get_peripheral_hz();
	uart_settings.ul_baudrate = ADL_FCM_UART_BAUDRATE;
	uart_settings.ul_mode = UART_MR_PAR_NO;

	/* Initialise the UART interface. */
	freertos_uart = freertos_uart_serial_init(UART1, &uart_settings, &driver_options);
	configASSERT(freertos_uart);
	
	/* Create task. */
	xTaskCreate(	ADL_FCM_UART_rxTask,			/* The task that implements the command console. */
	ADL_FCM_TASK_NAME,		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
	ADL_FCM_TASK_STACK_SIZE,	/* The size of the stack allocated to the task. */
	(void *) 0,				/* The parameter is used to pass the already configured UART port into the task. */
	ADL_FCM_TASK_PRIORITY,	/* The priority allocated to the task. */
	NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
	
	return freertos_uart;
}
	
s8 adl_fcmSwitchV24State ( u8 Handle, u8 State ) {
	if(!ADL_FCM_hUsartIf)return -1;
	ADL_FCM_State = State;
	return OK;
}

s8 adl_fcmSendData ( u8 Handle, u8 * Data, u16 DataLen ) {
	const portTickType max_block_time_ticks = 3000UL / portTICK_RATE_MS;
	if(!ADL_FCM_hUsartIf)return -1;
	/* The USART is configured to use a mutex on Tx, so can be safely written to directly. */
	return freertos_uart_write_packet(ADL_FCM_hUsartIf, Data, DataLen, max_block_time_ticks);
}

s8 adl_fcmSubscribe ( adl_fcmFlow_e Flow, adl_fcmCtrlHdlr_f CtrlHandler, adl_fcmDataHdlr_f DataHandler ) {
	if((Flow != ADL_PORT_UART1) || !CtrlHandler || !DataHandler) return ADL_RET_ERR_PARAM;
	ADL_FCM_CtrlCbk = CtrlHandler;
	ADL_FCM_DataCbk = DataHandler;
	if(!ADL_FCM_hUsartIf) ADL_FCM_hUsartIf = ADL_FCM_UART_CreateTask();
	return ADL_FCM_hUsartIf ? 1 : ERROR;
}