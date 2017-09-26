/*
 * swd_event.c
 *
 * Created: 3/25/2015 10:52:11 AM
 *  Author: Pan
 */ 
#include "adl_global.h"
#include "swd_event.h"
#include "apt_tracking.h"
#include "buffer.h"
#include "ini_config.h"
#include "apt_gateway.h"
#include "apt_tracking.h"
#include "misc_config.h"
#include "apt_swd.h"
#include "vprinter.h"
#include "buffer_Q512Flash.h"
#include "_lib_arcshell/arc_sh_swd.h"
#include "apt_hahd_gps.h"
#include "hdha_odo.h"
#include "card_authen.h"
#include "feature.h"
#include "swd_io.h"
#include "apt_buzzer.h"
#include "apt_led_send.h"


#define SWD_EVENT_TRACE		30

extern uint16_t				aptTracking_ha_Counter;
extern uint16_t				aptTracking_hd_Counter;

/*extern*/ bool bLivingInIdleMode = FALSE;
static u16 idle_min_counter = 0;
static u16 igoff_min_counter = 0;
static u16 report_counter = 0;
static u16 hahd_check_counter = 0;
static u16 backup_pwr_counter = 0;
static u8 staus_report = 0;


static u8 value_report(u8 event) {
	switch(event) {
	case SWD_EV10_PTO1_ON:		return 0x0A;break;
	case SWD_EV11_PTO1_OFF:		return 0x0B;break;
	case SWD_EV12_PTO2_ON:		return 0x0C;break;
	case SWD_EV13_PTO2_OFF:		return 0x0D;break;
	case SWD_EV14_PTO3_ON:		return 0x0E;break;
	case SWD_EV15_PTO3_OFF:		return 0x0F;break;
	case SWD_EV30_RUNNING:		return 0x1E;break;
	case SWD_EV31_IDLE:			return 0x1F;break;
	case SWD_EV32_EXIT_IDLE:	return 0x20;break;
	case SWD_EV33_IG_OFF:		return 0x21;break;
	case SWD_EV34_IG_ON:		return 0x22;break;
	case SWD_EV41_OVER_SPEED:	return 0x29;break;
	case SWD_EV50_REPORT:		return 0x32;break;
	case SWD_EV53_HARD_DE:		return 0x35;break;
	case SWD_EV54_HARD_AC:		return 0x36;break;
	case SWD_EV55_CARD_LOGIN:	return 0x37;break;
	case SWD_EV56_CARD_LOGOUT:	return 0x38;break;
	case SWD_EV60_RESET:		return 0x3C;break;
	case SWD_EV95_BACKUP:		return 0x5F;break;
	case SWD_EV96_SHUTDOWN:		return 0x60;break;
	default:break;
	}
	return 0x00;
}

static void swd_event_update1sec(void) {
	static u16 igoff_sec_counter = 0;
	static u16 idle_sec_counter = 0;
	
	UP_COUNTER_OVERFLOW_LIMIT(report_counter,TRUE,65535);
	UP_COUNTER_OVERFLOW_LIMIT(hahd_check_counter,TRUE,65535);
	UP_COUNTER_OVERFLOW_LIMIT(backup_pwr_counter,TRUE,65535);
	idle_sec_counter = (!bLivingInIdleMode) ? 0 : idle_sec_counter +1;
	igoff_sec_counter = (swdio_get_ig_status() == 1) ? 0 : igoff_sec_counter +1;

	if(idle_sec_counter >= 60) { // 1 minute
		UP_COUNTER_OVERFLOW_LIMIT(idle_min_counter,TRUE,65535);
		idle_sec_counter = 0;
	}
		
	if(igoff_sec_counter >= 60) { // 1 minute
		UP_COUNTER_OVERFLOW_LIMIT(igoff_min_counter,TRUE,65535);
		igoff_sec_counter = 0;
	}
	
}

void swd_event_update_report(int *sec_gps,int *sec_odo) {
	
	swd_event_update1sec();
	
	/* Report Battery */
	if(battery_isOperating ==TRUE) {
		if(backup_pwr_counter >= _1SecTick_Second(60)){
			report_counter = 0;
			backup_pwr_counter = 0;
			aptTracking_Runtime.Status.Event = SWD_EV95_BACKUP;
			aptTracking_Runtime.Status.R_Val = 100;
			aptTracking_Runtime.Status.S_Val = 0;
			aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
			apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
			vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV95(BatteryBackup) Value = %d",
											aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
											aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
		}
	}else backup_pwr_counter =  _1SecTick_Second(30);
	
	/* Report IG ON */
	if(swdio_get_ig_status() == 1) {
		if(report_counter >= _1SecTick_Second(SWD_REPORT_IG_ON_PERIOD*60)) {
		//if(report_counter >= _1SecTick_Second(1)) {
			report_counter = 0;
			aptTracking_Runtime.Status.Event = SWD_EV50_REPORT;
			aptTracking_Runtime.Status.R_Val = value_report(staus_report);
			aptTracking_Runtime.Status.S_Val = 0;
			aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
			apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
			vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV50(Report IG On) Value = %d",
											aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
											aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
		}
	}
	else {
		if(report_counter >= _1SecTick_Second(SWD_REPORT_IG_OFF_PERIOD*60)) {
			report_counter = 0;
			aptTracking_Runtime.Status.Event = SWD_EV50_REPORT;
			aptTracking_Runtime.Status.R_Val = value_report(staus_report);
			aptTracking_Runtime.Status.S_Val = 0;
			aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
			apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
			vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV50(Report IG Off) Value = %d",
											aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
											aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
		}
	}

	/* Report HA HD */
	if(hahd_check_counter >= _1SecTick_Second(1)) {
		s8 hahd_state = 0;
		int gps_speed = 0xFE;
		int odo_speed = 0xFE;
		u16 HA = 25,HD = 25;
		uint32_t rpm = swdio_get_pto2_freq();
		
		if(ini_config_utils.DTCUniq.Harshing.HA)HA = ini_config_utils.DTCUniq.Harshing.HA;
		if(ini_config_utils.DTCUniq.Harshing.HD)HD = ini_config_utils.DTCUniq.Harshing.HD;
		
		hahd_state = aptHdhaGps_state(aptTracking_Runtime.GPS.fixDimnt,aptTracking_Runtime.GPS.speed,
									aptTracking_Runtime.GPS.hDop,aptTracking_Runtime.GPS.satUsed,aptTracking_Runtime.GPS.course,HA,HD,1);
		
		if(!ini_config_utils.DTCUniq.HAHDCalibrate.already) {
			if(aptTracking_Runtime.GPS.fixDimnt) {
				ini_config_utils.DTCUniq.HAHDCalibrate.calValue = hdha_odo_calibrate(rpm,aptTracking_Runtime.GPS.speed,ini_config_utils.OverSpeed.Value);
				if(ini_config_utils.DTCUniq.HAHDCalibrate.calValue > 0 ) {
					ascii StingCmd[64] = {""};
					ini_config_utils.DTCUniq.HAHDCalibrate.already = TRUE;
					wm_sprintf(StingCmd,"AT#HDCAL=\"1\",\"%.3f\"",ini_config_utils.DTCUniq.HAHDCalibrate.calValue);
					arc_shell_execute( StingCmd);
					aptBuzzer_alert(4);
					vPrintfTrace((30,"HDCAL: %0.4f <RPM = %d & GPS-SPEED = %d>\r\n",ini_config_utils.DTCUniq.HAHDCalibrate.calValue,rpm,aptTracking_Runtime.GPS.speed));
					odo_speed = (int) ini_config_utils.DTCUniq.HAHDCalibrate.calValue * rpm;
				}
			}
		}else odo_speed = (int) ini_config_utils.DTCUniq.HAHDCalibrate.calValue * rpm;
		
		if(swd_feature_isEnable(FEAT_HAHD_GPS)) {// HAHD GPS Feature enabled
			if(aptTracking_Runtime.GPS.fixDimnt) {
				gps_speed = aptHahdGps_sec_speed_get();// Use Speed from calculation
				aptTracking_Runtime.HarshSpeed = gps_speed;
			}else aptTracking_Runtime.HarshSpeed = aptTracking_Runtime.GPS.speed;
		}
		else if(swd_feature_isEnable(FEAT_HAHD_ODO)) {// HAHD ODO Feature enabled
			if(ini_config_utils.DTCUniq.HAHDCalibrate.already) {
				aptTracking_Runtime.HarshSpeed = odo_speed;
			}else aptTracking_Runtime.HarshSpeed = aptTracking_Runtime.GPS.speed;
		}
		else aptTracking_Runtime.HarshSpeed = aptHahdGps_sec_speed_get();// Use Speed from calculation

		switch(hahd_state) {
			case 1:
			{
				float before,after;
				uint16_t val = 0;
				aptHdhaGps_speed_buffer(&before,&after);
				aptTracking_Runtime.Status.Event = SWD_EV53_HARD_DE;
				aptTracking_Runtime.Status.R_Val = ini_config_utils.DTCUniq.Harshing.HD;
				val = (uint8_t)before;
				val = (val << 8) | ((uint8_t)after);
				aptTracking_Runtime.Status.S_Val = val;
				aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
				UP_COUNTER_OVERFLOW_LIMIT(aptTracking_hd_Counter,TRUE,65535);

				vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV53(HARD DEACC) Value = %d",
												aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
												aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.S_Val));
				apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
			}
			break;
			case 2:
			{
				float before,after;
				uint16_t val = 0;
				aptHdhaGps_speed_buffer(&before,&after);
				aptTracking_Runtime.Status.Event = SWD_EV54_HARD_AC;
				aptTracking_Runtime.Status.R_Val = ini_config_utils.DTCUniq.Harshing.HA;
				val = (uint8_t)before;
				val = (val << 8) | ((uint8_t)after);
				aptTracking_Runtime.Status.S_Val = val;
				aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
				UP_COUNTER_OVERFLOW_LIMIT(aptTracking_ha_Counter,TRUE,65535);
				
				vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV54(HARD ACC) Value = %d",
				aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
				aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
				apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
			}
			break;
			default:
			break;
		}
		
		
		aptHahdGps_sec_speed_update();

		hahd_check_counter = 0;
		
		*sec_gps = gps_speed;
		*sec_odo = odo_speed;
	}
}




void swd_event_keep_status(u8 event, u16 casing) {
	switch(event) {
	case STARTUP_LECACY_EVCODE:
		{ /* STARTUP*/
			aptTracking_Runtime.Status.Event = SWD_EV60_RESET;
			aptTracking_Runtime.Status.R_Val = SWD_PARK_CODE_VALUE;
			aptTracking_Runtime.Status.S_Val = 0;
			aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
			aptTracking_save_idle_LatLon();
			apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
			vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV60(Reset) Value = %d",
											aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
											aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
		}
		break;
	case DINPUT_LECACY_EVCODE: 
		{ /* IO_CHANGE_EVENT*/
			/* PTO1 */
			if(casing == SWD_IO_CHANGE_PTO1) {
				report_counter = 0;
				if(aptTracking_Runtime.digGPI & 0x02) {// Now PTO1 is On
					swdio_light_pto_set(1);
					aptTracking_Runtime.Status.Event = SWD_EV10_PTO1_ON;
					aptTracking_Runtime.Status.R_Val = 0;
					aptTracking_Runtime.Status.S_Val = 0;
					aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
					apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
					vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV10(PTO1 On) Value = %d",
													aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
													aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
				
				}
				else {
					swdio_light_pto_set(0);
					aptTracking_Runtime.Status.Event = SWD_EV11_PTO1_OFF;
					aptTracking_Runtime.Status.R_Val = 0;
					aptTracking_Runtime.Status.S_Val = 0;
					aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
					apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
					vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV11(PTO1 Off) Value = %d",
													aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
													aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
				}
			}
		
			/* PTO2 */
			if(casing == SWD_IO_CHANGE_PTO2) {
				report_counter = 0;
				if(aptTracking_Runtime.digGPI & 0x04) {// Now PTO2 is On
					if(swd_feature_isEnable(FEAT_OPT_LIGHT_BY_PTO2))swdio_light_opt_set(1);
					aptTracking_Runtime.Status.Event = SWD_EV12_PTO2_ON;
					aptTracking_Runtime.Status.R_Val = 0;
					aptTracking_Runtime.Status.S_Val = 0;
					aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
					apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
					vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV12(PTO2 On) Value = %d",
													aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
													aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
				
				}
				else {
					if(swd_feature_isEnable(FEAT_OPT_LIGHT_BY_PTO2))swdio_light_opt_set(0);
					aptTracking_Runtime.Status.Event = SWD_EV13_PTO2_OFF;
					aptTracking_Runtime.Status.R_Val = 0;
					aptTracking_Runtime.Status.S_Val = 0;
					aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
					apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
					vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV13(PTO2 Off) Value = %d",
													aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
													aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
				}
			}
			
			/* PTO3 */
			if(casing == SWD_IO_CHANGE_PTO3) {
				report_counter = 0;
				if(aptTracking_Runtime.digGPI & 0x08) {// Now PTO3 is On
					aptTracking_Runtime.Status.Event = SWD_EV14_PTO3_ON;
					aptTracking_Runtime.Status.R_Val = 0;
					aptTracking_Runtime.Status.S_Val = 0;
					aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
					apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
					vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV12(PTO3 On) Value = %d",
													aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
													aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
		
				}
				else {
					aptTracking_Runtime.Status.Event = SWD_EV15_PTO3_OFF;
					aptTracking_Runtime.Status.R_Val = 0;
					aptTracking_Runtime.Status.S_Val = 0;
					aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
					apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
					vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV13(PTO3 Off) Value = %d",
													aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
													aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
				}
			}
		
			/* IG */
			if(casing == SWD_IO_CHANGE_IG) {
				report_counter = 0;
				if(aptTracking_Runtime.digGPI & 0x01) {// Now IG is On
					swdio_light_ig_set(1);
					aptSwd_submit_licensecard_alert(TRUE);
					staus_report = SWD_EV34_IG_ON;
					aptTracking_Runtime.Status.Event = SWD_EV34_IG_ON;
					aptTracking_Runtime.Status.R_Val = igoff_min_counter;
					aptTracking_Runtime.Status.S_Val = igoff_min_counter;
					aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
					igoff_min_counter = 0;
					apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
					vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV34(IG On) Value = %d",
													aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
													aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
				
				}
				else {// Now IG is Off
					if(bLivingInIdleMode == TRUE) { //Exit Idle
						bLivingInIdleMode = FALSE;
						aptTracking_Runtime.Status.Event = SWD_EV32_EXIT_IDLE;
						aptTracking_Runtime.Status.R_Val = idle_min_counter + SWD_IDLE_CODE_VALUE;
						aptTracking_Runtime.Status.S_Val = idle_min_counter + SWD_IDLE_CODE_VALUE;
						aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
						idle_min_counter = 0;
						aptTracking_backward_runtime(1);// backward 1 second
						aptTracking_load_idle_LatLon();// exit idle position same as idle
						apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
						aptTracking_backward_runtime(0);// restore to real time
						vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV32(Exit Idle) Value = %d",
														aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
														aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
					}
					// IG Off
					swdio_light_ig_set(0);
					aptLedSend_igoff();
					card_authenLicenseID(NULL);//Logout
					staus_report = SWD_EV33_IG_OFF;
					aptTracking_Runtime.Status.Event = SWD_EV33_IG_OFF;
					aptTracking_Runtime.Status.R_Val = SWD_PARK_CODE_VALUE;
					aptTracking_Runtime.Status.S_Val = 0;
					aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
					apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
					vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV33(IG Off)",
													aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
													aptTracking_Runtime.tmRTC.second));
												
					
				}
			}
		}
		break;
	case IDLING_LECACY_EVCODE:
		{ /* IDLE_EVENT*/
			if(bLivingInIdleMode == FALSE) { //Enter Idle
				report_counter = 0;
				bLivingInIdleMode = TRUE;
				staus_report = SWD_EV31_IDLE;
				aptTracking_Runtime.Status.Event = SWD_EV31_IDLE;
				aptTracking_Runtime.Status.R_Val = SWD_IDLE_CODE_VALUE;
				aptTracking_Runtime.Status.S_Val = 0;
				aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
				aptTracking_save_idle_LatLon();
				apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
				vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV31(Idle)",
												aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
												aptTracking_Runtime.tmRTC.second));				
			}
		}
		break;
	case OVERSPEED_LECACY_EVCODE: 
		{ /* OVERSPEED_EVENT */
			report_counter = 0;
			bLivingInIdleMode = FALSE;
			staus_report = SWD_EV41_OVER_SPEED;
			aptTracking_Runtime.Status.Event = SWD_EV41_OVER_SPEED;
			aptTracking_Runtime.Status.R_Val = ini_config_utils.OverSpeed.Value;
			aptTracking_Runtime.Status.S_Val = 0;
			aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
			apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
			vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV41(Over Speed) Speed = %d, Distance = %d",
											aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
											aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.GPS.speed,
											aptTracking_Runtime.TotalDistance));
		}
		break;
	case DRIVING_LECACY_EVCODE: 
		{ /* RUNNING_EVENT */
			report_counter = 0;
			if(bLivingInIdleMode == TRUE) { //Exit Idle
				bLivingInIdleMode = FALSE;
				aptTracking_Runtime.Status.Event = SWD_EV32_EXIT_IDLE;
				aptTracking_Runtime.Status.R_Val = idle_min_counter + SWD_IDLE_CODE_VALUE;
				aptTracking_Runtime.Status.S_Val = idle_min_counter + SWD_IDLE_CODE_VALUE;
				aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
				idle_min_counter = 0;
				aptTracking_backward_runtime(1);// backward 1 second
				aptTracking_load_idle_LatLon();// exit idle position same as idle
				apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
				aptTracking_backward_runtime(0);// restore to real time
				vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV32(Exit Idle) Value = %d",
												aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
												aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.Status.R_Val));
			}

			staus_report = SWD_EV30_RUNNING;
			aptTracking_Runtime.Status.Event = SWD_EV30_RUNNING;
			aptTracking_Runtime.Status.R_Val = ini_config_utils.Sampling.DriveMode;
			aptTracking_Runtime.Status.S_Val = 0;
			aptTracking_Runtime.Status.Trk3id = card_authenGetTrack3Tabid();
			apt_buffer_cpe_add(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
			vPrintfTrace((30,"%02d:%02d:%02d KEEP EVENT SWD-EV30(Driving) Speed = %d, Distance = %d",
											aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,
											aptTracking_Runtime.tmRTC.second,aptTracking_Runtime.GPS.speed,
											aptTracking_Runtime.TotalDistance));
				
		}
		break;
	default:
		break;
	}
}


s32 swd_event_InsertCommData(ascii * commData) {
	static aptTracking_runtime_t runtime_temp;
	static char strhexBuff[513] = {""};
	int ret = 0;
	
	memcpy(&runtime_temp,&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
	if((aptTracking_Runtime.Status.Event!=SWD_EV30_RUNNING)||(aptTracking_Runtime.Status.Event!=SWD_EV50_REPORT)) {
		if(!(aptTracking_Runtime.digGPI&0x01))  {
			runtime_temp.Status.Event = SWD_EV50_REPORT;
			runtime_temp.Status.R_Val = value_report(SWD_EV33_IG_OFF);
		}
		else {
			if(bLivingInIdleMode==TRUE) {
				runtime_temp.Status.Event = SWD_EV50_REPORT;
				runtime_temp.Status.R_Val = value_report(SWD_EV31_IDLE);
			}
			else {
				runtime_temp.Status.Event = SWD_EV30_RUNNING;			
				runtime_temp.Status.R_Val = ini_config_utils.Sampling.DriveMode;
			}
		}
	}
	
	ret = transform_to_sharpL(strhexBuff,&runtime_temp,commData);
	vPrintf("\r\nCPL: Add #COMM [%d Byte]\r\n",ret);
	//lvPutsLabel(30,"#LCOMM>> ",strhexBuff,ret);
	if(apt_buffer_cpl_fully()) ret = (-2);
	else ret = apt_buffer_cpl_add((u8*)strhexBuff, ret);
	return ret;
}