/*
 * apt_sms.c
 *
 * Created: 9/9/2016 12:58:59 PM
 *  Author: Panmaya
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
#include "apt_tracking.h"
#include "error.h"
#include "apt_tracking.h"

extern char aptswd_ccid[22];

typedef struct __APT_SMS_REC_T {
	ascii	Text[161];
	ascii	Tel [50];
	ascii	Time[50];
}aptSms_recv_t;


typedef struct __APT_SMS_OUT_T {
	ascii	Text[161];
	ascii	Tel [50];
	uint32_t option;
}aptSms_send_t;

#define APT_SMS_RCV_BUFF_NB		3

static s8						APT_SMS_Handle	= ERROR;
static aptSms_recv_t			APT_lstSMS_Rcv[APT_SMS_RCV_BUFF_NB] = {{"","",""},{"","",""},{"","",""}};
static aptSms_send_t			APT_lstSMS_Snd = {"",""};
static bool						APT_SMS_bSendOK = false;
	
#define MATCHED_STR(c,t)		!wm_strnicmp(c,t,strlen(c))

enum {
	SEND_OPT_NONE,
	SEND_OPT_REBOOT,
	SEND_OPT_CMD_UPDATE,
	SEND_OPT_DOTA_REPORT,
	SEND_OPT_GETRESET,
	SEND_OPT_G46C0,
	SEND_OPT_G97Z1,
	SEND_OPT_WDRST,
	SEND_OPT_PWRST,
	SEND_OPT_FDISK,
	SEND_OPT_DFRAG,
	SEND_OPT_CLRBF,
	SEND_OPT_RECFG,
};

static void sms_ctrl_handler ( u8 Event, u16 Nb ) {
	switch(Event) {
	case ADL_SMS_EVENT_SENDING_OK:
		vPutsTrace(30,"SMS Sent OK");
		APT_SMS_bSendOK = true;
		break;
	case ADL_SMS_EVENT_SENDING_ERROR:
		vPutsTrace(30,"SMS Sent ERROR");
		break;
	default:
		break;
	}
}

static bool sms_data_handler (ascii *SmsTel, ascii *SmsTime, ascii *SmsText) {
	vPutsTrace(30,SmsTel);
	vPutsTrace(30,SmsTime);
	vPutsTrace(30,SmsText);
	
	uint8_t i = 0;
	for(; i < APT_SMS_RCV_BUFF_NB ; i++) {if(APT_lstSMS_Rcv[i].Text[0]==0)break;}
	
	if(i>=APT_SMS_RCV_BUFF_NB){
		vPutsTrace(30,"SMS Receive Queue Fully");
		return FALSE;
	}
	
	strncpy(APT_lstSMS_Rcv[i].Text,SmsText,160);
	APT_lstSMS_Rcv[i].Text[160] = 0;
	strncpy(APT_lstSMS_Rcv[i].Tel,SmsTel,49);
	APT_lstSMS_Rcv[i].Tel[49] = 0;
	strncpy(APT_lstSMS_Rcv[i].Time,SmsTime,49);
	APT_lstSMS_Rcv[i].Time[49] = 0;

	return FALSE;
}

static void add_sms_queue(const ascii *Text, const ascii *Tel, u32 option) {
	if(APT_lstSMS_Snd.Text[0]==0) {
		wm_strcpy(APT_lstSMS_Snd.Text,Text);
		wm_strcpy(APT_lstSMS_Snd.Tel,Tel);
		APT_lstSMS_Snd.option = option;
	}else vPutsTrace(30,"SMS Send Queue Fully");
}

static bool sms_autherized(ascii *smsNumber) {
	/*
	 1.สุชาติ		0816673621
	 2.วีระยุทธ์		0823468310
	 3.จิตติ		0823468413
	 4.ภาพันธ์		0823467246
	 5.เรืองศิลป์		0823468458
	 6.สราวุฒณ์		0823469137
	 7.ปเนต		0823468552
	 8.ทัศนัย		0823468636
	 9.เกียรติศักดิ์	0823468625
	 10.พานุมาศ	0823468724
	 11.วิไลลักษณ์	0823468898
	 12.P'Kot		0818379885
	 13.Pan		0851370781
	 */
	
	ascii strNumber[20];
	u16 numLen = strlen(smsNumber);
	if (numLen > 10) {
		strNumber[0] = '0';
		strncpy(strNumber + 1, smsNumber + (numLen - 9), 9);
		strNumber[10] = '\0';
	}

	if (MATCHED_STR("0816673621", strNumber))return TRUE;		//1
	if (MATCHED_STR("0823468310", strNumber))return TRUE;		//2
	if (MATCHED_STR("0823468413", strNumber))return TRUE;		//3
	if (MATCHED_STR("0823467246", strNumber))return TRUE;		//4
	if (MATCHED_STR("0823468458", strNumber))return TRUE;		//5
	if (MATCHED_STR("0823469137", strNumber))return TRUE;		//6
	if (MATCHED_STR("0823468552", strNumber))return TRUE;		//7
	if (MATCHED_STR("0823468636", strNumber))return TRUE;		//8
	if (MATCHED_STR("0823468625", strNumber))return TRUE;		//9
	if (MATCHED_STR("0823468724", strNumber))return TRUE;		//10
	if (MATCHED_STR("0823468898", strNumber))return TRUE;		//11
	if (MATCHED_STR("0818379885", strNumber))return TRUE;		//12
	if (MATCHED_STR("0851370781", strNumber))return TRUE;		//13
	if (ini_config_utils.AdminPhone[0][0]) {
		if((strlen(ini_config_utils.AdminPhone[0])==10)
		&&(MATCHED_STR(ini_config_utils.AdminPhone[0], strNumber)))return TRUE;
	}
	if (ini_config_utils.AdminPhone[1][0]) {
		if((strlen(ini_config_utils.AdminPhone[1])==10)
		&&(MATCHED_STR(ini_config_utils.AdminPhone[1], strNumber)))return TRUE;
	}
	if (ini_config_utils.AdminPhone[2][0]) {
		if((strlen(ini_config_utils.AdminPhone[2])==10)
		&&(MATCHED_STR(ini_config_utils.AdminPhone[2], strNumber)))return TRUE;
	}
	if (ini_config_utils.AdminPhone[3][0]) {
		if((strlen(ini_config_utils.AdminPhone[3])==10)
		&&(MATCHED_STR(ini_config_utils.AdminPhone[3], strNumber)))return TRUE;
	}
	return FALSE;
}

static void APT_SMS_RESPONE_CHECK (const uint16_t send_delay) {
	static u8	send_counter = 0;
	static u16	Delay = 0;
	
	if((!arc_netsys.bSimReady)||(!arc_netsys.bGsmRegistered) || (!arc_netsys.bDeiveInitOK)) return;
	if(APT_lstSMS_Snd.Text[0]==0) return;
	if(APT_SMS_bSendOK ||(send_counter > 3))
	{	//Send success or time over
		send_counter = 0;
		APT_SMS_bSendOK = false;
		Delay = 0;
		switch(APT_lstSMS_Snd.option) {
		case SEND_OPT_REBOOT:
		case SEND_OPT_GETRESET:
			software_reset();
			break;
		case SEND_OPT_CMD_UPDATE:
			ini_config_setUpdate();
			break;
		case SEND_OPT_DOTA_REPORT:
			f_unlink(__OTA_LOG_PATH__);
			vPutsTrace(30,"DOTA Log Deleted");
			break;
		case SEND_OPT_WDRST:
			adl_atCmdSubscribeExecute("AT@WDRST",ADL_PORT_UART1);
			break;
		case SEND_OPT_PWRST:
			adl_atCmdSubscribeExecute("AT@PWRST",ADL_PORT_UART1);
			break;
		case SEND_OPT_FDISK:
			adl_atCmdSubscribeExecute("AT@FDISK",ADL_PORT_UART1);
			break;
		case SEND_OPT_DFRAG:
			adl_atCmdSubscribeExecute("AT@DFRAG",ADL_PORT_UART1);
			break;
		case SEND_OPT_G97Z1:
		case SEND_OPT_CLRBF:
			adl_atCmdSubscribeExecute("AT@CLRBF",ADL_PORT_UART1);
		break;
		case SEND_OPT_G46C0:
		case SEND_OPT_RECFG:
			adl_atCmdSubscribeExecute("AT@RECFG",ADL_PORT_UART1);
		break;
		}
		memset(&APT_lstSMS_Snd,0,sizeof(APT_lstSMS_Snd));
		
	}
	else {
		if(send_delay < Delay++) {
			send_counter++;	
			Delay = 0;
			adl_smsSend(APT_SMS_Handle,APT_lstSMS_Snd.Tel,APT_lstSMS_Snd.Text,ADL_SMS_MODE_TEXT);
			vPrintfTrace((30,"SMS to %s (%d)",APT_lstSMS_Snd.Tel,send_counter));
		}
	}
}

static void APT_SMS_CMD_CHECK (void) {
	ascii strBuff[161] = {""};
	bool permission_ok = false;
	uint8_t i = 0;
	for(; i < APT_SMS_RCV_BUFF_NB ; i++) {if(APT_lstSMS_Rcv[i].Text[0])break;}
	if(i>=APT_SMS_RCV_BUFF_NB) return;// No SMS
	aptSms_recv_t *sms = &APT_lstSMS_Rcv[i];
	
	if(MATCHED_STR("REBOOT",sms->Text))  {/*Reboot*/
		permission_ok = sms_autherized(sms->Tel);
		if(permission_ok) {
			strcpy(strBuff,"REBOOT:\n");
			strcat(strBuff,sms->Tel);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_REBOOT);
		}
		else {
			strcpy(strBuff,"REBOOT: Not Authorize\n");
			strcat(strBuff,sms->Tel);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
		}
	}
	else if(MATCHED_STR("WMDL=",sms->Text))  {/**/
		char * p = NULL;
		p = memmem(sms->Text,strlen(sms->Text),".swd",4);
		if(p) {
			*(p+4) = 0;//Add Null to end of command
			strcpy(strBuff,"AT#DOTA=");
			strcpy(strBuff + 8,sms->Text + 5);
			p = arc_shell_execute(strBuff);
			if(p){
				aptTracking_bootInfo.fw_source = FW_DOTA;
				aptTracking_bootInfo.verifyOK = FALSE;
				strcpy(aptTracking_bootInfo.sms_resp_numb,sms->Tel);
				strcpy(strBuff,"WMDL: OK\n");
				strcat(strBuff,sms->Tel);
				add_sms_queue(strBuff, sms->Tel,SEND_OPT_CMD_UPDATE);
			}
			else {
				strcpy(strBuff,"WMDL: Parameters Error\n");
				strcat(strBuff,sms->Tel);
				add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
			}
		}else {
			strcpy(strBuff,"WMDL: Command Error\n");
			strcat(strBuff,sms->Tel);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
		}
	}
	else if(MATCHED_STR("GET_CONFIG",sms->Text))  {/**/
		snprintf(strBuff,160,"AppliState=OK\nV.%03d%s\nAPN:%s,%s,%s\nTCP:%s,%d,0\nIdle:%d\n",
		SWD2015_FW_VERSION,ini_config_utils.DTCUniq.sFwName,
		ini_config_net.PPP.APN,ini_config_net.PPP.User,ini_config_net.PPP.PassW,
		ini_config_net.TCPServer1.IP,ini_config_net.TCPServer1.Port,
		ini_config_utils.Idling.Value);
		add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
	}
	else if(MATCHED_STR("GETRESET",sms->Text))  {/*Get Status and then Reset*/
		permission_ok = sms_autherized(sms->Tel);
		if(permission_ok) {
			snprintf(strBuff,160,"G:%d V:%d CON:%d TCP:%d WfS:%ld\nResetOK\nState=0\nAPN:%s,%s,%s\nTCP:%s,%d,0\nCar at:0,0",
			arc_netsys.bGprsAttached,arc_netsys.bGsmRegistered,arc_netsys.bPdpActivated,
			arc_hltcp_isConnected(CPE_SERVER_CTX_ID),apt_buffer_cpe_nbItem()+apt_buffer_cpl_nbItem(),
			ini_config_net.PPP.APN,ini_config_net.PPP.User,ini_config_net.PPP.PassW,
			ini_config_net.TCPServer1.IP,ini_config_net.TCPServer1.Port);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_GETRESET);
		}
		else {
			strcpy(strBuff,"GETRESET: Not Authorize\n");
			strcat(strBuff,sms->Tel);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
		}
		
	}
	else if(MATCHED_STR("GETSTATUS",sms->Text))  {/**/
		sprintf(strBuff,"$GVERSION=%03d%s,MODEL=SWR3G,SERIAL=%s,IMEI=%s,SIM=%s,IG=%s,SIGNAL=%d:0,GPRSSTATUS=%d:%d:%d,GPS=%s,BUFFER=%ld",
		SWD2015_FW_VERSION,ini_config_utils.DTCUniq.sFwName,
		aptswd_ccid,arc_netimei,ini_config_utils.DTCUniq.sSIMNumb,
		swdio_get_ig_status()?"ON":"OFF",arc_netsys.signLev,
		arc_netsys.bGsmRegistered,arc_netsys.bGprsAttached,
		arc_netsys.bPdpActivated,aptTracking_Runtime.GPS.fixDimnt>=3?"GPS_OK":"LOSS",
		apt_buffer_cpe_nbItem()+apt_buffer_cpl_nbItem());
		add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
	}
	else if(MATCHED_STR("GOP1OFF=",sms->Text))  {/**/
		permission_ok = sms_autherized(sms->Tel);
		if(permission_ok) {
			if(!wm_strcmp(sms->Text + 8, ini_config_utils.DTCUniq.sSIMNumb + 6 /*0851370781*/)) {
				arc_shell_execute("AT#OUTPUT1=0");
				swdio_set_outp1_status(1);//Connected to NC Relay
				add_sms_queue("GOUT1 COMMAND DEACTIVATED", sms->Tel,SEND_OPT_CMD_UPDATE);
			}else add_sms_queue("<CANCEL COMMAND RELAY [CODE SIM NOT CORRECT]", sms->Tel,SEND_OPT_NONE);
		}
		else {
			strcpy(strBuff,"GOP1OFF: Not Authorize\n");
			strcat(strBuff,sms->Tel);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
		}
	}
	else if(MATCHED_STR("GOP1ON=",sms->Text))  {/**/
		permission_ok = sms_autherized(sms->Tel);
		if(permission_ok) {
			if(!wm_strcmp(sms->Text + 7, ini_config_utils.DTCUniq.sSIMNumb + 6 /*0851370781*/)){
				arc_shell_execute("AT#OUTPUT1=1");
				swdio_set_outp1_status(0);//Connected to NC Relay
				add_sms_queue("GOUT1 COMMAND ACTIVATE", sms->Tel,SEND_OPT_CMD_UPDATE);	
			}else add_sms_queue("<CANCEL COMMAND RELAY [CODE SIM NOT CORRECT]", sms->Tel,SEND_OPT_NONE);
		}
		else {
			strcpy(strBuff,"GOP1ON: Not Authorize\n");
			strcat(strBuff,sms->Tel);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
		}
	}
	else if(MATCHED_STR("G46C0",sms->Text))  {/*reboot and get config from server again*/
		permission_ok = sms_autherized(sms->Tel);
		if(permission_ok){
			strcpy(strBuff,":G46C0 OK\n");
			strcat(strBuff,sms->Tel);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_G46C0);
		}
		else {
			strcpy(strBuff,"G46C0: Not Authorize\n");
			strcat(strBuff,sms->Tel);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
		}
	}
	else if(MATCHED_STR("G97Z1",sms->Text))  {/*Clear Buffer and Get Config*/
		permission_ok = sms_autherized(sms->Tel);
		if(permission_ok){
			add_sms_queue("RESET CPU AND CLEAR BUFFER OK...", sms->Tel,SEND_OPT_G97Z1);	
		}
		else {
			strcpy(strBuff,"G97Z1: Not Authorize\n");
			strcat(strBuff,sms->Tel);
			add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
		}
	}
	else if(MATCHED_STR("GETADC",sms->Text))  {/*ADC Oil*/
		sprintf(strBuff, "VOLTAGE\nOIL:%.2f VCAR:%.2f VBATT:%.2f",
		(float)aptTracking_Runtime.Fuel_Voltage/1000,(float) aptTracking_Runtime.MAIN_Voltage/1000,(float)aptTracking_Runtime.BATT_Voltage/1000 );
		 add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
	}
	else if(MATCHED_STR("G33EE",sms->Text))  {/* Location */
		sprintf(strBuff, "A%d,%s,%05ld,%s,\nLATTITUTE=%0.6f\nLONGTITUTE=%0.6f\nSPEED=%0.2f\n%s\n%s\nAT_CELLSITE:>?",
		SWD2015_FW_VERSION,aptSwd_get_sim_ccid(),apt_buffer_cpe_nbItem()+apt_buffer_cpl_nbItem(),
		ini_config_utils.DTCUniq.sSIMNumb, aptTracking_Runtime.GPS.lat,aptTracking_Runtime.GPS.lon,
		(double)aptTracking_Runtime.GPS.speed,aptTracking_Runtime.GPS.fixDimnt>=3?"GPS_OK":"GPS_LOSS",
		swdio_get_ig_status()? bLivingInIdleMode ? "CAR IDLE" : "CAR POWER ON" : "CAR STOP");
		add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
	}
	else if((MATCHED_STR("GSTOP=",sms->Text))&& swd_feature_isEnable(FEAT_SMS_CUTSTART))  {
		permission_ok = sms_autherized(sms->Tel);
		if(permission_ok) {
			if(!wm_strcmp(sms->Text + 6, ini_config_utils.DTCUniq.sSIMNumb + 6 /*0851370781*/)) {
				arc_shell_execute("AT#OUTPUT1=0");
				swdio_set_outp1_status(1);//Connected to NC Relay
				add_sms_queue("GSTOP COMMAND RELAY ACTIVATED", sms->Tel,SEND_OPT_CMD_UPDATE);
			}else add_sms_queue("<CANCEL COMMAND RELAY[CODE SIM NOT CORRECT]", sms->Tel,SEND_OPT_NONE);
		}
	}
	else if((MATCHED_STR("GRUN=",sms->Text))&& swd_feature_isEnable(FEAT_SMS_CUTSTART))  {
		permission_ok = sms_autherized(sms->Tel);
		if(permission_ok) {
			if(!wm_strcmp(sms->Text + 5, ini_config_utils.DTCUniq.sSIMNumb + 6 /*0851370781*/)) {
				arc_shell_execute("AT#OUTPUT1=1");
				swdio_set_outp1_status(0);//Connected to NC Relay
				add_sms_queue("GRUN COMMAND RELAY ACTIVATED", sms->Tel,SEND_OPT_CMD_UPDATE);
			}else add_sms_queue("<CANCEL COMMAND RELAY[CODE SIM NOT CORRECT]", sms->Tel,SEND_OPT_NONE);
		}
	}
	else if((MATCHED_STR("GWAIT=",sms->Text))&& swd_feature_isEnable(FEAT_SMS_CUTSTART))  {
		permission_ok = sms_autherized(sms->Tel);
		if(permission_ok) {
			if(!wm_strcmp(sms->Text + 6, ini_config_utils.DTCUniq.sSIMNumb + 6 /*0851370781*/)) {
				arc_shell_execute("AT#OUTPUT1=0");
				if(!swdio_get_ig_status()) swdio_set_outp1_status(1);//Connected to NC Relay
				add_sms_queue("GWAIT COMMAND RELAY ACTIVATED", sms->Tel,SEND_OPT_CMD_UPDATE);
			}else add_sms_queue("<CANCEL COMMAND RELAY[CODE SIM NOT CORRECT]", sms->Tel,SEND_OPT_NONE);	
		}
	}
	else{
		if(sms->Text[strlen(sms->Text)-1] == '?')permission_ok = true;
		else permission_ok = sms_autherized(sms->Tel);
		
		if(MATCHED_STR("AT#",sms->Text) && permission_ok)  {
			char * p = NULL;
			if(!wm_strnicmp(sms->Text+3,"DOTA=",5)) {
				p = memmem(sms->Text,strlen(sms->Text),".swd",4);
				if(p) {
					*(p+4) = 0;//Add Null to end of command
					p = arc_shell_execute(sms->Text);
					if(p){
						aptTracking_bootInfo.fw_source = FW_DOTA;
						aptTracking_bootInfo.verifyOK = FALSE;
						strcpy(aptTracking_bootInfo.sms_resp_numb,sms->Tel);
						snprintf(strBuff,160,p,strlen(p));
						add_sms_queue(strBuff, sms->Tel,SEND_OPT_CMD_UPDATE);
					}
					else {
						strcpy(strBuff,"#DOTA: Parameters Error\n");
						strcat(strBuff,sms->Tel);
						add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
					}
				}
				else {
					strcpy(strBuff,"#DOTA: Command Error\n");
					strcat(strBuff,sms->Tel);
					add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
				}
			}
			else {
				const ascii *ptxt = sms->Text;
				for(;*ptxt;ptxt++) {
					if(*ptxt =='=' || *ptxt =='?') break;
				}
				wm_strncpy(strBuff,sms->Text+2, (ptxt - sms->Text)-2);//Remove 'AT'
				strBuff[(ptxt - sms->Text)-2] = 0;
				wm_strcat(strBuff,": ");//Append ':
				
				p = arc_shell_execute(sms->Text);
				if(p){
					strcat(strBuff,p);
					add_sms_queue(strBuff, sms->Tel,SEND_OPT_CMD_UPDATE);
				}
				else {
					strcat(strBuff,"Command Error");
					add_sms_queue(strBuff, sms->Tel,SEND_OPT_NONE);
				}
			}
		}
		else if(MATCHED_STR("AT@",sms->Text) && permission_ok)  {// MCU Internal Subscribe Command
			if(MATCHED_STR("WDRST",sms->Text+3)) add_sms_queue("@WDRST: OK", sms->Tel,SEND_OPT_WDRST);
			else if(MATCHED_STR("PWRST",sms->Text+3)) add_sms_queue("@PWRST: OK", sms->Tel,SEND_OPT_PWRST);
			else if(MATCHED_STR("FDISK",sms->Text+3)) add_sms_queue("@FDISK: OK", sms->Tel,SEND_OPT_FDISK);
			else if(MATCHED_STR("DFRAG",sms->Text+3)) add_sms_queue("@DFRAG: OK", sms->Tel,SEND_OPT_DFRAG);
			else if(MATCHED_STR("CLRBF",sms->Text+3)) add_sms_queue("@CLRBF: OK", sms->Tel,SEND_OPT_CLRBF);
			else if(MATCHED_STR("RECFG",sms->Text+3)) add_sms_queue("@RECFG: OK", sms->Tel,SEND_OPT_RECFG);
			else {
				bool isOK = true;
				isOK = adl_atCmdSubscribeExecute(sms->Text,ADL_PORT_UART1);
				if(isOK) strcat(sms->Text,": OK");
				else  strcat(sms->Text,": ERROR");
				add_sms_queue(sms->Text+2, sms->Tel,SEND_OPT_NONE);
			}
		}
		else if(MATCHED_STR("AT!",sms->Text) && permission_ok)  {// GSM Command
			sms->Text[2] = '+';
			adl_atCmdCreate(sms->Text, ADL_AT_PORT_TYPE(ADL_PORT_UART1,FALSE),NULL,(void*)0);
		}
		else if(MATCHED_STR("AT*",sms->Text) && permission_ok)  {// Open AT Command
			sms->Text[2] = '#';
			adl_atCmdCreate(sms->Text, ADL_AT_PORT_TYPE(ADL_PORT_UART1,FALSE),NULL,(void*)0);
		}
	}
	
	memset(sms,0,sizeof(aptSms_recv_t));
}

void aptSms_SendForStart(void) {
	FIL ota_object;
	FRESULT res;
	
	res = f_open(&ota_object, (char const *)__OTA_LOG_PATH__, FA_READ | FA_OPEN_EXISTING);
	if (res == FR_OK) {
		size_t bytes;
		arc_bootld_init_t boot_init;
		res = f_read(&ota_object, &boot_init, sizeof(arc_bootld_init_t), &bytes);
		if (res == FR_OK) {
			if(boot_init.fw_source == FW_DOTA) {
				char msg_buff[65] = {""};
				vPrintfTrace((0,"SMS REMOTE: %s",boot_init.sms_resp_numb));
				if(boot_init.verifyOK) {
					sprintf(msg_buff,"#DOTA: Success\r\nV.%d %s:%s",SWD2015_FW_VERSION,__DATE__,__TIME__);
					vPutsTrace(0,msg_buff);
				}
				else {
					strcpy(msg_buff,"#DOTA: FAILED");
					vPutsTrace(0,msg_buff);
				}
				add_sms_queue(msg_buff, boot_init.sms_resp_numb,SEND_OPT_DOTA_REPORT);
			}else vPutsTrace(0,"OTA FROM LOCAL SOURCE");
		}else vPutsTrace(0,"OTA READ ERROR");
		f_close(&ota_object);
	}
	else {
		vPutsTrace(0,"OTA LOG NOT FOUND");
		char msg_buff[163] = {"\r\n"};
		error_reset_reason(msg_buff+2);
		add_sms_queue(msg_buff+2, "0851370781",SEND_OPT_NONE);	
		vPuts(msg_buff);
	}
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
void aptSms_task(void * pvParameters) {
	static const portTickType xFrequency = 100UL / portTICK_RATE_MS;
	
	static uint32_t * pWdCount = NULL;
		
	pWdCount = (uint32_t*)pvParameters;

	APT_SMS_Handle = adl_smsSubscribe(sms_data_handler, sms_ctrl_handler, ADL_SMS_MODE_TEXT);
	
	while(!aptSwd_task_completed()){
		*pWdCount = 0;//Clear watchdog
		vTaskDelay(10);
	}
	
	for(;;) {

		*pWdCount = 0;//Clear watchdog
		
		APT_SMS_CMD_CHECK();
		APT_SMS_RESPONE_CHECK(_100mSecTick_Second(15)); //   15 seconds
				
		vTaskDelay(xFrequency);
	}
}