/*
 * arc_sh_tcp1.c
 *
 *  Created on: 29 Á.¤. 2555
 *      Author: Pan
 */

#include "adl_global.h"
#include "arc_sh_tcps1.h"
#include "common/arc_shell_common.h"

typedef struct __ARC_SH_TCP_SERVE1_STRUCT_ {
	ascii addr [64];
	ascii port [6];
}__arc_sh_tcps1_t;

static __arc_sh_tcps1_t ppp = {
	.addr = "202.149.98.139",
	.port = "900",
};
static char config_dir[6] = {""};


void arc_shell_tcps1_InitModule(const char dir[6]) {
	strcpy(config_dir,dir);
	if(adl_flhSubscribe(config_dir, 1)==OK)
	adl_flhWrite(config_dir,0,sizeof(__arc_sh_tcps1_t),(u8*)&ppp);
	else {
		if(adl_flhExist(config_dir,0)!= sizeof(__arc_sh_tcps1_t)) {
			adl_flhWrite(config_dir,0,sizeof(__arc_sh_tcps1_t),(u8*)&ppp);
		}else adl_flhRead(config_dir,0,sizeof(__arc_sh_tcps1_t),(u8*)&ppp);
	}
}

void arc_shell_tcp_Serve1_AddrHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.addr, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_tcps1_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.addr);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.addr);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}
void arc_shell_tcp_Serve1_PortHandler(adl_atCmdPreParser_t * Command){
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) {
			if(wm_atoi(strBuff)<=65535)wm_strcpy (ppp.port, strBuff);
		}
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_tcps1_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.port);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.port);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);

}


