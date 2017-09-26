/*
 * arc_hlftp.c
 *
 * Created: 10/6/2558 17:34:21
 *  Author: pan
 */ 
#include "var/arc_hlnet.h"
#include "ctrl/arc_hlaccess.h"
#include "apt/apt_gsm_uart.h"
#include "arc_hlftp.h"
#include "arc_hlcnx.h"
#include "arc_hltcp.h"
#include "../_lib_xmodem/xmodem.h"
#include "../_lib_bootld/boot_trigger.h"

#include <stdarg.h>

#define ARC_HLFTP_DWL_MINUTE(_t)	(_t*60)
#define ARC_HLFTP_DWL_TIMEOUT		ARC_HLFTP_DWL_MINUTE(8)

#define ARC_HLFTP_SINGLE_TASK		1

#if ARC_HLFTP_SINGLE_TASK
static bool ARC_HLFTP_Task_Runing = FALSE;
#endif

typedef enum ARC_HLFTP_TASK_SEQ_E {
	ARC_HLFTP_TASK_IDLE,
	ARC_HLFTP_TASK_DWL_CFG,
	ARC_HLFTP_TASK_DWL_WAIT,
	ARC_HLFTP_TASK_XMD_CFG,
	ARC_HLFTP_TASK_XMD_WAIT,
	ARC_HLFTP_TASK_FW_INTALL,
}arc_hlftp_taskseq_e;

typedef struct ARC_HLFTPCFG_T {
	uint16_t  cnxid;
	arc_hlftp_mode_e tcpcfg_mode;
	char remote_addr[64];
	char user[32];
	char pass[32];
	uint16_t   tcp_port;
	uint16_t   source_port;
	uint8_t	file_type;
	bool	isPassive;
}arc_hlftpcfg_t;

typedef struct ARC_HLFTP_OBJ_T {
	bool enableTask;
	xTaskHandle hTask;
	uint8_t *pTaskName;
	uint8_t *pFileName;
	uint8_t *pFilePath;
	uint8_t	session_id;
	uint16_t dev_timeout;
	uint16_t alive_time;
	bool connected;
	arc_hlftpcfg_t cfg;
	arc_hlftp_taskseq_e eTaskSeq;
}arc_hlftpobject_t;

static arc_hlftp_fCbk ARC_HLFTP_Cbk = NULL;
static arc_hlftpobject_t *ARC_HLFTP_ObjTab[ARC_HLFTP_MAX_SESSION]= {NULL,NULL};

#define	ARC_HLFTP_TRY_MAXNB		5

#define ARC_HLFTP_FCHECK(_x)	do{\
									char cmd[12] = {"AT#FTPFRET\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

#define ARC_HLFTP_DOWNLD(_x) do{\
								char cmd[170] = {"AT#FTPFILE="};\
								sprintf(cmd+11,"\"%s\",%d,\"%s\",\"%s\",\"%s\",\"%s\"\r",	ARC_HLFTP_ObjTab[_x]->cfg.remote_addr,\
																							ARC_HLFTP_ObjTab[_x]->cfg.tcp_port,\
																							ARC_HLFTP_ObjTab[_x]->cfg.user,\
																							ARC_HLFTP_ObjTab[_x]->cfg.pass,\
																							ARC_HLFTP_ObjTab[_x]->pFileName,\
																							ARC_HLFTP_ObjTab[_x]->pFilePath);\
								ARC_HLNET_STR_DEBUG(cmd);\
								aptGsmUart_Flush();\
								aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
							}while(0)
											
/*--------------------------------------------------------------------*/
/* Unsolicited:                                                   */
/* Properties:          Set Options                          */
/*Expect String:                                                */
/*--------------------------------------------------------------------*/
static void ARC_HLFTP_OPTION_SET(va_list *ap, int option, arc_hlftpcfg_t *tcpcfg){
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
			strncpy(tcpcfg->remote_addr, pstr,64);
		}
		break;
		case ARC_HLNET_OPT_USER:
		{
			char * pstr = NULL;
			pstr = va_arg(*ap,char*);
			strncpy(tcpcfg->user, pstr,32);
		}
		break;
		case ARC_HLNET_OPT_PASS:
		{
			char * pstr = NULL;
			pstr = va_arg(*ap,char*);
			strncpy(tcpcfg->pass, pstr,32);
		}
		break;
		case ARC_HLNET_OPT_PORT:
		tcpcfg->tcp_port =  va_arg(*ap,int);
		break;
		case ARC_HLNET_OPT_SRCPORT:
		tcpcfg->source_port =  va_arg(*ap,int);
		break;
		case ARC_HLNET_OPT_FTYPE:
		tcpcfg->file_type =  va_arg(*ap,int);
		break;
		case ARC_HLNET_OPT_PASSIVE:
		tcpcfg->isPassive =  va_arg(*ap,int);
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
static arc_hlftp_taskseq_e ARC_HLFTP_CoRoutine(uint8_t conf_id, arc_hlftp_taskseq_e CurrState) {
	uint8_t * pStr = NULL;
	uint8_t * pLocalData = NULL;
	uint16_t iLocalDataSize = 0;
	arc_hlcnxcfg_t *hcnx = NULL;
	static uint16_t ppp_timeout = 0;
	
	
	switch(CurrState) {
	case ARC_HLFTP_TASK_IDLE:
		break;
	case ARC_HLFTP_TASK_DWL_CFG:
	
		if(!arc_netsys.bDeiveInitOK) {
			vPutsTrace(0,"FTP WAIT DEVICE INIT");
			break;
		}
		
		if(ppp_timeout < (60*30)) {
			static uint16_t connect_age = 0;
			//uint8_t timeout = 0;
			ppp_timeout++;
			
			if(arc_netsys.bPdpActivated) connect_age++;
			else {
				vPrintfTrace((0,"FTP WAIT NET CONNECT: %d",ppp_timeout));
				connect_age = 0;
				vTaskDelay(1000);
				break;
			}
			
			
			if(connect_age < 20){
				vPrintfTrace((0,"FTP WAIT NET STABLE: %d",connect_age));
				vTaskDelay(1000);
				break;
			}
		}
		
		hcnx = arc_hlcnx_GetConfig(ARC_HLFTP_ObjTab[conf_id]->cfg.cnxid);
		if(hcnx->is_activated && arc_netsys.bGsmRegistered) {
			uint8_t timeout = 0;
			ARC_HLFTP_DOWNLD(conf_id);
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,6,1000);
				pStr = NULL;
				if(iLocalDataSize>=6) pStr = memmem(pLocalData,iLocalDataSize,"\r\nOK\r\n",6);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLFTP_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			if(!pStr) {
				if(iLocalDataSize) vPuts("\r\n#FTPFILE: CMD FAILED\r\n");
				else vPuts("\r\n#FTPFILE: NO ANSWER\r\n");
				return ARC_HLFTP_TASK_DWL_CFG;
			}
			else {
				vPutsTrace(0,"FTP DOTA START...");
				return ARC_HLFTP_TASK_DWL_WAIT;
			}
		}
		else if(!arc_netsys.bGsmRegistered) {
			vPutsTrace(0,"FTP DOTA WAIT GSM ...");
		}else vPutsTrace(0,"FTP DOTA NO CONFIG ...");
		break;
	case ARC_HLFTP_TASK_DWL_WAIT:
		{
			static uint32_t timeout_count = 0;
			vPrintfTrace((0,"FTP DOTA... Timeout(%d)",ARC_HLFTP_DWL_TIMEOUT-timeout_count));
			
			uint8_t timeout = 0;
			ARC_HLFTP_FCHECK(conf_id);
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,15,1000);
				pStr = NULL;
				if(iLocalDataSize>=15) pStr = memmem(pLocalData,iLocalDataSize,"#FTPFRET: ",10);
				vTaskDelay(1);
				timeout_count++;
			}while(!pStr && (timeout <= ARC_HLFTP_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			enum {
				DOTA_WAIT,
				DOTA_SUCCESS,
				DOTA_FAILED,
			}dwl_state = DOTA_WAIT;
			
			if(pStr) {
				if(memmem(pLocalData,iLocalDataSize,"FIN",3))dwl_state = DOTA_SUCCESS;
				else if(memmem(pLocalData,iLocalDataSize,"FAI",3))dwl_state = DOTA_FAILED;
			}
			
			if(dwl_state) {
				if(dwl_state == DOTA_SUCCESS) return ARC_HLFTP_TASK_XMD_CFG;
				else {
					vPutsTrace(0,"FTP DOTA FAILED");
					vTaskDelay(1000);
					/* Perform the software reset. */
					rstc_start_software_reset(RSTC);
					return ARC_HLFTP_TASK_IDLE;
				}
			}
			else {
				if(timeout_count > ARC_HLFTP_DWL_TIMEOUT) {
					timeout_count = 0;
					vPutsTrace(0,"FTP DOTA TIMEOUT");
					vTaskDelay(1000);
					/* Perform the software reset. */
					rstc_start_software_reset(RSTC);
					return ARC_HLFTP_TASK_IDLE;
				}
			}	
		}
		break;
	case ARC_HLFTP_TASK_XMD_CFG:
		vPutsTrace(0,"DOTA EOF...");
		return ARC_HLFTP_TASK_FW_INTALL;
		break;
	case ARC_HLFTP_TASK_FW_INTALL:
		dota_trigger();
		break;
	default:
		break;
	}
	return CurrState;
}


/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Close TCP               */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLFTP_DelTask(uint8_t conf_id) {
	if(conf_id >= ARC_HLFTP_MAX_SESSION) return ERROR;
	if(!ARC_HLFTP_Cbk) return ERROR;
	if(!ARC_HLFTP_ObjTab[conf_id])return ERROR;
	adl_memRelease(ARC_HLFTP_ObjTab[conf_id]->pFileName);
	adl_memRelease(ARC_HLFTP_ObjTab[conf_id]->pFilePath);
	adl_memRelease(ARC_HLFTP_ObjTab[conf_id]->pTaskName);
	adl_memRelease(ARC_HLFTP_ObjTab[conf_id]);
	return TRUE;
}


/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Task                                       */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLFTP_RunTask(uint8_t pTaskId) {
	if(ARC_HLFTP_ObjTab[pTaskId]->enableTask == FALSE) return FALSE;
	
	ARC_HLFTP_ObjTab[pTaskId]->eTaskSeq = ARC_HLFTP_CoRoutine(pTaskId, ARC_HLFTP_ObjTab[pTaskId]->eTaskSeq);
	ARC_HLFTP_ObjTab[pTaskId]->alive_time++;
	
	if(ARC_HLFTP_ObjTab[pTaskId]->dev_timeout >= 20) {
		ARC_HLFTP_Cbk(pTaskId, ARC_HLNET_EV_TIMEOUT, NULL);
		ARC_HLFTP_ObjTab[pTaskId]->dev_timeout = 0;
		ARC_HLFTP_ObjTab[pTaskId]->alive_time = 0;
	}else if(ARC_HLFTP_ObjTab[pTaskId]->alive_time >= 20) {
		ARC_HLFTP_Cbk(pTaskId, ARC_HLNET_EV_ALIVE, NULL);
		ARC_HLFTP_ObjTab[pTaskId]->dev_timeout = 0;
		ARC_HLFTP_ObjTab[pTaskId]->alive_time = 0;
	}
	return TRUE;
}


/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Start Task              */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hlftp_Dota(uint8_t conf_id, const char* name, const char* path) {
	arc_hlctrl_para_t * pvTask = NULL;
	uint8_t * pTaskName = NULL;
	uint8_t * pTargetName = NULL;
	uint8_t * pTargetPath = NULL;
	if(conf_id >= ARC_HLFTP_MAX_SESSION) return ERROR; // More over ID
	if(!ARC_HLFTP_Cbk) return ERROR; // No subscribed
	if(!ARC_HLFTP_ObjTab[conf_id])return ERROR; // Misconfiguration for TCP
	if(!arc_hlcnx_GetConfig(ARC_HLFTP_ObjTab[conf_id]->cfg.cnxid))return ERROR;// Misconfiguration for CNX
	if(ARC_HLFTP_ObjTab[conf_id]->eTaskSeq != ARC_HLFTP_TASK_IDLE){
		return ERROR;// Running already
	}
	
	#if ARC_HLFTP_SINGLE_TASK
	if(ARC_HLFTP_Task_Runing) return ERROR;// Running already
	ARC_HLFTP_Task_Runing = TRUE;
	#endif
	
	ARC_HLFTP_ObjTab[conf_id]->eTaskSeq = ARC_HLFTP_TASK_DWL_CFG;

	pTaskName = adl_memGet(strlen((char*)ARC_HLFTP_TASK_NAME)+3);
	pTargetName = adl_memGet(strlen((char*)name)+1);
	pTargetPath = adl_memGet(strlen((char*)path)+1);
	if((!pTaskName)||(!pTargetName)||(!pTargetPath)) return ERROR;
	sprintf((char*)pTaskName,"%s_%d",ARC_HLFTP_TASK_NAME,conf_id);
	strcpy((char*)pTargetName,name);
	strcpy((char*)pTargetPath,path);
	ARC_HLFTP_ObjTab[conf_id]->pTaskName = pTaskName;
	ARC_HLFTP_ObjTab[conf_id]->pFileName = pTargetName;
	ARC_HLFTP_ObjTab[conf_id]->pFilePath = pTargetPath;
	
	pvTask = adl_memGet(sizeof(arc_hlctrl_para_t));
	if(!pvTask) return ERROR;
	pvTask->id = conf_id;
	pvTask->func = &ARC_HLFTP_RunTask;
	pvTask->delc = &ARC_HLFTP_DelTask;
	pvTask->wd = NULL;
	
	xTaskCreate (	arc_hlCtrl_AccessTask,				/* The task that implements the command console. */
					(const int8_t *const)pTaskName,		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					ARC_HLFTP_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					pvTask,								/* The parameter is used to pass the already configured UART port into the task. */
					ARC_HLFTP_TASK_PRIORITY,			/* The priority allocated to the task. */
					ARC_HLFTP_ObjTab[conf_id]->hTask	/* Used to store the handle to the created task */
				);
	return OK;
}
/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Set config              */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hlftp_SetConfig(uint8_t conf_id, int opt, ...) {
	va_list ap;
	if(conf_id >= ARC_HLFTP_MAX_SESSION) return ERROR;
	va_start(ap, opt);
	if(!ARC_HLFTP_ObjTab[conf_id]) {
		ARC_HLFTP_ObjTab[conf_id] = adl_memGet(sizeof(arc_hlftpobject_t));
		if(!ARC_HLFTP_ObjTab[conf_id])return ERROR;
		ARC_HLFTP_ObjTab[conf_id]->cfg.cnxid = 0;
		ARC_HLFTP_ObjTab[conf_id]->cfg.tcpcfg_mode = ARC_HLFTP_MODE_SERVER;
		strcpy(ARC_HLFTP_ObjTab[conf_id]->cfg.remote_addr,"");
		ARC_HLFTP_ObjTab[conf_id]->cfg.tcp_port = 21;
		ARC_HLFTP_ObjTab[conf_id]->cfg.source_port = 50;
		ARC_HLFTP_ObjTab[conf_id]->cfg.file_type = 'I';
		ARC_HLFTP_ObjTab[conf_id]->cfg.isPassive = TRUE;

		ARC_HLFTP_ObjTab[conf_id]->enableTask = TRUE;
		ARC_HLFTP_ObjTab[conf_id]->hTask = NULL;
		ARC_HLFTP_ObjTab[conf_id]->pTaskName = NULL;
		ARC_HLFTP_ObjTab[conf_id]->pFileName = NULL;
		ARC_HLFTP_ObjTab[conf_id]->pFilePath = NULL;
		ARC_HLFTP_ObjTab[conf_id]->session_id = 0;
		ARC_HLFTP_ObjTab[conf_id]->connected = FALSE;
		ARC_HLFTP_ObjTab[conf_id]->dev_timeout = 0;
		ARC_HLFTP_ObjTab[conf_id]->eTaskSeq = ARC_HLFTP_TASK_IDLE;
	}
	
	while(opt) {
		ARC_HLFTP_OPTION_SET(&ap,opt,&ARC_HLFTP_ObjTab[conf_id]->cfg);//set option value
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
int8_t arc_hlftp_Subscribe(arc_hlftp_fCbk cbk) {
	uint8_t sid = 0;
	if(!cbk) return ERROR;
	if(ARC_HLFTP_Cbk) {
		ARC_HLFTP_Cbk = cbk;
		return OK;
	}
	ARC_HLFTP_Cbk = cbk;

	for(;sid<ARC_HLFTP_MAX_SESSION;sid++) {
		ARC_HLFTP_ObjTab[sid] = NULL;
	}
	return OK;
}