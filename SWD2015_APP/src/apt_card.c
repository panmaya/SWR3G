/*
 * card_reader.c
 *
 * Created: 2/7/2558 10:33:00
 *  Author: pan
 */ 
#include <adl_global.h>
#include "buffer_Q512Flash.h"
#include "apt_card.h"
#include "misc_config.h"
#include "_drv_serial/uart/freertos_uart_serial.h"
#include "apt_tracking.h"
#include "secbox_cmd.h"

/* The size of the buffer provided to the UART driver
for storage of received  bytes. */
#define APT_CARD_BUFFER_SIZE		200
#define APT_CARD_UART_BAUDRATE		9600
#define APT_CARD_DRV_STACK_SIZE		512

static uint8_t APT_CARD_DriverStack[APT_CARD_DRV_STACK_SIZE];

static ascii cardCmdBuffer[APT_CARD_BUFFER_SIZE + 18] = {"AT#CARD=\""};
static ascii cardDataBuffer[APT_CARD_BUFFER_SIZE + 1] = {""};
static int cardDataLen = 0;

static void APT_CARD_DATA_Handler ( u16 DataSize, u8 * Data ) {
	int returnPos;
	static bool is_card_data_found = FALSE;

	if((Data[0]=='%')&&(is_card_data_found == FALSE)) { //Card data just came to port
		is_card_data_found=TRUE;
		memcpy(cardDataBuffer, Data, DataSize);
		cardDataLen = DataSize;
		cardDataBuffer[DataSize] = '\0';
		returnPos = strSearchToken(cardDataBuffer,'?',3,cardDataLen);
		if(returnPos > 0) {
			cardDataBuffer[returnPos+1] = '\0';
			wm_strRemoveCRLF(cardCmdBuffer + 9, cardDataBuffer, returnPos+1);
			strcat(cardCmdBuffer, "\"");			
			secbox_cmd_Card(cardCmdBuffer,strlen(cardCmdBuffer));
			is_card_data_found=FALSE;
		}
	}
	else if(is_card_data_found == TRUE) {
		if((cardDataLen + DataSize) < APT_CARD_BUFFER_SIZE) {
			memcpy(cardDataBuffer + cardDataLen, Data, DataSize);
			cardDataLen += DataSize;
			cardDataBuffer[cardDataLen] = '\0';
			returnPos = strSearchToken(cardDataBuffer,'?',3,cardDataLen);
			if(returnPos>0) {
				cardDataBuffer[returnPos+1] = '\0';
				wm_strRemoveCRLF(cardCmdBuffer + 9, cardDataBuffer, returnPos+1);
				strcat(cardCmdBuffer, "\"");
				secbox_cmd_Card(cardCmdBuffer,strlen(cardCmdBuffer));
				is_card_data_found=FALSE;
			}
		}
		else {
			is_card_data_found = FALSE;
			memset(cardDataBuffer,0,APT_CARD_BUFFER_SIZE);
		}
	}
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         UART RX Task                           */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
void aptCard_task(void * pvParameters)
{
	static uint8_t input_string[APT_CARD_BUFFER_SIZE +1];
	static portBASE_TYPE returned_value = 0;
	static freertos_uart_if uart_if = NULL;

	static uint32_t * pWdCount = NULL;
	
	pWdCount = (uint32_t*)pvParameters;
	
	freertos_peripheral_options_t driver_options = {
		APT_CARD_DriverStack,							/* The buffer used internally by the UART driver to store incoming characters. */
		APT_CARD_DRV_STACK_SIZE,							/* The size of the buffer provided to the UART driver to store incoming characters. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 4,/* The priority used by the UART interrupts. */
		UART_RS232,										/* Configure the UART for RS232 operation. */
		0		/* Use access mutex on Tx (as more than one task transmits) but not Rx. Wait for a Tx to complete before returning from send functions. */
	};

	sam_uart_opt_t uart_settings;
	
	uart_settings.ul_mck = sysclk_get_peripheral_hz();
	uart_settings.ul_baudrate = APT_CARD_UART_BAUDRATE;
	uart_settings.ul_mode = UART_MR_PAR_NO;

	/* Initialise the UART interface. */
	uart_if = freertos_uart_serial_init(UART1, &uart_settings, &driver_options);
	configASSERT(uart_if);
	
	while(!aptTracking_task_completed()){
		*pWdCount = 0;//Clear watchdog
		vTaskDelay(10);
	}
	
	for (;;) {
		
		*pWdCount = 0;//Clear watchdog
		
		returned_value = freertos_uart_serial_read_packet(	uart_if, input_string, APT_CARD_BUFFER_SIZE, 500UL/ portTICK_RATE_MS);
		if (returned_value > 0) {
			input_string[returned_value] = 0;
			APT_CARD_DATA_Handler(returned_value,input_string);
		}
		vTaskDelay(10);
	}
}


