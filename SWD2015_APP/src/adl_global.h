/*
 * adl_global.h
 *
 * Created: 24/9/2557 14:47:21
 *  Author: pan
 */ 


#ifndef ADL_GLOBAL_H_
#define ADL_GLOBAL_H_

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <float.h>


#include <asf.h>

#include <conf_access.h>
#include <conf_at45dbx.h>
#include <conf_fm24clx.h>
#include <conf_ads1113.h>
#include <conf_bmi055.h>
#include <conf_lis3dh.h>
#include <conf_lsm6ds3.h>
#include <conf_board.h>
#include <conf_clock.h>
#include <conf_fatfs.h>
//#include <conf_nf.h>
//#include <conf_sd_mmc.h>
#include <conf_spi_master.h>
#include <conf_uart_serial.h>
//#include <conf_virtual_mem.h>
#include <FreeRTOSConfig.h>

#include <vprinter.h>

//#include "wm_types.h"					// Sierra Wireless types definitions
#include <_lib_wm/wm_types.h>		// Sierra Wireless types definitions
//#include "wm_stdio.h"					// Sierra Wireless standart input/output definitions
#include <_lib_wm/wm_stdio.h>		// Sierra Wireless standart input/output definitions
//#include "wm_list.h"					// Sierra Wireless List and Stack API definitions
#include <_lib_wm/wm_list.h>        // Sierra Wireless List and Stack API definitions
//#include "adl_str.h"					// String service
#include <_lib_adl/adl_str.h>       // String service
//#include "adl_port.h"					// Port service
#include <_lib_adl/adl_port.h>      // Port service
//#include "adl_at.h"					// AT commands related structures definition
#include <_lib_adl/adl_at.h>        // AT commands related structures definition


//#include "adl_traces.h" 				// Traces macros definition
#include <_lib_adl/adl_traces.h> 	// Traces macros definition
//#include "adl_memory.h" 				// Memory management functions
#include <_lib_adl/adl_memory.h> 	// Memory management functions
//#include "adl_error.h"  					// Error functions and definitions
#include <_lib_adl/adl_error.h>  	// Error functions and definitions
//#include "adl_flash.h"  					// Flash Objects management
#include <_lib_adl/adl_flash.h>  	// Flash Objects management
//#include "adl_gpio.h"   					// Gpio management
#include <_lib_adl/adl_gpio.h>    	// Gpio management
//#include "adl_bus.h"    					// Bus management
//#include "adl_fcm.h"    					// FCM management
#include <_lib_adl/adl_fcm.h>    	// FCM management
//#include "adl_osa.h"					// OSA management

//#include "adl_AppliInit.h"
#include <_lib_adl/adl_AppliInit.h>
//#include "adl_UnSoHandler.h"
#include <_lib_adl/adl_UnSoHandler.h>
//#include "adl_RspHandler.h"
#include <_lib_adl/adl_RspHandler.h>
//#include "adl_CmdHandler.h"
#include <_lib_adl/adl_CmdHandler.h>
//#include "adl_CmdStackHandler.h"
#include <_lib_adl/adl_CmdStackHandler.h>
//#include "adl_TimerHandler.h"
//#include <_lib_adl/adl_TimerHandler.h>


//#include "adl_sim.h"				// SIM service
#include <_lib_adl/adl_sim.h>	// SIM service
//#include "adl_sms.h"				// SMS service
#include <_lib_adl/adl_sms.h>	// SMS service
//#include "adl_call.h"				// CALL service
//#include "adl_gprs.h"				// GPRS service
//#include "adl_safe.h"				// Application safe mode service
//#include "adl_ad.h"				// Application & Data storage service
//#include "adl_rtc.h"				// RTC service
#include <_lib_adl/adl_rtc.h>    	// RTC service
//#include "adl_ctx.h"				// Execution context service
//#include "adl_msg.h"				// Messages service
//#include "adl_irq.h"				// Interruptions service
//#include "adl_tcu.h"				// TCU service
//#include "adl_vs.h"				// VariSpeed service
//#include "adl_sem.h"				 // Semaphores service
//#include "adl_extint.h"				// External interruption service
//#include "adl_dac.h"				// DAC service
//#include "adl_adc.h"				// DAC service
//#include "adl_reg.h"				// Registry service
//#include "adl_audio.h"				// Audio service
#include <_lib_adl/adl_audio.h> // Audio service
//#include "adl_queue.h"				// Queue service
//#include "adl_L3info.h"				// L3 info service
//#include "adl_sds.h"				// Secured Data Storage service
//#include "adl_wd.h"				// Watchdog service
//#include "adl_ids.h"				// Intelligent Device service
//#include "adl_event.h"				// Event service
//#include "adl_OpenDevice.h"			// Open Device service
//#include "adl_osclk.h"				// OS clock
//#include "adl_plmn.h"				// PLMN
//#include "adl_filesystem.h"			// File System
//#include "adl_remdebug.h"			// Remote Debugging

#include <_lib_arcnethl/var/arc_hlnet.h>

//#include <_lib_led/arc_led.h>

#include <_hw_ctrl/hw_ctrl.h>

//#include "gps.h"
#include <_lib_gps/nmea_gps.h>
//#include "arc_shell.h"
#include <_lib_arcshell/arc_shell.h>

//#include "freertos_twi_master.h"
//#include <_freertos_usart_serial/freertos_twi_master.h>
//#include "freertos_twi_master_task.h"
//#include <_freertos_usart_serial/freertos_twi_master_task.h>
//#include "freertos_uart_serial.h"
//#include <_freertos_usart_serial/freertos_uart_serial.h>
//#include "freertos_usart_serial.h"
//#include <_freertos_usart_serial/freertos_usart_serial.h>
//#include "freertos_uart_serialTask.h"
//#include <_freertos_usart_serial/freertos_uart_serialTask.h>
//#include "freertos_usart_serialTask.h"
//#include <_freertos_usart_serial/freertos_usart_serialTask.h>
//#include "freertos_usart_serial_config.h"
//#include <_freertos_usart_serial/freertos_usart_serial_config.h>

//#include <_freertos/twi/master/common/freertos_twi_master_task.h>
#include <_drv_serial/twi/master/freertos_twi_fm24c.h>
#include <_drv_serial/twi/master/freertos_twi_ads1113.h>
#include <_drv_serial/twi/master/freertos_twi_bmi055.h>
#include <_drv_serial/twi/master/freertos_twi_lis3dh.h>
#include <_drv_serial/twi/master/freertos_twi_lsm6ds3.h>

//#include "arc_atcmd.h"
//#include <_lib_arcatcmd/arc_atcmd.h>
//#include "arc_net.h"
//#include <_lib_gprs/arc_net.h>
//#include "arc_pdpcontext.h"
//#include <_lib_arcnet/arc_pdpcontext.h>


//#include <ASF/common/components/memory/data_flash/at45dbx/at45dbx_mem.h>
//#include <ASF/common/components/memory/data_flash/at45dbx/at45dbx.h>
#include <_lib_at45dbx/at45dbx_mem.h>
#include <_lib_at45dbx/at45dbx.h>
#include <_lib_fm24clx/fm24clx.h>
#include <_lib_fm24clx/drv_fram24c16.h>
//#include "sys_global.h"

//#define TRACE((a,b))			do { } while(0)

//#define adl_CmdStack_TASK_NAME					(const int8_t *const) "ADL_CMDSTACK"
//#define adl_UnsHandle_TASK_NAME					(const int8_t *const) "ADL_UNSHANDLE"

/* The priorities at which various tasks will get created. */
//#define adl_CmdStack_TASK_PRIORITY              (tskIDLE_PRIORITY + 1)
//#define adl_UnsHandle_TASK_PRIORITY             (tskIDLE_PRIORITY + 1)


//#define mainCDC_CLI_TASK_PRIORITY               (tskIDLE_PRIORITY + 1)
//#define mainUSART_ECHO_TASK_PRIORITY            (tskIDLE_PRIORITY)
//#define mainSPI_FLASH_TASK_PRIORITY             (tskIDLE_PRIORITY)
//#define mainTWI_EEPROM_TASK_PRIORITY            (tskIDLE_PRIORITY)

/* The stack sizes allocated to the various tasks. */
//#define adl_CmdStack_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE * 2)
//#define adl_UnsHandle_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE * 2)


//#define mainCDC_CLI_TASK_STACK_SIZE				(configMINIMAL_STACK_SIZE * 2)
//#define mainUSART_ECHO_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE)
//#define mainSPI_FLASH_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE * 2)
//#define mainTWI_EEPROM_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE * 2)


//s8 adl_CmdStackHandler_init(void);
//s8 adl_UnSoHandler_init(void);
//s8 adl_flash_init(uint8_t lun_id);

#endif /* ADL_GLOBAL_H_ */
