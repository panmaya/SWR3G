
/*
 * arc_nethlCtrl.c
 *
 * Created: 20/2/2558 15:27:49
 *  Author: pan
 */ 

#include <adl_global.h>
#include "apt_gsm_uart.h"
#include "_drv_serial/usart/freertos_usart_serial.h"


#define APT_GSM_UART_UART_BAUDRATE		115200
#define APT_GSM_UART_DRV_STACK_SIZE		APT_GSM_UART_BUFFER_SIZE
#define APT_GSM_UART_RCV_SIZE			64//256

/* The buffer provided to the UART driver to store incoming character in. */
static struct {
	uint16_t size;
	uint8_t  data[APT_GSM_UART_BUFFER_SIZE+1];
}APT_GSM_UART_RcvBuff = {0,{0}};
/* The UART instance used for input and output. */
static freertos_usart_if ARC_HLCtrl_hUsartIf = NULL;
/* Driver stack */
static uint8_t APT_GSM_UART_DriverStack[ APT_GSM_UART_BUFFER_SIZE ] = {0};
static uint8_t input_string[APT_GSM_UART_RCV_SIZE + 1];
static volatile portBASE_TYPE returned_value = 0;

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Flush Serial                              */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
void aptGsmUart_Flush(void) {
	while(returned_value > 0)vTaskDelay(1);
	APT_GSM_UART_RcvBuff.size = 0;//re-positioning buffer
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Read Serial                              */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int32_t aptGsmUart_Read(uint8_t** Data, uint16_t nByte ,uint16_t delay_ms) {
	static uint8_t retBuff[APT_GSM_UART_BUFFER_SIZE+1];
	
	while((APT_GSM_UART_RcvBuff.size < nByte) && delay_ms) {
		vTaskDelay(1UL / portTICK_RATE_MS);
		delay_ms--;
	}
	if( APT_GSM_UART_RcvBuff.size) {
		memcpy(retBuff, APT_GSM_UART_RcvBuff.data, APT_GSM_UART_RcvBuff.size);
		retBuff[APT_GSM_UART_RcvBuff.size] = 0;
		*Data = (uint8_t*)&retBuff;
	}else *Data = NULL;
	return  APT_GSM_UART_RcvBuff.size;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Write Serial                             */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int32_t aptGsmUart_Write(uint8_t* Data, uint16_t DataLen) {
	if(!ARC_HLCtrl_hUsartIf)return -1;
	/* The USART is configured to use a mutex on Tx, so can be safely written to directly. */
	//vPuts((char*)Data);
	//while(!usart_is_tx_buf_empty(USART1));
	return freertos_usart_write_packet(ARC_HLCtrl_hUsartIf, Data, DataLen, 3000UL / portTICK_RATE_MS);
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         UART RX Task                           */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
void aptGsmUart_task(void * pvParameters) {
	static uint32_t * pWdCount = NULL;
	
	pWdCount = (uint32_t*)pvParameters;
	
	static const sam_usart_opt_t usart_settings = {
		APT_GSM_UART_UART_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		0 /* Only used in IrDA mode. */
	}; /*_RB_ TODO This is not SAM specific, not a good thing. */

	static freertos_peripheral_options_t driver_options = {
		APT_GSM_UART_DriverStack,							/* The buffer used internally by the UART driver to store incoming characters. */
		APT_GSM_UART_DRV_STACK_SIZE,						/* The size of the buffer provided to the UART driver to store incoming characters. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3,	/* The priority used by the UART interrupts. */
		USART_RS232,									/* Configure the UART for RS232 operation. */
		(USE_TX_ACCESS_MUTEX | WAIT_TX_COMPLETE)		/* Use access mutex on Tx  and Rx (as more than one task transmits) .*/
	};
		
	/* Initialise the UART interface. */
	ARC_HLCtrl_hUsartIf = freertos_usart_serial_init(USART1, &usart_settings, &driver_options);
		
	configASSERT(ARC_HLCtrl_hUsartIf);
	
	for (;;) {
		
		*pWdCount = 0;//Clear watchdog

		returned_value = freertos_usart_serial_read_packet(	ARC_HLCtrl_hUsartIf,
															input_string,
															APT_GSM_UART_RCV_SIZE,
															(10UL/ portTICK_RATE_MS));
													
		if (returned_value > 0) {
			returned_value =	(APT_GSM_UART_RcvBuff.size + returned_value) < APT_GSM_UART_BUFFER_SIZE
								? returned_value :(APT_GSM_UART_BUFFER_SIZE - APT_GSM_UART_RcvBuff.size);
			if(returned_value > 0) {
				memcpy((uint8_t*)APT_GSM_UART_RcvBuff.data + APT_GSM_UART_RcvBuff.size,
						(uint8_t*)input_string,
						returned_value);
					
				APT_GSM_UART_RcvBuff.size += (uint16_t)returned_value;
				APT_GSM_UART_RcvBuff.data[APT_GSM_UART_RcvBuff.size] = 0;
			}
		}
		vTaskDelay(1);// 14.4 byte/1ms @ 115200
	}
}
