/*
 * apt_basis.c
 *
 * Created: 22/1/2558 9:53:13
 *  Author: pan
 */ 
#include "adl_global.h"
#include "misc_config.h"
#include "apt_tracking.h"
#include "ini_config.h"
#include "buffer.h"
#include "trans_form.h"
#include "distance_calc.h"
#include "apt_gateway.h"
#include "feature.h"
#include "swd_event.h"
#include "_lib_gps/nmea_gps.h"
#include "_lib_gps/nmea.h"
#include "apt_swd.h"
#include "apt_gps_data.h"
#include "vprinter.h"
#include "apt_afec.h"
#include "middleValue_sort.h"
#include "swd_io.h"
#include "_lib_arcnethl/arc_hlftp.h"
#include "_lib_arcnethl/arc_hlhttp.h"
#include "_lib_arcnethl/arc_hlgsm.h"
#include "_lib_arcnethl/arc_hltcp.h"
#include "xm0110_compat.h"
#include "geofence.h"
#include "apt_sms.h"
#include "error.h"
#include "os.h"
#include "apt_gps_wakeup.h"
#include "apt_afec.h"
#include "_lib_bootld/boot_trigger.h"

#define APT_TRACKING_TRACE		5


#define APT_TRACKING_24H_inSec	(24*60)//(24*3600)
#define APT_TRACKING_7H_inSec	(7*60)//(7*3600)
#define APT_TRACKING_4H_inSec	(4*60)//(4*3600)
#define APT_TRACKING_30M_inSec	(30)//(30*60)

#define APT_TRACKING_PLAY_SOUND_WAIT 15


#define APT_TRACKING_ADC_BUFF_SIZE	50

/*extern*/aptTracking_runtime_t 	aptTracking_Runtime;

/*extern*/aptTracking_runtime_t 	aptTracking_Realtime;

/*extern*/ uint16_t				aptTracking_tempsensor_timeout = 0;
/*extern*/ uint16_t				aptTracking_rfid_timeout = 0;
/*extern*/ uint16_t				aptTracking_ha_Counter = 0;
/*extern*/ uint16_t				aptTracking_hd_Counter = 0;
/*extern*/ arc_bootld_init_t		aptTracking_bootInfo = {FW_LOCAL,FALSE,SWD2015_FW_VERSION,""};
	

/*extern*/aptTracking_driver_t		aptTracking_Driver[GPBM_LICENSE_TRK3_MAX_ID];
	
static bool						APT_TRACKING_RTC_isUpToDate = FALSE;
static bool						APT_TRACKING_TaskCompleted = FALSE;

static int						APT_TRACKING_SEC_GPS_Speed, APT_TRACKING_SEC_ODO_Speed = 0xFE;
static aptTracking_runtime_t	APT_TRACKING_IDLE_Info;
static aptTracking_runtime_t	APT_TRACKING_RuntimeHistory[5];
static ini_config_utils_t 		APT_TRACKING_cfgCompare =	{
														{0,0},//OverSpeed
														{0,0},//Sampling Drive mode
														{0,0},//Input1
														{0,0},//Input2
														{0,0},//Input3
														{0,0},//Input4
														{0,0},//Output1
														{0,0},//Output2
														{"","","",""},//SMS Server , User1-4, SMS-FeatureMask
														{FALSE,FALSE},//CtrlRequest
														{0,0},//Idling
														{"","","",FALSE,"","",
														{25/*default HA=25*/,25/*default HD=25*/},{FALSE,0}/*HAHD Calibrate*/,0x00/*Option Mask*/,0x00/*VEH_TYPE*/,0x00/*VEH_ATTR*/}
};

static u8 APT_TRACKING_PTO3_iLogic = 0;

static geofence_t APT_TRACKING_GeofenceTab[GEOFENCE_MAX_TABLE_ID];
static u8 APT_TRACKING_GeofenceVersion = 0;
static s16 APT_TRACKING_nbGeofenceTab = 0;
static bool APT_TRACKING_bRTCSync = false;
static bool APT_TRACKING_bSoundplaying = false;

static uint8_t test_speed = 0;
static bool bSoundCmd_Error = false;

//extern char * geo_data;

extern uint8_t cpe_del_trigger;
extern uint8_t cpl_del_trigger;

/*
const char geo_data[] = {
	"01\r\n"
	"01,01,13.864917,100.451436,0a,C,00,00,00,00,00,00,00,2d,1e,05,05\r\n"
	"02,01,13.8762089,100.442132,0a,C,00,00,00,00,00,00,00,2d,1e,03,05\r\n"
	"03,01,13.876374,100.433244,0a,C,00,00,00,00,00,00,00,2d,1e,03,05\r\n"
	"04,01,13.9125113,100.4451246,0a,C,00,00,00,00,00,00,00,32,1e,05,05\r\n"
	"05,01,13.888890,100.450036,0a,C,00,00,00,00,00,00,00,28,1e,04,05\r\n"
	"06,01,13.918308,100.452031,0a,C,00,00,00,00,00,00,00,1e,1e,03,05\r\n"
	"07,01,13.926493,100.458876,0a,C,00,00,00,00,00,00,00,32,1e,05,05\r\n"
	"08,01,13.9125113,100.4451246,0a,C,00,00,00,00,00,00,00,28,1e,04,05\r\n"
	"09,01,13.9334323,100.4537721,0a,C,00,00,00,00,00,00,00,32,1e,05,05\r\n"
	"0a,01,13.9467607,100.458128,0a,C,00,00,00,00,00,00,00,28,1e,04,05\r\n"
	"0b,01,13.9515192,100.461765,0a,C,00,00,00,00,00,00,00,28,1e,04,05\r\n"
	"0c,01,13.9532789,100.4703588,0a,C,00,00,00,00,00,00,00,28,1e,04,05\r\n"
	"0d,01,13.9583683,100.4703913,0a,C,00,00,00,00,00,00,00,28,1e,04,05\r\n"
	"0e,01,13.9650423,100.4601024,0a,C,00,00,00,00,00,00,00,28,1e,04,05\r\n"
	"0f,01,13.9749869,100.4452368,0a,C,00,00,00,00,00,00,00,28,1e,04,05\r\n"
};
*/

bool aptTracking_SoundCmd_Error(void) {
	return bSoundCmd_Error;
}

bool aptTracking_task_completed(void) { 
	return APT_TRACKING_TaskCompleted;
}
	
void aptTracking_save_idle_LatLon(void) {
	memcpy(&APT_TRACKING_IDLE_Info,&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
}

void aptTracking_load_idle_LatLon(void) {
	aptTracking_Runtime.GPS.lat = APT_TRACKING_IDLE_Info.GPS.lat;
	aptTracking_Runtime.GPS.lon = APT_TRACKING_IDLE_Info.GPS.lon;
}

void aptTracking_backward_runtime(u8 atSecond) {
	if(atSecond<5) {
		aptTracking_Runtime.tmRTC.day = APT_TRACKING_RuntimeHistory[atSecond].tmRTC.day;
		aptTracking_Runtime.tmRTC.month = APT_TRACKING_RuntimeHistory[atSecond].tmRTC.month;
		aptTracking_Runtime.tmRTC.year = APT_TRACKING_RuntimeHistory[atSecond].tmRTC.year;
		aptTracking_Runtime.tmRTC.hour = APT_TRACKING_RuntimeHistory[atSecond].tmRTC.hour;
		aptTracking_Runtime.tmRTC.minute = APT_TRACKING_RuntimeHistory[atSecond].tmRTC.minute;
		aptTracking_Runtime.tmRTC.second = APT_TRACKING_RuntimeHistory[atSecond].tmRTC.second;
	}
}

bool aptTracking_CCLK_SyncTime(void) {
	static adl_rtcTime_t tTime;
	gps_pvtTimeDate_t * info = &aptGpsWakeup_PvtRawInfo.tTimeDateInfo;
	if(IS_A_GOOD_DATE(info->day,info->month,info->year)) {
		tTime.Day = info->day;
		tTime.Month = info->month;
		tTime.Year = info->year;
		tTime.Hour = info->hours;
		tTime.Minute = info->minutes;
		tTime.Second = info->seconds;
		if(adl_rtcSetTime(&tTime) >= 0){
			APT_TRACKING_bRTCSync = true;
			return TRUE;
		}
	}
	else {
		adl_rtcGetTime(&tTime);
		if(IS_A_GOOD_DATE(tTime.Day,tTime.Month,tTime.Year)) {
			APT_TRACKING_bRTCSync = true;
			return FALSE;
		}
	}
	return FALSE;
}

void aptTracking_print_secdata(void) {
	static s16 my_second = -1;
	int lat_deg,lat_min,lon_deg,lon_min;
	float hdhacal_value = 0; //temp
	ascii strBuff[130] = {""};
		
	lat_deg = aptTracking_Runtime.GPS.lat;
	lat_min = (aptTracking_Runtime.GPS.lat-lat_deg)*100000;
	lon_deg = aptTracking_Runtime.GPS.lon;
	lon_min = (aptTracking_Runtime.GPS.lon-lon_deg)*100000;
	
	if(my_second != aptTracking_Runtime.tmRTC.second) {
		my_second = aptTracking_Runtime.tmRTC.second;
		snprintf(strBuff,129,"\r\n#GPSDATA=%02d%02d%02d,%02d%02d%02d,"
							"%03d,%03d,"
							"%03d.%05d,%03d.%05d,"
							"%03.3f,%3.3f,%03.0f,%04.1f\r\n",
							aptTracking_Runtime.tmRTC.hour,aptTracking_Runtime.tmRTC.minute,aptTracking_Runtime.tmRTC.second,
							aptTracking_Runtime.tmRTC.day,aptTracking_Runtime.tmRTC.month,aptTracking_Runtime.tmRTC.year%100,
							APT_TRACKING_SEC_ODO_Speed,APT_TRACKING_SEC_GPS_Speed,
							lat_deg,lat_min,lon_deg,lon_min,
							(float)hdhacal_value,(float)aptTracking_Runtime.GPS.hDop,(float)aptTracking_Runtime.GPS.course,(float)aptTracking_Runtime.TotalDistance/1000);
		vPutsIntermediat(strBuff);
	}
}

static void APT_TRACKING_FTP_Handler(uint8_t conf_id, int8_t event, void *ctx) { }
static void APT_TRACKING_HTTP_Handler(int8_t event, void *ctx) {
	static u8 itry = 0;
	if(event != ARC_HLNET_EV_CONNECTED) {
		itry++;
		if(itry <= 3) arc_shell_execute("AT#HTTPFILE=1");//Repeat Again
		else {
			itry = 0;
			arc_shell_execute("AT#HTTPFILE=0");//Stop with failed
		}
	}
	else {//Success
		itry = 0;
		arc_shell_execute("AT#HTTPFILE=2");
		vTaskDelay(10);
		software_reset();
	}
}

static bool APT_TRACKING_RTC_Update(void) {
	static adl_rtcTime_t tstmp;
	adl_rtcGetTime(&tstmp);
	if(IS_A_GOOD_DATE(tstmp.Day,tstmp.Month,tstmp.Year)) {
		aptTracking_Runtime.tmRTC.year = tstmp.Year;
		aptTracking_Runtime.tmRTC.month = tstmp.Month;
		aptTracking_Runtime.tmRTC.day = tstmp.Day;
		aptTracking_Runtime.tmRTC.hour = tstmp.Hour;
		aptTracking_Runtime.tmRTC.minute = tstmp.Minute;
		aptTracking_Runtime.tmRTC.second = tstmp.Second;
		return TRUE;
	}
	return FALSE;
}


bool aptTracking_RuntimeGps_Update (gps_pvtInfo_t *info) {
	static uint16_t gps_sniCompare = 0;
	uint16_t gps_sniCurrent = 0;
	
	APT_TRACKING_RTC_isUpToDate = APT_TRACKING_RTC_Update();

	if(APT_TRACKING_RTC_isUpToDate) {
		if(aptTracking_Runtime.tmRTC.second != APT_TRACKING_RuntimeHistory[0].tmRTC.second) { //keep old data
			for(s8 i =3; i >= 0; i--) {memcpy( &APT_TRACKING_RuntimeHistory[i+1], &APT_TRACKING_RuntimeHistory[i], sizeof(aptTracking_runtime_t));}
			memcpy (&APT_TRACKING_RuntimeHistory[0], &aptTracking_Runtime, sizeof(aptTracking_runtime_t));	
		}
	}
	
	gps_sniCurrent = aptGpsData_getInfo(info);
	
	if((gps_sniCurrent == 0)||(gps_sniCurrent == gps_sniCompare)) return FALSE;//No Data Update
	
	gps_sniCompare = gps_sniCurrent;//Keep new
	
	aptTracking_Runtime.GPS.day = info->tTimeDateInfo.day;
	aptTracking_Runtime.GPS.month = info->tTimeDateInfo.month;
	aptTracking_Runtime.GPS.year = info->tTimeDateInfo.year;
	aptTracking_Runtime.GPS.hour = info->tTimeDateInfo.hours;
	aptTracking_Runtime.GPS.minute = info->tTimeDateInfo.minutes;
	aptTracking_Runtime.GPS.second = info->tTimeDateInfo.seconds;
	aptTracking_Runtime.GPS.satUsed = info->tSatellitesInfo.satsUsed;
	aptTracking_Runtime.GPS.hDop = info->tPositionInfo.hDop;
	aptTracking_Runtime.GPS.fixDimnt	= info->tPositionInfo.eFixState;
	aptTracking_Runtime.GPS.lat		= info->tPositionInfo.latitude;
	aptTracking_Runtime.GPS.lon		= info->tPositionInfo.longitude;
	//aptTracking_Runtime.GPS.speed		= info->pvt.tPositionInfo.speed*3.600;//convert m/s to KM/h
	aptTracking_Runtime.GPS.speed		= info->tPositionInfo.speed;
	aptTracking_Runtime.GPS.course	= info->tPositionInfo.course;
	return TRUE;
}

static void APT_TRACKING_ReLoadMemData(void) {
	ascii strRsp[100]= {"No Backup Data Saved"};
	aptTracking_runtime_t	restore;
	if(apt_buffer_cpe_read_GPBMEM(&restore,sizeof(aptTracking_runtime_t))>0) {
		APT_TRACKING_cfgCompare.Input1.Logic	 =	((restore.digGPI & 0x0001)>>0);//IG
		APT_TRACKING_cfgCompare.Input2.Logic	 =	((restore.digGPI & 0x0002)>>1);//PTO1
		APT_TRACKING_cfgCompare.Input3.Logic	 =	((restore.digGPI & 0x0004)>>2);//PTO2

		aptTracking_Runtime.digGPI			 =	restore.digGPI;
		aptTracking_Runtime.GPS.lat			 = 	restore.GPS.lat;
		aptTracking_Runtime.GPS.lon			 = 	restore.GPS.lon;
		
		aptTracking_Runtime.TotalDistance		 =	restore.TotalDistance;
		aptTracking_Runtime.TripTime			=	restore.TripTime;
		aptTracking_Runtime.TripDistance		=	restore.TripDistance;
		
		aptTracking_Runtime.IDLE_Counter      =   restore.IDLE_Counter;
		aptTracking_Runtime.IGON_Counter      =   restore.IGON_Counter;
		aptTracking_Runtime.OVSP_Counter      =   restore.OVSP_Counter;
		aptTracking_Runtime.PTO_Counter       =   restore.PTO_Counter;
		
		aptTracking_Runtime.MAXSpeedPerMin	=   restore.MAXSpeedPerMin;
		aptTracking_Runtime.OVSPCountPerMin	=   restore.OVSPCountPerMin;
		aptTracking_Runtime.HACountPerMin		=   restore.HACountPerMin;
		aptTracking_Runtime.HDCountPerMin		=   restore.HDCountPerMin;
		aptTracking_Runtime.DistancePerDay	=   restore.DistancePerDay;
		aptTracking_Runtime.DriveTimePerDay	=   restore.DriveTimePerDay;
		
		aptTracking_Runtime.RFID				=   restore.RFID;
		aptTracking_Runtime.TempSensor[0]		=   restore.TempSensor[0];
		aptTracking_Runtime.TempSensor[1]		=   restore.TempSensor[1];
		aptTracking_Runtime.TempSensor[2]		=   restore.TempSensor[2];
		aptTracking_Runtime.TempSensor[3]		=   restore.TempSensor[3];

		wm_sprintf(strRsp,"Backup Found: Distance = %lu",(unsigned long)aptTracking_Runtime.TotalDistance);
	}
	vPutsTrace(30,strRsp);
}

static bool APT_TRACKING_SOUND_PLAY_RSP_Handler(adl_atResponse_t *paras) {
	bSoundCmd_Error = TRUE;
	return FALSE;
}

static void APT_TRACKING_SOUND_PLAY(const char *cmd) {
	if(APT_TRACKING_bSoundplaying==false) {
		APT_TRACKING_bSoundplaying = true;
		adl_atCmdCreate((char*)cmd,ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),APT_TRACKING_SOUND_PLAY_RSP_Handler,"ERROR",NULL);
	}
}

static void APT_TRACKING_WarnningGeofence(void){
	static uint8_t test_id = 0;
	static distance_calc_coordinate_t p0,p1;
		
	p0.lat = aptTracking_Runtime.GPS.lat;
	p0.lon = aptTracking_Runtime.GPS.lon;
		
	p1.lat = APT_TRACKING_GeofenceTab[test_id].lat;
	p1.lon = APT_TRACKING_GeofenceTab[test_id].lon;
	
	if((p1.lat > 0.01)||(p1.lon > 0.01)) {
	
		u32	iTmpDist = (double) distance_calc_lac(p0,p1);
		
		if(iTmpDist < (APT_TRACKING_GeofenceTab[test_id].radius_meter*10)) {//Already in the area
			if(aptTracking_Runtime.GPS.speed/*test_speed*/  > APT_TRACKING_GeofenceTab[test_id].speed_value_before_warning){
				vPrintfTrace((30,"Geofence %d = %d meter , %d kph",test_id,iTmpDist, aptTracking_Runtime.GPS.speed));
				switch(APT_TRACKING_GeofenceTab[test_id].speed_value_before_warning) {
					case 30:
					//AT#AUDIOPLAY="b30.wav"
					APT_TRACKING_SOUND_PLAY("AT#AUDIOPLAY=\"war30.wav\"");
					//adl_atCmdCreate((char*)"AT#AUDIOPLAY=\"2.wav\"",ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);
					break;
					case 45:
					//AT#AUDIOPLAY="b45.wav"
					APT_TRACKING_SOUND_PLAY("AT#AUDIOPLAY=\"war45.wav\"");
					//adl_atCmdCreate((char*)"AT#AUDIOPLAY=\"3.wav\"",ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);
					break;
					case 60:
					//AT#AUDIOPLAY="b60.wav"
					APT_TRACKING_SOUND_PLAY("AT#AUDIOPLAY=\"war60.wav\"");
					//adl_atCmdCreate((char*)"AT#AUDIOPLAY=\"4.wav\"",ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);
					break;
				}
			}
		}
		else if(iTmpDist < ((APT_TRACKING_GeofenceTab[test_id].radius_meter*10) + 100)) {//Another 100 meters going to area
			if(aptTracking_Runtime.GPS.speed/*test_speed*/ > APT_TRACKING_GeofenceTab[test_id].speed_value_before_warning){
				vPrintfTrace((30,"Geofence %d = %d meter , %d kph",test_id,iTmpDist, aptTracking_Runtime.GPS.speed));
				switch(APT_TRACKING_GeofenceTab[test_id].speed_value_before_warning) {
				case 30:
					//AT#AUDIOPLAY="a30.wav"
					APT_TRACKING_SOUND_PLAY("AT#AUDIOPLAY=\"lim30.wav\"");
					//adl_atCmdCreate((char*)"AT#AUDIOPLAY=\"8.wav\"",ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);
					break;
				case 45:
					//AT#AUDIOPLAY="a45.wav"
					APT_TRACKING_SOUND_PLAY("AT#AUDIOPLAY=\"lim45.wav\"");
					//adl_atCmdCreate((char*)"AT#AUDIOPLAY=\"9.wav\"",ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);
					break;
				case 60:
					//AT#AUDIOPLAY="a60.wav"
					APT_TRACKING_SOUND_PLAY("AT#AUDIOPLAY=\"lim60.wav\"");
					//adl_atCmdCreate((char*)"AT#AUDIOPLAY=\"10.wav\"",ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);
					break;
				}
			}
		}
	}
	
	test_id = (test_id >= (GEOFENCE_MAX_TABLE_ID-1)) ? 0 : test_id + 1;
}

static void APT_TRACKING_1Sec_WarnningDriverTime(void) {
	
	for(uint8_t i = 0 ; i < GPBM_LICENSE_TRK3_MAX_ID ; i ++) {
		
		UP_COUNTER_OVERFLOW_LIMIT(aptTracking_Driver[i].IDActiveMinute,(aptTracking_Driver[i].id[0]),APT_TRACKING_24H_inSec);
		
		if(aptTracking_Driver[i].isLogin) {
			
			UP_COUNTER_OVERFLOW_LIMIT(aptTracking_Driver[i].TotalWorkMinute,true,APT_TRACKING_24H_inSec);
			UP_COUNTER_OVERFLOW_LIMIT(aptTracking_Driver[i].RoundWorkMinute,true,APT_TRACKING_24H_inSec);
			
			if(aptTracking_Driver[i].TotalWorkMinute > APT_TRACKING_7H_inSec){
				//Warning  work time over 7 hour per day
				APT_TRACKING_SOUND_PLAY("AT#AUDIOPLAY=\"7h.wav\"");
				//adl_atCmdCreate((char*)"AT#AUDIOPLAY=\"12.wav\"",ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);
			}
			else {
				if(aptTracking_Driver[i].RoundWorkMinute >= APT_TRACKING_4H_inSec) {
					if(aptTracking_Driver[i].RoundRestMinute < APT_TRACKING_30M_inSec) {
						//Warning to rest 30 minute
						APT_TRACKING_SOUND_PLAY("AT#AUDIOPLAY=\"4h.wav\"");
						//adl_atCmdCreate((char*)"AT#AUDIOPLAY=\"11.wav\"",ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);
					}else aptTracking_Driver[i].RoundWorkMinute = 0;
				}else aptTracking_Driver[i].RoundRestMinute = 0;
			}
		}
		else {//!aptTracking_Driver[i].isLogin
			if(aptTracking_Driver[i].id[0]) {
				UP_COUNTER_OVERFLOW_LIMIT(aptTracking_Driver[i].RoundRestMinute,true,APT_TRACKING_24H_inSec);
			}
			
			if(aptTracking_Driver[i].IDActiveMinute >= APT_TRACKING_24H_inSec) {
				aptTracking_Driver[i].IDActiveMinute = 0;
				aptTracking_Driver[i].TotalWorkMinute = 0;
				aptTracking_Driver[i].RoundWorkMinute = 0;
				aptTracking_Driver[i].RoundRestMinute = 0;
				aptTracking_Driver[i].id[0] = 0;
				buffer_gpbm_write_offset(i + GPBM_LICENSE_TRK3_FIRST_ADDR, GPBM_DRIVER_WORK_HOUR_OFFSET, (u8*)&aptTracking_Driver[i],sizeof(aptTracking_driver_t));
			}
		}
		
		
			
		if(!(aptTracking_Driver[i].IDActiveMinute % 60) && aptTracking_Driver[i].id[0]) {
			buffer_gpbm_write_offset(i + GPBM_LICENSE_TRK3_FIRST_ADDR, GPBM_DRIVER_WORK_HOUR_OFFSET, (u8*)&aptTracking_Driver[i],sizeof(aptTracking_driver_t));
		}
		
		if(aptTracking_Driver[i].id[0]) {
			vPrintfTrace((30," Driver-%d[%d]: TotalTime %d, Working %d, Rested %d",i,aptTracking_Driver[i].isLogin,aptTracking_Driver[i].TotalWorkMinute,
			aptTracking_Driver[i].RoundWorkMinute,
			aptTracking_Driver[i].RoundRestMinute));
		}
	}
}

static void APT_TRACKING_1Sec_Workspace(void) {// 1 Sec Routine
	
	static u16				iMoveContinueCount = 0;
	static u16				adcArray[APT_TRACKING_ADC_BUFF_SIZE] = {0};
	static u16				adcPos = 0;
	static bool				isFirstStartUpSystem = TRUE;
	static u16				OverSpeedPerMinute = 0;
	static bool				NowIsOverSpeed = FALSE;
	static uint16_t			delay_soundPlayer = 0;

	// Speed Continue Monitoring
	if(swd_feature_isEnable(FEAT_DISTANCE_NO_IG))UP_COUNTER_OVERFLOW_LIMIT(iMoveContinueCount,((aptTracking_Runtime.GPS.speed)&&(aptTracking_Runtime.GPS.fixDimnt)),65535);
	else UP_COUNTER_OVERFLOW_LIMIT(iMoveContinueCount,(swdio_get_ig_status()&&(aptTracking_Runtime.GPS.speed)&&(aptTracking_Runtime.GPS.fixDimnt)),65535);
	// Sampling Counter in Driving mode 
	UP_COUNTER_OVERFLOW_LIMIT(APT_TRACKING_cfgCompare.Sampling.DriveMode,swdio_get_ig_status(),65535);
	// Idle Mode Monitoring
	if(swd_feature_isEnable(FEAT_NO_IDLE)) APT_TRACKING_cfgCompare.Idling.Delay = 0;
	else UP_COUNTER_OVERFLOW_LIMIT(APT_TRACKING_cfgCompare.Idling.Delay,(swdio_get_ig_status()&&(aptTracking_Runtime.GPS.speed<=ini_config_utils.Idling.Value)),65535);
	// Over Speed Monitoring
	NowIsOverSpeed = swdio_get_ig_status()&&(aptGpsWakeup_PvtRawInfo.tPositionInfo.speed>=ini_config_utils.OverSpeed.Value)? TRUE:FALSE;
	if(swd_feature_isEnable(FEAT_DLAY_OVERSP))
	UP_COUNTER_OVERFLOW_LIMIT(APT_TRACKING_cfgCompare.OverSpeed.Delay,NowIsOverSpeed,65535);
	else APT_TRACKING_cfgCompare.OverSpeed.Delay = NowIsOverSpeed ? ini_config_utils.OverSpeed.Delay : 0;
	// IG Monitoring
	UP_COUNTER_OVERFLOW_LIMIT(APT_TRACKING_cfgCompare.Input1.Delay,(APT_TRACKING_cfgCompare.Input1.Logic!=swdio_get_ig_status()),65535);
	// PTO1 Monitoring
	if(swd_feature_isEnable(FEAT_PTO1_EVENT))
	UP_COUNTER_OVERFLOW_LIMIT(APT_TRACKING_cfgCompare.Input2.Delay,(APT_TRACKING_cfgCompare.Input2.Logic!=swdio_get_pto1_status()),65535);
	// PTO2 Monitoring
	if(swd_feature_isEnable(FEAT_RPM_BY_PTO2)) {
		aptTracking_Runtime.RPM_Frequency = swdio_get_pto2_freq();
		if(swd_feature_isEnable(FEAT_OPT_LIGHT_BY_RPM)) {
			if (aptTracking_Runtime.RPM_Frequency )swdio_light_opt_set(1);
			else swdio_light_opt_set(0);
		}
	}
	else if(swd_feature_isEnable(FEAT_PTO2_EVENT))
	UP_COUNTER_OVERFLOW_LIMIT(APT_TRACKING_cfgCompare.Input3.Delay,(APT_TRACKING_cfgCompare.Input3.Logic!=swdio_get_pto2_status()),65535);
	// PTO3 Monitoring
	if(swd_feature_isEnable(FEAT_PTO3_EVENT))
	UP_COUNTER_OVERFLOW_LIMIT(APT_TRACKING_cfgCompare.Input4.Delay,(APT_TRACKING_cfgCompare.Input4.Logic!=APT_TRACKING_PTO3_iLogic),65535);
	// Output1 ReState Delay
	UP_COUNTER_OVERFLOW_LIMIT(APT_TRACKING_cfgCompare.Output1.Delay,(ini_config_utils.Output1.Delay),65535);
	// Output2 ReState Delay
	UP_COUNTER_OVERFLOW_LIMIT(APT_TRACKING_cfgCompare.Output2.Delay,(ini_config_utils.Output2.Delay),65535);
	
	if(APT_TRACKING_bSoundplaying) delay_soundPlayer++;
	if(delay_soundPlayer > APT_TRACKING_PLAY_SOUND_WAIT) {
		APT_TRACKING_bSoundplaying = false;
		delay_soundPlayer = 0;
	}
	
	if(cpe_del_trigger) {
		apt_buffer_cpe_del();
		cpe_del_trigger--;
	}
	
	if(cpl_del_trigger) {
		apt_buffer_cpl_del();
		cpl_del_trigger--;
	}
	
	swd_event_update_report(&APT_TRACKING_SEC_GPS_Speed, &APT_TRACKING_SEC_ODO_Speed);
	
	// Temperature sensor timeout
	if(aptTracking_tempsensor_timeout > _1SecTick_Second(60)) { // 60 seconds
		aptTracking_Runtime.TempSensor[0] = SWD_TEMP_SS_NO_CONNECT;
		aptTracking_Runtime.TempSensor[1] = SWD_TEMP_SS_NO_CONNECT;
		aptTracking_Runtime.TempSensor[2] = SWD_TEMP_SS_NO_CONNECT;
		aptTracking_Runtime.TempSensor[3] = SWD_TEMP_SS_NO_CONNECT;
	}else aptTracking_tempsensor_timeout++;
	
	// RFID timeout
	if(aptTracking_rfid_timeout > _1SecTick_Second(60)) { // 60 seconds
		aptTracking_Runtime.RFID = SWD_RFID_NO_CONNECT;
	}else aptTracking_rfid_timeout++;
	
	// ADC Oil Sort
	if(swd_feature_isEnable(FEAT_OIL_SUPPORT)) {
		adcArray[adcPos] = (uint16_t)aptAfec_getRaw(APT_AFEC_EXT_INPUT_CH);
		adcPos = (adcPos >= (APT_TRACKING_ADC_BUFF_SIZE -1)) ? 0 : adcPos + 1;
		aptTracking_Runtime.adcIN.ch2 = middleValue_Sort(adcArray,50);
		aptTracking_Runtime.Fuel_Voltage = (uint16_t)aptAfec_ConvertRaw(APT_AFEC_EXT_INPUT_CH,aptTracking_Runtime.adcIN.ch2);
	}
	else {
		aptTracking_Runtime.adcIN.ch2 = (uint16_t)aptAfec_getRaw(APT_AFEC_EXT_INPUT_CH);
		aptTracking_Runtime.Fuel_Voltage = (uint16_t)aptAfec_ConvertRaw(APT_AFEC_EXT_INPUT_CH,aptTracking_Runtime.adcIN.ch2);
		if(aptTracking_Runtime.Fuel_Voltage > 3 ) APT_TRACKING_PTO3_iLogic = 1;
		else APT_TRACKING_PTO3_iLogic = 0;
	}
	
	if(isFirstStartUpSystem && APT_TRACKING_RTC_isUpToDate) {
		isFirstStartUpSystem = FALSE;
		swd_event_keep_status(STARTUP_LECACY_EVCODE,0);
	}
	
	// Over Speed Check
	if(swdio_get_ig_status()) {
		static uint8_t	time_counter = 0;
		static uint16_t	maxspeed_reccord = 0;
		
		maxspeed_reccord = aptTracking_Runtime.GPS.speed > maxspeed_reccord ? aptTracking_Runtime.GPS.speed : maxspeed_reccord;
		UP_COUNTER_OVERFLOW_LIMIT(aptTracking_Runtime.TripTime,TRUE,65535);
		UP_COUNTER_OVERFLOW_LIMIT(aptTracking_Runtime.DriveTimePerDay,TRUE,65535);
			
		if(time_counter >= _1SecTick_Second(60)) { // 60 Seconds
			aptTracking_Runtime.OVSPCountPerMin = OverSpeedPerMinute;
			aptTracking_Runtime.MAXSpeedPerMin = maxspeed_reccord;
			aptTracking_Runtime.HACountPerMin = aptTracking_ha_Counter;
			aptTracking_Runtime.HDCountPerMin = aptTracking_hd_Counter;
			OverSpeedPerMinute = 0;
			maxspeed_reccord = 0;
			aptTracking_ha_Counter = 0;
			aptTracking_hd_Counter = 0;
			time_counter = 0;
		}time_counter++;
		
		if((APT_TRACKING_cfgCompare.OverSpeed.Delay >= ini_config_utils.OverSpeed.Delay)&&NowIsOverSpeed) aptSwd_submit_overspeed(TRUE);
		else aptSwd_submit_overspeed(FALSE);
	}else aptSwd_submit_overspeed(FALSE);
	
	// Drive Schedule(IG ON)
	if(APT_TRACKING_cfgCompare.Idling.Delay > _1SecTick_Second(ini_config_utils.Idling.Delay)) {
	//if(APT_TRACKING_cfgCompare.Idling.Delay > _1SecTick_Second(60)) {
		if(APT_TRACKING_RTC_isUpToDate) {
			APT_TRACKING_cfgCompare.Sampling.DriveMode = 0;
			swd_event_keep_status(IDLING_LECACY_EVCODE,0);
		}
	}
	else if(APT_TRACKING_cfgCompare.Sampling.DriveMode >= _1SecTick_Second(ini_config_utils.Sampling.DriveMode)) {
	//else if(APT_TRACKING_cfgCompare.Sampling.DriveMode >= _1SecTick_Second(20)) {
		if(APT_TRACKING_RTC_isUpToDate) {
			if((APT_TRACKING_cfgCompare.OverSpeed.Delay >= ini_config_utils.OverSpeed.Delay)&&NowIsOverSpeed) {
				APT_TRACKING_cfgCompare.Sampling.DriveMode = 0;
				swd_event_keep_status(OVERSPEED_LECACY_EVCODE,0);
				vPutsTrace(30,"OVER SPEED");
				UP_COUNTER_OVERFLOW_LIMIT(OverSpeedPerMinute,TRUE,65535);
				UP_COUNTER_OVERFLOW_LIMIT(aptTracking_Runtime.OVSP_Counter,TRUE,65535);
			}
			else {
				APT_TRACKING_cfgCompare.Sampling.DriveMode = 0;
				swd_event_keep_status(DRIVING_LECACY_EVCODE,0);
			}
		}
	}
		
	// Digital Input changed and Output reset
	if(APT_TRACKING_cfgCompare.Input1.Delay > _1SecTick_Second(ini_config_utils.Input1.Delay)) {
		if(APT_TRACKING_RTC_isUpToDate) {
			APT_TRACKING_cfgCompare.Input1.Logic = swdio_get_ig_status();
			aptTracking_Runtime.digGPI &= ~(1 << 0);// Clear bit 0
			aptTracking_Runtime.digGPI |= ((APT_TRACKING_cfgCompare.Input1.Logic << 0)&0x01);//OR bit 0 (1-BCD)
			swd_event_keep_status(DINPUT_LECACY_EVCODE,SWD_IO_CHANGE_IG);
			
			vPutsTrace(30,"INPUT-IG SWITCHED");
			
			if(APT_TRACKING_cfgCompare.Input1.Logic) {
				aptTracking_Runtime.TripTime = 0;
				aptTracking_Runtime.TripDistance = 0;
				aptTracking_Runtime.HACountPerMin = 0;
				aptTracking_Runtime.HDCountPerMin = 0;
				aptTracking_Runtime.MAXSpeedPerMin = 0;
				aptTracking_Runtime.OVSPCountPerMin = 0;
				UP_COUNTER_OVERFLOW_LIMIT(aptTracking_Runtime.IGON_Counter,TRUE,65535);
			}
			else {
				if((aptTracking_Runtime.IGON_Counter >= 2) && gpbr_read(GPBR_SOUND_ID)) gpbr_write(GPBR_SOUND_ID,0);
				if(ini_config_utils.Output1.Logic) swdio_set_outp1_status(0);//Connected to NC Relay
				else swdio_set_outp1_status(1);//Connected to NC Relay
			}
		}
	}
		
	if(APT_TRACKING_cfgCompare.Input2.Delay > _1SecTick_Second(ini_config_utils.Input2.Delay)) {
		if(APT_TRACKING_RTC_isUpToDate) {
			APT_TRACKING_cfgCompare.Input2.Logic = swdio_get_pto1_status();
			aptTracking_Runtime.digGPI &= ~(1 << 1);// Clear bit 1
			aptTracking_Runtime.digGPI |= ((APT_TRACKING_cfgCompare.Input2.Logic << 1)&0x02);//OR bit 1(2-BCD)
			swd_event_keep_status(DINPUT_LECACY_EVCODE,SWD_IO_CHANGE_PTO1);

			vPutsTrace(30,"INPUT-PTO1 SWITCHED");
			if(APT_TRACKING_cfgCompare.Input2.Logic) {
				UP_COUNTER_OVERFLOW_LIMIT(aptTracking_Runtime.PTO_Counter,TRUE,65535);
			}
		}
	}
		
	if(APT_TRACKING_cfgCompare.Input3.Delay > _1SecTick_Second(ini_config_utils.Input3.Delay)) {
		if(APT_TRACKING_RTC_isUpToDate) {
			APT_TRACKING_cfgCompare.Input3.Logic = swdio_get_pto2_status();
			aptTracking_Runtime.digGPI &= ~(1 << 2);// Clear bit 2
			aptTracking_Runtime.digGPI |= ((APT_TRACKING_cfgCompare.Input3.Logic << 2)&0x04);//OR bit 2(4-BCD)
			swd_event_keep_status(DINPUT_LECACY_EVCODE,SWD_IO_CHANGE_PTO2);
			vPutsTrace(30,"INPUT-PTO2 SWITCHED");
		}
	}
	
	if(APT_TRACKING_cfgCompare.Input4.Delay > _1SecTick_Second(ini_config_utils.Input4.Delay)) {
		if(APT_TRACKING_RTC_isUpToDate) {
			APT_TRACKING_cfgCompare.Input4.Logic = APT_TRACKING_PTO3_iLogic;
			aptTracking_Runtime.digGPI &= ~(1 << 3);// Clear bit 3
			aptTracking_Runtime.digGPI |= ((APT_TRACKING_cfgCompare.Input4.Logic << 3)&0x08);//OR bit 4(8-BCD)
			swd_event_keep_status(DINPUT_LECACY_EVCODE,SWD_IO_CHANGE_PTO3);
			vPutsTrace(30,"INPUT-PTO3 SWITCHED");
		}
	}
	
	// Output Re-Logic
	if(APT_TRACKING_cfgCompare.Output1.Delay > _1SecTick_Second(ini_config_utils.Output1.Delay)) {
		ini_config_utils.Output1.Logic = ini_config_utils.Output1.Logic ? 0 : 1;//TOGGLE
		if(ini_config_utils.Output1.Logic) {
			ini_config_utils.Output1.Delay = ini_config_atoi_strParameter("AT#OUTPUT1=1,0",2);
		}else {
			ini_config_utils.Output1.Delay = ini_config_atoi_strParameter("AT#OUTPUT1=0,0",2);
		}
		vPutsTrace(30,"OUTPUT:1 TOGGLE LOGIC");
	}
	
	if(APT_TRACKING_cfgCompare.Output2.Delay > _1SecTick_Second(ini_config_utils.Output2.Delay)) {
		ini_config_utils.Output2.Logic = ini_config_utils.Output2.Logic ? 0 : 1;//TOGGLE
		if(ini_config_utils.Output2.Logic) {
			ini_config_utils.Output2.Delay = ini_config_atoi_strParameter("AT#OUTPUT2=1,0",2);
		}else {
			ini_config_utils.Output2.Delay = ini_config_atoi_strParameter("AT#OUTPUT2=0,0",2);
		}
		vPutsTrace(30,"OUTPUT:2 TOGGLE LOGIC");
	}
	
	
	// Distance Update //
	if(iMoveContinueCount >=_1SecTick_Second(5)) { //Speed Continuously 10 seconds
		static distance_calc_coordinate_t p0 = {0.0,0.0};
		distance_calc_coordinate_t p1 = {aptTracking_Runtime.GPS.lat,aptTracking_Runtime.GPS.lon};
		if(IS_A_GOOD_LOC(p1.lat,p1.lon,1)) {
			if((p0.lat < 0.1)&&(p0.lon < 0.1)) { // Update 1st reference point
				p0.lat = p1.lat;
				p0.lon = p1.lon;
			}
			else {
				u32	iTmpDist = (double) distance_calc_lac(p0,p1);
				if(iTmpDist > 5) {//Lowest accuracy at 20 meters other No Update Point
					aptTracking_Runtime.TotalDistance = (aptTracking_Runtime.TotalDistance+iTmpDist)>=LIMIT_TRIP_DISTANCE ? LIMIT_TRIP_DISTANCE : aptTracking_Runtime.TotalDistance+iTmpDist;
					aptTracking_Runtime.TripDistance  = (aptTracking_Runtime.TripDistance+iTmpDist )>=LIMIT_TRIP_DISTANCE ? LIMIT_TRIP_DISTANCE : aptTracking_Runtime.TripDistance+iTmpDist;
					aptTracking_Runtime.DistancePerDay = (aptTracking_Runtime.DistancePerDay+iTmpDist )>=LIMIT_TRIP_DISTANCE ? LIMIT_TRIP_DISTANCE : aptTracking_Runtime.DistancePerDay+iTmpDist;
					// Update new reference point
					p0.lat = p1.lat;
					p0.lon = p1.lon;
				}
			}
		}
	}
	
	if((aptTracking_Runtime.tmRTC.hour==0)&&(aptTracking_Runtime.tmRTC.minute==0)&&(aptTracking_Runtime.tmRTC.second < 10)) aptTracking_Runtime.DistancePerDay = 0;
	if((aptTracking_Runtime.tmRTC.hour==0)&&(aptTracking_Runtime.tmRTC.minute==0)&&(aptTracking_Runtime.tmRTC.second < 10)) aptTracking_Runtime.DriveTimePerDay = 0;
	
	apt_buffer_cpe_write_GPBMEM(&aptTracking_Runtime,sizeof(aptTracking_runtime_t));
}

static void APT_TRACKING_100mS_Workspace(void) {
	static u8				print_show = 0;
	
	
	if(APT_TRACKING_RTC_isUpToDate) aptTracking_print_secdata();
	
	if(swd_feature_isEnable(FEATURE_GEOFENCE_SOUND_4SCG)) {
		static u8				delay_1sec = 0;
		delay_1sec = delay_1sec >= 255 ? 255 : delay_1sec + 1;
		
		APT_TRACKING_WarnningGeofence();
		
		if(delay_1sec >= 10) {
			//static char				gpsinfo_atcmd_buffer[35];
			//snprintf(gpsinfo_atcmd_buffer,34,"AT#GPSINFO=\"%f\",\"%f\",\"%d\"",aptTracking_Runtime.GPS.lat,aptTracking_Runtime.GPS.lon,aptTracking_Runtime.GPS.speed);
			//adl_atCmdCreate((char*)gpsinfo_atcmd_buffer,ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);
			APT_TRACKING_1Sec_WarnningDriverTime();
			delay_1sec = 0;
		}
	}
	
	// HTTP File
	if(ini_config_utils.CtrlRequest.bHttpFile==1) {
		if(arc_netsys.bSimReady && arc_netsys.bGsmRegistered) {
			char *pURL = adl_memGet(strlen(SWD_HTTP_URL_ZONE) + 12 );
			strcpy (pURL, SWD_HTTP_URL_ZONE);
			strcat (pURL,ini_config_utils.DTCUniq.sSIMNumb);
			ini_config_utils.CtrlRequest.bHttpFile = ini_config_atoi_strParameter("AT#HTTPFILE=0",1);
			vTaskDelay(100);
			if(arc_hlhttp_Browse(1, pURL)>=0)vPutsTrace(APT_TRACKING_TRACE,"HTTP: CREATE OK\r\n");
			else vPutsTrace(APT_TRACKING_TRACE,"HTTP: CREATE FAILED\r\n");
			adl_memRelease(pURL);
		}
	}
	
	// FTP DOTA
	if(ini_config_utils.CtrlRequest.bDota) {
		if(arc_netsys.bSimReady && arc_netsys.bGsmRegistered) {
			if(aptTracking_bootInfo.fw_source == FW_DOTA) {
				FIL ota_object;
				FRESULT res;
				size_t bytes;
				res = f_open(&ota_object, (char const *)__OTA_LOG_PATH__, FA_WRITE | FA_OPEN_ALWAYS);
				if (res == FR_OK){
					res = f_write(&ota_object, &aptTracking_bootInfo, sizeof(arc_bootld_init_t), &bytes);
					if(res == FR_OK) vPutsTrace(30,"SMS-DOTA Parameter Saved");
					else vPrintfTrace((30,"SMS-DOTA Parameter Save Failed!!! ,f_write %d",res));
					f_close(&ota_object);
				}else vPrintfTrace((30,"SMS-DOTA Parameter Save Failed!!! ,f_open %d",res));
			}
		
			vPutsTrace(30,"DOTA INIIALIZED");

			ini_config_utils.CtrlRequest.bDota = ini_config_atoi_strParameter("AT#DOTA=0",1);
			arc_hlftp_SetConfig	(	1,
									ARC_HLNET_OPT_CNXID, 1,
									ARC_HLNET_OPT_USER,ini_config_net.DOTAServer.User,
									ARC_HLNET_OPT_PASS,ini_config_net.DOTAServer.PassW,
									ARC_HLNET_OPT_ADDR,ini_config_net.DOTAServer.IP,
									ARC_HLNET_OPT_PORT,21,
									ARC_HLNET_OPT_END );
			if(arc_hlftp_Dota(1, ini_config_net.DOTAServer.FileN , "./" ) >= 0) vPutsTrace(30,"FTP: CREATED OK\r\n");
			else vPutsTrace(30,"FTP: CREATE FAILED\r\n");
		}
	}
	
	// Display diagnostic
	if(print_show++ >= _100mSecTick_Second(5)) { // 5 Seconds
		print_show = 0;
		vPrintfTrace((30,"GPS: %02d-%02d-%d %02d:%02d:%02d Fx%dD \"%f-%f\"",	aptTracking_Runtime.GPS.day,aptTracking_Runtime.GPS.month,aptTracking_Runtime.GPS.year,
																				aptTracking_Runtime.GPS.hour,aptTracking_Runtime.GPS.minute,aptTracking_Runtime.GPS.second,
																				aptTracking_Runtime.GPS.fixDimnt>3?3:aptTracking_Runtime.GPS.fixDimnt,aptTracking_Runtime.GPS.lat,aptTracking_Runtime.GPS.lon));
	
		if(aptSwd_task_completed()) {
			vPrintfTrace((30,"Buffer #E = %d, #L = %d , Delay = %d Sec",apt_buffer_cpe_nbItem(),apt_buffer_cpl_nbItem(),aptGateway_uploadDelay()));
		}else vPrintfTrace((30,"SWD Initialize State"));
	}
}

static void APT_TRACKING_Load_Geofence(void) {
	FIL ota_object;
	FRESULT res;
	size_t bytes;
	
	res = f_open(&ota_object, (char const *)__HTTP_DATA_PATH__, FA_READ | FA_OPEN_EXISTING);
	if (res == FR_OK) {
		char * pTempData = adl_memGet(__HTTP_DATA_MAXSIZE__);
		res = f_read(&ota_object,pTempData, __HTTP_DATA_MAXSIZE__ , &bytes);
		if (res == FR_OK) {
			APT_TRACKING_nbGeofenceTab = geofence_decode_raw_config( pTempData, bytes , APT_TRACKING_GeofenceTab, &APT_TRACKING_GeofenceVersion);
			vPrintfTrace((0,"\r\ngeofence vers = %02x number of table = %d\r\n",APT_TRACKING_GeofenceVersion,APT_TRACKING_nbGeofenceTab));
		}else vPutsTrace(0,"HTTPFILE READ ERROR");
		f_close(&ota_object);
		adl_memRelease(pTempData);
	}else vPutsTrace(0,"HTTPFILE NOT FOUND");
}




static void _APT_TRACKING_Speed_cmdHandler (adl_atCmdPreParser_t * Params) {
	u8 temp = 0x00;
	switch(Params->Type) {
		case ADL_CMD_TYPE_PARA:
		temp = ( atoi(ADL_GET_PARAM(Params,0)) );
		test_speed = temp;
		default:
		break;
	}
	printf("\r\n@SPEED: %d \r\nOK\r\n",temp);
}

static void _APT_TRACKING_DriverIn_cmdHandler (adl_atCmdPreParser_t * Params) {
	u8 temp = 0x00;
	switch(Params->Type) {
		case ADL_CMD_TYPE_PARA:
		temp = ( atoi(ADL_GET_PARAM(Params,0)) );
		aptTracking_Driver[temp].isLogin = true;
		aptTracking_Driver[temp].id[0] = 'A';
		default:
		break;
	}
	printf("\r\n@DRIVERIN: %d \r\nOK\r\n",temp);
}

static void _APT_TRACKING_DriverOut_cmdHandler (adl_atCmdPreParser_t * Params) {
	u8 temp = 0x00;
	ascii StingCmd[20] = {""};
	switch(Params->Type) {
		case ADL_CMD_TYPE_PARA:
		temp = ( atoi(ADL_GET_PARAM(Params,0)) );
		aptTracking_Driver[temp].isLogin = false;
		default:
		break;
	}
	printf("\r\n@DRIVEROUT: %d \r\nOK\r\n",temp);
}

static void _APT_TRACKING_Geofence_cmdHandler (adl_atCmdPreParser_t * Params) {
	u8 temp = 0x00;
	switch(Params->Type) {
		case ADL_CMD_TYPE_PARA:
		temp = ( atoi(ADL_GET_PARAM(Params,0)) );
		APT_TRACKING_GeofenceTab[temp].lat = ( atof(ADL_GET_PARAM(Params,1)) );
		APT_TRACKING_GeofenceTab[temp].lon = ( atof(ADL_GET_PARAM(Params,2)) );
		APT_TRACKING_GeofenceTab[temp].radius_meter = ( atoi(ADL_GET_PARAM(Params,3)) );
		APT_TRACKING_GeofenceTab[temp].speed_value_exceed_warninig = ( atoi(ADL_GET_PARAM(Params,4)) );
		default:
		break;
	}
	printf("\r\n@GEOFENCE-%d: %f, %f, %d, %d \r\nOK\r\n",temp,
	APT_TRACKING_GeofenceTab[temp].lat,APT_TRACKING_GeofenceTab[temp].lon,
	APT_TRACKING_GeofenceTab[temp].radius_meter,APT_TRACKING_GeofenceTab[temp].speed_value_exceed_warninig);
}

/***************************************************************************/
/*  Function   : adl_main                                                  */
/*-------------------------------------------------------------------------*/
/*  Object     : Customer application initialization                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilization                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  InitType          |   |   |   |  Application start mode reason         */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
void aptTracking_task(void * pvParameters) {
	static uint32_t counter_mSec1 = 0;
	static uint32_t counter_mSec2 = 0;
	static uint32_t * pWdCount = NULL;
	
	
	
	pWdCount = (uint32_t*)pvParameters;
	
	aptTracking_Runtime.RFID = SWD_RFID_NO_CONNECT;
	aptTracking_Runtime.TempSensor[0] = SWD_TEMP_SS_NO_CONNECT;
	aptTracking_Runtime.TempSensor[1] = SWD_TEMP_SS_NO_CONNECT;
	aptTracking_Runtime.TempSensor[2] = SWD_TEMP_SS_NO_CONNECT;
	aptTracking_Runtime.TempSensor[3] = SWD_TEMP_SS_NO_CONNECT;
	
	while(!aptSwd_task_completed()) {
		vTaskDelay(1000);
		*pWdCount = 0;//Clear watchdog
	}
	
	if(ini_config_utils.Output1.Logic) swdio_set_outp1_status(0);//Connected to NC Relay
	else swdio_set_outp1_status(1);//Connected to NC Relay
	if(swd_feature_isEnable(FEAT_EXT_BUZZER)) swdio_set_outp2_status(0);
	else {
		if(ini_config_utils.Output2.Logic) swdio_set_outp2_status(1);
		else swdio_set_outp2_status(0);
	}

	APT_TRACKING_RTC_isUpToDate = APT_TRACKING_RTC_Update();
	
	arc_hlftp_Subscribe(APT_TRACKING_FTP_Handler);
	
	arc_hlhttp_Subscribe(APT_TRACKING_HTTP_Handler);
	
	APT_TRACKING_ReLoadMemData();
	
	//aptSms_SendForStart();
	
	if(swd_feature_isEnable(FEATURE_GEOFENCE_SOUND_4SCG)) {
		
		APT_TRACKING_Load_Geofence();
		
		//APT_TRACKING_nbGeofenceTab = geofence_decode_raw_config( geo_data, strlen(geo_data) , APT_TRACKING_GeofenceTab, &APT_TRACKING_GeofenceVersion);
		
		//vPrintfTrace((30,"Go-Version %d",APT_TRACKING_GeofenceVersion));	
	}
	
	for(;APT_TRACKING_bRTCSync==false;) {
		*pWdCount = 0;//Clear watchdog
		vPutsTrace(30,"Wait RTC Sync..");
		vTaskDelay(1000);
	}
	
	for(uint16_t i = 0; i < APT_TRACKING_nbGeofenceTab ; i++) {
		vPrintfTrace((30,"Go-%d: %f,%f,%d,%d",i,APT_TRACKING_GeofenceTab[i].lat,
		APT_TRACKING_GeofenceTab[i].lon,APT_TRACKING_GeofenceTab[i].radius_meter,
		APT_TRACKING_GeofenceTab[i].speed_value_before_warning));
	}
	
	APT_TRACKING_TaskCompleted = TRUE;
	
	adl_atCmdSubscribe("AT@GEOFENCE", _APT_TRACKING_Geofence_cmdHandler, ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST|0x55);
	adl_atCmdSubscribe("AT@SPEED", _APT_TRACKING_Speed_cmdHandler, ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST|0x11);
	adl_atCmdSubscribe("AT@DRIVERIN", _APT_TRACKING_DriverIn_cmdHandler, ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST|0x11);
	adl_atCmdSubscribe("AT@DRIVEROUT", _APT_TRACKING_DriverOut_cmdHandler, ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST|0x11);

	
	for(;;) {
		
		counter_mSec1 += (*pWdCount);
		counter_mSec2 += (*pWdCount);
		
		*pWdCount = 0;//Clear watchdog

		if(counter_mSec2 >= 100) {
			counter_mSec2 = 0;
			APT_TRACKING_100mS_Workspace();
		}
		
		if(counter_mSec1 >= 1000) {
			counter_mSec1 = 0;
			APT_TRACKING_1Sec_Workspace();
		}
		
		vTaskDelay(10);
	}	
}
