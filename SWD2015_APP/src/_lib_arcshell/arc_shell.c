/*
 * shell.c
 *
 *  Created on: 29 Á.¤. 2555
 *      Author: Pan
 */

#include "adl_global.h"
#include "arc_shell.h"
#include "arc_sh_ppp.h"
#include "arc_sh_tcps1.h"
#include "arc_sh_btt.h"
#include "arc_sh_tools.h"
#include "arc_sh_swd.h"

static xSemaphoreHandle cmdShell_sem = NULL;

static void arc_shell_listConfigHandler(adl_atCmdPreParser_t * Command);

typedef struct ARC_SHELL_COMMAND_T  {
   const ascii*  sCommand;
   u16	iCmdLen;
   void (*adl_atCmdHandler_t)(adl_atCmdPreParser_t * Params );
   u16	cmdAttributes;
   u8	maxParameters;
   u8	minParameters;
}arc_shell_command_t;

static uint8_t ARC_SHELL_CMD_Buffer[ARC_SHELL_MAX_CMD_LEN + sizeof(adl_atCmdPreParser_t) + 1];


const arc_shell_command_t arc_shell_command[ ] = {
		{"AT#LCONFIG"		, 	10	,	arc_shell_listConfigHandler				, (ADL_CMD_TYPE_PARA |ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT), 1,0 },
		//   EXECUTE FROM HERE    //
		{"AT#DOTA"			, 	 7	,	arc_shell_tools_DOTAForceHandler		, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 4,1 },
		{"AT#HTTPFILE"		, 	11	,	arc_shell_tools_HttpFileHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		//       LIST CONFIG START FROM HERE //
		{"AT#CPIN"			, 	 7	,	arc_shell_ppp_cpinHandler				, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		{"AT#APNSERV"		,	10	,	arc_shell_ppp_apnHandler				, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		{"AT#APNUN"			, 	 8	,	arc_shell_ppp_userHandler				, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		{"AT#APNPW"			, 	 8	,	arc_shell_ppp_passHandler				, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		{"AT#TCPSERV"		, 	10	,	arc_shell_tcp_Serve1_AddrHandler		, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		{"AT#TCPPORT"		, 	10	,	arc_shell_tcp_Serve1_PortHandler		, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		{"AT#SAMPLING"		, 	11	,	arc_shell_apt_SamplingHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 2,2 },
		{"AT#OVERSPEED"		, 	12	,	arc_shell_apt_OverSpeedHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 2,1 },
		{"AT#IDLESPEED"		, 	12	,	arc_shell_apt_IdlingModeHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 2,1 },
		{"AT#OUTPUT1"		, 	10	,	arc_shell_apt_Output1Handler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 2,1 },
		{"AT#OUTPUT2"		, 	10	,	arc_shell_apt_Output2Handler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 2,1 },
		{"AT#SMSUSER1"		, 	11	,	arc_shell_apt_AdminPhoneNb1Handler		, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		{"AT#SMSUSER2"		, 	11	,	arc_shell_apt_AdminPhoneNb2Handler		, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		{"AT#SMSUSER3"		, 	11	,	arc_shell_apt_AdminPhoneNb3Handler		, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		{"AT#SMSUSER4"		, 	11	,	arc_shell_apt_AdminPhoneNb4Handler		, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST), 1,1 },
		//  SWD //
		{"AT#COMP"			, 	7	,	arc_shell_swd_CompanyGetHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT ), 1,1 },
		{"AT#BOXID"			, 	8	,	arc_shell_swd_BoxidGetHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT ), 1,1 },
		{"AT#SIMNUM"		, 	9	,	arc_shell_swd_SimnumGetHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT ), 1,1 },
		{"AT#POINT"			, 	8	,	arc_shell_swd_PointGetHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT ), 1,1 },
		{"AT#OVERSPW"		, 	10	,	arc_shell_swd_OverspwGetHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT ), 1,1 },
		{"AT#EMAIL"			, 	8	,	arc_shell_swd_EmailGetHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT ), 1,1 },
		{"AT#SETHD"			, 	8	,	arc_shell_swd_HDHAHandler				, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT ), 2,2 },
		{"AT#HDCAL"			, 	8	,	arc_shell_swd_HDHACalHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT ), 2,2 },
		{"AT#SWDOPT"		, 	9	,	arc_shell_swd_OptionHandler				, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST ), 1,1 },
		{"AT#VEHTYPE"		, 	10	,	arc_shell_swd_VehTypeHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST ), 1,1 },
		{"AT#VEHATTR"		, 	10	,	arc_shell_swd_VehAttrHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST ), 1,1 },
		{"AT#SWDFM"			, 	8	,	arc_shell_swd_FeatureHandler			, (ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST ), 1,1 },
		{ NULL				,	 0	,	(void*)0								, 													  (0), 0,0 },
};

static ascii * cmd_shell_exe(const ascii* sCmd, adl_port_e port) {
	static char cmd_shell_RespStack[ARC_SHELL_MAX_CMD_LEN] = {""};// For containing answer string
	static char cmd_shell_ParamBuffer[ARC_SHELL_MAX_CMD_LEN] = {""};

	u16 i = 0, len = 0;
	adl_atCmdPreParser_t *cmd = (void*)0;
	cmd_shell_RespStack[0] = '\0';
	if(strlen(sCmd) >= ARC_SHELL_MAX_CMD_LEN)return NULL;
	for(;sCmd[len];len++){if((sCmd[len]=='=')||(sCmd[len]=='?'))break;}
	if(port == ADL_PORT_NONE) i = 1;// EXCLUDED LIST COMMAND
	else i = 0;// INCLUDED LIST COMMAND
	for(;(arc_shell_command[i].iCmdLen!=0);i++) {
		if(arc_shell_command[i].iCmdLen!=len)continue;
		if(!wm_strnicmp(arc_shell_command[i].sCommand,sCmd,len)) {
			len = wm_strlen(sCmd);//length include all parameters
			cmd = (adl_atCmdPreParser_t*)&ARC_SHELL_CMD_Buffer[0];//adl_memGet((u16)(sizeof(adl_atCmdPreParser_t)+len+1));
			//if(!cmd)return NULL;
			cmd->NbPara = 0;
			cmd->StrLength = len;
			cmd->Port = port;//ADL_PORT_NONE;
			wm_strcpy(cmd->StrData,sCmd);
			cmd->Contxt = &cmd_shell_RespStack[0];
			cmd->ParaList = wm_lstCreate(0,(void*)0);
			switch(sCmd[arc_shell_command[i].iCmdLen]) {
			case '=':
				if(sCmd[arc_shell_command[i].iCmdLen+1]=='?') {
					cmd->Type = ADL_CMD_TYPE_TEST;
				}else 	cmd->Type = ADL_CMD_TYPE_PARA;
				break;
			case '?':	cmd->Type = ADL_CMD_TYPE_READ;
				break;
			case 0:		cmd->Type = ADL_CMD_TYPE_ACT;
				break;
			default:	cmd->Type = ADL_CMD_TYPE_ROOT;
				break;
			}
			if(cmd->Type & arc_shell_command[i].cmdAttributes) {
				if(cmd->Type == ADL_CMD_TYPE_PARA) {
					for(u16 pc = 0; pc < arc_shell_command[i].maxParameters; pc++) {
						cmd_shell_ParamBuffer[0] = '\0';
						char *ptrBuff = wm_strGetParameterString(cmd_shell_ParamBuffer,sizeof(cmd_shell_ParamBuffer),sCmd,(pc+1));
						if(ptrBuff && cmd_shell_ParamBuffer[0]) {
							if(pc >= arc_shell_command[i].maxParameters) {
								wm_lstDestroy(cmd->ParaList);
								//adl_memRelease(cmd);
								return NULL;
							}
							ptrBuff = adl_memGet(wm_strlen(cmd_shell_ParamBuffer)+1);
							if(!ptrBuff)return NULL;
							wm_strcpy(ptrBuff,cmd_shell_ParamBuffer);
							cmd->NbPara++;
							wm_lstAddItem(cmd->ParaList,ptrBuff);
						}
						else { // Unknown parameter format
							//wm_lstDestroy(cmd->ParaList);
							//adl_memRelease(cmd);
							//return NULL;
						}
					}
					if(cmd->NbPara < arc_shell_command[i].minParameters) {
						wm_lstDestroy(cmd->ParaList);
						//adl_memRelease(cmd);
						return NULL;
					}
				}
			}else {
				wm_lstDestroy(cmd->ParaList);
				//adl_memRelease(cmd);
				return NULL;
			}

			arc_shell_command[i].adl_atCmdHandler_t(cmd);
			wm_lstDestroy(cmd->ParaList);
			//adl_memRelease(cmd);
			return &cmd_shell_RespStack[0];
		}
	}

	return NULL;	
}

void arc_shell_delete(void) {
	u16 i = 0;
	for(i=1;(arc_shell_command[i].iCmdLen!=0);i++) {// EXCLUDED LIST COMMAND
		arc_shell_command[i].adl_atCmdHandler_t(NULL);
	}
}

ascii* arc_shell_execute_port(const ascii* sCmd, adl_port_e port) {
	ascii *pRet = NULL;
	if( xSemaphoreTake( cmdShell_sem, ( portTickType ) 5000UL / portTICK_RATE_MS ) == pdTRUE ) {
		pRet = cmd_shell_exe(sCmd,port);
		xSemaphoreGive( cmdShell_sem );
	}else vPrintf("\r\nCmdShell: %s Locked\r\n",sCmd);
	return pRet;
}

ascii* arc_shell_execute(const ascii* sCmd) {
	return arc_shell_execute_port(sCmd, ADL_PORT_NONE);
}


static void arc_shell_listConfigHandler(adl_atCmdPreParser_t * Command) {
	ascii cmd[30] = {""};
	ascii resp[120] = {""};
	for(u16 i = 3;(arc_shell_command[i].iCmdLen!=0);i++) {
		wm_strcpy(cmd,arc_shell_command[i].sCommand);
		wm_strcat(cmd,"?");
		ascii *strBuff = cmd_shell_exe(cmd,ADL_PORT_NONE);
		if(strBuff) {
			strcpy(resp,"\r\n");
			strcpy(resp+2,arc_shell_command[i].sCommand+2);
			wm_strcat(resp,": ");
			wm_strcat(resp,strBuff);
			adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),resp);	
		}
	}

	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),"\r\n\r\nOK\r\n");
}
	

void arc_shell_Subscribe(arc_shell_postCtx rsphdl) {
	if(cmdShell_sem)return;
	cmdShell_sem = xSemaphoreCreateMutex();
	arc_shell_apt_InitModule("shbtt");
	vTaskDelay(200);
	arc_shell_ppp_InitModule("shppp");
	vTaskDelay(200);
	arc_shell_swd_InitModule("shswd");
	vTaskDelay(200);
	arc_shell_tcps1_InitModule("shtcp");
	vTaskDelay(200);
	arc_shell_tools_InitModule("shmis");
	vTaskDelay(200);
	arc_shell_commonSubscribe(rsphdl);
	for(u16 i = 0;arc_shell_command[i].iCmdLen!=0;i++) {
		adl_atCmdSubscribe(	arc_shell_command[i].sCommand , arc_shell_command[i].adl_atCmdHandler_t,
							arc_shell_command[i].cmdAttributes | arc_shell_command[i].minParameters | (arc_shell_command[i].maxParameters << 4) );
	}
}