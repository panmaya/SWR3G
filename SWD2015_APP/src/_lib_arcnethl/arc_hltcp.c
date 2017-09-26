/*
 * arc_hlxtcp.c
 *
 * Created: 13/11/2557 14:24:45
 *  Author: pan
 */ 
#include "var/arc_hlnet.h"
#include "ctrl/arc_hlaccess.h"
#include "apt/apt_gsm_uart.h"
#include "arc_hltcp.h"
#include "arc_hlcnx.h"
#include "misc_config.h"

#include <stdarg.h>

typedef enum ARC_HLTCP_TASK_SEQ_E {
	ARC_HLTCP_TASK_IDLE,
	ARC_HLTCP_TASK_TCPCFG,
	ARC_HLTCP_TASK_CONNECT,
	ARC_HLTCP_TASK_GET_STAT,
	ARC_HLTCP_TASK_DATA,
}arc_hltcp_taskseq_e;

typedef struct ARC_HLTCPCFG_T {
	uint16_t  cnxid;
	arc_hltcp_mode_e tcpcfg_mode;
	char remote_addr[16];
	uint16_t   tcp_port;
	uint16_t   source_port;
	bool  data_mode;
	bool  urc_endtcp;
}arc_hltcpcfg_t;

typedef struct ARC_HLTCP_STATUS_T {
	enum {
	ARC_HLTCP_SOCK_NOTDEF,
	ARC_HLTCP_SOCK_NOTUSE,
	ARC_HLTCP_SOCK_OPENING,
	ARC_HLTCP_SOCK_CONNECT,
	ARC_HLTCP_SOCK_CLOSING,
	ARC_HLTCP_SOCK_CLOSED,		
	}esock;
	int8_t	notify;
	uint16_t send_remain;
	uint16_t recv_size;
}arc_hltcp_status_e;

typedef struct ARC_HLTCP_SNDBUFF_T {
	uint16_t length;
	uint16_t idx;
	uint8_t  data[1];
}arc_hltcp_SndBuff_t;

typedef struct ARC_HLTCP_OBJ_T {
	bool enableTask;
	xTaskHandle hTask;
	uint8_t *pTaskName;
	uint8_t	session_id;
	uint16_t dev_timeout;
	uint16_t alive_time;
	bool connected;
	uint16_t sending_size;
	uint16_t reading_size;
	arc_hltcpcfg_t cfg;
	arc_hltcp_status_e eStatus;
	arc_hltcp_taskseq_e eTaskSeq;
	arc_hltcp_SndBuff_t *pSndBuff;
}arc_hltcpobject_t;

static arc_hltcp_fCbk ARC_HLTCP_Cbk = NULL;
static arc_hltcpobject_t *ARC_HLTCP_ObjTab[ARC_HLTCP_MAX_SESSION]= {NULL,NULL,NULL,NULL};
static uint8_t ARC_HLTCP_CTX_Buffer[APT_GSM_UART_BUFFER_SIZE];

#define	ARC_HLTCP_TRY_MAXNB		5
#define ARC_HLTCP_GETSTAT_TIMEOUT	60

//AT+KCGPADDR
#define ARC_HLCNX_GETADDR()	do{\
								char cmd[15] = {"AT+KCGPADDR\r"};\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
							}while(0)

//AT+KTCPCFG=[<cnx cnf>],<mode>,[<tcp remoteaddress>],<tcp_port>[,[<source_port>][, [<data_mode>],[<URC-ENDTCPenable>]]]
#define ARC_HLTCP_TCPCFG(_x) do{\
								char cmd[100] = {"AT+KTCPCFG="};\
								sprintf(cmd+11,"%d,%d,\"%s\",%d\r",	ARC_HLTCP_ObjTab[_x]->cfg.cnxid,\
																	ARC_HLTCP_ObjTab[_x]->cfg.tcpcfg_mode,\
																	ARC_HLTCP_ObjTab[_x]->cfg.remote_addr,\
																	ARC_HLTCP_ObjTab[_x]->cfg.tcp_port);\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
							}while(0)
							
//AT+KTCPCNX=<session_id>				
#define ARC_HLTCP_TCPCNX(_x) do{\
								char cmd[33] = {"AT+KTCPCNX="};\
								sprintf(cmd+11,"%d\r",ARC_HLTCP_ObjTab[_x]->session_id);\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
							}while(0)
							
//AT+KTCPCLOSE=<session_id>,<closing_type>							
#define ARC_HLTCP_TCPCLOSE(_x) do{\
								char cmd[33] = {"AT+KTCPCLOSE="};\
								sprintf(cmd+13,"%d,0\r",ARC_HLTCP_ObjTab[_x]->session_id);\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)
								
//AT+KTCPDEL=<session_id>								
#define ARC_HLTCP_TCPDEL(_x) do{\
								char cmd[33] = {"AT+KTCPDEL="};\
								sprintf(cmd+11,"%d\r",ARC_HLTCP_ObjTab[_x]->session_id);\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
							}while(0)

//AT+KTCPRCV=<session_id>,<n-data>
#define ARC_HLTCP_TCPRCV(_x) do{\
								char cmd[33] = {"AT+KTCPRCV="};\
								sprintf(cmd+11,"%d,%d\r",ARC_HLTCP_ObjTab[_x]->session_id, ARC_HLTCP_ObjTab[_x]->reading_size);\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
							}while(0)

//AT+KTCPSND=<session_id>,<n-data>
#define ARC_HLTCP_TCPSND(_x) do{\
								char cmd[33] = {"AT+KTCPSND="};\
								sprintf(cmd+11,"%d,%d\r",ARC_HLTCP_ObjTab[_x]->session_id,\
														ARC_HLTCP_ObjTab[_x]->sending_size);\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
							}while(0)

//AT+KTCPSTAT=<session_id>
#define ARC_HLTCP_TCPSTAT(_x) do{\
								char cmd[33] = {"AT+KTCPSTAT="};\
								sprintf(cmd+12,"%d\r",ARC_HLTCP_ObjTab[_x]->session_id);\
								/*ARC_HLNET_STR_DEBUG(cmd);*/\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
							}while(0)
							
//AT+KURCCFG=<protocol>,<active>			
#define ARC_HLTCP_DIS_URC() do{\
								char cmd[33] = {"AT+KURCCFG=\"TCP\",0\r"};\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
							}while(0)
							
//AT+KURCCFG=<protocol>,<active>
#define ARC_HLTCP_EOF_FORCE() do{\
								char cmd[33] = {ARC_HLNET_EOF_PATTERN};\
								char cmd2[2] = {"\r"};\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								vTaskDelay(10);\
								aptGsmUart_Write((uint8_t*)cmd2,strlen(cmd2));\
							}while(0)



bool arc_hltcp_isConnected(uint8_t conf_id){
	//if(arc_netsys.bPdpActivated==FALSE) return FALSE;
	return ARC_HLTCP_ObjTab[conf_id]->eStatus.esock == ARC_HLTCP_SOCK_CONNECT ? TRUE : FALSE;
}					
/*--------------------------------------------------------------------*/
/* Unsolicited:                                                   */
/* Properties:          Set Options                          */
/*Expect String:                                                */
/*--------------------------------------------------------------------*/
static void ARC_HLTCP_OPTION_SET(va_list *ap, int option, arc_hltcpcfg_t *tcpcfg){
	switch(option) {
		case ARC_HLNET_OPT_CNXID:
		tcpcfg->cnxid =  va_arg(*ap,int);
		break;
		case ARC_HLNET_OPT_MODE:
		tcpcfg->tcpcfg_mode =  va_arg(*ap,int);
		break;
		case ARC_HLNET_OPT_ADDR:
		{
			char * pstr = NULL;
			pstr = va_arg(*ap,char*);
			strcpy(tcpcfg->remote_addr, pstr);
		}
		break;
		case ARC_HLNET_OPT_PORT:
		tcpcfg->tcp_port =  va_arg(*ap,int);
		break;
		case ARC_HLNET_OPT_SRCPORT:
		tcpcfg->source_port =  va_arg(*ap,int);
		break;
		case ARC_HLNET_OPT_DATAMODE:
		tcpcfg->data_mode =  va_arg(*ap,int);
		break;
		case ARC_HLNET_OPT_URCTCP:
		tcpcfg->urc_endtcp =  va_arg(*ap,int);
		break;
		case ARC_HLNET_OPT_END:
		default:
		/* nothing to do*/
		break;
	}
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Procedure Timer Handler           */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static arc_hltcp_taskseq_e ARC_HLTCP_CoRoutine(uint8_t conf_id, arc_hltcp_taskseq_e CurrState) {
	uint8_t * pStr = NULL;
	uint8_t * pLocalData = NULL;
	uint16_t iLocalDataSize = 0;
	arc_hlcnxcfg_t *hcnx = NULL;
	uint8_t timeout = 0;
	static uint16_t getstat_wait2sendCount = 0;
	static uint16_t terminate_timeout = 0;
				
	switch(CurrState) {
	case ARC_HLTCP_TASK_TCPCFG:
	
		if(!arc_netsys.bDeiveInitOK) break;
		
		getstat_wait2sendCount = 0;
		ARC_HLTCP_ObjTab[conf_id]->connected = FALSE;
		ARC_HLTCP_ObjTab[conf_id]->session_id = 0;
		hcnx = arc_hlcnx_GetConfig(ARC_HLTCP_ObjTab[conf_id]->cfg.cnxid);
		if(hcnx->is_activated) {
			ARC_HLTCP_TCPCFG(conf_id);
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,10,1000);
				pStr = NULL;
				if(iLocalDataSize>=10) pStr = memmem(pLocalData,iLocalDataSize,"+KTCPCFG: ",10);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLTCP_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			if(pStr) {
				char strid[5] = {""};
				uint32_t i = 0;
				bool isCRdetected = FALSE;
				ARC_HLTCP_ObjTab[conf_id]->dev_timeout = 0;
				
				for(;pStr[i]&&(!isCRdetected);i++) { if(pStr[i]=='\r') isCRdetected = TRUE; }
				
				if(isCRdetected) {
					wm_strGetParameterString(strid, sizeof(strid),(char*)pStr,1);
					ARC_HLTCP_ObjTab[conf_id]->session_id = wm_atoi(strid);

					ARC_HLTCP_DIS_URC();
					timeout = 0;
					do {
						timeout++;
						iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
						pStr = NULL;
						if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"OK",2);
						vTaskDelay(1);
					}while(!pStr && (timeout <= ARC_HLTCP_TRY_MAXNB));
					
					ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
					
					if(pStr) {
						// Call Back to subscriber function
						if(!ARC_HLTCP_ObjTab[conf_id]->pSndBuff) {
							ARC_HLTCP_Cbk(conf_id, ARC_HLNET_EV_RELEASE, NULL);
						}
						return ARC_HLTCP_TASK_CONNECT;
					}
					else {
						if(iLocalDataSize) vPutsTrace(0,"+KURCCFG: CMD FAILED");
					}
				}
				else {
					vTaskDelay(1000);
					ARC_HLTCP_EOF_FORCE();
				}
			}
			else {
				ARC_HLTCP_ObjTab[conf_id]->dev_timeout++;
				vTaskDelay(1000);
				ARC_HLTCP_EOF_FORCE();
				if(iLocalDataSize) vPutsTrace(0,"+KTCPCFG: CMD FAILED");
			}
		}
		break;
	case ARC_HLTCP_TASK_CONNECT:
		if(!ARC_HLTCP_ObjTab[conf_id]->pSndBuff) {
			ARC_HLTCP_ObjTab[conf_id]->sending_size = 0;
			break;//Loop wait for send command
		}
		if(ARC_HLTCP_ObjTab[conf_id]->session_id > 0) {
			ARC_HLTCP_TCPCNX(conf_id);
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
				pStr = NULL;
				if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"OK",2);
				vTaskDelay(1);
			}while(!pStr && (timeout <= (ARC_HLTCP_TRY_MAXNB*2)));
			
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			if(pStr){
				// Call Back to subscriber function
				ARC_HLTCP_ObjTab[conf_id]->dev_timeout = 0;
				ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size = 0;
				ARC_HLTCP_ObjTab[conf_id]->eStatus.send_remain = 0;
				ARC_HLTCP_Cbk(conf_id, ARC_HLNET_EV_CONNECTED, NULL);
				return ARC_HLTCP_TASK_GET_STAT;
				//return ARC_HLTCP_TASK_DATA;
			}else {
				ARC_HLTCP_ObjTab[conf_id]->dev_timeout++;
				vTaskDelay(1000);
				ARC_HLTCP_EOF_FORCE();
				if(iLocalDataSize) vPutsTrace(0,"+KTCPCNX: CMD FAILED");
			}
		}
		break;
	case ARC_HLTCP_TASK_GET_STAT:
		vTaskDelay(1000);
		ARC_HLTCP_TCPSTAT(conf_id);
		vTaskDelay(300);//wait for GSM modem switch mode
		timeout = 0;
		do {
			timeout++;
			iLocalDataSize = aptGsmUart_Read(&pLocalData,11,1000);
			pStr = NULL;
			if(iLocalDataSize>=11) pStr = memmem(pLocalData,iLocalDataSize,"+KTCPSTAT: ",11);
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLTCP_TRY_MAXNB));
		
		if(pStr){
			char strid[5] = {""};
			uint32_t i = 0;
			bool isCRdetected = FALSE;
			ARC_HLTCP_ObjTab[conf_id]->dev_timeout = 0;
			
			for(;pStr[i]&&(!isCRdetected);i++) { if(pStr[i]=='\r') isCRdetected = TRUE; }
				
			if(isCRdetected) {
				wm_strGetParameterString(strid, sizeof(strid), (char*)pStr,1);
				ARC_HLTCP_ObjTab[conf_id]->eStatus.esock =  wm_atoi(strid);
				//vPrintfTrace((0,"\r\nARC_HLTCP_ObjTab[%d]->eStatus.esock = %d",conf_id,ARC_HLTCP_ObjTab[conf_id]->eStatus.esock);
				wm_strGetParameterString(strid, sizeof(strid), (char*)pStr,2);
				ARC_HLTCP_ObjTab[conf_id]->eStatus.notify =  wm_atoi(strid);
				//vPrintfTrace((0,"\r\nARC_HLTCP_ObjTab[%d]->eStatus.notify = %d",conf_id,ARC_HLTCP_ObjTab[conf_id]->eStatus.notify);
				wm_strGetParameterString(strid, sizeof(strid), (char*)pStr,3);
				ARC_HLTCP_ObjTab[conf_id]->eStatus.send_remain =  wm_atoi(strid);
				//vPrintfTrace((0,"\r\nARC_HLTCP_ObjTab[%d]->eStatus.send_remain = %d",conf_id,ARC_HLTCP_ObjTab[conf_id]->eStatus.send_remain);
				wm_strGetParameterString(strid, sizeof(strid), (char*)pStr,4);
				ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size =  wm_atoi(strid);
				//vPrintfTrace((0,"\r\nARC_HLTCP_ObjTab[%d]->eStatus.recv_size = %d",conf_id,ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size);
			}
			else {
				ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
				vPutsTrace(0,"+KTCPSTAT: CMD FAILED");
				return ARC_HLTCP_TASK_GET_STAT;
			}
		}else {
			// maybe something error
			ARC_HLTCP_ObjTab[conf_id]->dev_timeout++;
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			if(iLocalDataSize) vPutsTrace(0,"+KTCPSTAT: CMD FAILED");
			return ARC_HLTCP_TASK_GET_STAT;
		}

		if(ARC_HLTCP_ObjTab[conf_id]->eStatus.notify >= 0) { // some error found
			ARC_HLTCP_ObjTab[conf_id]->connected = FALSE;
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			ARC_HLTCP_TCPCLOSE(conf_id);
			iLocalDataSize = aptGsmUart_Read(&pLocalData,2,3000);
			ARC_HLTCP_TCPDEL(conf_id);
			iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
			// Call Back to subscriber function
			ARC_HLTCP_Cbk(conf_id, ARC_HLNET_EV_ERR_BASE + ARC_HLTCP_ObjTab[conf_id]->eStatus.notify,NULL);
			return ARC_HLTCP_TASK_TCPCFG;
		}
		
		if(ARC_HLTCP_ObjTab[conf_id]->connected && !arc_netsys.bPdpActivated)  { // some error found
			if(terminate_timeout++ > 10) {
				terminate_timeout = 0;
				ARC_HLTCP_ObjTab[conf_id]->connected = FALSE;
				//ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
				//ARC_HLTCP_TCPCLOSE(conf_id);
				//iLocalDataSize = aptGsmUart_Read(&pLocalData,2,3000);
				//ARC_HLTCP_TCPDEL(conf_id);
				//iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
				// Call Back to subscriber function
				//ARC_HLTCP_Cbk(conf_id, ARC_HLNET_EV_ERR_BASE ,NULL);
				//return ARC_HLTCP_TASK_TCPCFG;
			}
		}else terminate_timeout = 0;

		switch(ARC_HLTCP_ObjTab[conf_id]->eStatus.esock) {
		case ARC_HLTCP_SOCK_CONNECT:
			getstat_wait2sendCount = 0;
			ARC_HLTCP_ObjTab[conf_id]->connected = TRUE;
			if((ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size)//Some data arrived 
			||(ARC_HLTCP_ObjTab[conf_id]->eStatus.send_remain == 0)){ //Or delivered out of data
				return ARC_HLTCP_TASK_DATA;
			}
		case ARC_HLTCP_SOCK_OPENING:
			return ARC_HLTCP_TASK_GET_STAT;
			break;
		case ARC_HLTCP_SOCK_CLOSED:
			if(ARC_HLTCP_ObjTab[conf_id]->connected==TRUE) {//Socket was Disconnected
				ARC_HLTCP_ObjTab[conf_id]->connected = FALSE;
				if((ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size)//Some data existing
				||(ARC_HLTCP_ObjTab[conf_id]->eStatus.send_remain)) {//Or Send buffer remaining
					ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
					return ARC_HLTCP_TASK_DATA;
				}
				
				vPrintfTrace((0,"+KTCPSTAT: SOCKET %d DISCONNECTED",ARC_HLTCP_ObjTab[conf_id]->session_id));
				
				ARC_HLTCP_TCPCLOSE(conf_id);
				iLocalDataSize = aptGsmUart_Read(&pLocalData,2,2000);
				ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
				ARC_HLTCP_TCPDEL(conf_id);
				iLocalDataSize = aptGsmUart_Read(&pLocalData,2,2000);
				ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
				ARC_HLTCP_EOF_FORCE();
			}
			else {
				if(ARC_HLTCP_ObjTab[conf_id]->pSndBuff) {
					getstat_wait2sendCount++;
					if(getstat_wait2sendCount < ARC_HLTCP_GETSTAT_TIMEOUT ){
						return ARC_HLTCP_TASK_GET_STAT;
					}
				}
			}
			if(ARC_HLTCP_ObjTab[conf_id]->pSndBuff) {
				adl_memRelease(ARC_HLTCP_ObjTab[conf_id]->pSndBuff);
				ARC_HLTCP_ObjTab[conf_id]->pSndBuff = NULL;
				ARC_HLTCP_ObjTab[conf_id]->sending_size = 0;
			}
			if(getstat_wait2sendCount >= ARC_HLTCP_GETSTAT_TIMEOUT) {
				getstat_wait2sendCount = 0;
				ARC_HLTCP_Cbk(conf_id, ARC_HLNET_EV_CONN_ERR,NULL);
				return ARC_HLTCP_TASK_TCPCFG;
			}
			break;
		case ARC_HLTCP_SOCK_CLOSING:
			return ARC_HLTCP_TASK_GET_STAT;
			break;
		default:
			break;
		}
		ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
		return ARC_HLTCP_TASK_TCPCFG;
		break;
	case ARC_HLTCP_TASK_DATA:
		if(ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size) {// Got TCP Data Notify
			static const uint8_t headersize = ARC_HLNET_CONNECT_STR_LEN + ARC_HLNET_EOF_PATTERN_LEN + ARC_HLNET_OK_RESP_STR_LEN;
			/*
			<CR><LF>CONNECT<CR><LF> Data............... --EOF--Pattern-- <CR><LF>OK<CR><LF> 
			*/	
			ARC_HLTCP_ObjTab[conf_id]->reading_size = ((ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size + headersize) > APT_GSM_UART_BUFFER_SIZE)
													? (APT_GSM_UART_BUFFER_SIZE - headersize) : ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size;
			ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size = 0;
			ARC_HLTCP_TCPRCV(conf_id);
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,ARC_HLTCP_ObjTab[conf_id]->reading_size,3000);
				pStr = NULL;
				if(iLocalDataSize>=33) pStr = memmem(pLocalData,iLocalDataSize,"--EOF--Pattern--\r\nOK\r\n",22);
				vTaskDelay(3);
			}while(!pStr && (timeout <= ARC_HLTCP_TRY_MAXNB));
			
			if(pStr) pStr = memmem(pLocalData,iLocalDataSize,"\r\nCONNECT\r\n",11);
			
			lvPutsLabel(30,"HLTCP: ",(char*)pLocalData,iLocalDataSize);
			
			if((pStr)&&(iLocalDataSize>33)) {
				//arc_hltcp_ctx_t *pCx = adl_memGet(sizeof(arc_hltcp_ctx_t)+iLocalDataSize);// Original Data length
				arc_hltcp_ctx_t *pCx = (arc_hltcp_ctx_t *)&ARC_HLTCP_CTX_Buffer[0];
				ARC_HLTCP_ObjTab[conf_id]->dev_timeout = 0;
				if(pCx) {
					pCx->length = ARC_HLTCP_ObjTab[conf_id]->reading_size;
					memcpy(	pCx->data,// Decode TCP Frame
							pStr /*pLocalData*/ + ARC_HLNET_CONNECT_STR_LEN,// Remove "CONNECT "string
							pCx->length);// Active Size
					ARC_HLTCP_Cbk(conf_id, ARC_HLNET_EV_READ, pCx);// Call Back to subscriber function
					//adl_memRelease(pCx);
				}
				else {
					vPrintfTrace((0,"+KTCPRCV: TCP-%d CAN NOT Allocate memory",conf_id));
					vTaskDelay(1000);
					ARC_HLTCP_EOF_FORCE();
				}
			}else {
				ARC_HLTCP_ObjTab[conf_id]->dev_timeout++;
				vTaskDelay(1000);
				ARC_HLTCP_EOF_FORCE();
				vPrintfTrace((0,"+KTCPRCV: TCP-%d CAN NOT CONNECT",conf_id));
			}
		}
		else if(ARC_HLTCP_ObjTab[conf_id]->eStatus.send_remain == 0) {// all TCP data was sent

			uint8_t* pBuff = NULL;
			
			if(!ARC_HLTCP_ObjTab[conf_id]->pSndBuff) {
				ARC_HLTCP_ObjTab[conf_id]->sending_size = 0;
				return ARC_HLTCP_TASK_GET_STAT;
			}
			
			// send function being used here
			if(ARC_HLTCP_ObjTab[conf_id]->sending_size) {// data sent inprogress
				ARC_HLTCP_ObjTab[conf_id]->pSndBuff->idx += ARC_HLTCP_ObjTab[conf_id]->sending_size;// next send index
				ARC_HLTCP_ObjTab[conf_id]->pSndBuff->length -= ARC_HLTCP_ObjTab[conf_id]->sending_size;// data size remain buffer
				if(ARC_HLTCP_ObjTab[conf_id]->pSndBuff->length <= 0) {// all data was send out
					adl_memRelease(ARC_HLTCP_ObjTab[conf_id]->pSndBuff);
					ARC_HLTCP_ObjTab[conf_id]->pSndBuff = NULL;
					ARC_HLTCP_Cbk(conf_id, ARC_HLNET_EV_RELEASE, NULL);// Call Back to subscriber function
					ARC_HLTCP_ObjTab[conf_id]->sending_size = 0;
					return ARC_HLTCP_TASK_GET_STAT;
				}
			}
			
			// Here is The "ARC_HLTCP_ObjTab[conf_id]->pSndBuff->length" >= 0
			// calculate next sending data size
			ARC_HLTCP_ObjTab[conf_id]->sending_size = ((ARC_HLTCP_ObjTab[conf_id]->pSndBuff->length + ARC_HLNET_EOF_PATTERN_LEN)  > APT_GSM_UART_BUFFER_SIZE)
														?(APT_GSM_UART_BUFFER_SIZE - ARC_HLNET_EOF_PATTERN_LEN) : ARC_HLTCP_ObjTab[conf_id]->pSndBuff->length;
														
			
			//pBuff = adl_memGet(ARC_HLTCP_ObjTab[conf_id]->sending_size + ARC_HLNET_EOF_PATTERN_LEN);
			pBuff = (uint8_t*)&ARC_HLTCP_CTX_Buffer[0];
			if(!pBuff){
				//ARC_HLTCP_Cbk(conf_id, ARC_HLNET_EV_RELEASE, NULL);
				return ARC_HLTCP_TASK_GET_STAT;
			}
				
			// request send process
			ARC_HLTCP_TCPSND(conf_id);
				
			//<CR><LF>CONNECT<CR><LF>
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,11,1000);
				pStr = NULL;
				if(iLocalDataSize>=11) pStr = memmem(pLocalData,iLocalDataSize,"\r\nCONNECT\r\n",11);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLTCP_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			aptGsmUart_Flush();
			
			if(pStr) { // Found connect response from module
				uint8_t enter_buff[2] = {'\r'};
				ARC_HLTCP_ObjTab[conf_id]->dev_timeout = 0;
				memcpy(	pBuff,
						ARC_HLTCP_ObjTab[conf_id]->pSndBuff->data + ARC_HLTCP_ObjTab[conf_id]->pSndBuff->idx,
						ARC_HLTCP_ObjTab[conf_id]->sending_size);
				memcpy(	pBuff + ARC_HLTCP_ObjTab[conf_id]->sending_size,
						ARC_HLNET_EOF_PATTERN ,
						ARC_HLNET_EOF_PATTERN_LEN);
				//ARC_HLNET_STRN_DEBUG(pBuff,ARC_HLTCP_ObjTab[conf_id]->sending_size + ARC_HLNET_EOF_PATTERN_LEN);
				vPrintfTrace((0,"+KTCSND: TCP-%d SUBMIT %d Byte",conf_id,ARC_HLTCP_ObjTab[conf_id]->sending_size));
				
				aptGsmUart_Write( pBuff, ARC_HLTCP_ObjTab[conf_id]->sending_size);
				vTaskDelay(1000);
				aptGsmUart_Write( pBuff + ARC_HLTCP_ObjTab[conf_id]->sending_size, ARC_HLNET_EOF_PATTERN_LEN);
				vTaskDelay(1000);
				aptGsmUart_Write( enter_buff, 1);
				vTaskDelay(2000);//3000
				timeout = 0;
				do {
					iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
					pStr = NULL;
					if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"OK",2);
					ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
					if(timeout) vPrintfTrace((0,"+KTCSND: TCP-%d WAIT RESPONSE ...",conf_id));
					timeout++;
					vTaskDelay(1);
				}while(!pStr && (timeout <= ARC_HLTCP_TRY_MAXNB));
				if(pStr) {
						// completed
						ARC_HLTCP_ObjTab[conf_id]->dev_timeout = 0;
				}
				else {
					ARC_HLTCP_ObjTab[conf_id]->dev_timeout++;
					vPrintfTrace((0,"+KTCSND: TCP-%d WAIT TIMEOUT",conf_id));
					vTaskDelay(1000);
					ARC_HLTCP_EOF_FORCE();
				}
			}
			else {// module no response
				ARC_HLTCP_ObjTab[conf_id]->dev_timeout++;
				vPrintfTrace((0,"+KTCSND: TCP-%d CAN NOT CONNECT",conf_id));
				vTaskDelay(1000);
				ARC_HLTCP_EOF_FORCE();
			}
			
			//if(pBuff)adl_memRelease(pBuff);
		}
		return ARC_HLTCP_TASK_GET_STAT;
		break;
	default:
		break;
	}
	return CurrState;
}
/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Send TCP Data        */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hltcp_Send(uint8_t conf_id, uint16_t len, uint8_t * data){
	if(conf_id >= ARC_HLTCP_MAX_SESSION) return ERROR;// More over ID
	if(len==0)return ERROR;// length over
	if(!ARC_HLTCP_Cbk) return ERROR;// No subscribed
	if(!ARC_HLTCP_ObjTab[conf_id])return ERROR; // Misconfiguration
	if(ARC_HLTCP_ObjTab[conf_id]->pSndBuff)return ERROR;// Running already
	

	ARC_HLTCP_ObjTab[conf_id]->pSndBuff = adl_memGet(sizeof(arc_hltcp_SndBuff_t) + 512);
	if(ARC_HLTCP_ObjTab[conf_id]->pSndBuff) {
		len = (len > 512) ? 512 : len;
		memset(ARC_HLTCP_ObjTab[conf_id]->pSndBuff->data,0,512);
		memcpy(ARC_HLTCP_ObjTab[conf_id]->pSndBuff->data,data,len);
		ARC_HLTCP_ObjTab[conf_id]->pSndBuff->length = len;
		ARC_HLTCP_ObjTab[conf_id]->pSndBuff->idx = 0;
	}
	else {
		//ARC_HLTCP_Cbk(conf_id, ARC_HLNET_EV_RELEASE, NULL);
	}

	return OK;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Close TCP               */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hltcp_Stop(uint8_t conf_id){
	if(conf_id >= ARC_HLTCP_MAX_SESSION) return ERROR;
	if(!ARC_HLTCP_Cbk) return ERROR;
	if(!ARC_HLTCP_ObjTab[conf_id])return ERROR;
	ARC_HLTCP_ObjTab[conf_id]->enableTask = FALSE;
	return OK;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Task                                       */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLTCP_RunTask(uint8_t pTaskId) {
	if(ARC_HLTCP_ObjTab[pTaskId]->enableTask == FALSE) return FALSE;
	
	ARC_HLTCP_ObjTab[pTaskId]->eTaskSeq = ARC_HLTCP_CoRoutine(pTaskId, ARC_HLTCP_ObjTab[pTaskId]->eTaskSeq);
	ARC_HLTCP_ObjTab[pTaskId]->alive_time++;
	
	if(ARC_HLTCP_ObjTab[pTaskId]->dev_timeout >= 20) {
		ARC_HLTCP_Cbk(pTaskId, ARC_HLNET_EV_TIMEOUT, NULL);
		ARC_HLTCP_ObjTab[pTaskId]->dev_timeout = 0;
		ARC_HLTCP_ObjTab[pTaskId]->alive_time = 0;
	}else if(ARC_HLTCP_ObjTab[pTaskId]->alive_time >= 20) {
		ARC_HLTCP_Cbk(pTaskId, ARC_HLNET_EV_ALIVE, NULL);
		ARC_HLTCP_ObjTab[pTaskId]->dev_timeout = 0;
		ARC_HLTCP_ObjTab[pTaskId]->alive_time = 0;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Close TCP               */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLTCP_DelTask(uint8_t conf_id){
	if(conf_id >= ARC_HLTCP_MAX_SESSION) return ERROR;
	if(!ARC_HLTCP_Cbk) return ERROR;
	if(!ARC_HLTCP_ObjTab[conf_id])return ERROR;
	ARC_HLTCP_EOF_FORCE();
	vTaskDelay(1000);
	ARC_HLTCP_TCPCLOSE(conf_id);
	if(ARC_HLTCP_ObjTab[conf_id]->pSndBuff) adl_memRelease(ARC_HLTCP_ObjTab[conf_id]->pSndBuff);
	adl_memRelease(ARC_HLTCP_ObjTab[conf_id]->pTaskName);
	adl_memRelease(ARC_HLTCP_ObjTab[conf_id]);
	return TRUE;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Start Task              */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hltcp_Start(uint8_t conf_id, uint32_t* wd, xTaskHandle *x) {
	arc_hlctrl_para_t * pvTask = NULL;
	uint8_t * pTaskName = NULL;
	if(conf_id >= ARC_HLTCP_MAX_SESSION) return ERROR; // More over ID
	if(!ARC_HLTCP_Cbk) return ERROR; // No subscribed
	if(!ARC_HLTCP_ObjTab[conf_id])return ERROR; // Misconfiguration for TCP
	if(!arc_hlcnx_GetConfig(ARC_HLTCP_ObjTab[conf_id]->cfg.cnxid))return ERROR;// Misconfiguration for CNX
	if(ARC_HLTCP_ObjTab[conf_id]->eTaskSeq != ARC_HLTCP_TASK_IDLE){
		ARC_HLTCP_EOF_FORCE();
		vTaskDelay(1000);
		ARC_HLTCP_EOF_FORCE();
		vTaskDelay(1000);
		ARC_HLTCP_TCPCLOSE(conf_id);
		ARC_HLTCP_ObjTab[conf_id]->eTaskSeq = ARC_HLTCP_TASK_TCPCFG;
		return ERROR;// Running already
	}
	
	ARC_HLTCP_ObjTab[conf_id]->eTaskSeq = ARC_HLTCP_TASK_TCPCFG;

	pTaskName = adl_memGet(strlen((char*)ARC_HLTCP_TASK_NAME)+3);
	if(!pTaskName) return ERROR;
	sprintf((char*)pTaskName,"%s_%d",ARC_HLTCP_TASK_NAME,conf_id);
	ARC_HLTCP_ObjTab[conf_id]->pTaskName = pTaskName;
	
	pvTask = adl_memGet(sizeof(arc_hlctrl_para_t));
	if(!pvTask) return ERROR;
	pvTask->id = conf_id;
	pvTask->func = &ARC_HLTCP_RunTask;
	pvTask->delc = &ARC_HLTCP_DelTask;
	pvTask->wd = wd;
	
	xTaskCreate (	arc_hlCtrl_AccessTask,				/* The task that implements the command console. */
					(const int8_t *const)pTaskName,		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					ARC_HLTCP_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					pvTask,								/* The parameter is used to pass the already configured UART port into the task. */
					ARC_HLTCP_TASK_PRIORITY,			/* The priority allocated to the task. */
					x//ARC_HLTCP_ObjTab[conf_id]->hTask	/* Used to store the handle to the created task */
				);
	return OK;
}

int8_t arc_hldms_Start(uint8_t conf_id) {
	arc_hlctrl_para_t * pvTask = NULL;
	uint8_t * pTaskName = NULL;
	if(conf_id >= ARC_HLTCP_MAX_SESSION) return ERROR; // More over ID
	if(!ARC_HLTCP_Cbk) return ERROR; // No subscribed
	if(!ARC_HLTCP_ObjTab[conf_id])return ERROR; // Misconfiguration for TCP
	if(!arc_hlcnx_GetConfig(ARC_HLTCP_ObjTab[conf_id]->cfg.cnxid))return ERROR;// Misconfiguration for CNX
	if(ARC_HLTCP_ObjTab[conf_id]->eTaskSeq != ARC_HLTCP_TASK_IDLE){
		ARC_HLTCP_EOF_FORCE();
		vTaskDelay(1000);
		ARC_HLTCP_EOF_FORCE();
		vTaskDelay(1000);
		ARC_HLTCP_TCPCLOSE(conf_id);
		ARC_HLTCP_ObjTab[conf_id]->eTaskSeq = ARC_HLTCP_TASK_TCPCFG;
		return ERROR;// Running already
	}
	
	ARC_HLTCP_ObjTab[conf_id]->eTaskSeq = ARC_HLTCP_TASK_TCPCFG;

	pTaskName = adl_memGet(strlen((char*)ARC_HLTCP_TASK_NAME)+3);
	if(!pTaskName) return ERROR;
	sprintf((char*)pTaskName,"%s_%d",ARC_HLTCP_TASK_NAME,conf_id);
	ARC_HLTCP_ObjTab[conf_id]->pTaskName = pTaskName;
	
	pvTask = adl_memGet(sizeof(arc_hlctrl_para_t));
	if(!pvTask) return ERROR;
	pvTask->id = conf_id;
	pvTask->func = &ARC_HLTCP_RunTask;
	pvTask->delc = &ARC_HLTCP_DelTask;
	pvTask->wd = NULL;
	
	xTaskCreate (	arc_hlCtrl_AccessTask,				/* The task that implements the command console. */
					(const int8_t *const)pTaskName,		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					ARC_HLTCP_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					pvTask,								/* The parameter is used to pass the already configured UART port into the task. */
					ARC_HLTCP_TASK_PRIORITY,			/* The priority allocated to the task. */
					ARC_HLTCP_ObjTab[conf_id]->hTask	/* Used to store the handle to the created task */
				);
	return OK;
}


int8_t arc_hltcp_ReStart(uint8_t conf_id) {
	//arc_hlctrl_para_t * pvTask = NULL;
	//uint8_t * pTaskName = NULL;
	if(conf_id >= ARC_HLTCP_MAX_SESSION) return ERROR; // More over ID
	if(!ARC_HLTCP_Cbk) return ERROR; // No subscribed
	if(!ARC_HLTCP_ObjTab[conf_id])return ERROR; // Misconfiguration for TCP
	if(!arc_hlcnx_GetConfig(ARC_HLTCP_ObjTab[conf_id]->cfg.cnxid))return ERROR;// Misconfiguration for CNX
	if(ARC_HLTCP_ObjTab[conf_id]->eTaskSeq != ARC_HLTCP_TASK_IDLE){
		ARC_HLTCP_EOF_FORCE();
		vTaskDelay(1000);
		ARC_HLTCP_EOF_FORCE();
		vTaskDelay(1000);
		ARC_HLTCP_TCPCLOSE(conf_id);
		ARC_HLTCP_ObjTab[conf_id]->eTaskSeq = ARC_HLTCP_TASK_TCPCFG;
		return OK;// Running already
	}
	return ERROR;
}
/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Set config              */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hltcp_SetConfig(uint8_t conf_id, int opt, ...) {
	va_list ap;
	if(conf_id >= ARC_HLTCP_MAX_SESSION) return ERROR;
	va_start(ap, opt);
	if(!ARC_HLTCP_ObjTab[conf_id]) {
		ARC_HLTCP_ObjTab[conf_id] = adl_memGet(sizeof(arc_hltcpobject_t));
		if(!ARC_HLTCP_ObjTab[conf_id])return ERROR;
		ARC_HLTCP_ObjTab[conf_id]->cfg.cnxid = 0;
		ARC_HLTCP_ObjTab[conf_id]->cfg.tcpcfg_mode = ARC_HLTCP_MODE_SERVER;
		strcpy(ARC_HLTCP_ObjTab[conf_id]->cfg.remote_addr,"");
		ARC_HLTCP_ObjTab[conf_id]->cfg.tcp_port = 80;
		ARC_HLTCP_ObjTab[conf_id]->cfg.source_port = 50;
		ARC_HLTCP_ObjTab[conf_id]->cfg.data_mode = FALSE;
		ARC_HLTCP_ObjTab[conf_id]->cfg.urc_endtcp = FALSE;
		
		ARC_HLTCP_ObjTab[conf_id]->enableTask = TRUE;
		ARC_HLTCP_ObjTab[conf_id]->hTask = NULL;
		ARC_HLTCP_ObjTab[conf_id]->pTaskName = NULL;
		ARC_HLTCP_ObjTab[conf_id]->session_id = 0;
		ARC_HLTCP_ObjTab[conf_id]->connected = FALSE;
		ARC_HLTCP_ObjTab[conf_id]->dev_timeout = 0;
		ARC_HLTCP_ObjTab[conf_id]->sending_size = 0;
		ARC_HLTCP_ObjTab[conf_id]->reading_size = 0;
		ARC_HLTCP_ObjTab[conf_id]->eTaskSeq = ARC_HLTCP_TASK_IDLE;
		ARC_HLTCP_ObjTab[conf_id]->eStatus.esock = ARC_HLTCP_SOCK_NOTDEF;
		ARC_HLTCP_ObjTab[conf_id]->eStatus.recv_size = 0;
		ARC_HLTCP_ObjTab[conf_id]->eStatus.send_remain = 0;
		ARC_HLTCP_ObjTab[conf_id]->eStatus.notify = -1;
		ARC_HLTCP_ObjTab[conf_id]->pSndBuff = NULL;
	}
	
	while(opt) {
		ARC_HLTCP_OPTION_SET(&ap,opt,&ARC_HLTCP_ObjTab[conf_id]->cfg);//set option value
		opt = va_arg(ap,int);//Get next option
	};
	
	va_end(ap);
	return OK;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Subscribe              */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hltcp_Subscribe(arc_hltcp_fCbk cbk) {
	uint8_t sid = 0;
	if(!cbk) return ERROR;
	if(ARC_HLTCP_Cbk) {
		ARC_HLTCP_Cbk = cbk;
		return OK;
	}
	
	ARC_HLTCP_Cbk = cbk;
	
	for(;sid<ARC_HLTCP_MAX_SESSION;sid++) {
		ARC_HLTCP_ObjTab[sid] = NULL;
	}
	return OK;
}