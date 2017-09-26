/*
 * main_start.c
 *
 * Created: 2/24/2016 6:18:23 PM
 *  Author: Panmaya
 */ 


#include "adl_global.h"
#include "apt_tracking.h"
#include "apt_gateway.h"
#include "apt_swd.h"
#include "error.h"
#include "fat_system.h"
#include "apt_gps_data.h"
#include "asf.h"
#include "apt_afec.h"
#include "ini_config.h"
#include "buffer.h"
#include "vprinter.h"
#include "_lib_arcnethl/arc_hlgsm.h"
#include "_lib_arcnethl/arc_hltcp.h"
#include "apt_afec.h"
#include "apt_card.h"
#include "card_authen.h"
#include "../_lib_bootld/boot_trigger.h"
#include "misc_config.h"
#include "xm0110_compat.h"
#include "feature.h"
#include "apt_debug_port.h"
#include "test_qc.h"
#include "misc_cmd.h"
#include "apt_buzzer.h"
#include "swd_io.h"
#include "buffer_gpbm.h"

extern void sleep_entry ( char mode, uint16_t waitsec, bool init_chip );
extern void aptAfec_SetupADC(void);

#define HELLO_PRINT_APP(_myDate,_myTime)		do {\
		adl_rtcTime_t Time;\
		adl_rtcGetTime(&Time);\
		vPrintf("\r\n*****************************************************\r\n");\
		vPrintf("-- %s\r\n",adl_InitApplicationName);\
		vPrintf("-- BOARD %s built %s : %s\r\n", BOARD_NAME, _myDate, _myTime);\
		vPrintf("-- %s\r\n",adl_InitCompanyName);\
		vPrintf("-- VERSION %s\r\n",adl_InitApplicationVersion);\
		vPrintf("-- TODAY: %02d/%02d/%04d",Time.Day,Time.Month,Time.Year);\
		vPrintf("\r\n*****************************************************\r\n");\
	}while(0)



void aptStart_task (void * pvParameters);

static bool aptStart_Initialized = false;

static void shellconfig_changed(void) {  ini_config_setUpdate(); }
	
static bool task_handle_printed[24];

bool aptStart_isHibernated = false;

bool aptStart_Completed(void);

bool aptStart_Completed(void) {
	return aptStart_Initialized;
}

void i2c_reset(void)
{
	uint32_t i;
	/* MEM reset
	 * a) Clock up to 9 cycles (use 100K)
	 * b) look for SDA high in each cycle while SCL is high and then
	 * c) Create a start condition as SDA is high
	 */
	/* - Enable pin output mode */
	ioport_set_pin_dir(BOARD_CLK_TWI_FMRAM, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(BOARD_CLK_TWI_FMRAM, 1);
	ioport_enable_pin(BOARD_CLK_TWI_FMRAM);
	for (i = 0; i < 10; i ++) {
		vTaskDelay(5);
		ioport_set_pin_level(BOARD_CLK_TWI_FMRAM, 0);
		vTaskDelay(5);
		ioport_set_pin_level(BOARD_CLK_TWI_FMRAM, 1);
	}
	/* - Restore pin peripheral mode */
	ioport_set_pin_mode(BOARD_CLK_TWI_FMRAM, BOARD_CLK_TWI_MUX_FMRAM);
	ioport_disable_pin(BOARD_CLK_TWI_FMRAM);
	/* - Start condition will do on R/W start */
}

static void gps_init_board(void) {
	
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	/* Initialize IOPORTs */
	ioport_init();
	
	/* Configure the pins connected to GPS  WAKE UP  Signal as input*/
	ioport_set_pin_dir(PIN_WAKEUP_GPS, IOPORT_DIR_INPUT);
	
	/* Configure the pins connected to GPS  ON/OFF control
	 default initial state to LOW(OFF).*/
	ioport_set_pin_dir(PIN_ONOFF_GPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_ONOFF_GPS, 1 ? PIN_ONOFF_GPS_OFF : PIN_ONOFF_GPS_ON);
	
	/* Configure the pins connected to GPS  Reset control
	 default initial state to Low (OFF).*/
	ioport_set_pin_dir(PIN_RST_GPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_RST_GPS, 1 ? PIN_RST_GPS_OFF : PIN_RST_GPS_ON);

	/* Configure the pins connected to GPS  Enable control
	 default initial state to High (ON).*/
	ioport_set_pin_dir(PIN_EN_GPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EN_GPS, 1 ? PIN_EN_GPS_ON : PIN_EN_GPS_OFF);

	/* Configure the pins connected to GPS  EXT Antenna control
	 default initial state to High (ON).*/
	ioport_set_pin_dir(PIN_EXTANT_GPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EXTANT_GPS, 1 ? PIN_EXTANT_GPS_ON : PIN_EXTANT_GPS_OFF);
	
	hw_ctrl_3v6_Reg(FALSE);//USE Battery
	
	hw_ctrl_p5v_Power(FALSE);

}

void aptStart_task (void * pvParameters) {
	
	static unsigned portBASE_TYPE highwater = 0;
	static uint16_t iADCDelay_Batt = 0;
	static uint16_t iADCDelay_ExtP = 0;
	static bool	bBatter_Used = FALSE;
	static bool	bExtPow_Used = TRUE;
	static FIL hFile;
	static FRESULT res;
	static uint32_t gps_Hibernate_delay = 0;
	
	static float	vADC_ExtPower = 0;
	static float	vADC_Battery = 0;

	static uint32_t * pWdCount = NULL;
		
	pWdCount = (uint32_t*)pvParameters;
	
	vPrintEnable();
	vPrintTraceSet(255,0);//Disable ALL
	
	at45dbx_init();

	if(!at45dbx_mem_check()) {
		/* Capture error. */
		ioport_set_pin_level(LED_MCU, LED_ACTIVE_LEVEL);
		ioport_set_pin_level(LED_GSM, LED_ACTIVE_LEVEL);
		ioport_set_pin_level(LED_GPS, LED_ACTIVE_LEVEL);
		ioport_set_pin_level(LED_SEND, LED_ACTIVE_LEVEL);
		ioport_set_pin_level(LED_IG,  LED_ACTIVE_LEVEL);
		ioport_set_pin_level(LED_PTO, LED_ACTIVE_LEVEL);
		ioport_set_pin_level(LED_OPT, LED_ACTIVE_LEVEL);
		while(1){
			vPuts("\r\nSYSERR: BUFFER Not Found!!!\r\n");
			vTaskDelay(1000);
		}
	}
	
	if(gpbr_read(GPBR_FDISK_ID)) {
		gpbr_write(GPBR_FDISK_ID,0);
		fat_system_mountdisk(LUN_ID_0,1);//Format
		software_reset();
	}else fat_system_mountdisk(LUN_ID_0,0);
	
	vTaskDelay(500);
	
	fat_system_printdiskfree(LUN_ID_0);

	adl_flash_init(LUN_ID_0);
	
	res = f_open(&hFile, (char const *)__BOOT_DIAG_PATH__, FA_READ | FA_OPEN_EXISTING);
	if (res == FR_OK) {
		uint8_t wd_counter = 0;
		size_t bytes = 0;
		
		f_read(&hFile, &wd_counter, 1, &bytes);
		f_close(&hFile);
		f_unlink(__BOOT_DIAG_PATH__);
		apt_startup_mode  = APT_STARTUP_FOR_QC_MODE;
		if(wd_counter <= 1 )WDOG_TestPassed = TRUE;
		
		vPrintTraceSet(255,0);//Disable ALL

		//HELLO_PRINT_QC(__DATE__, __TIME__);
		//HELLO_PRINT_QC("Dec 28 2016", "17:28:10");
		HELLO_PRINT_QC("Aug 24 2017", "17:28:10");
		
		aptBuzzer_qc();
	}
	else {

		//HELLO_PRINT_APP(__DATE__, __TIME__);
		//HELLO_PRINT_APP("Dec 28 2016", "17:28:10");
		HELLO_PRINT_APP("Aug 24 2017", "17:28:10");
	}
	
	vTaskDelay(500);
	
	hw_ctrl_p5v_Power(TRUE);
	
	i2c_reset();
	
	buffer_init();

	arc_shell_Subscribe(shellconfig_changed);
	
	INI_CONFIG_LOAD();
	
	swd_feature_Init(ini_config_utils.DTCUniq.sFwName);

	adl_CmdStackHandler_init();
	
	misc_cmd_Subscribe();
	
	if(apt_startup_mode  != APT_STARTUP_FOR_QC_MODE) {
		if(ini_config_utils.Output1.Logic) swdio_set_outp1_status(0);//Connected to NC Relay
		else swdio_set_outp1_status(1);//Connected to NC Relay
		if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(0);
		else {
			if(ini_config_utils.Output2.Logic) swdio_set_outp2_status(1);
			else swdio_set_outp2_status(0);
		}

		vPrintTraceSet(31,1);//Enable  #GPSDATA:
		vPrintTraceSet(30,1);//Enable  General Infomation
		
		apt_startup_mode = APT_STARTUP_NORMAL;
		
		aptStart_Initialized = true;
	}
	else {
		
		uint8_t fram_data = 0;
		if(drv_fram24c16_Read(0, 1, &fram_data) == OK)FRAM_TestPassed = TRUE;
	}
	
	
	for(;;) {//1 ms Step
		
		*pWdCount = 0;//Clear watchdog
		
		if(apt_startup_mode == APT_STARTUP_FOR_QC_MODE) {
			test_qc(NULL);
			vTaskDelay(1000);
			continue;
		}
		
		if(printTraceOn==1) {
			vPrintTraceSet(255,0);//Disable ALL
			vPrintTraceSet(31,1);//Enable  #GPSDATA:
		}
		if(printTraceOn) printTraceOn--;

		vADC_ExtPower = aptAfec_get(APT_AFEC_EXT_POWER_CH);
		vADC_Battery = aptAfec_get(APT_AFEC_BAT_POWER_CH);
		
		aptTracking_Runtime.BATT_Voltage = (int)vADC_Battery;
		aptTracking_Runtime.MAIN_Voltage = (int)vADC_ExtPower;
		
		if(vADC_ExtPower < VEXT_MIN_THRESHOLD){//Threshold for switch to internal Battery 
			iADCDelay_ExtP = 0;
			if(bBatter_Used==FALSE)iADCDelay_Batt++;
			else iADCDelay_Batt = 0;
			
			if(iADCDelay_Batt >= 1){//Battery is the most priority, So fast switched
				iADCDelay_Batt = 0;
				bBatter_Used = TRUE;
				bExtPow_Used = FALSE;
				hw_ctrl_3v6_Reg(FALSE);//USE Battery
				hw_ctrl_p5v_Power(FALSE);
				battery_isOperating=TRUE;
				vPrintf("\r\nSwitch Supply-Backup: %.3f, %.3f\r\n",vADC_ExtPower,vADC_Battery);
			}
		}
		else {//Switch to external power supply
			iADCDelay_Batt = 0;
			if(bExtPow_Used==FALSE)iADCDelay_ExtP++;
			else iADCDelay_ExtP = 0;
			
			if(iADCDelay_ExtP >= 10000){// 10 Second delay before switch to External Supply
				iADCDelay_ExtP = 0;
				bBatter_Used = FALSE;
				bExtPow_Used = TRUE;
				hw_ctrl_3v6_Reg(TRUE);//USE DC EXT
				hw_ctrl_p5v_Power(TRUE);
				battery_isOperating=FALSE;
				vPrintf("\r\nSwitch Supply-Main: %.3f, %.3f\r\n",vADC_ExtPower,vADC_Battery);
				if(aptStart_isHibernated)  {
					aptStart_isHibernated = false;
					/* Perform the software reset. */
					rstc_start_software_reset(RSTC);
				}
			}
		}
		
		if(aptTracking_Runtime.GPS.fixDimnt>=3) {
			gps_Hibernate_delay = gps_Hibernate_delay >= 4294967295 ? 4294967295 : gps_Hibernate_delay + 1;
		}
			
		if(swd_feature_isEnable(FEAT_GPS_HIBERNATED) && battery_isOperating) {
			
			static uint8_t mul = 1;
			
			if(gps_Hibernate_delay > _1mSecTick_Second(60 * mul)) {
				
				gps_Hibernate_delay = 0;
				
				mul = 1;
				
				while(hw_ctrl_gps_Wakeup()) { // GPS Hibernated  Trigger
					ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_ON);
					vTaskDelay(200);
					ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);
					vTaskDelay(200);
				}
				
				vTaskDelay(1000);
				
				vTaskSuspendAll();
				
				aptStart_isHibernated = true;
				
				uint16_t rtc_batt_charge_count = 0;
				
				sleep_entry('W', 1, true );//Sleep 1 Sec with initial IO low power
				
				do {
					afec_disable_interrupt(AFEC0, AFEC_INTERRUPT_ALL);
					afec_disable(AFEC0);//Sleep manager inside this function
					
					rtc_batt_charge_count++;
					
					if(rtc_batt_charge_count == 12*15) {
						gps_init_board();//On GPS Power for charge RTC Backup BATT
						vTaskDelay (1000);
						while(!hw_ctrl_gps_Wakeup()) { // GPS Wakeup  Trigger
							ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_ON);
							vTaskDelay(200);
							ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);
							vTaskDelay(200);
						}
						printf("\r\n GPS Acquired Signal\r\n");
						sleep_entry('W', 5, false );//Sleep 5 Sec without initial IO
					}
					if(rtc_batt_charge_count == 12*16) {
						while(hw_ctrl_gps_Wakeup()) { // GPS Hibernated  Trigger
							ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_ON);
							vTaskDelay(200);
							ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);
							vTaskDelay(200);
						}
						printf("\r\n GPS Hibernated\r\n");
						sleep_entry('W', 5, false );//Sleep 5 Sec without initial IO
					}
					else if(rtc_batt_charge_count == 12*17) {
						printf("\r\n GPS RTC Stop charged\r\n");
						sleep_entry('W', 5, true );//Sleep 5 Sec with initial IO low power
						rtc_batt_charge_count = 0;
					}
					else sleep_entry('W', 5, false );//Sleep 5 Sec without initial IO
					
					aptAfec_SetupADC();
					
					afec_start_software_conversion(AFEC0);
					
					do { vTaskDelay(10);
					}while(afec_get_interrupt_status(AFEC0) & (1 << AFEC_CHANNEL_3));
					
					vADC_ExtPower = aptAfec_get(APT_AFEC_EXT_POWER_CH);
					vADC_Battery = aptAfec_get(APT_AFEC_BAT_POWER_CH);
					
					printf("\r\nHibernated: %.3f, %.3f\r\n",vADC_ExtPower,vADC_Battery);
					
				}while(vADC_ExtPower < VEXT_MIN_THRESHOLD);

				
				if(vADC_ExtPower >= VEXT_MIN_THRESHOLD){
					aptStart_isHibernated = false;
					/* Perform the software reset. */
					rstc_start_software_reset(RSTC);
				}
				else {
					gps_init_board();
					vTaskDelay (1000);
					while(!hw_ctrl_gps_Wakeup()) { // GPS Wakeup  Trigger
						ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_ON);
						vTaskDelay(200);
						ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);
						vTaskDelay(200);
					}
					printf("\r\nWakeUp: %.3f, %.3f\r\n",vADC_ExtPower,vADC_Battery);
					
					mul = 5;//24MHZ x 5  = 120 MHZ
					
					//xTaskResumeAll();
				}
			}//else if(!(gps_Hibernate_delay%100))printf("\r\nHibernate Delay: %lu\r\n",gps_Hibernate_delay);
		}
		
		switch(adl_flash_getErr()) {
		case FR_DISK_ERR:
		case FR_INT_ERR:
		case FR_NOT_READY:
		case FR_INVALID_NAME:
		case FR_INVALID_OBJECT:
		case FR_WRITE_PROTECTED:
		case FR_INVALID_DRIVE:
		case FR_NOT_ENABLED:
		case FR_NO_FILESYSTEM:
		case FR_MKFS_ABORTED:
		case FR_NOT_ENOUGH_CORE:
		case FR_TOO_MANY_OPEN_FILES:
		case FR_INVALID_PARAMETER:
		case FR_LOCKED:
			vPutsTrace(30,"Dfrag------Need");
			break;
			adl_atCmdSubscribeExecute("AT@DFRAG",ADL_PORT_UART1);
			break;
		case FR_OK:
			break;
		}
		
		if(os_stackOverFlow()||os_memAllocFailed()||os_assertFailed()) {
			vTaskDelay(100);
			software_reset();
		}
		
		static uint16_t chk_task_delay = 0;
		static uint16_t task_check_nb = 0;
		if(chk_task_delay >= _1mSecTick_Second(1)) {
			chk_task_delay = 0;
			if( task_check_nb < all_task_created_nb) {
				if(tassk_handle[task_check_nb]) {
					highwater = uxTaskGetStackHighWaterMark( tassk_handle[task_check_nb] );
					if(highwater < configMINIMAL_STACK_SIZE)
					vPrintf("\r\nTask-%d: Low Memory<%lu Byte>\r\n",task_check_nb+1,highwater);
					else if(wd_monitor[task_check_nb] > _1mSecTick_Second(3))
					vPrintf("\r\nTask-%d: Timeout %d ms\r\n",task_check_nb+1,wd_monitor[task_check_nb]);
					else if(task_handle_printed[task_check_nb] == false) {
						for(uint8_t i = 0 ; i < all_task_created_nb ; i++ ) {
							if(task_handle_printed[i]==false){
								vPrintf("\r\nTask-%d: %s Start @ 0x%x",i+1,pcTaskGetTaskName(tassk_handle[i]),tassk_handle[i]);
								task_handle_printed[i] = true;
							}
						}
					}
				}
				task_check_nb++;
			}
			else {
				task_check_nb = 0;
				vPrintfTrace((30,"SYS: %lu, %ld Task, %d Free, Uptime = %d+%d:%d:%d", os_getIdleHook()/all_task_created_nb,
						uxTaskGetNumberOfTasks(), xPortGetFreeHeapSize(),uptime_dd,uptime_hh,uptime_mm,uptime_ss));
			}
			
		}else chk_task_delay++;
		
		vTaskDelay(1);
	}
}