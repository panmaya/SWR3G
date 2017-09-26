/*
 * arc_hlxpdp.c
 *
 * Created: 13/11/2557 11:26:47
 *  Author: pan
 */ 
#include "var/arc_hlnet.h"
#include "ctrl/arc_hlaccess.h"
#include "apt/apt_gsm_uart.h"
#include "arc_hlcnx.h"
#include "arc_hlgsm.h"
#include <stdarg.h>

static arc_hlcnxcfg_t *ARC_HLCNX_CfgProfile[ARC_HLCNX_MAX_SESSION] = {NULL, NULL, NULL};
	
#define  ARC_HLCNX_RSP_DELAY	300
#define  ARC_HLCNX_TRY_MAXNB	5

typedef enum ARC_HLCNX_TASK_SEQ_E {
	ARC_HLCNX_TASK_INIT,
	ARC_HLCNX_TASK_CPIN,
	ARC_HLCNX_TASK_CONF,
	ARC_HLCNX_TASK_CREG,
	ARC_HLCNX_TASK_CGREG,
}arc_hlcnx_taskseq_e;
					

#define ARC_HLCNX_SIMRDY()		do{\
									char cmd[12] = {"AT+CPIN?\r"};\
									/*(ARC_HLNET_STR_DEBUG(cmd);*/\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
									vTaskDelay(ARC_HLCNX_RSP_DELAY);\
								}while(0)

#define ARC_HLCNX_GSMRDY()		do{\
									char cmd[12] = {"AT+CREG?\r"};\
									/*ARC_HLNET_STR_DEBUG(cmd);*/\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
									vTaskDelay(ARC_HLCNX_RSP_DELAY);\
								}while(0)
								
#define ARC_HLCNX_GPRSRDY()		do{\
									char cmd[12] = {"AT+CGREG?\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
									vTaskDelay(ARC_HLCNX_RSP_DELAY);\
								}while(0)
								
#define ARC_HLCNX_IPADDR()		do{\
									char cmd[12] = {"AT+CGPADDR\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
									vTaskDelay(ARC_HLCNX_RSP_DELAY);\
								}while(0)
								
#define ARC_HLCNX_IPSTAT()		do{\
									char cmd[12] = {"AT#IPSTAT\r"};\
									/*ARC_HLNET_STR_DEBUG(cmd);*/\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
									vTaskDelay(ARC_HLCNX_RSP_DELAY);\
								}while(0)
								
#define ARC_HLCNX_INIT()		do{\
									char cmd[64] = {"AT+KPATTERN=\""};\
									strcpy(cmd+13,ARC_HLNET_EOF_PATTERN);\
									strcat(cmd+13,"\"\r");\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
									vTaskDelay(100);\
								}while(0)

//AT+KCNXCFG=<cnx cnf>,“GPRS”,<apn>[,[<login>][,[<password>][,[<ip>][,[<dns1>][,<dns2>],,[<cnx_sustained>]]]]]
#define ARC_HLCNX_CNXCFG(_x)	do{\
									char cmd[160] = {"AT+KCNXCFG="};\
									sprintf(cmd+11,"%d,\"GPRS\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\r",_x,\
																				ARC_HLCNX_CfgProfile[_x]->apn,\
																				ARC_HLCNX_CfgProfile[_x]->login,\
																				ARC_HLCNX_CfgProfile[_x]->password,\
																				ARC_HLCNX_CfgProfile[_x]->ip,\
																				ARC_HLCNX_CfgProfile[_x]->dns1,\
																				ARC_HLCNX_CfgProfile[_x]->dns2);\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
									vTaskDelay(ARC_HLCNX_RSP_DELAY);\
								}while(0)
								
							
/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Config option routine                */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static void ARC_HLNET_OPTION_SET(va_list *ap, int option, arc_hlcnxcfg_t *cnxcfg){
	switch(option) {
		case ARC_HLNET_OPT_APN:
			{
				char * pstr = NULL;
				pstr = va_arg(*ap,char*);
				strcpy(cnxcfg->apn, pstr);
			}
			break;
		case ARC_HLNET_OPT_USER:
			{
				char * pstr = NULL;
				pstr = va_arg(*ap,char*);
				strcpy(cnxcfg->login, pstr);
			}
			break;
		case ARC_HLNET_OPT_PASS:
			{
				char * pstr = NULL;
				pstr = va_arg(*ap,char*);
				strcpy(cnxcfg->password, pstr);
			}
			break;
		case ARC_HLNET_OPT_IP:
			{
				char * pstr = NULL;
				pstr = va_arg(*ap,char*);
				strcpy(cnxcfg->ip, pstr);
			}
			break;
		case ARC_HLNET_OPT_DNS1:
			{
				char * pstr = NULL;
				pstr = va_arg(*ap,char*);
				strcpy(cnxcfg->dns1, pstr);
			}
			break;
		case ARC_HLNET_OPT_DNS2:
			{
				char * pstr = NULL;
				pstr = va_arg(*ap,char*);
				strcpy(cnxcfg->dns2, pstr);
			}
			break;
		case ARC_HLNET_OPT_SUSTAINED:
			cnxcfg->is_sustained =  va_arg(*ap,int);
			break;
		case ARC_HLNET_OPT_END:
		default:
			/* nothing to do*/
			break;
	}
}

arc_hlcnxcfg_t * arc_hlcnx_GetConfig(uint8_t pid) {
	return ARC_HLCNX_CfgProfile[pid];
}


/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Set config profile                      */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hlcnx_SetConfig(uint8_t pid, int opt, ...) {
	va_list ap;
	if(pid >= ARC_HLCNX_MAX_SESSION) return ERROR;
	va_start(ap, opt);
	if(!ARC_HLCNX_CfgProfile[pid]) {
		ARC_HLCNX_CfgProfile[pid] = adl_memGet(sizeof(arc_hlcnxcfg_t));
		if(!ARC_HLCNX_CfgProfile[pid])return ERROR;
		strcpy(ARC_HLCNX_CfgProfile[pid]->apn,"");
		strcpy(ARC_HLCNX_CfgProfile[pid]->login,"");
		strcpy(ARC_HLCNX_CfgProfile[pid]->password,"");
		strcpy(ARC_HLCNX_CfgProfile[pid]->ip,"0.0.0.0");
		strcpy(ARC_HLCNX_CfgProfile[pid]->dns1,"0.0.0.0");
		strcpy(ARC_HLCNX_CfgProfile[pid]->dns2,"0.0.0.0");
		ARC_HLCNX_CfgProfile[pid]->is_sustained = FALSE;
		ARC_HLCNX_CfgProfile[pid]->is_activated = FALSE;
	}/* nothing to do*/
		
	while(opt) {
		ARC_HLNET_OPTION_SET(&ap,opt,ARC_HLCNX_CfgProfile[pid]);//set option value
		opt = va_arg(ap,int);//Get next option
	}
		
	va_end(ap);
	
	return OK;
}

static bool ARC_HLCNX_CheckIPStat(void) {
	char * pStr = NULL;
	uint8_t * Data = NULL;
	uint16_t DataSize = 0;
	uint8_t timeout = 0;
	
	ARC_HLCNX_IPSTAT();
	timeout = 0;
	do {
		timeout++;
		DataSize = aptGsmUart_Read(&Data,9,3000);
		pStr = NULL;
		if(DataSize>=9) pStr = memmem(Data,DataSize,"#IPSTAT: ",9);
		vTaskDelay(1);
	}while(!pStr && (timeout <= ARC_HLCNX_TRY_MAXNB));
	
	if(pStr) {
		uint32_t i = 0;
		char local_sBuff[3] = {""};
		bool isCRdetected = FALSE;
		static bool ipstat = FALSE;
			
		arc_netdev_noresponse = 0;
			
		for(;pStr[i]&&(!isCRdetected);i++) { if(pStr[i]=='\r') isCRdetected = TRUE; }

		if(isCRdetected) {
			ipstat = atoi(wm_strGetParameterString(local_sBuff,sizeof(local_sBuff),pStr,1));
			//if(!ipstat)vPutsTrace(0,"NO IP");
		}
			
		if(ipstat!=arc_netsys.bPdpActivated) {
			
			ARC_HLNET_STRN_DEBUG(Data,DataSize);
			
			arc_netsys.bPdpActivated = ipstat;
			
			if(ipstat==FALSE) {
				strcpy(arc_netip,"0.0.0.0");
				vPutsTrace(0,"IP DISCONNECT");
			}
			else {
				ARC_HLCNX_IPADDR();
				timeout = 0;
				do {
					timeout++;
					DataSize = aptGsmUart_Read(&Data,10,3000);
					pStr = NULL;
					if(DataSize>=10) pStr = memmem(Data,DataSize,"+CGPADDR: ",10);
					vTaskDelay(1);
				}while(!pStr && (timeout <= ARC_HLCNX_TRY_MAXNB));
					
				ARC_HLNET_STRN_DEBUG(Data,DataSize);
					
				if(pStr) {
					uint32_t n = 0;
					bool isCRFound = FALSE;
						
					arc_netdev_noresponse = 0;
						
					for(;pStr[n]&&(!isCRFound);n++) { if(pStr[n]=='\r') isCRFound = TRUE; }

					if(isCRFound) wm_strGetParameterString(arc_netip,sizeof(arc_netip),pStr,2);
					
					vPrintfTrace((0,"IP CONNECT: %s",arc_netip));
					
					return TRUE;
						
				}else { if(DataSize) vPutsTrace(0,"+CGPADDR: CMD FAILED");}
			}
		}
	}else { if(DataSize) vPutsTrace(0,"#IPSTAT: CMD FAILED");}
		
	return FALSE;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:        PPP Healthy                             */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLCNX_RunTask(uint8_t pTaskId) {
	char * pStr = NULL;
	uint8_t * Data = NULL;
	uint16_t DataSize = 0;
	static bool wasInit = false;
	static arc_hlcnx_taskseq_e seq = ARC_HLCNX_TASK_INIT;
	static uint16_t ipstack_checking_period = 0;
	uint8_t timeout = 0;
	
	if(!ioport_get_pin_level(PIN_SIMPRESS))return TRUE;
	if(!arc_netsys.bDeiveInitOK) return TRUE;
	
	ipstack_checking_period++;
	if(ipstack_checking_period > 10 ) {
		ipstack_checking_period = 0;
		if(ARC_HLCNX_CheckIPStat()) {//New IP
			//Update CGREG
			seq = ARC_HLCNX_TASK_CGREG;
		}
		return TRUE;
	}
	

	switch(seq) {
	case ARC_HLCNX_TASK_INIT:
	case ARC_HLCNX_TASK_CPIN:
		//if(!ioport_get_pin_level(PIN_SIMPRESS))break;
		//if(!arc_netsys.bDeiveInitOK) break;
		ARC_HLCNX_SIMRDY();
		timeout = 0;
		do {
			timeout++;
			DataSize = aptGsmUart_Read(&Data,12,1000);
			pStr = NULL;
			if(DataSize>=12) pStr = memmem(Data,DataSize,"+CPIN: READY",12);
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLCNX_TRY_MAXNB));
		
		ARC_HLNET_STRN_DEBUG(Data,DataSize);
		
		if(pStr) {
			arc_netsys.bSimReady = TRUE;
			arc_netdev_noresponse = 0;
			
			if(wasInit == false) {
				ARC_HLCNX_INIT();
				timeout = 0;
				do {
					timeout++;
					DataSize = aptGsmUart_Read(&Data,2,1000);
					pStr = NULL;
					if(DataSize>=2) pStr = memmem(Data,DataSize,"OK",2);
					vTaskDelay(1);
				}while(!pStr && (timeout <= ARC_HLCNX_TRY_MAXNB));
				
				ARC_HLNET_STRN_DEBUG(Data,DataSize);
				
				if(pStr) {
					wasInit = true;
					seq = ARC_HLCNX_TASK_CONF;
				}else if(DataSize) vPutsTrace(0,"+KPATTERN: CMD FAILED");
			}
		}
		else {
			arc_netsys.bSimReady = FALSE;
			if(DataSize) vPutsTrace(0,"+CPIN: CMD FAILED");
			pStr = NULL;
			if(DataSize>=5) pStr = memmem(Data,DataSize,"ERROR",5);
			if(!pStr) arc_netdev_noresponse = 1;
		}
		break;
	case ARC_HLCNX_TASK_CONF:
		{	uint8_t pid = 0;
			for(;pid<ARC_HLCNX_MAX_SESSION;pid++) {
				if(ARC_HLCNX_CfgProfile[pid] &&(!ARC_HLCNX_CfgProfile[pid]->is_activated)) {
					ARC_HLCNX_CNXCFG(pid);
					timeout = 0;
					do {
						timeout++;
						DataSize = aptGsmUart_Read(&Data,2,2000);
						pStr = NULL;
						if(DataSize>=2) pStr = memmem(Data,DataSize,"OK",2);
						vTaskDelay(1);
					}while(!pStr && (timeout <= ARC_HLCNX_TRY_MAXNB));
					
					ARC_HLNET_STRN_DEBUG(Data,DataSize);
					
					if(pStr) {
						ARC_HLCNX_CfgProfile[pid]->is_activated = true;
					}
					else {
						if(DataSize) vPutsTrace(0,"+KCNXCFG: CMD FAILED");
						ARC_HLCNX_CfgProfile[pid]->is_activated = false;
					}
				}
			}
			
			if(arc_netsys.bPdpActivated) {
				arc_netdev_noresponse = 0;
				arc_netsys.bSimReady = TRUE;
				arc_netsys.bGsmRegistered = TRUE;
				arc_netsys.bGprsAttached = TRUE;
				break;
			}
			else seq = ARC_HLCNX_TASK_CREG;
			//seq = ARC_HLCNX_TASK_INIT;
		}
		break;
	case ARC_HLCNX_TASK_CREG:
		{	static uint16_t cnx_idleDelay = 0;
			if(arc_netsys.bGsmRegistered==TRUE) {
				cnx_idleDelay++;
				if(cnx_idleDelay < 30000) {
					seq = ARC_HLCNX_TASK_CONF;
					break;
				}
				cnx_idleDelay = 0;
			}else cnx_idleDelay = 0;
			
			ARC_HLCNX_GSMRDY();
			timeout = 0;
			do {
				timeout++;
				DataSize = aptGsmUart_Read(&Data,7,3000);
				pStr = NULL;
				if(DataSize>=7) pStr = memmem(Data,DataSize,"+CREG: ",7);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLCNX_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(Data,DataSize);
			
			if(pStr) {
				char local_sBuff[3] = {""};
				uint32_t i = 0;
				bool isCRdetected = FALSE;
				uint8_t ireg = 0;
				
				arc_netdev_noresponse = 0;
				
				for(;pStr[i]&&(!isCRdetected);i++) { if(pStr[i]=='\r') isCRdetected = TRUE; }
				
				if(isCRdetected) {
					wm_strGetParameterString(local_sBuff,sizeof(local_sBuff),pStr,2);
					ireg = wm_atoi(local_sBuff);
					if((ireg==1) || (ireg == 5)) {
						arc_netsys.bGsmRegistered = TRUE;
						if(ireg == 5)arc_netsys.bNetworkRoaming = TRUE;
						vPrintfTrace((0,"+CREG: %s",ireg==1?"HOME":"ROAMING"));
						seq = ARC_HLCNX_TASK_CGREG;
						break;
					}
					else {
						arc_netsys.bGsmRegistered = FALSE;
						arc_netsys.bNetworkRoaming = FALSE;
					}
				}else break;
					
			}else {
				if(DataSize) vPutsTrace(0,"+CREG: CMD FAILED");
				seq = ARC_HLCNX_TASK_CGREG;
				pStr = NULL;
				if(DataSize>=5) pStr = memmem(Data,DataSize,"ERROR",5);
				if(!pStr)arc_netdev_noresponse = 1;
				break;
			}
			
			seq = ARC_HLCNX_TASK_CONF;
		}
		break;
	case ARC_HLCNX_TASK_CGREG:
		{
			static uint16_t cnx_idleDelay = 0;
			
			if(arc_netsys.bGprsAttached==TRUE) {
				cnx_idleDelay++;
				if(cnx_idleDelay < 30000) {
					seq = ARC_HLCNX_TASK_CREG;
					break;
				}
				cnx_idleDelay = 0;
			}else cnx_idleDelay = 0;
			
			ARC_HLCNX_GPRSRDY();
		
			timeout = 0;
			do {
				timeout++;
				DataSize = aptGsmUart_Read(&Data,8,3000);
				pStr = NULL;
				if(DataSize>=8) pStr = memmem(Data,DataSize,"+CGREG: ",8);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLCNX_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(Data,DataSize);

			if(pStr) {
				char local_sBuff[3] = {""};
				uint32_t i = 0;
				bool isCRdetected = FALSE;
				uint8_t ireg = 0;
			
				arc_netdev_noresponse = 0;
				
				for(;pStr[i]&&(!isCRdetected);i++) { if(pStr[i]=='\r') isCRdetected = TRUE; }
				
				if(isCRdetected) {
					wm_strGetParameterString(local_sBuff,sizeof(local_sBuff),pStr,2);
					ireg = wm_atoi(local_sBuff);
					if((ireg==1) || (ireg == 5)) {
						arc_netsys.bGprsAttached = TRUE;
						if(ireg == 5)arc_netsys.bNetworkRoaming = TRUE;
						vPrintfTrace((0,"+CGREG: %s",ireg==1?"HOME":"ROAMING"));
					}
					else {
						arc_netsys.bGprsAttached = FALSE;
						arc_netsys.bNetworkRoaming = FALSE;
					}
					wm_strGetParameterString(local_sBuff,sizeof(local_sBuff),pStr,5);
					ireg = wm_atoi(local_sBuff);
					if(ireg==2) arc_netsys.b3GRegistered = TRUE;
					else arc_netsys.b3GRegistered = FALSE;
				}else break;
			}else {
				if(DataSize) vPutsTrace(0,"+CGREG: CMD FAILED");
				pStr = NULL;
				if(DataSize>=5) pStr = memmem(Data,DataSize,"ERROR",5);
				if(!pStr)arc_netdev_noresponse = 1;
			}
			seq = ARC_HLCNX_TASK_CONF;
		}
		break;
	default:
		break;
	}
	
	return TRUE;
}

static bool ARC_HLCNX_DelTask(uint8_t conf_id) {
	return TRUE;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Initial Task                               */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
void arc_hlcnx_Init(uint32_t * wd, xTaskHandle *x) {

	static arc_hlctrl_para_t pvTask = { 
		.id = 0,
		.func = &ARC_HLCNX_RunTask,
		.delc = &ARC_HLCNX_DelTask
	};
		
	pvTask.wd = wd;

	/* Create task. */
	xTaskCreate (	arc_hlCtrl_AccessTask,				/* The task that implements the command console. */
					ARC_HLCNX_TASK_NAME,				/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					ARC_HLCNX_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					&pvTask,							/* The parameter is used to pass the already configured UART port into the task. */
					ARC_HLCNX_TASK_PRIORITY,			/* The priority allocated to the task. */
					x								/* Used to store the handle to the created task - in this case the handle is not required. */
				);
}