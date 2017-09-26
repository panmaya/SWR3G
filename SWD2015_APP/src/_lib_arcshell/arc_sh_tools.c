/*
 * arc_sh_tools.c
 *
 *  Created on: 22 ¡.¾. 2555
 *      Author: Pan
 */
#include "adl_global.h"
#include "arc_sh_tools.h"
#include "common/arc_shell_common.h"

typedef struct __ARC_SH_TOOLS_T_ {
	ascii httpRequested[2];
	struct {
		ascii want[ 2];
		ascii addr[64];
		ascii user[32];
		ascii pass[32];
		ascii file[32];
	}DOTA;
}__arc_sh_tools_t;

static __arc_sh_tools_t ppp = {
	.httpRequested = "0",
	.DOTA = {
		.want = "0",
		.addr = "0.0.0.0",
		.user = "anonymous",
		.pass = "1234",
		.file = "fw.dwl"
	}
};

static char config_dir[6] = {""};
	
	
void arc_shell_tools_InitModule(const char dir[6]) {
	strcpy(config_dir,dir);
	if(adl_flhSubscribe(config_dir, 1)==OK)
	adl_flhWrite(config_dir,0,sizeof(__arc_sh_tools_t),(u8*)&ppp);
	else {
		if(adl_flhExist(config_dir,0)!= sizeof(__arc_sh_tools_t)) {
			adl_flhWrite(config_dir,0,sizeof(__arc_sh_tools_t),(u8*)&ppp);
		}else adl_flhRead(config_dir,0,sizeof(__arc_sh_tools_t),(u8*)&ppp);
	}
}


void arc_shell_tools_HttpFileHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.httpRequested, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_tools_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.httpRequested);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.httpRequested);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}

void arc_shell_tools_DOTAForceHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		if(Command->NbPara==1) { // Only Enable Flag Set
			strBuff = ADL_GET_PARAM (Command, 0);
			if(strBuff && strBuff[0]) wm_strcpy (ppp.DOTA.want, strBuff);
		}
		else {
			wm_strcpy(ppp.DOTA.want, "1");
			strBuff = ADL_GET_PARAM (Command, 0);
			if(strBuff && strBuff[0]) wm_strcpy (ppp.DOTA.addr, strBuff);
			strBuff = ADL_GET_PARAM (Command, 1);
			if(strBuff && strBuff[0]) wm_strcpy (ppp.DOTA.user, strBuff);
			strBuff = ADL_GET_PARAM (Command, 2);
			if(strBuff && strBuff[0]) wm_strcpy (ppp.DOTA.pass, strBuff);
			strBuff = ADL_GET_PARAM (Command, 3);
			if(strBuff && strBuff[0]) wm_strcpy (ppp.DOTA.file, strBuff);
		}
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_tools_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_TEST:
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.DOTA.want);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.DOTA.addr);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.DOTA.user);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.DOTA.pass);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.DOTA.file);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.DOTA.want);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.DOTA.addr);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.DOTA.user);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.DOTA.pass);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.DOTA.file);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}

