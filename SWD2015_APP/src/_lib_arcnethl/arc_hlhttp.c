/*
 * arc_hlhttp.c
 *
 * Created: 7/1/2016 11:33:53 AM
 *  Author: Panmaya
 */ 
#include "var/arc_hlnet.h"
#include "ctrl/arc_hlaccess.h"
#include "apt/apt_gsm_uart.h"
#include "arc_hlhttp.h"
#include "arc_hlcnx.h"
#include "arc_hltcp.h"

#include <stdarg.h>

#define ARC_HLHTTP_DWL_MINUTE(_t)	(_t*60)
#define ARC_HLHTTP_DWL_TIMEOUT		ARC_HLHTTP_DWL_MINUTE(1)

typedef enum ARC_HLHTTP_TASK_SEQ_E {
	ARC_HLHTTP_TASK_GET,
	ARC_HLHTTP_TASK_WAIT,
}arc_hlhttp_taskseq_e;

typedef struct ARC_HLHTTP_OBJ_T {
	bool enableTask;
	xTaskHandle hTask;
	uint8_t *pTaskName;
	char *pTargetName;
	uint8_t	session_id;
	uint16_t dev_timeout;
	uint16_t alive_time;
	bool connected;
	arc_hlhttp_taskseq_e eTaskSeq;
}arc_hlhttpobject_t;

static arc_hlhttp_fCbk ARC_HLHTTP_Cbk = NULL;
static arc_hlhttpobject_t *ARC_HLHTTP_ObjTab = NULL;

#define	ARC_HLHTTP_TRY_MAXNB		5

#define ARC_HLHTTP_FCHECK(_x)	do{\
									char cmd[12] = {"AT#HTTPRET\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

#define ARC_HLHTTP_GET(_x)	do{		char *cmd = adl_memGet(strlen(_x) + 15);\
									sprintf(cmd,"AT#HTTPGET=\"%s\"\r",_x);\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
									adl_memRelease(cmd);\
							}while(0)
							
/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Close TCP               */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static int8_t arc_hlhttp_Close(void){
	ARC_HLHTTP_ObjTab->enableTask = FALSE;
	return OK;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Procedure Timer Handler           */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static arc_hlhttp_taskseq_e ARC_HLHTTP_CoRoutine(uint8_t conf_id, arc_hlhttp_taskseq_e CurrState) {
	uint8_t * pStr = NULL;
	uint8_t * pLocalData = NULL;
	uint16_t iLocalDataSize = 0;

	switch(CurrState) {
	case ARC_HLHTTP_TASK_GET:
		{
			uint8_t timeout = 0;
			ARC_HLHTTP_GET(ARC_HLHTTP_ObjTab->pTargetName);
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,6,1000);
				pStr = NULL;
				if(iLocalDataSize>=6) pStr = memmem(pLocalData,iLocalDataSize,"\r\nOK\r\n",6);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLHTTP_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			if(!pStr) {
				if(iLocalDataSize) vPuts("\r\n#HTTPGET: CMD FAILED\r\n");
				else vPuts("\r\n#HTTPGET: NO ANSWER\r\n");
				return ARC_HLHTTP_TASK_GET;
			}
			else {
				vPutsTrace(0,"HTTP BROWSING..");
				return ARC_HLHTTP_TASK_WAIT;
			}
		}
		break;
	case ARC_HLHTTP_TASK_WAIT:
		{
			static uint32_t timeout_count = 0;
			vPrintfTrace((0,"HTTP ... Timeout(%d)",ARC_HLHTTP_DWL_TIMEOUT-timeout_count));
			
			uint8_t timeout = 0;
			ARC_HLHTTP_FCHECK(conf_id);
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,15,1000);
				pStr = NULL;
				if(iLocalDataSize>=15) pStr = memmem(pLocalData,iLocalDataSize,"#HTTPRET: ",10);
				vTaskDelay(1);
				timeout_count++;
			}while(!pStr && (timeout <= ARC_HLHTTP_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			enum {
				BROWSE_WAIT,
				BROWSE_SUCCESS,
				BROWSE_FAILED,
			}browse_state = BROWSE_WAIT;
			
			if(pStr) {
				if(memmem(pLocalData,iLocalDataSize,"FIN",3))browse_state = BROWSE_SUCCESS;
				else if(memmem(pLocalData,iLocalDataSize,"FAI",3))browse_state = BROWSE_FAILED;
			}
			
			if(browse_state) {
				if(browse_state == BROWSE_SUCCESS) ARC_HLHTTP_Cbk( ARC_HLNET_EV_CONNECTED, NULL);
				else ARC_HLHTTP_Cbk( ARC_HLNET_EV_CONN_ERR, NULL);
				arc_hlhttp_Close();
			}
			else {
				if(timeout_count > ARC_HLHTTP_DWL_TIMEOUT) {
					timeout_count = 0;
					ARC_HLHTTP_Cbk( ARC_HLNET_EV_TIMEOUT, NULL);
					arc_hlhttp_Close();
				}
			}
		}
		break;
	default:
		break;
	}
	return CurrState;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Task                                       */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLHTTP_RunTask(uint8_t pTaskId) {
	if(ARC_HLHTTP_ObjTab->enableTask == FALSE) return FALSE;
	
	ARC_HLHTTP_ObjTab->eTaskSeq = ARC_HLHTTP_CoRoutine(pTaskId, ARC_HLHTTP_ObjTab->eTaskSeq);
	ARC_HLHTTP_ObjTab->alive_time++;
	
	if(ARC_HLHTTP_ObjTab->dev_timeout >= 20) {
		ARC_HLHTTP_Cbk( ARC_HLNET_EV_TIMEOUT, NULL);
		ARC_HLHTTP_ObjTab->dev_timeout = 0;
		ARC_HLHTTP_ObjTab->alive_time = 0;
		}else if(ARC_HLHTTP_ObjTab->alive_time >= 20) {
		ARC_HLHTTP_Cbk( ARC_HLNET_EV_ALIVE, NULL);
		ARC_HLHTTP_ObjTab->dev_timeout = 0;
		ARC_HLHTTP_ObjTab->alive_time = 0;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Close TCP               */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLHTTP_DelTask(uint8_t conf_id) {
	if(!ARC_HLHTTP_Cbk) return ERROR;// No subscribed
	if(!ARC_HLHTTP_ObjTab)return ERROR;// Misconfiguration for TCP
	adl_memRelease(ARC_HLHTTP_ObjTab->pTaskName);
	adl_memRelease(ARC_HLHTTP_ObjTab->pTargetName);
	adl_memRelease(ARC_HLHTTP_ObjTab);
	ARC_HLHTTP_ObjTab = NULL;
	return TRUE;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Start Task              */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hlhttp_Browse(uint16_t cnxid, const char* url) {
	arc_hlctrl_para_t * pvTask = NULL;
	uint8_t * pTaskName = NULL;
	char * pTargetName = NULL;
	if(!ARC_HLHTTP_Cbk) return ERROR; // No subscribed
	if(!arc_hlcnx_GetConfig(cnxid))return ERROR;// Misconfiguration for CNX
	if(!ARC_HLHTTP_ObjTab) { // Misconfiguration for TCP
		ARC_HLHTTP_ObjTab = adl_memGet(sizeof(arc_hlhttpobject_t));
		if(!ARC_HLHTTP_ObjTab)return ERROR;
		ARC_HLHTTP_ObjTab->enableTask = TRUE;
		ARC_HLHTTP_ObjTab->hTask = NULL;
		ARC_HLHTTP_ObjTab->pTaskName = NULL;
		ARC_HLHTTP_ObjTab->pTargetName = NULL;
		ARC_HLHTTP_ObjTab->session_id = 0;
		ARC_HLHTTP_ObjTab->connected = FALSE;
		ARC_HLHTTP_ObjTab->dev_timeout = 0;
		ARC_HLHTTP_ObjTab->eTaskSeq = ARC_HLHTTP_TASK_GET;
	}else return ERROR;// Running already

	pTaskName = adl_memGet(strlen((char*)ARC_HLHTTP_TASK_NAME)+3);
	pTargetName = adl_memGet(strlen((char*)url) +1 );
	if((!pTaskName)||(!pTargetName)) return ERROR;
	sprintf((char*)pTaskName,"%s",ARC_HLHTTP_TASK_NAME);
	strcpy((char*)pTargetName,url);
	ARC_HLHTTP_ObjTab->pTaskName = pTaskName;
	ARC_HLHTTP_ObjTab->pTargetName = pTargetName;
	
	pvTask = adl_memGet(sizeof(arc_hlctrl_para_t));
	if(!pvTask) return ERROR;
	pvTask->id = 1;
	pvTask->func = &ARC_HLHTTP_RunTask;
	pvTask->delc = &ARC_HLHTTP_DelTask;
	pvTask->wd = NULL;
	
	xTaskCreate (	arc_hlCtrl_AccessTask,				/* The task that implements the command console. */
					(const int8_t *const)pTaskName,		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					ARC_HLHTTP_TASK_STACK_SIZE,			/* The size of the stack allocated to the task. */
					pvTask,								/* The parameter is used to pass the already configured UART port into the task. */
					ARC_HLHTTP_TASK_PRIORITY,			/* The priority allocated to the task. */
					ARC_HLHTTP_ObjTab->hTask			/* Used to store the handle to the created task */
				);
	return OK;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Function For Subscribe              */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t arc_hlhttp_Subscribe(arc_hlhttp_fCbk cbk) {
	if(!cbk) return ERROR;
	if(ARC_HLHTTP_Cbk) {
		ARC_HLHTTP_Cbk = cbk;
		return OK;
	}
	ARC_HLHTTP_Cbk = cbk;
	
	return OK;
}