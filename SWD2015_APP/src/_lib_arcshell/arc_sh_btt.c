/*
 * arc_sh_btt.c
 *
 *  Created on: 29 Á.¤. 2555
 *      Author: Pan
 */

#include "adl_global.h"
#include "arc_sh_btt.h"
#include "common/arc_shell_common.h"

typedef struct __ARC_SH_APT_UTILS_STRUCT_ {
	struct {
		ascii park [6];
		ascii drive[6];
	}sampling;
	struct {
		ascii logic[2];
		ascii delay[6];
	}output1;
	struct {
		ascii logic[2];
		ascii delay[6];
	}output2;
	struct {
		ascii value[4];
		ascii delay[6];
	}overspeed;
	struct {
		ascii speed[4];
		ascii delay[6];
	}idling;
	
	ascii adminPhone[4][30];

}__arc_sh_apt_utils_t;

static __arc_sh_apt_utils_t ppp = {
	.sampling = {
		.park = "120", .drive = "60"
	},
	.output1 = {
		.logic = "1", .delay = "0"
	},
	.output2 = {
		.logic = "0", .delay = "0"
	},
	.overspeed = {
		.value = "100", .delay = "2"
	},
	.idling = {
		.speed = "7", .delay = "120"
	},
	.adminPhone = {"","","",""}
};

static char config_dir[6] = {""};
	

void arc_shell_apt_InitModule(const char dir[6]) {
	strcpy(config_dir,dir);
	if(adl_flhSubscribe(config_dir, 1)==OK)
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_apt_utils_t),(u8*)&ppp);
	else {
		if(adl_flhExist(config_dir,0)!= sizeof(__arc_sh_apt_utils_t)) {
			adl_flhWrite(config_dir,0,sizeof(__arc_sh_apt_utils_t),(u8*)&ppp);
		}else adl_flhRead(config_dir,0,sizeof(__arc_sh_apt_utils_t),(u8*)&ppp);
	}
}

void arc_shell_apt_SamplingHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;
	
	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0] && strBuff[1]) {
			if(wm_atoi(strBuff)<=6000)wm_strcpy (ppp.sampling.park, strBuff);
		}
		strBuff = ADL_GET_PARAM (Command, 1);
		if(strBuff && strBuff[0] && strBuff[1]) {
			if(wm_atoi(strBuff)<=6000)wm_strcpy (ppp.sampling.drive, strBuff);
		}
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_apt_utils_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.sampling.park);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.sampling.drive);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.sampling.park);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.sampling.drive);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}
void arc_shell_apt_IdlingModeHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;
	
	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) {
			if(wm_atoi(strBuff)<=999)wm_strcpy (ppp.idling.speed, strBuff);
		}
		strBuff = ADL_GET_PARAM (Command, 1);
		if(strBuff && strBuff[0]) {
			if(wm_atoi(strBuff)<=6000)wm_strcpy (ppp.idling.delay, strBuff);
		}
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_apt_utils_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.idling.speed);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.idling.delay);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.idling.speed);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.idling.delay);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}

void arc_shell_apt_OverSpeedHandler(adl_atCmdPreParser_t * Command){
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) {
			if(wm_atoi(strBuff)<=999)wm_strcpy (ppp.overspeed.value, strBuff);
		}
		strBuff = ADL_GET_PARAM (Command, 1);
		if(strBuff && strBuff[0]) {
			if(wm_atoi(strBuff)<=6000)wm_strcpy (ppp.overspeed.delay, strBuff);
		}
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_apt_utils_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.overspeed.value);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.overspeed.delay);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.overspeed.value);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.overspeed.delay);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);

}

static void ARC_SHELL_APT_SUPER_SMS(adl_atCmdPreParser_t * Command, u8 sui){
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.adminPhone[sui], strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_apt_utils_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.adminPhone[sui]);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.adminPhone[sui]);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}

void arc_shell_apt_AdminPhoneNb1Handler(adl_atCmdPreParser_t * Command){ARC_SHELL_APT_SUPER_SMS(Command, 0 );}
void arc_shell_apt_AdminPhoneNb2Handler(adl_atCmdPreParser_t * Command){ARC_SHELL_APT_SUPER_SMS(Command, 1 );}
void arc_shell_apt_AdminPhoneNb3Handler(adl_atCmdPreParser_t * Command){ARC_SHELL_APT_SUPER_SMS(Command, 2 );}
void arc_shell_apt_AdminPhoneNb4Handler(adl_atCmdPreParser_t * Command){ARC_SHELL_APT_SUPER_SMS(Command, 3 );}


void arc_shell_apt_Output1Handler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0])  {
			if(wm_atoi(strBuff)<2)wm_strcpy (ppp.output1.logic, strBuff);
			strBuff = ADL_GET_PARAM (Command, 1);
			if(strBuff && strBuff[0]) {
				if(wm_atoi(strBuff)<=6000)wm_strcpy (ppp.output1.delay, strBuff);
			}
		}
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_apt_utils_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.output1.logic);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.output1.delay);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.output1.logic);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.output1.delay);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


void arc_shell_apt_Output2Handler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
	case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0])  {
			if(wm_atoi(strBuff)<2)wm_strcpy (ppp.output2.logic, strBuff);
			strBuff = ADL_GET_PARAM (Command, 1);
			if(strBuff && strBuff[0]) {
				if(wm_atoi(strBuff)<=6000)wm_strcpy (ppp.output2.delay, strBuff);
			}
		}
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_apt_utils_t),(u8*)&ppp);
		break;
	case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.output2.logic);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.output2.delay);
		break;
	default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.output2.logic);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.output2.delay);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}
