/*
 * consoler.c
 *
 * Created: 20/3/2558 14:58:57
 *  Author: pan
 */ 
#include <adl_global.h>
#include "apt_debug_port.h"
#include "misc_config.h"
#include "_lib_bootld/boot_trigger.h"
#include "_lib_xmodem/xmodem.h"
#include "_lib_arcshell/common/arc_shell_common.h"
#include "_drv_serial/uart/freertos_uart_serial.h"
#include "_drv_serial/usart/freertos_usart_serial.h"
#include "error.h"
#include "apt_tracking.h"
#include "secbox_cmd.h"

#define APT_DBGPORT_CMD_Q_MAX			1
#define APT_DBGPORT_UART_BUFF_SIZE		APT_DBGPORT_CMD_MAX_LEN
#define APT_DBGPORT_PDC_STACK_SIZE		(APT_DBGPORT_UART_BUFF_SIZE * 2)

#define APT_DBGPORT_LIMIT_CMD_ON		100

static uint8_t APT_DBGPORT_PDCDriverStack[APT_DBGPORT_PDC_STACK_SIZE];
static freertos_uart_if APT_DBGPORT_hUart = NULL;
static uint8_t APT_DBGPORT_CMD_Count = 0;
static char APT_DBGPORT_CMD_AtBuffer[APT_DBGPORT_CMD_MAX_LEN + 1] = {""};
static uint8_t APT_DBGPORT_CMDOn = 0;


static bool arc_atcmd_addList (char c ) {
	static struct {
		uint16_t	idx;
		char		data[APT_DBGPORT_UART_BUFF_SIZE+1];
	}tBuff;
	
	switch(tBuff.idx) {
	case 0://Expect 'A' Or 'a'
		if((c =='A')||(c =='a')) {
			tBuff.data[0] = 'A';
			tBuff.idx = 1;
		}
		break;
	case 1://Expect 'T' Or 't'
		if((c =='T')||(c =='t')) {
			tBuff.data[1] = 'T';
			tBuff.idx = 2;
		}else tBuff.idx = 0;
		break;
	default:
		if((c =='\r')||(c =='\n')){
			tBuff.data[tBuff.idx] = 0;
			if(!strncmp("#SEC=",tBuff.data+2,5)) secbox_cmd_Sec(tBuff.data,tBuff.idx);
			else if(!strncmp("#COMM=",tBuff.data+2,6)) {
				if(aptTracking_task_completed())secbox_cmd_Comm(tBuff.data,tBuff.idx);
			}
			else if(!strncmp("#CARD=",tBuff.data+2,6)) {
				if(aptTracking_task_completed())secbox_cmd_Card(tBuff.data,tBuff.idx);
			}
			else if(!strncmp("#TEMP=",tBuff.data+2,6)) secbox_cmd_Temp(tBuff.data,tBuff.idx);
			else if(!strncmp("#RFID=",tBuff.data+2,6)) secbox_cmd_Rfid(tBuff.data,tBuff.idx);
			else if(!stricmp("+WDM=1",tBuff.data+2)) {
				APT_DBGPORT_CMDOn = APT_DBGPORT_LIMIT_CMD_ON;
				puts("\r\n+WDM: ON\r\n");
			}
			else if(!stricmp("+WDM=0",tBuff.data+2)) {
				APT_DBGPORT_CMDOn = 0;
				puts("\r\n+WDM: OFF\r\n");
			}
			else if(!stricmp("+WDWL",tBuff.data+2)) update_trigger();
			else if(APT_DBGPORT_CMDOn){
				if(APT_DBGPORT_CMD_Count < APT_DBGPORT_CMD_Q_MAX) {
					strcpy(APT_DBGPORT_CMD_AtBuffer,tBuff.data);
					APT_DBGPORT_CMD_Count++;
				}
			}
			tBuff.idx = 0;
			return true;
		}
		else if(isascii(c)) {
			tBuff.data[tBuff.idx] = c;
			tBuff.idx = (tBuff.idx < APT_DBGPORT_UART_BUFF_SIZE) ? tBuff.idx + 1 : tBuff.idx ;
		}
		break;
	}
	
	return false;
}

bool arc_atcmd_exe ( void ) {
	if(APT_DBGPORT_CMD_Count) {
		ascii *p = &APT_DBGPORT_CMD_AtBuffer[0];
		switch(*(p+2)) {
		case '#': arc_shell_execute_port((char*)p,ADL_PORT_UART1); break;
		case '@': adl_atCmdSubscribeExecute((char*)p,ADL_PORT_UART1);break;
		default: adl_atCmdCreate((char*)p,ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL); break;
		}
		APT_DBGPORT_CMD_Count--;
		return true;
	}
	return false;
}

void aptDbgPort_puts(const char* pStr) {
	while(!uart_is_tx_buf_empty(CONSOLE_UART)) vTaskDelay(1);
	puts(pStr);
}

void aptDbgPort_task(void * pvParameters ) {
	static uint8_t input_string[APT_DBGPORT_UART_BUFF_SIZE+1];
	static portBASE_TYPE returned_value = 0;
	
	static uint32_t * pWdCount = NULL;
		
	pWdCount = (uint32_t*)pvParameters;
	
	static  sam_uart_opt_t uart_settings;
	
	static  freertos_peripheral_options_t driver_options = {
		APT_DBGPORT_PDCDriverStack,						/* The buffer used internally by the UART driver to store incoming characters. */
		APT_DBGPORT_PDC_STACK_SIZE,						/* The size of the buffer provided to the UART driver to store incoming characters. */
		APT_DBGPORT_TASK_INTERRUPT_PRIORITY,				/* The priority used by the UART interrupts. */
		UART_RS232,										/* Configure the UART for RS232 operation. */
		0//(WAIT_TX_COMPLETE | USE_TX_ACCESS_MUTEX)		/* Use access mutex on Tx (as more than one task transmits) but not Rx. Wait for a Tx to complete before returning from send functions. */
	};
	
	uart_settings.ul_mck = sysclk_get_peripheral_hz();
	uart_settings.ul_baudrate = CONF_UART_BAUDRATE;
	uart_settings.ul_mode = UART_MR_PAR_NO;
	
	APT_DBGPORT_hUart = freertos_uart_serial_init(CONSOLE_UART, &uart_settings, &driver_options);
	
	configASSERT(APT_DBGPORT_hUart);
	
	for (;;) {
			
		*pWdCount = 0;//Clear watchdog
			
		if(APT_DBGPORT_CMDOn) {
			if(arc_atcmd_exe()) APT_DBGPORT_CMDOn--;
			if(APT_DBGPORT_CMDOn==0)puts("\r\n+WDM: OFF\r\n");
		}
			
		returned_value = freertos_uart_serial_read_packet(	APT_DBGPORT_hUart, input_string, APT_DBGPORT_UART_BUFF_SIZE, 10UL / portTICK_RATE_MS);
		if (returned_value > 0) {
			for(uint16_t i = 0;i < returned_value ;i++) arc_atcmd_addList ( input_string[i]);
		}
		
		vTaskDelay(1);
	}
}

