/*
 * CFile1.c
 *
 * Created: 3/25/2015 5:12:31 PM
 *  Author: Pan
 */ 
/*
 * CFile1.c
 *
 * Created: 3/25/2015 4:02:05 PM
 *  Author: Pan
 */ 
#include "adl_global.h"
#include "_lib_arcnethl/arc_hltcp.h"
#include "_lib_arcnethl/arc_hlcnx.h"
#include "ini_config.h"
#include "apt_tracking.h"
#include "buffer.h"
#include "trans_form.h"
#include "math.h"
#include "misc_config.h"
#include "apt_gateway.h"
#include "apt_swd.h"
#include "misc_config.h"
#include "distance_calc.h"
#include "_lib_gps/nmea_gps.h"
#include "_lib_gps/nmea.h"
#include "swd_register.h"
#include "apt_gps_data.h"
#include "../_hw_ctrl/hw_ctrl.h"
#include "vprinter.h"
#include "apt_afec.h"
#include "_lib_bootld/boot_trigger.h"
#include "apt_hahd_gps.h"
#include "feature.h"
#include "swd_io.h"
#include "xm0110_compat.h"
#include "error.h"
#include "apt_buzzer.h"
#include "apt_led_send.h"

#define	APT_SWD_TRACE	7

#define APT_SWD_DELAY_CLRBUFF		3
#define APT_SWD_DELAY_SLEEP			3
#define APT_SWD_DELAY_IGON			120
#define APT_SWD_DELAY_IGOFF			60
#define APT_SWD_DELAY_GPSFIX		900
#define APT_SWD_DELAY_GPSLOST		600
#define APT_SWD_DELAY_DMS_RESP		5
#define APT_SWD_DEALY_CFG_RESP		80
#define APT_SWD_DELAY_GET_CCID		5
#define APT_SWD_MAXTRY_GET_DMS		7

typedef struct APT_SWD_PRECONFIG_T {
	struct {
		ascii		APN[32];
		ascii		User[16];
		ascii		PassW[16];
	}PPP;
	struct {
		ascii		IP[16];
		u16			Port;
	}TCPServer;
	struct {
		ascii		ccid[22];
		ascii		Operator[2];
	}Sim;
}aptSwd_preconfig_t;

static bool APT_SWD_TaskCompleted = FALSE;
static aptSwd_preconfig_t  APT_SWD_DMS_Config = {{"","",""},{DMS_SERVER_IP_DEFAULT,DMS_SERVER_PORT_DEFAULT},{"",""}};
static gps_pvtInfo_t	APT_SWD_PvtRawInfo;
static bool OverSpeed_FlagSet = FALSE;
static u16 LicenseCard_Alert_Duration = 0;
static arc_hltcp_ctx_t * dms_data_ctx = NULL;

extern bool aptStart_Completed(void);

/*extern*/ char aptswd_ccid[22] = {""};


static enum {
		CHK_CLR_BUFFER,
		WAIT_IG_ON,
		WAIT_GPS_FIX,
		CHECK_CONFIG,
		NO_CONFIG_FOUND,
		REQUEST_DMS_CONFIG,
		WAIT_CONFIG_DMS,
		CLOSE_DMS_SERVER,
		LOAD_CONFIG,
		QUERY_BUFFER1,
		QUERY_BUFFER2,
		QUERY_COMPLETED,
		ALTERNATE_JOB,
}apt_swdState = CHK_CLR_BUFFER;


ascii * aptSwd_get_sim_ccid(void) {
	return APT_SWD_DMS_Config.Sim.ccid;
}

ascii * aptSwd_get_sim_operator(void) {
	return APT_SWD_DMS_Config.Sim.Operator;
}

u8 aptSwd_gps_fix_status(void) {
	return APT_SWD_PvtRawInfo.tPositionInfo.eFixState;
}

bool aptSwd_task_completed(void) { 
	return APT_SWD_TaskCompleted;
}
	
void aptSwd_submit_overspeed(bool status) {
	OverSpeed_FlagSet = status;
}
	
void aptSwd_submit_licensecard_alert(bool status) {
	switch((ini_config_utils.DTCUniq.optionmask & 0xE0)>>6) {
		case 0: LicenseCard_Alert_Duration = 0;/*Never Used Card Reader */
		break;
		default: LicenseCard_Alert_Duration =  status ? APT_CARD_ALERT_TIME : 0;
		break;
	}
}

static void APT_SWD_CHANNEL_SET_Handler ( u8 ssid, s8 event, void *context ) {
	ascii buffer[100] = {"DMS-"};
	switch(event) {
	case ARC_HLNET_EV_READ:
	{
		arc_hltcp_ctx_t * pcx = context;
		sprintf(buffer+4,"%d: RCV %d Bytes[%x,%x,%x,%x,%x,%x]..",
		ssid,pcx->length,pcx->data[0],pcx->data[1],pcx->data[2],pcx->data[3],pcx->data[4],pcx->data[5]);
		vPutsTrace(30,buffer);
		
		if((context)&&(!dms_data_ctx)) {
			dms_data_ctx = (arc_hltcp_ctx_t *)adl_memGet(sizeof(arc_hltcp_ctx_t) + pcx->length + 1);
			if(dms_data_ctx){
				memcpy(dms_data_ctx->data,pcx->data,pcx->length);
				dms_data_ctx->data[pcx->length] = 0;
				dms_data_ctx->length = pcx->length;
			}else vPrintf("\r\nDMS Buffer Alloc Failed\r\n");
		}else vPrintf("\r\nDMS Content Unknown\r\n");
		
		return;
	}
	break;
	case ARC_HLNET_EV_ALIVE: return; break;
	case ARC_HLNET_EV_TIMEOUT: sprintf(buffer+4,"%d: TIMEOUT",ssid); break;
	case ARC_HLNET_EV_CONNECTED: sprintf(buffer+4,"%d: CON OK",ssid); break;
	case ARC_HLNET_EV_PEER_CLOSE: sprintf(buffer+4,"%d: PEER CLOSE",ssid); break;
	case ARC_HLNET_EV_CONN_ERR: sprintf(buffer+4,"%d: CON ERR",ssid); break;
	case ARC_HLNET_EV_NORMAL_ERR: sprintf(buffer+4,"%d: NORM ERR",ssid); break;
	case ARC_HLNET_EV_ACCEPT_ERR: sprintf(buffer+4,"%d: ACCEPT ERR",ssid); break;
	case ARC_HLNET_EV_RELEASE: sprintf(buffer+4,"%d: RELEASE",ssid); break;
	case ARC_HLNET_EV_NETW_ERR: sprintf(buffer+4,"%d: NW ERR",ssid); break;
	case ARC_HLNET_EV_MAX_SOCK: sprintf(buffer+4,"%d: MAX SOCK",ssid); break;
	case ARC_HLNET_EV_MEM_ERR:sprintf(buffer+4,"%d: MEM ERR",ssid); break;
	case ARC_HLNET_EV_DNS_ERR: sprintf(buffer+4,"%d: DNS ERR",ssid); break;
	case ARC_HLNET_EV_SEND_WAIT: sprintf(buffer+4,"%d: SND WAIT",ssid); break;
	case ARC_HLNET_EV_BAD_SSID:	sprintf(buffer+4,"%d: BAD SSID",ssid); break;
	case ARC_HLNET_EV_SSID_RUNING: sprintf(buffer+4,"%d: SSID RUNING",ssid); break;
	case ARC_HLNET_EV_SSID_FULLY: sprintf(buffer+4,"%d: SSID FULLY",ssid); break;
	default: break;
	}
	
	vPutsTrace(30,buffer);
}

static bool SWD_CCID_RSP_Handler(adl_atResponse_t *paras) {
	wm_strGetParameterString(APT_SWD_DMS_Config.Sim.ccid,sizeof(APT_SWD_DMS_Config.Sim.ccid),paras->StrData,1);
	strcpy(aptswd_ccid,APT_SWD_DMS_Config.Sim.ccid);
	return FALSE;
}

static void APT_SWD_SET_SIM_Operator(aptSwd_preconfig_t * cfg) {
	if((cfg->Sim.ccid[2]=='6')&&(cfg->Sim.ccid[3]=='6')&&(cfg->Sim.ccid[4]=='1')&&(cfg->Sim.ccid[5]=='8')) {
		wm_strcpy(cfg->Sim.Operator,"D");
		wm_strcpy(cfg->PPP.APN,"www.dtac.co.th");
	}
	else if((cfg->Sim.ccid[2]=='6')&&(cfg->Sim.ccid[3]=='6')&&(cfg->Sim.ccid[4]=='0')&&(cfg->Sim.ccid[5]=='1')) {
		wm_strcpy(cfg->Sim.Operator,"A");
		wm_strcpy(cfg->PPP.APN,"internet");
	}
	else if((cfg->Sim.ccid[2]=='6')&&(cfg->Sim.ccid[3]=='6')&&(cfg->Sim.ccid[4]=='9')&&(cfg->Sim.ccid[5]=='9')) {
		wm_strcpy(cfg->Sim.Operator,"T");
		wm_strcpy(cfg->PPP.APN,"internet");
	}
	else if((cfg->Sim.ccid[2]=='8')&&(cfg->Sim.ccid[3]=='4')&&(cfg->Sim.ccid[4]=='0')&&(cfg->Sim.ccid[5]=='1')) {
		wm_strcpy(cfg->Sim.Operator,"V");
		wm_strcpy(cfg->PPP.APN,"m-web");
	}
	else if((cfg->Sim.ccid[2]=='4')&&(cfg->Sim.ccid[3]=='5')&&(cfg->Sim.ccid[4]=='7')&&(cfg->Sim.ccid[5]=='0')&&(cfg->Sim.ccid[6]=='2')) {
		wm_strcpy(cfg->Sim.Operator,"L");
		wm_strcpy(cfg->PPP.APN,"etlnet");
	}
	else {
		wm_strcpy(cfg->Sim.Operator,"N");
		wm_strcpy(cfg->PPP.APN,"internet");
	}

}	


static s8 APT_SWD_GPS_Update(void) {
	static uint16_t gps_sniCompare = 0;
	uint16_t gps_sniCurrent = 0;
	
	gps_sniCurrent = aptGpsData_getInfo(&APT_SWD_PvtRawInfo);
	if((gps_sniCurrent == 0)||(gps_sniCurrent == gps_sniCompare)) return 0;//No Data Update
	else gps_sniCompare = gps_sniCurrent;//Keep new
	return 1;
}

static void APT_SWD_REQUEST_DMS_Config(u8 ssid) {
	char TotalData[52] = {""};
	uint8_t  len =  swd_register_make_sndframe(TotalData,APT_SWD_DMS_Config.Sim.ccid, APT_SWD_DMS_Config.Sim.Operator);
	arc_hltcp_Send(ssid, len,(u8 *) TotalData);
	vPrintfTrace((30,"$GSTR SERVER=%s",TotalData));
}


static void swd_save_config(swd_register_cfg_t * cfg) {
	ascii StingCmd[65]={""};
	wm_sprintf(StingCmd,"AT#SWDFM=\"%s\"",cfg->sFwName);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#COMP=\"%s\"",cfg->sCompany);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#BOXID=\"%s\"",cfg->sCarID);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#SIMNUM=\"%s\"",cfg->sSIMNumb);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#POINT=%d",cfg->set_point);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#OVERSPW=%d",cfg->bAlertOversp);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#EMAIL=\"%s\"",cfg->sEmail);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#OVERSPEED=%d,%d",cfg->set_over,cfg->set_delay);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#SAMPLING=%d,%d",APT_PARK_SAMPLING_DEFAULT,cfg->set_samp);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#IDLESPEED=%d,%d",SWD_IDLE_SPEED_DEFAULT,cfg->set_idle);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#TCPSERV=\"%s\"",cfg->tcprecservall);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#TCPPORT=%s",cfg->tcprecservport);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#APNSERV=\"%s\"",cfg->apnall);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#SETHD=\"%d\",\"%d\"",cfg->HA,cfg->HD);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#SWDOPT=\"%d\"",cfg->optionmask);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#VEHTYPE=\"%d\"",cfg->vehicle_type);
	arc_shell_execute( StingCmd);
	wm_sprintf(StingCmd,"AT#VEHATTR=\"%d\"",cfg->vehicle_attr);
	arc_shell_execute( StingCmd);
}


static void APT_SWD_1SEC_WorkSpace (void) {
	static u16 delay_counter =0;
	static u8 tryNb4DmsGet = 0;
	switch(apt_swdState) {
	case CHK_CLR_BUFFER:
		if((swdio_get_clrbuf_status()|| gpbr_read(GPBR_CLRBF_ID))
		&& (aptAfec_get(APT_AFEC_EXT_POWER_CH) >= VEXT_MIN_THRESHOLD)) {
			delay_counter++;
			if(delay_counter >= APT_SWD_DELAY_CLRBUFF) {
				vPutsTrace(30,"Clear Buffer Active");
				if(swdio_get_clrbuf_status()) {
					uint8_t tmp[120];
					memset(tmp,0,120);
					apt_buffer_cpe_erase(0);//Erase FRAM and Flash
					apt_buffer_cpe_write_GPBMEM(tmp,120);
					vPutsTrace(30,"Clear All CPE");
					card_authenClean();
					gpbr_write(GPBR_SOUND_ID,1);//Sound for installation
					aptBuzzer_clrbuff();
					aptLedSend_clrbuffer();
				}
				else {//gpbr_read(GPBR_CLRBF_ID)
					if(gpbr_read(GPBR_DFRAG_ID)==0){//Don't Clear FRAM
						vPutsTrace(30,"Clear CPE Flash Only");
						apt_buffer_cpe_erase(2);//Erase Flash Only
					}
					else {
						uint8_t tmp[120];
						memset(tmp,0,120);
						apt_buffer_cpe_erase(0);//Erase FRAM and Flash
						apt_buffer_cpe_write_GPBMEM(tmp,120);
						vPutsTrace(30,"Clear All CPE");
						card_authenClean();
						gpbr_write(GPBR_SOUND_ID,1);//Sound for installation
						aptBuzzer_clrbuff();
						aptLedSend_clrbuffer();
					}
				}
				apt_buffer_cpl_erase();
				vPutsTrace(30,"Clear All CPL");
				gpbr_write(GPBR_DFRAG_ID,0);//Clear DFRAG
				gpbr_write(GPBR_CLRBF_ID,0);//Clear CLRBF
				gpbr_write(GPBR_RECFG_ID,1);//Set Re Config
				delay_counter = 0;
				apt_swdState = WAIT_IG_ON;
			}
		}else apt_swdState = WAIT_IG_ON;
		break;
	case WAIT_IG_ON:
		if(swdio_get_ig_status()) {
			if(gpbr_read(GPBR_SOUND_ID)) {
				vTaskDelay(900);
				aptBuzzer_alert(1);
			}
			swdio_light_ig_set(1);
			aptLedSend_set(0);
			delay_counter=0;
			apt_swdState = WAIT_GPS_FIX;
		}
		else {
			swdio_light_ig_set(0);
			delay_counter++;
			
			vPrintfTrace((30,"Wait IG - %d",delay_counter));
			
			if(gpbr_read(GPBR_SOUND_ID)) {
				if(delay_counter >= APT_SWD_DELAY_IGON) {
					delay_counter=0;
					aptBuzzer_alert(1);
					aptLedSend_set(0);
					apt_swdState = WAIT_GPS_FIX;
				}
			}
			else {
				if(delay_counter >= APT_SWD_DELAY_IGOFF) {
					delay_counter=0;
					aptLedSend_set(0);
					apt_swdState = WAIT_GPS_FIX;
				}
			}
		}
		break;
	case WAIT_GPS_FIX:
		if(APT_SWD_PvtRawInfo.tPositionInfo.eFixState == GPS_FIX_3D){
			vPrintfTrace((30,"%d/%d/%d - %d:%d:%d, %dD Lat = %.5f , Lon = %.5f",
			APT_SWD_PvtRawInfo.tTimeDateInfo.day,
			APT_SWD_PvtRawInfo.tTimeDateInfo.month,APT_SWD_PvtRawInfo.tTimeDateInfo.year,
			APT_SWD_PvtRawInfo.tTimeDateInfo.hours,
			APT_SWD_PvtRawInfo.tTimeDateInfo.minutes,APT_SWD_PvtRawInfo.tTimeDateInfo.seconds,
			APT_SWD_PvtRawInfo.tPositionInfo.eFixState,
			APT_SWD_PvtRawInfo.tPositionInfo.latitude,APT_SWD_PvtRawInfo.tPositionInfo.longitude));
			
			if(gpbr_read(GPBR_SOUND_ID)) {
				vTaskDelay(900);
				aptBuzzer_alert(3);
			}
			delay_counter=0;
			apt_swdState = CHECK_CONFIG;
		}
		else {
			
			delay_counter++;
			vPrintfTrace((30,"Wait GPS - %d",delay_counter));
			if(gpbr_read(GPBR_SOUND_ID)) {
				if(delay_counter >= APT_SWD_DELAY_GPSFIX) {
					delay_counter=0;
					apt_swdState = CHECK_CONFIG;
				}
			}
			else {
				if(delay_counter >= APT_SWD_DELAY_GPSLOST) {
					delay_counter=0;
					apt_swdState = CHECK_CONFIG;
				}
			}
		}
		break;
	case CHECK_CONFIG:
		if(APT_SWD_DMS_Config.Sim.ccid[0]) {
			vPrintfTrace((30,"+CCID: %s",APT_SWD_DMS_Config.Sim.ccid));
			APT_SWD_SET_SIM_Operator(&APT_SWD_DMS_Config);
			if(gpbr_read(GPBR_RECFG_ID)==0) {
				vPutsTrace(30,"Found DMS Config Saved");
				apt_swdState = QUERY_BUFFER1;
			}
			else {
				vPutsTrace(30,"DMS Re-Config");
				delay_counter=0;
				apt_swdState = NO_CONFIG_FOUND;
			}
			delay_counter=0;
		}
		else {
			delay_counter++;
			if(delay_counter >= APT_SWD_DELAY_GET_CCID) {
				delay_counter=0;
				vPutsTrace(30,"Read CCID");
				adl_atCmdCreate("AT+CCID", ADL_AT_PORT_TYPE(ADL_PORT_UART1,FALSE),SWD_CCID_RSP_Handler,"+CCID:",(void*)0);
			}
		}
		break;
	case NO_CONFIG_FOUND:
		arc_hlcnx_SetConfig(1,
		ARC_HLNET_OPT_APN, APT_SWD_DMS_Config.PPP.APN,
		ARC_HLNET_OPT_USER, APT_SWD_DMS_Config.PPP.User,
		ARC_HLNET_OPT_PASS,APT_SWD_DMS_Config.PPP.PassW,
		ARC_HLNET_OPT_END);
		
		arc_hltcp_SetConfig(DMS_SERVER_CTX_ID,
		ARC_HLNET_OPT_CNXID, 1,
		ARC_HLNET_OPT_MODE,ARC_HLTCP_MODE_CLIENT,
		ARC_HLNET_OPT_ADDR,APT_SWD_DMS_Config.TCPServer.IP,
		ARC_HLNET_OPT_PORT,APT_SWD_DMS_Config.TCPServer.Port,
		ARC_HLNET_OPT_END);
		
		arc_hldms_Start(DMS_SERVER_CTX_ID);
		vPrintfTrace((30,"DMS Server %d: Created",DMS_SERVER_CTX_ID));
		delay_counter=0;
		apt_swdState = REQUEST_DMS_CONFIG;
		break;
	case REQUEST_DMS_CONFIG:
		delay_counter++;
		if(delay_counter > APT_SWD_DELAY_DMS_RESP) {
			vPrintfTrace((30,"DMS Request Round %d",tryNb4DmsGet+1));
			APT_SWD_REQUEST_DMS_Config(DMS_SERVER_CTX_ID);
			delay_counter=0;
			apt_swdState = WAIT_CONFIG_DMS;
		}
		break;
	case WAIT_CONFIG_DMS:
		if(dms_data_ctx) {
			swd_register_cfg_t swd_serverCFG = {"000","","","",0,0,0,0,0,0,"","","","","",0,0,0,0,0};
			swd_serverCFG.spk_gain = 45;
			swd_serverCFG.mic_gain = 63;
			if(swd_register_cfg_decode( &swd_serverCFG, dms_data_ctx->data, dms_data_ctx->length)) {
				swd_save_config(&swd_serverCFG);
				gpbr_write(GPBR_RECFG_ID,0);//Clear Re Config
				vPrintfTrace((30,"COMP: %s",swd_serverCFG.sCompany));
				vPrintfTrace((30,"BOX-ID: %s",swd_serverCFG.sCarID));
				vPrintfTrace((30,"SIM: %s",swd_serverCFG.sSIMNumb));
				vPrintfTrace((30,"EMAIL: %s",swd_serverCFG.sEmail));
				vPrintfTrace((30,"APN: %s",swd_serverCFG.apnall));
				vPrintfTrace((30,"SERVER: %s, PORT: %s",swd_serverCFG.tcprecservall,swd_serverCFG.tcprecservport));
				vPrintfTrace((30,"SAMPLING: %d, IDLE: %d",swd_serverCFG.set_samp,swd_serverCFG.set_idle));
				vPrintfTrace((30,"OVSP: %d, ALERT: %d",swd_serverCFG.set_over,swd_serverCFG.bAlertOversp));
				vPrintfTrace((30,"HA-HD: %d,%d",swd_serverCFG.HA,swd_serverCFG.HD));
				if(gpbr_read(GPBR_SOUND_ID)) aptBuzzer_alert(5);
				apt_swdState = CLOSE_DMS_SERVER;
	
				char buffer[64] = {""};
				sprintf(buffer,"ATS0=2;+WBHV=8,1;+WDGR=1,%d;+WDGT=1,%d;&W",swd_serverCFG.spk_gain,swd_serverCFG.mic_gain);
				adl_atCmdCreate((char*)buffer,ADL_AT_PORT_TYPE(ADL_PORT_UART1,TRUE),NULL);

			}
			else {
				vPutsTrace(30,"DMS Config Unpack Failed");
				delay_counter=0;
				tryNb4DmsGet++;
				if(tryNb4DmsGet < APT_SWD_MAXTRY_GET_DMS ) apt_swdState = REQUEST_DMS_CONFIG;
				else {
					if(gpbr_read(GPBR_SOUND_ID)) aptBuzzer_error();
					apt_swdState = CLOSE_DMS_SERVER;
				}
			}
			adl_memRelease(dms_data_ctx);
			dms_data_ctx = NULL;
		}
		else {
			delay_counter++;
			vPrintfTrace((30,"Wait DMS Response %d",delay_counter));
			if(delay_counter >= APT_SWD_DEALY_CFG_RESP) {
				delay_counter=0;
				if(gpbr_read(GPBR_SOUND_ID)) aptBuzzer_error();
				apt_swdState = CLOSE_DMS_SERVER;
			}
		}
		break;
	case CLOSE_DMS_SERVER:
		vPutsTrace(30,"Close DMS Connection");
		arc_hltcp_Stop(DMS_SERVER_CTX_ID);
		apt_swdState = LOAD_CONFIG;
		if(gpbr_read(GPBR_RECFG_ID))software_reset();//Config Failed
		break;
	case LOAD_CONFIG:
		vPutsTrace(30,"Loading Config...");
		INI_CONFIG_LOAD();
		swd_feature_Init(ini_config_utils.DTCUniq.sFwName);
		apt_swdState = QUERY_BUFFER1;
		break;
	case QUERY_BUFFER1:
		{	int32_t items = apt_buffer_cpl_sort();
			vPrintfTrace((30,"CPL Sort = %ld Items",items));
			apt_swdState = QUERY_BUFFER2;
		}
		break;
	case QUERY_BUFFER2:
		{	int32_t items = apt_buffer_cpe_sort();
			vPrintfTrace((30,"CPE Sort = %ld Items",items));
			apt_swdState = QUERY_COMPLETED;
		}
		break;
	case QUERY_COMPLETED:
		if(gpbr_read(GPBR_SOUND_ID))  {
			vTaskDelay(900);
			aptBuzzer_alert(2);
		}
			
		if(!card_authenInit()) {
			aptSwd_submit_licensecard_alert(TRUE);
			vPutsTrace(30,"Driver Card Alert");
		}
			
		apt_swdState = ALTERNATE_JOB;
		memset(&APT_SWD_PvtRawInfo,0,sizeof(gps_pvtInfo_t));
		vPutsTrace(30,"Initial All Completed");
		APT_SWD_TaskCompleted = TRUE;
		break;
	case ALTERNATE_JOB:
		{
			static u16 alert_silence_perioad = 5;
				
			if(alert_silence_perioad == 0) { 
				alert_silence_perioad = 2;
					if((ini_config_utils.DTCUniq.bAlertOversp==TRUE)&&(OverSpeed_FlagSet==TRUE))  aptBuzzer_alert(2);
					else if((LicenseCard_Alert_Duration>0)&& (aptTracking_Runtime.digGPI & 0x01)) {// License Card Sound Alert ( available when IG on)
						aptBuzzer_alert(4);
	  					alert_silence_perioad = 3;//6;
					}
			}else alert_silence_perioad--;
			
			if((LicenseCard_Alert_Duration > 0)&& (aptTracking_Runtime.digGPI & 0x01)) LicenseCard_Alert_Duration--;
			else LicenseCard_Alert_Duration = 0;
		}
		break;
		default:
		break;
	}
}

void aptSwd_task(void * pvParameters) {
	static uint32_t counter_mSec1 = 0;
	static uint32_t * pWdCount = NULL;
	static uint16_t delay_counter = 0;
	
	pWdCount = (uint32_t*)pvParameters;
	
	while(aptStart_Completed() == false) {
		*pWdCount = 0;//Clear watchdog
		vTaskDelay(10);
	}
	
	arc_hltcp_Subscribe(APT_SWD_CHANNEL_SET_Handler);
	
	
	for(;;) {
		
		counter_mSec1 += (*pWdCount);
		
		*pWdCount = 0;//Clear watchdog
		
		// Load new configurations
		if(ini_config_isUpdate()) {
			INI_CONFIG_LOAD();
			vPutsTrace(30,"CONF. UPDATED");
		}
		
		if(counter_mSec1 >= 1000) {
			counter_mSec1 = 0;
			
			if(!APT_SWD_TaskCompleted) APT_SWD_GPS_Update();
			
			APT_SWD_1SEC_WorkSpace();
			
			/// Here check must to be Under ////
			
			if(apt_swdState > CHK_CLR_BUFFER) {
				if(swdio_get_ig_status() && gpbr_read(GPBR_RESET_ID)) {// Reset Pending
					gpbr_write(GPBR_RESET_ID,0);
					software_reset();
				}//else if(gpbr_read(GPBR_CLRBF_ID)) software_reset();// Clear Buffer
			}
			
			if(swdio_get_clrbuf_status()
			||(battery_isOperating &&(aptAfec_get(APT_AFEC_BAT_POWER_CH) < VINT_MIN_THRESHOLD))) {
				delay_counter++;
				if(delay_counter >= APT_SWD_DELAY_SLEEP) {
					delay_counter=0;
					if(aptAfec_get(APT_AFEC_EXT_POWER_CH) < VEXT_MIN_THRESHOLD) {
						vPutsTrace(30,"SLEEP ACTIVATED");
						vTaskDelay(100);
						sleep_trigger();// Sleep Active
					}
				}
			}else delay_counter = 0;
		}
		
		vTaskDelay(100);
	}
}