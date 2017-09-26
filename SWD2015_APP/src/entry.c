/*
 * entry.c
 *
 * Created: 26/11/2557 11:53:19
 *  Author: pan
 */ 
#include <adl_global.h>
#include "fat_system.h"
#include "apt_debug_port.h"
#include "apt_tracking.h"
#include "apt_gateway.h"
#include "apt_swd.h"
#include "ini_config.h"
#include "buffer.h"
#include "vprinter.h"
#include "apt_gps_data.h"
#include "_lib_arcnethl/arc_hlgsm.h"
#include "_lib_arcnethl/arc_hlcnx.h"
#include "apt_afec.h"
#include "misc_config.h"
#include "swd_io.h"
#include "error.h"
#include "os.h"
#include "misc_cmd.h"
#include "apt_hahd_gps.h"
#include "feature.h"
#include "../_hw_ctrl/hw_ctrl.h"

extern void update_fattime_1ms(void);

// Tasks Table
extern void aptStart_task (void*);
extern void aptSms_task (void*);
extern void aptBuzzer_task (void*);
extern void aptLedSend_task (void*);
extern void aptGsmWakeup_task (void*);
extern void aptGpsWakeup_task (void*);
extern void aptPrintDbg_task (void*);
extern void aptCard_task (void*);
extern void aptGsmUart_task (void*);


/*extern*/ const adl_InitTasks_t adl_InitTasks[ ] = {
	
	{ aptStart_task,	configMINIMAL_STACK_SIZE + 1370,	"aptStart",		4},
	{ aptPrintDbg_task,	configMINIMAL_STACK_SIZE + 600,		"aptPrintDbg",	4},
	{ aptSwd_task,		configMINIMAL_STACK_SIZE + 1350,	"aptSwd",		3},
	{ aptTracking_task,	configMINIMAL_STACK_SIZE + 1350,	"aptTracking",	3},
	{ aptGateway_task,	configMINIMAL_STACK_SIZE + 1350,	"aptGateway",	3},
	{ aptAfec_task,		configMINIMAL_STACK_SIZE + 80,		"aptAfec",		2},
	{ aptSms_task,		configMINIMAL_STACK_SIZE + 550,		"aptSms",		2},
	{ aptBuzzer_task,	configMINIMAL_STACK_SIZE + 80,		"aptBuzzer",	3/*2*/},
	{ aptLedSend_task,	configMINIMAL_STACK_SIZE + 160,		"aptLedSend",	1},
	{ aptGsmWakeup_task,configMINIMAL_STACK_SIZE + 160,		"aptLedGsm",	1},
	{ aptGpsWakeup_task,configMINIMAL_STACK_SIZE + 170,		"aptLedGps",	1},
	{ aptHahdGps_task,	configMINIMAL_STACK_SIZE + 150,		"aptHahdGps",	1},
	{ aptGsmUart_task,	configMINIMAL_STACK_SIZE + 440,		"aptGsmUart",	0},//Serial PDC
	{ aptGpsData_task,	configMINIMAL_STACK_SIZE + 570,		"aptGpsData",	0},//Serial PDC
	{ aptCard_task,		configMINIMAL_STACK_SIZE + 1200,	"aptCard",		0},//Serial PDC
	{ aptDbgPort_task,	configMINIMAL_STACK_SIZE + 1420,	"aptDbgPort",	0},//Serial PDC
	{ 0, 0, 0, 0 }
};
// Application name definition
/*extern*/ const ascii adl_InitApplicationName[] = BOARD_NAME;
// Company name definition
/*extern*/ const ascii adl_InitCompanyName[] = "Aerial Communications Co.,Ltd.";
// Application version definition
/*extern*/ const ascii adl_InitApplicationVersion[] = {TOSTRING(SWD2015_FW_VERSION)};
/*extern*/ int16_t ibuild_day = 1,ibuild_month = 1,ibuild_year = 2015;
/*extern*/ uint8_t		uptime_dd, uptime_hh, uptime_mm, uptime_ss = 0;
/*extern*/ uint8_t		apt_startup_mode = APT_STARTUP_INPROGRESS;
/*extern*/ bool		battery_isOperating = FALSE;
/*extern*/ freertos_twi_if TWI1_hBus = NULL;
/*extern*/ bool FRAM_TestPassed = FALSE;
/*extern*/ bool WDOG_TestPassed = FALSE;
/*extern*/ uint32_t	printTraceOn = 0;
/*extern*/ uint8_t all_task_created_nb = 0;
/*extern*/ volatile uint32_t wd_monitor[32];
/*extern*/ xTaskHandle tassk_handle[32];

static uint32_t tc0_status = 0;
static uint16_t mcu_led_timing = 0;
static uint16_t ms = 0;
static bool wd_is_timeout = false;
static volatile uint32_t os_err_timeout = 0;
static adl_InitType_e entry_startup_type = ADL_INIT_POWER_ON;
inline adl_InitType_e adl_InitGetType (void) { return entry_startup_type; }
	
extern bool aptStart_isHibernated;

	
void RSTC_Handler(void)
{
	rstc_get_status(RSTC);//Clear the interrupt.
	NVIC_DisableIRQ(RSTC_IRQn);
	rstc_disable_user_reset_interrupt(RSTC);
	rstc_enable_user_reset(RSTC);
	/* NRST assert. */
	rstc_set_external_reset(RSTC, 0);// Set the external reset length. 60 us
	rstc_reset_extern(RSTC);//Asserts the NRST pin for external resets.
}

/**
 * \brief Interrupt handler for TC0. Record the number of bytes received,
 * and then restart a read transfer on the USART if the transfer was stopped.
 */
void TC0_Handler(void) {
	/* Read TC0 Status. */
	tc0_status = tc_get_status(TC0, 0);

	/* RC compare. */
	if (((tc0_status & TC_SR_CPCS) == TC_SR_CPCS)){
		ms++;
		if(ms>=1000){
			uptime_ss++;
			ms=0;
		}
		if(uptime_ss>=60){
			uptime_mm++;
			uptime_ss=0;
		}
		if(uptime_mm>=60){
			uptime_hh++;
			uptime_mm=0;
		}
		if(uptime_hh>=24){
			uptime_dd++;
			uptime_hh=0;
		}
		
		mcu_led_timing++;
		if(mcu_led_timing >= 600) {
			ioport_toggle_pin_level(LED_MCU);
			mcu_led_timing = 0;
		}

		/* Watchdog Monitor*/
		hw_ctrl_wd_Toggle();
		
		//update_fattime_1ms();
		
		//update_adl_rtc_fattime_1ms();
		
		update_adl_io_1ms();
		
		if(os_stackOverFlow()||os_memAllocFailed()||os_assertFailed()) os_err_timeout++;
		
		for(uint8_t i = 0; i < all_task_created_nb ; i++) {
			if(wd_monitor[i] < _1mSecTick_Second(60*5)) wd_monitor[i]++;
			else {
				if(!aptStart_isHibernated) wd_is_timeout = true;
			}
		}
		
		if(wd_is_timeout || misc_cmd_wdreset() || (os_err_timeout >  _1mSecTick_Second(10) )) {
			//f_mount(0, NULL);//un_mount fatfs
			f_mount(NULL, "0:",0);//un_mount fatfs
			tc_stop(TC0, 0);
		}
	}
}

static void complied_get_date(void) {
	char s_month[5]= {""};
	char time[] = { __DATE__ };
	const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	uint8_t inext = 0;
	for(uint8_t i = 0; i < strlen(__DATE__) ; i++) {
		if(time[i] == ' ') {
			time[i] = 0;
			if(s_month[0]==0) {
				strcpy(s_month,time);
				if(time[i+1] == ' ') i++;
			}
			else {
				ibuild_day = atoi(time + inext + 1);
				ibuild_year = atoi(time + i + 1);
				break;
			}
			inext = i;
		}
	}
	
	ibuild_month = (strstr(month_names, s_month) - month_names)/3;
	ibuild_month += 1;
}


/**
 * \brief Configure Timer Counter 0 (TC0) to generate an interrupt every 200ms.

 */
static void configure_tc0(void) {
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk;
	const uint16_t TC_FREQ = 1000;/** Timer counter frequency in Hz. */

	/* Get system clock. */
	ul_sysclk = sysclk_get_cpu_hz();

	/* Configure PMC. */
	pmc_enable_periph_clk(ID_TC0);

	/* Configure TC for a frequency and trigger on RC compare. */
	tc_find_mck_divisor(TC_FREQ, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC0, 0, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC0, 0, (ul_sysclk / ul_div) / TC_FREQ);

	/* Configure and enable interrupt on RC compare. */
	NVIC_SetPriority(TC0_IRQn, 8);
	NVIC_EnableIRQ((IRQn_Type)ID_TC0);
	tc_enable_interrupt(TC0, 0, TC_IER_CPCS);
	
	/* Start the Timer. */
	tc_start(TC0, 0);// ENABLES CLOCK => TC_SR0.CLKSTA = 1
}

int main (void) {

	const usart_serial_options_t options = {
		.baudrate   = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits   = CONF_UART_STOP_BITS
	};
	
	/* ASF function to setup clocking. */
	sysclk_init();
	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(0);
	
	/* Atmel library function to setup for the evaluation kit being used. */
	board_init(FALSE);
	
	delay_ms (1000);//Peripherally power off 3 seconds
	ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_ON);
	delay_ms(200);
	ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);

	delay_ms (2000);//Peripherally power off 3 seconds
	
	board_init(TRUE);
	
	configure_tc0();
	
	efc_init(EFC,FLASH_ACCESS_MODE_128,6);
	
	//rstc_disable_user_reset(RSTC);
	//NVIC_DisableIRQ(RSTC_IRQn);
	//rstc_enable_user_reset_interrupt(RSTC);
	//NVIC_ClearPendingIRQ(RSTC_IRQn);
	//NVIC_SetPriority(RSTC_IRQn, 0);
	//NVIC_EnableIRQ(RSTC_IRQn);
	
	//entry_rtc_setup
	/* Enable the External 32K oscillator */
	pmc_switch_sclk_to_32kxtal(PMC_OSC_XTAL);
	/* Wait for 32K oscillator ready */
	while (!pmc_osc_is_ready_32kxtal());
	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(RTC, 0);//24 Hour Mode
	
	hw_ctrl_fram_ResetBus();
	
	delay_ms(10);
	// JTAG Shared Pin With TWI1; So Must to Disabled
	hw_ctrl_jtag_Disable();

	hw_ctrl_a2d_ResetBus();
	delay_ms(10);
		
	/* Enable unit test output peripheral. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &options);
	
	swdio_init();
	
	complied_get_date();
	
	vPrintInit();
	
	entry_startup_type =  error_init_by();
	
	for(uint8_t i = 0; i < 32 ;i++) { tassk_handle[i] = NULL; wd_monitor[i] = 0;}

	for(uint8_t i = 0;adl_InitTasks[i].EntryPoint;i++) {
		xTaskCreate(	adl_InitTasks[i].EntryPoint,
						(const signed char*) adl_InitTasks[i].
						Name,adl_InitTasks[i].StackSize,
						(uint32_t*)&wd_monitor[i],
						tskIDLE_PRIORITY + adl_InitTasks[i].Priority,
						&tassk_handle[i]
					);
		all_task_created_nb++;
	}
	
	arc_netInit(0);
	
	arc_hlgsm_cmdInit((uint32_t*)&wd_monitor[all_task_created_nb],&tassk_handle[all_task_created_nb]);
	all_task_created_nb++;

	arc_hlgsm_smsInit((uint32_t*)&wd_monitor[all_task_created_nb],&tassk_handle[all_task_created_nb]);
	all_task_created_nb++;

	arc_hlcnx_Init((uint32_t*)&wd_monitor[all_task_created_nb],&tassk_handle[all_task_created_nb]);
	all_task_created_nb++;

	while(sizeof(aptTracking_runtime_t)>120) puts("\r\n runtime over size\r\n");
	
	vTaskStartScheduler();
	
	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for (;;) {}
}
