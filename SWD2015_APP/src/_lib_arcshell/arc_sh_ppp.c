/*
 * arc_sh_apnserv.c
 *
 *  Created on: 29 Á.¤. 2555
 *      Author: Pan
 */

#include "adl_global.h"
#include "arc_sh_ppp.h"
#include "common/arc_shell_common.h"

typedef struct __ARC_SH_PPP_STRUCT_ {
	ascii apn [30];
	ascii user[20];
	ascii pass[20];
	ascii cpin[5];
}__arc_sh_ppp_t;

static __arc_sh_ppp_t ppp = {
	.apn = "www.dtac.co.th",
	.user = "",
	.pass = "",
	.cpin = ""
};
static char config_dir[6] = {""};

void arc_shell_ppp_InitModule(const char dir[6]) {
	strcpy(config_dir,dir);
	if(adl_flhSubscribe(config_dir, 1)==OK)
	adl_flhWrite(config_dir,0,sizeof(__arc_sh_ppp_t),(u8*)&ppp);
	else {
		if(adl_flhExist(config_dir,0)!= sizeof(__arc_sh_ppp_t)) {
			adl_flhWrite(config_dir,0,sizeof(__arc_sh_ppp_t),(u8*)&ppp);
		}else adl_flhRead(config_dir,0,sizeof(__arc_sh_ppp_t),(u8*)&ppp);
	}
}


void arc_shell_ppp_apnHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.apn, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_ppp_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.apn);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.apn);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}
void arc_shell_ppp_userHandler(adl_atCmdPreParser_t * Command){
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;
	
	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.user, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_ppp_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.user);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.user);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);

}
void arc_shell_ppp_passHandler(adl_atCmdPreParser_t * Command){
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.pass, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_ppp_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.pass);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.pass);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}

void arc_shell_ppp_cpinHandler(adl_atCmdPreParser_t * Command){
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;
	
	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.cpin, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_ppp_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.cpin);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.cpin);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}
