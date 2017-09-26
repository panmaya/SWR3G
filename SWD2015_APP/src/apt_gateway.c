/*
 * apt_gateway.c
 *
 *  Created on: 24 ¡.¾. 2555
 *      Author: Pan
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
#include "feature.h"
#include "apt_swd.h"
#include "vprinter.h"
#include "card_authen.h"
#include "swd_io.h"
#include "swd_event.h"
#include "error.h"
#include "apt_led_send.h"

#define APT_GATEWAY_TRACE	6

extern aptTracking_runtime_t 	aptTracking_Realtime;

static uint16_t APT_GATEWAY_ServerNoRspTime = 0;
static uint16_t APT_GATEWAY_Upload_TimeDelay = 0;
static trans_form_raw_t APT_GATEWAY_TCP_RawUploadCx = {0,(void*)0};
static bool APT_GATEWAY_TCP_RcvOKResp = false;
static enum {UPL_NODATA, UPL_GPBM_BUFFER , UPL_CPE_BUFFER , UPL_CPL_BUFFER} APT_GATEWAY_UPL_Type = UPL_NODATA;
	
#define  APT_GATEWAY_TCP_STACK_SIZE		513
static uint8_t APT_GATEWAY_TCP_STACK_Buffer[APT_GATEWAY_TCP_STACK_SIZE];
static ascii APT_GATEWAY_TCP_CMD_Buffer[161] = {""};
	
static uint8_t GPBM_Buffer_Index = 0;

uint8_t cpe_del_trigger = false;
uint8_t cpl_del_trigger = false;

static void APP_CHANNEL_SET_Handler(u8 ssid, s8 event, void *context){
	ascii buffer[41] = {"TCP "};
	switch(event) {
		case ARC_HLNET_EV_READ:
		if(context) {
			arc_hltcp_ctx_t* ctx = context;
			ctx->data[ctx->length] = 0;
			strcat(buffer+4,"RCV");
			
			if(ctx->length>=2) {
				if(strstr((char*)ctx->data,"oK")){
					aptLedSend_server(TRUE);
					APT_GATEWAY_ServerNoRspTime = 0;//Clear
					APT_GATEWAY_TCP_RawUploadCx.size = 0;//Clear
					APT_GATEWAY_Upload_TimeDelay = 0;//Clear
					APT_GATEWAY_TCP_RcvOKResp = true;
					break;
				}
				else if(strstr((char*)ctx->data,"ER")){
					APT_GATEWAY_ServerNoRspTime = 0;//Clear
					APT_GATEWAY_TCP_RawUploadCx.size = 0;//Clear
					APT_GATEWAY_Upload_TimeDelay = 0;//Clear
					aptLedSend_server(FALSE);
					break;
				}
				else if((ctx->length>=6)) strncpy(APT_GATEWAY_TCP_CMD_Buffer,(char*)ctx->data,160);
			}
		}
		break;
		case ARC_HLNET_EV_CONNECTED:strcat(buffer+4,"CON OK");break;
		case ARC_HLNET_EV_PEER_CLOSE:strcat(buffer+4,"PEER CLOSE");break;
		case ARC_HLNET_EV_CONN_ERR:strcat(buffer+4,"CON ERR");break;
		case ARC_HLNET_EV_NORMAL_ERR:strcat(buffer+4,"NORM ERR");break;
		case ARC_HLNET_EV_ACCEPT_ERR:strcat(buffer+4,"ACCEPT ERR");break;
		case ARC_HLNET_EV_TIMEOUT: strcat(buffer+4,"TIMEOUT");break;
		case ARC_HLNET_EV_NETW_ERR: strcat(buffer+4,"NW ERR");break;
		case ARC_HLNET_EV_MAX_SOCK: strcat(buffer+4,"MAX SOCK");break;
		case ARC_HLNET_EV_MEM_ERR:strcat(buffer+4,"MEM ERR");break;
		case ARC_HLNET_EV_DNS_ERR: strcat(buffer+4,"DNS ERR");break;
		case ARC_HLNET_EV_SEND_WAIT: strcat(buffer+4,"SND WAIT");break;
		case ARC_HLNET_EV_BAD_SSID:	strcat(buffer+4,"BAD SSID");break;
		case ARC_HLNET_EV_SSID_RUNING: strcat(buffer+4,"SSID RUNING");break;
		case ARC_HLNET_EV_SSID_FULLY: strcat(buffer+4,"SSID FULLY");break;
		case ARC_HLNET_EV_RELEASE: buffer[0] = '\0';break;
		case ARC_HLNET_EV_ALIVE: return; break;
	}
	if(buffer[0])vPutsTrace(APT_GATEWAY_TRACE,buffer);
}

static void APT_GATEWAY_1Sec_Workspace ( void ) {// 1 Second Timer
	static uint8_t delay_for_next_send = 0;
	static uint8_t print_show_delay = 0;
	static uint32_t cpe_time = 0;
	static uint32_t cpl_time = 0;
	
	if(print_show_delay > _1SecTick_Second(5)) {
		print_show_delay = 0;
		vPrintfTrace((30,"SIM-%d, CSQ-%d, GSM-%d, GPRS-%d, 3G-%d, IP-%d, DMS-%d, DAT-%d",arc_netsys.bSimReady,arc_netsys.signLev,
			arc_netsys.bGsmRegistered?arc_netsys.bNetworkRoaming?5:1:0,
			arc_netsys.bGprsAttached,arc_netsys.b3GRegistered,arc_netsys.bPdpActivated,
			arc_hltcp_isConnected(DMS_SERVER_CTX_ID),arc_hltcp_isConnected(CPE_SERVER_CTX_ID)));
	}else print_show_delay++;
	
	UP_COUNTER_OVERFLOW_LIMIT(APT_GATEWAY_ServerNoRspTime,TRUE,65535);

	if(APT_GATEWAY_TCP_RawUploadCx.size!=0) {
		UP_COUNTER_OVERFLOW_ZERO(APT_GATEWAY_Upload_TimeDelay,TRUE,60*60*12);
		if(!(APT_GATEWAY_Upload_TimeDelay % _1SecTick_Second(60*10))) { // 10 minute wait; Something wrong
			vPrintfTrace((30,"TCP RE-START"));
			arc_hltcp_ReStart(CPE_SERVER_CTX_ID);
		}
		else if(!(APT_GATEWAY_Upload_TimeDelay % _1SecTick_Second(30))) { // server no response 60 sec -> try upload
			vPrintfTrace((30,"TCP TRY RE-SEND"));
			APT_GATEWAY_TCP_RawUploadCx.size = 0;
		}
		return;	
	}
	
	if(APT_GATEWAY_TCP_RcvOKResp) {
		if(APT_GATEWAY_UPL_Type == UPL_CPE_BUFFER)cpe_del_trigger++;//apt_buffer_cpe_del();
		else if(APT_GATEWAY_UPL_Type == UPL_CPL_BUFFER)cpl_del_trigger++;//apt_buffer_cpl_del();
		else if((APT_GATEWAY_UPL_Type == UPL_GPBM_BUFFER)
		&&(GPBM_Buffer_Index>=GPBM_LOGIN_LOGOUT_1_ADDR))buffer_gpbm_erase(GPBM_Buffer_Index);
		APT_GATEWAY_TCP_RcvOKResp = false;
	}
	
	if((!arc_netsys.bSimReady)||(!arc_netsys.bGsmRegistered)) return;
	
	if(delay_for_next_send < _1SecTick_Second(5)){
		delay_for_next_send++;
		if(!(delay_for_next_send%2)) {
			if(apt_buffer_cpe_nbItem()){
				cpe_time = apt_buffer_cpe_ctime();
				if(cpe_time==0){
					vPrintfTrace((30,"CPE: Bad Sector - Delete %ld Byte",apt_buffer_cpe_csize()));
					apt_buffer_cpe_del();
				}
			}else cpe_time = 0;
		}
		else {
			if(apt_buffer_cpl_nbItem()){
				cpl_time = apt_buffer_cpl_ctime();
				if(cpl_time==0){
					vPrintfTrace((30,"CPL: Bad Sector - Delete %ld Byte",apt_buffer_cpl_csize()));
					apt_buffer_cpl_del();
				}
			}else cpl_time = 0;
		}
		return;
	}else delay_for_next_send = 0;
	
	if(cpe_time && cpl_time) cpe_time = cpl_time <= cpe_time ? 0 : cpe_time;
	
	uint32_t oldest_gpbm_sn = 0xFFFFFFFF;
	GPBM_Buffer_Index = 0;
	
	for(uint8_t i = GPBM_LOGIN_LOGOUT_1_ADDR; i <= GPBM_LOGIN_LOGOUT_5_ADDR ; i+=4) {
		u32 exist_sn = buffer_gpbm_getsn(i);
		if(exist_sn && (exist_sn <= oldest_gpbm_sn)) {
			oldest_gpbm_sn = exist_sn;
			GPBM_Buffer_Index = i;//Find Oldest Record ID
		}
	}
		
	if(GPBM_Buffer_Index) {
		memset(APT_GATEWAY_TCP_RawUploadCx.data,0,APT_GATEWAY_TCP_STACK_SIZE);
		buffer_gpbm_read(GPBM_Buffer_Index, APT_GATEWAY_TCP_RawUploadCx.data, 512);
		APT_GATEWAY_TCP_RawUploadCx.size = strlen((char*)APT_GATEWAY_TCP_RawUploadCx.data);
		APT_GATEWAY_TCP_RawUploadCx.data[APT_GATEWAY_TCP_RawUploadCx.size] = '\0';
		APT_GATEWAY_UPL_Type = UPL_GPBM_BUFFER;
		lvPutsLabel(30,"CA> ",(char*)APT_GATEWAY_TCP_RawUploadCx.data,APT_GATEWAY_TCP_RawUploadCx.size);
		arc_hltcp_Send(CPE_SERVER_CTX_ID, (u16)APT_GATEWAY_TCP_RawUploadCx.size, (u8*)APT_GATEWAY_TCP_RawUploadCx.data);
		aptLedSend_sending();
	}
	else if(cpe_time) {
		static aptTracking_runtime_t apt_bufferData;
		if((apt_buffer_cpe_csize()==sizeof(aptTracking_runtime_t))) {
			apt_buffer_cpe_get(&apt_bufferData,sizeof(aptTracking_runtime_t));
			memset(APT_GATEWAY_TCP_RawUploadCx.data,0,APT_GATEWAY_TCP_STACK_SIZE);
			APT_GATEWAY_TCP_RawUploadCx.size = transform_to_sharpE((char*)APT_GATEWAY_TCP_RawUploadCx.data, &apt_bufferData, card_authenGetTrack3(apt_bufferData.Status.Trk3id));
			APT_GATEWAY_TCP_RawUploadCx.data[APT_GATEWAY_TCP_RawUploadCx.size] = '\0';
			APT_GATEWAY_UPL_Type = UPL_CPE_BUFFER;
			lvPutsLabel(30,"CE> ",(char*)APT_GATEWAY_TCP_RawUploadCx.data,APT_GATEWAY_TCP_RawUploadCx.size);
			arc_hltcp_Send(CPE_SERVER_CTX_ID, (u16)APT_GATEWAY_TCP_RawUploadCx.size, (u8*)APT_GATEWAY_TCP_RawUploadCx.data);
			aptLedSend_sending();
		}
		else {
			vPrintfTrace((30,"CPE: Unknown Data - Delete %ld Byte",apt_buffer_cpe_csize()));
			apt_buffer_cpe_del();
		}
	}
	else if(cpl_time){
		static s32 size_data = 0;
		size_data = apt_buffer_cpl_csize();
		if((size_data > 0) && (size_data <= 512)) {
			memset(APT_GATEWAY_TCP_RawUploadCx.data,0,APT_GATEWAY_TCP_STACK_SIZE);
			APT_GATEWAY_TCP_RawUploadCx.size = size_data;
			apt_buffer_cpl_get(APT_GATEWAY_TCP_RawUploadCx.data,size_data);
			APT_GATEWAY_TCP_RawUploadCx.data[APT_GATEWAY_TCP_RawUploadCx.size] = '\0';
			APT_GATEWAY_UPL_Type = UPL_CPL_BUFFER;
			lvPutsLabel(30,"CL> ",(char*)APT_GATEWAY_TCP_RawUploadCx.data,APT_GATEWAY_TCP_RawUploadCx.size);
			arc_hltcp_Send(CPE_SERVER_CTX_ID, (u16)APT_GATEWAY_TCP_RawUploadCx.size, (u8*)APT_GATEWAY_TCP_RawUploadCx.data);
			aptLedSend_sending();
		}
		else {
			vPrintfTrace((30,"CPL: Unknown Data - Delete %ld Byte",apt_buffer_cpl_csize()));
			apt_buffer_cpl_del();
		}
	}
	else {
		if(APT_GATEWAY_TCP_CMD_Buffer[0]) {
			ascii strBuff[161] = {""};
			char *pBuff = &APT_GATEWAY_TCP_CMD_Buffer[0];
			
			if(!strncmp(pBuff,"AT#",3)) {
				const ascii *ptxt = pBuff;
				for(;*ptxt;ptxt++) {
					if(*ptxt =='=' || *ptxt =='?') break;
				}
				wm_strncpy(strBuff,pBuff+2, (ptxt - pBuff)-2);//Remove 'AT'
				strBuff[(ptxt - pBuff)-2] = 0;
				wm_strcat(strBuff,": ");//Append ':
				char * p = arc_shell_execute(pBuff);
				if(p)strcat(strBuff,p);
				else strcat(strBuff,"Command Error");
				arc_hltcp_Send(CPE_SERVER_CTX_ID, (u16)strlen(strBuff), (u8*)strBuff);
			}
			memset(APT_GATEWAY_TCP_CMD_Buffer,0,sizeof(APT_GATEWAY_TCP_CMD_Buffer));
		}
	}
}

bool aptGateway_serverTimeout(void) {  
	return APT_GATEWAY_ServerNoRspTime > _1SecTick_Second(20*60) ? true : false;//20 minuites
}

uint16_t aptGateway_uploadDelay(void) {
	return APT_GATEWAY_Upload_TimeDelay;
}
	
void aptGateway_task(void * pvParameters) {
	static uint32_t counter_mSec1 = 0;
	static uint32_t * pWdCount = NULL;
		
	pWdCount = (uint32_t*)pvParameters;
	
	APT_GATEWAY_TCP_RawUploadCx.data = &APT_GATEWAY_TCP_STACK_Buffer[0];
	APT_GATEWAY_TCP_RawUploadCx.size = 0;
	
	while(!aptTracking_task_completed()){
		*pWdCount = 0;//Clear watchdog
		vTaskDelay(10);
	}
	

	arc_hltcp_Subscribe(APP_CHANNEL_SET_Handler);
	
	arc_hlcnx_SetConfig	(	1,
							ARC_HLNET_OPT_APN, ini_config_net.PPP.APN,
							ARC_HLNET_OPT_USER, ini_config_net.PPP.User,
							ARC_HLNET_OPT_PASS,ini_config_net.PPP.PassW,
							ARC_HLNET_OPT_END );
						
	arc_hltcp_SetConfig	(	CPE_SERVER_CTX_ID,
							ARC_HLNET_OPT_CNXID, 1,
							ARC_HLNET_OPT_MODE,ARC_HLTCP_MODE_CLIENT,
							ARC_HLNET_OPT_ADDR,ini_config_net.TCPServer1.IP,
							ARC_HLNET_OPT_PORT,ini_config_net.TCPServer1.Port,
							ARC_HLNET_OPT_END );
	if(arc_hltcp_Start(CPE_SERVER_CTX_ID,(uint32_t*)&wd_monitor[all_task_created_nb],&tassk_handle[all_task_created_nb])>=0) {
		all_task_created_nb++;
		vPutsTrace(APT_GATEWAY_TRACE,"TCP-DAT: CREATED OK\r\n");
	}
	else {
		vPutsTrace(APT_GATEWAY_TRACE,"TCP-DAT: CREATE FAILED\r\n");
		software_reset();
	}
				
	for(;;) {
		
		counter_mSec1 += (*pWdCount);
		
		*pWdCount = 0;//Clear watchdog
		
		if(counter_mSec1 >= 1000) {
			counter_mSec1 = 0;
			APT_GATEWAY_1Sec_Workspace();
		}
		
		vTaskDelay(100);
		
	}
}