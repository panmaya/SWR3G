/*
 * arc_hlgsm.c
 *
 * Created: 20/2/2558 16:37:10
 *  Author: pan
 */ 
#include <adl_global.h>
#include "arc_hlgsm.h"
#include "ctrl/arc_hlaccess.h"
#include "apt/apt_gsm_uart.h"
#include "../_lib_adl/adl_CmdStackHandler.h"
#include "../_lib_adl/private/adl_CmdStack_Private.h"
#include "../_lib_adl/private/adl_sms_Private.h"
#include "../_lib_xmodem/xmodem.h"
#include <stdarg.h>

#define ARC_HLGSM_TRY_MAXNB		3

//AT+CMGR=<index>
#define ARC_HLGSM_CHECK()		do {\
									char cmd[12] = {"ATE0\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)
								
#define ARC_HLGSM_IMEI()		do{\
									char cmd[12] = {"AT+WIMEI?\r"};\
									/*ARC_HLNET_STR_DEBUG(cmd);*/\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

#define ARC_HLGSM_INIT()		do {\
									/*char cmd[22] = {"ATE0&K0+KSLEEP=2;&W\r"};*/\
									char cmd[22] = {"ATE0Q0V1S0=1&D0&S0&W\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

//AT
#define ARC_HLGSM_ATCMD(_s,_n)	do {\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)_s,_n);\
								}while(0)
								
#define ARC_HLGSM_SIGNAL()		do{\
									char cmd[12] = {"AT+CSQ\r"};\
									/*ARC_HLNET_STR_DEBUG(cmd);*/\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)
								
#define ARC_HLGSM_GSMRDY()		do{\
									char cmd[12] = {"AT+CREG?\r"};\
									/*ARC_HLNET_STR_DEBUG(cmd);*/\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

//AT+CMGS=<text>
#define ARC_HLGSM_CMGS(_s)		do {\
									char cmd[44] = {"AT+CMGS="};\
									sprintf(cmd+8, "\"%s\"\r",_s);\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)
//AT+CMGF=[<mode>]
#define ARC_HLGSM_CMGF(_e)		do {\
									char cmd[12] = {"AT+CMGF="};\
									sprintf(cmd+8, "%d\r",_e);\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

//AT+CMGR=<index>
#define ARC_HLGSM_CMGR(_e)		do {\
									char cmd[12] = {"AT+CMGR="};\
									sprintf(cmd+8, "%d\r",_e);\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

//AT+CMGL=[<stat>]
#define ARC_HLGSM_CMGL()		do {\
									char cmd[25] = {"AT+CMGL=\"ALL\"\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

//AT+CMGD=<index>[,<delflag>]
#define ARC_HLGSM_CMGD(_i,_f)	do {\
									char cmd[25] = {"AT+CMGD="};\
									sprintf(cmd+8, "%d,%d\r",_i,_f);\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)
								
//AT#CMTI
#define ARC_HLGSM_CMTI()		do {\
									char cmd[12] = {"AT#CMTI\r"};\
									/*ARC_HLNET_STR_DEBUG(cmd);*/\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

#define ARC_HLGSM_SIMRDY()		do{\
									char cmd[12] = {"AT+CPIN?\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)
								
#define ARC_HLGSM_SKIPPB()		do{\
									char cmd[15] = {"AT+WAIP=1;&W\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)
								
#define ARC_HLGSM_TESTAPP()		do{\
									char cmd[15] = {"AT#VERSION\r"};\
									ARC_HLNET_STR_DEBUG(cmd);\
									aptGsmUart_Flush();\
									aptGsmUart_Write((uint8_t*)cmd,strlen(cmd));\
								}while(0)

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:        GSM AT Command                     */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLGSM_AT_RunTask(uint8_t pTaskId) { //GSM AT  General Command
	uint8_t timeout = 0;
	adl_cmdstack_at_item_t *  atcmd = adl_atCmdListGet();
	
	if(atcmd) {
		uint16_t iLocalDataSize = 0;
		uint8_t *pLocalData = NULL;
		char * pStr = NULL;

		ARC_HLGSM_ATCMD((uint8_t*)atcmd->atstr,strlen(atcmd->atstr));
		
		//<CR><LF> .... <CR><LF>
		timeout = 0;
		do {
			timeout++;
			iLocalDataSize = aptGsmUart_Read(&pLocalData,2,3000);
			pStr = NULL;
			if(iLocalDataSize>=3) pStr = memmem(pLocalData,iLocalDataSize,"\n",1);
			if(pStr) {
				pStr = strchr(pStr,'\r');
			}else pStr = NULL;
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));

		if((atcmd->rspFlag & 0x01)&& iLocalDataSize) vPuts((ascii*)pLocalData);// Send Response to serial if flag set
		
		if(atcmd->rsphdlr && pStr) {
			u8 i = 0;
			ascii * sBuff = NULL;
			sBuff = wm_lstGetItem(atcmd->hRspList,i);
			while(sBuff) {
				ascii * pstr = strstr((ascii*)pLocalData,sBuff);
				if(pstr) {
					adl_atResponse_t * rsp = NULL;
					rsp = adl_memGet(sizeof(adl_atResponse_t)+257);
					if(!rsp) return TRUE;
					u16 len = strlen(pstr);
					len = len > 256 ? 256 : len;
					rsp->StrLength = len;
					rsp->Dest = (atcmd->rspFlag & 0xFF00) >> 8;
					strncpy(rsp->StrData,pstr,len);
					rsp->StrData[len] = 0;
					atcmd->rsphdlr(rsp);
					adl_memRelease(rsp);
				}
				i++;
				sBuff = wm_lstGetItem(atcmd->hRspList,i);
				vTaskDelay(1);
			}
		}
		adl_atCmdListFree(atcmd);
	}
	return TRUE;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:        GSM AT Command                     */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLGSM_AT_DelTask(uint8_t pTaskId) { //GSM AT  General Command
	return TRUE;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:        GSM SMS Task                          */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLGSM_SMS_RunTask(uint8_t pTaskId) { // SMS
	uint8_t * pStr = NULL;
	uint8_t * pLocalData = NULL;
	uint16_t iLocalDataSize = 0;
	uint8_t timeout = 0;
	static uint8_t sms_operation = 0;
	static const uint8_t sms_terminator = 0x1A;
	adl_smssnd_item_t *snd_item =  adl_smsSendListGet();
	
	switch(sms_operation) {
	case 0: //Wait GSM Start up And Initial
		ARC_HLGSM_CHECK();
		timeout = 0;
		do {
			timeout++;
			iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
			pStr = NULL;
			if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"OK",2);
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
		
		ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
		
		if(pStr) {// Answer from module OK
			ARC_HLGSM_TESTAPP();
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,12,1000);
				pStr = NULL;
				if(iLocalDataSize>=12) pStr = memmem(pLocalData,iLocalDataSize,"#VERSION: ",10);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));

			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			if(pStr) {// Answer from module OK
				char local_sBuff[30] = {""};
				arc_netdev_noresponse = 0;
				wm_strGetParameterString(local_sBuff,sizeof(local_sBuff),(char*)pStr,1);
				arc_netsys.modemAppVersion = atoi(local_sBuff);
			}
			
			ARC_HLGSM_SKIPPB();
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
				pStr = NULL;
				if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"OK",2);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
					
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			ARC_HLGSM_INIT();
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
				pStr = NULL;
				if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"OK",2);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			sms_operation++;
		}
		break;
	case 1:
		ARC_HLGSM_IMEI();
		timeout = 0;
		do {
			timeout++;
			iLocalDataSize = aptGsmUart_Read(&pLocalData,8,1000);
			pStr = NULL;
			if(iLocalDataSize>=8) pStr = memmem(pLocalData,iLocalDataSize,"+WIMEI: ",8);
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
		
		ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
		
		if(pStr) {// Answer from module OK
			char local_sBuff[30] = {""};
			arc_netdev_noresponse = 0;
			wm_strGetParameterString(local_sBuff,sizeof(local_sBuff),(char*)pStr,1);
			strcpy(arc_netimei,local_sBuff);
			sms_operation++;
		}
		break;
	case 2: //Wait SIM Ready
		if(!ioport_get_pin_level(PIN_SIMPRESS))break;
		ARC_HLGSM_SIMRDY();
		timeout = 0;
		do {
			timeout++;
			iLocalDataSize = aptGsmUart_Read(&pLocalData,12,1000);
			pStr = NULL;
			if(iLocalDataSize>=12) pStr = memmem(pLocalData,iLocalDataSize,"+CPIN: READY",12);
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
		
		ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
		if(pStr) {
			arc_netsys.bSimReady = TRUE;
			arc_netdev_noresponse = 0;
			sms_operation++;
		}else {
			arc_netsys.bSimReady = FALSE;
			if(iLocalDataSize) vPutsTrace(0,"+CPIN: CMD FAILED");
			pStr = NULL;
			if(iLocalDataSize>=5) pStr = memmem(pLocalData,iLocalDataSize,"ERROR",5);
			if(!pStr) arc_netdev_noresponse = 1;
		}
		break;
	case 3: // Delete SMS Stored
		//ARC_HLGSM_CMGD(sms_operation-1,0);
		ARC_HLGSM_CMGD(1,4);
		timeout = 0;
		do {
			timeout++;
			iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
			pStr = NULL;
			if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"OK",2);
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
		ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
		if(pStr) sms_operation++;
		else vTaskDelay(3000);
		break;
	case 4: //Init text Mode
		ARC_HLGSM_CMGF(1);
		timeout = 0;
		do {
			timeout++;
			iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
			pStr = NULL;
			if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"OK",2);
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
		
		ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
		if(pStr) {
			arc_netsys.bDeiveInitOK = TRUE;
			sms_operation++;
		}
		break;
	case 5:
		ARC_HLGSM_GSMRDY();
		timeout = 0;
		do {
			timeout++;
			iLocalDataSize = aptGsmUart_Read(&pLocalData,7,1000);
			pStr = NULL;
			if(iLocalDataSize>=7) pStr = memmem(pLocalData,iLocalDataSize,"+CREG: ",7);
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
		
		if(pStr) {// Answer from module OK
			char local_sBuff[30] = {""};
			uint8_t ireg = 0;
			arc_netdev_noresponse = 0;
			wm_strGetParameterString(local_sBuff,sizeof(local_sBuff),(char*)pStr,2);
			ireg = atoi(local_sBuff);
			if((ireg==1) || (ireg == 5)) {
				vPrintfTrace((0,"+CREG: %s",ireg==1?"HOME":"ROAMING"));
				arc_netsys.bGsmRegistered = TRUE;
				sms_operation++;
				break;
			}else arc_netsys.bGsmRegistered = FALSE;
		}
		break;
	case 6:
		ARC_HLGSM_SIGNAL();
		timeout = 0;
		do {
			timeout++;
			iLocalDataSize = aptGsmUart_Read(&pLocalData,6,1000);
			pStr = NULL;
			if(iLocalDataSize>=6) pStr = memmem(pLocalData,iLocalDataSize,"+CSQ: ",6);
			vTaskDelay(1);
		}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
			
		if(pStr) {// Answer from module OK
			char local_sBuff[30] = {""};
			arc_netdev_noresponse = 0;
			wm_strGetParameterString(local_sBuff,sizeof(local_sBuff),(char*)pStr,1);
			arc_netsys.signLev = atoi(local_sBuff);
		}
	default:// Send && Receive Operation
		if(snd_item) {
			pStr = NULL;
			pLocalData = NULL;
			iLocalDataSize = 0;
			adl_smssubscr_item_t * subscr_item = adl_smsSubscrListGet(snd_item->Handle);
			
			ARC_HLGSM_CMGS(snd_item->SmsTel);
			vTaskDelay(100 / portTICK_RATE_MS);
			timeout = 0;
			do {
				timeout++;
				iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
				pStr = NULL;
				if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"> ",2);
				vTaskDelay(1);
			}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
			
			ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
			
			if(pStr) {
				uint16_t sms_len = strlen(snd_item->SmsText);
				aptGsmUart_Flush();
				ARC_HLNET_STRN_DEBUG(snd_item->SmsText,strlen(snd_item->SmsText));
				snd_item->SmsText[sms_len] = sms_terminator;
				aptGsmUart_Write((uint8_t*)snd_item->SmsText,sms_len+1);
				vTaskDelay(1000 / portTICK_RATE_MS);
				timeout = 0;
				do {
					timeout++;
					iLocalDataSize = aptGsmUart_Read(&pLocalData,9,2000);
					pStr = NULL;
					if(iLocalDataSize>=9) pStr = memmem(pLocalData,iLocalDataSize,"+CMGS: ",7);
					vTaskDelay(1);
				}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
				
				ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);

				if(pStr) {
					subscr_item->SmsCtrl(ADL_SMS_EVENT_SENDING_MR,0);
					vTaskDelay(300 / portTICK_RATE_MS);
					timeout = 0;
					do {
						timeout++;
						iLocalDataSize = aptGsmUart_Read(&pLocalData,2,1000);
						pStr = NULL;
						if(iLocalDataSize>=2) pStr = memmem(pLocalData,iLocalDataSize,"OK",2);
						vTaskDelay(1);
					}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
					
					if(pStr) {//OK
						subscr_item->SmsCtrl(ADL_SMS_EVENT_SENDING_OK,0);
					}else {//ERROR
						aptGsmUart_Write((uint8_t*)&sms_terminator,1);
						subscr_item->SmsCtrl(ADL_SMS_EVENT_SENDING_ERROR,0);
					}
				}
				else {//ERROR
					aptGsmUart_Write((uint8_t*)&sms_terminator,1);
					subscr_item->SmsCtrl(ADL_SMS_EVENT_SENDING_ERROR,0);
				}
			}
			else {
				//ERROR
				aptGsmUart_Write((uint8_t*)&sms_terminator,1);
				subscr_item->SmsCtrl(ADL_SMS_EVENT_SENDING_ERROR,0);
			}

			adl_smsSendListFree(snd_item);
		}
		else
		{
			adl_smsrecv_item_t smsRcv = {0,"","",""};
			adl_smssubscr_item_t * subscr_item = adl_smsSubscrListGet(1);
			if(!subscr_item) return TRUE;// Not found subscribe function
			else 
			{
				uint8_t err_readtry = 3;
				uint8_t   read_index = 0;//arc_hlCtrl_GetCmtiIndex();
				static uint8_t cmti_delay = 5;
				
				cmti_delay--;
				if(cmti_delay)return TRUE;
				cmti_delay = 5;
				
				ARC_HLGSM_CMTI();
				
				timeout = 0;
				do {
					timeout++;
					iLocalDataSize = aptGsmUart_Read(&pLocalData,7,1000);
					pStr = NULL;
					if(iLocalDataSize>=7) pStr = memmem(pLocalData,iLocalDataSize,"#CMTI: ",7);
					vTaskDelay(1);
				}while(!pStr && (timeout <= ARC_HLGSM_TRY_MAXNB));
						
				ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
				
				if(pStr) {
					char local_sBuff[10] = {""};
					wm_strGetParameterString(local_sBuff,sizeof(local_sBuff),(char*)pStr,1);
					read_index = atoi(local_sBuff);
				}else vPrintfTrace((30,"+CMTI: Not response"));
				
				if(read_index == 0) {

					return TRUE;// Not found message
				}
				
				do {
					pStr = NULL;
					pLocalData = NULL;
					iLocalDataSize = 0;
					
					err_readtry--;
					
					ARC_HLGSM_CMGR(read_index);
					//vTaskDelay(1000 / portTICK_RATE_MS);
					//+CMGR: "REC UNREAD","+31628870634",,"11/01/09,10:26:26+04"
					//This is text message 1
					//<CR><LF>OK<CR><LF>
					timeout = 0;
					do {
						timeout++;
						vTaskDelay(1000 / portTICK_RATE_MS);
						iLocalDataSize = aptGsmUart_Read(&pLocalData,50,1000);
						pStr = NULL;
						if(iLocalDataSize>=50) pStr =  memmem(pLocalData,iLocalDataSize,"\r\nOK\r\n",6);
						vTaskDelay(100);
					}while(!pStr && (timeout <= (ARC_HLGSM_TRY_MAXNB * 2)));
				
					ARC_HLNET_STRN_DEBUG(pLocalData,iLocalDataSize);
				
					if(pStr) pStr = memmem(pLocalData,iLocalDataSize,"+CMGR: ",7);
				
					if(pStr) {
						bool ret = adl_smsGetParameterString(&smsRcv, (char*)pStr);
						if(ret) {
							subscr_item->SmsData(smsRcv.SmsTel,smsRcv.SmsTime,smsRcv.SmsText);
							err_readtry = 0;
						}else vPutsTrace(30,"SMS Parameter Incorect!!!");
					}else vPutsTrace(30,"SMS Read Process Incompleted !!!");
					
				}while(err_readtry);
				
				ARC_HLGSM_CMGD(read_index,0);
			}
		}
		break;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:        GSM SMS Task                          */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
static bool ARC_HLGSM_SMS_DelTask(uint8_t pTaskId) { // SMS
	return TRUE;
}


/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Initial Task                               */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
void arc_hlgsm_cmdInit(uint32_t* wd, xTaskHandle *x) {
	
	static arc_hlctrl_para_t pvATTask = {
		.id = 0,
		.func = &ARC_HLGSM_AT_RunTask,
		.delc = &ARC_HLGSM_AT_DelTask
	};
	
	pvATTask.wd = wd;
	
	/* Create task. */
	xTaskCreate (	arc_hlCtrl_AccessTask,				/* The task that implements the command console. */
					ARC_HLGSM_AT_TASK_NAME,				/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					ARC_HLGSM_AT_TASK_STACK_SIZE,		/* The size of the stack allocated to the task. */
					&pvATTask,							/* The parameter is used to pass the already configured UART port into the task. */
					ARC_HLGSM_CMDTASK_PRIORITY,			/* The priority allocated to the task. */
					x									/* Used to store the handle to the created task - in this case the handle is not required. */
				);
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Initial Task                               */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
void arc_hlgsm_smsInit(uint32_t* wd, xTaskHandle *x) {
	

	static arc_hlctrl_para_t pvSMSTask = {
		.id = 0,
		.func = &ARC_HLGSM_SMS_RunTask,
		.delc = &ARC_HLGSM_SMS_DelTask
	};
	
	pvSMSTask.wd = wd;
	
	/* Create task. */
	xTaskCreate (	arc_hlCtrl_AccessTask,				/* The task that implements the command console. */
					ARC_HLGSM_SMS_TASK_NAME,			/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					ARC_HLGSM_SMS_TASK_STACK_SIZE,		/* The size of the stack allocated to the task. */
					&pvSMSTask,							/* The parameter is used to pass the already configured UART port into the task. */
					ARC_HLGSM_SMSTASK_PRIORITY,			/* The priority allocated to the task. */
					x								/* Used to store the handle to the created task - in this case the handle is not required. */
				);

}