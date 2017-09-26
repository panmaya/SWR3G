/*
 * CFile1.c
 *
 * Created: 3/26/2015 3:25:53 PM
 *  Author: Pan
 */ 
/*
 * CFile1.c
 *
 * Created: 3/25/2015 3:30:32 PM
 *  Author: Pan
 */ 

#include "adl_global.h"
#include "arc_sh_swd.h"
#include "common/arc_shell_common.h"
#include "apt_tracking.h"
#include "card_authen.h"
#include "apt_gateway.h"
#include "trans_form.h"
#include "misc_config.h"
#include "apt_swd.h"
#include "swd_event.h"

typedef struct __ARC_SH_SWD_T_ {

	ascii Company[20];
	ascii box_id[16];
	ascii phone[20];
	ascii point[2];
	ascii overspeed_warn[2];
	ascii email[32];

	ascii hd_value[5];
	ascii ha_value[5];
	ascii reserve1[5];
	ascii reserve2[5];
	ascii sFwName[4];//max = "999"
	
	struct {
		ascii is_saved[2];
		ascii value[13];
	}hdCal;
	
	ascii swdoption[4];//max = "255"
	
	struct {
		ascii s_type[5];//max = "xxxx"
		ascii s_attr[4];//max = "255"
	}vehicle;
}__arc_sh_swd_t;

static 	__arc_sh_swd_t ppp = {"","","","1","0","mail@noconfig.com","25","25","","","000",{"0","0"},"0",{"0","0"}};
static char config_dir[6] = {""};

void arc_shell_swd_InitModule(const char dir[6]) {
	strcpy(config_dir,dir);
	if(adl_flhSubscribe(config_dir, 1)==OK)
	adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
	else {
		if(adl_flhExist(config_dir,0)!= sizeof(__arc_sh_swd_t)) {
			adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		}else adl_flhRead(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
	}
}


void arc_shell_swd_CompanyGetHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.Company, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.Company);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.Company);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


void arc_shell_swd_BoxidGetHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.box_id, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.box_id);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.box_id);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


void arc_shell_swd_SimnumGetHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.phone, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.phone);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.phone);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


void arc_shell_swd_PointGetHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.point, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.point);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.point);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


void arc_shell_swd_OverspwGetHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.overspeed_warn, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.overspeed_warn);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.overspeed_warn);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


void arc_shell_swd_EmailGetHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.email, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.email);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.email);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}

void arc_shell_swd_HDHAHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;
	ascii * strBuff2 = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		strBuff2 = ADL_GET_PARAM (Command, 1);
		if((strBuff && strBuff[0])&&(strBuff2 && strBuff2[0])){
			 wm_strcpy (ppp.hd_value, strBuff);
			 wm_strcpy (ppp.ha_value, strBuff2);
		}
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.hd_value);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.ha_value);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.hd_value);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.ha_value);
		
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


void arc_shell_swd_HDHACalHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;
	ascii * strBuff2 = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		strBuff2 = ADL_GET_PARAM (Command, 1);
		if((strBuff && strBuff[0])&&(strBuff2 && strBuff2[0])){
			wm_strcpy (ppp.hdCal.is_saved, strBuff);
			wm_strcpy (ppp.hdCal.value, strBuff2);
		}
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.hdCal.is_saved);
		wm_strcat(arc_shell_common_rspBuffer,",");
		wm_strcat(arc_shell_common_rspBuffer,ppp.hdCal.value);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.hdCal.is_saved);
		wm_strcat(Command->Contxt,",");
		wm_strcat(Command->Contxt,ppp.hdCal.value);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}

void arc_shell_swd_FeatureHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.sFwName, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.sFwName);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.sFwName);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


void arc_shell_swd_OptionHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.swdoption, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.swdoption);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.swdoption);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


void arc_shell_swd_VehTypeHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.vehicle.s_type, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.vehicle.s_type);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.vehicle.s_type);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}

void arc_shell_swd_VehAttrHandler(adl_atCmdPreParser_t * Command) {
	strcpy(arc_shell_common_rspBuffer,"\r\n");
	ascii * strBuff = NULL;

	arc_shell_commonToUpper(arc_shell_common_rspBuffer+2,Command->StrData);

	switch(Command->Type) {
		case ADL_CMD_TYPE_PARA:
		strBuff = ADL_GET_PARAM (Command, 0);
		if(strBuff && strBuff[0]) wm_strcpy (ppp.vehicle.s_attr, strBuff);
		adl_flhWrite(config_dir,0,sizeof(__arc_sh_swd_t),(u8*)&ppp);
		break;
		case ADL_CMD_TYPE_ACT:
		case ADL_CMD_TYPE_TEST:
		case ADL_CMD_TYPE_READ:
		wm_strcat(arc_shell_common_rspBuffer,ppp.vehicle.s_attr);
		break;
		default:
		break;
	}
	if(Command->Port == ADL_PORT_NONE) {
		wm_strcpy(Command->Contxt,ppp.vehicle.s_attr);
		return;
	}
	wm_strcat(arc_shell_common_rspBuffer,"\r\n\r\nOK\r\n");
	adl_atSendResponse(ADL_AT_PORT_TYPE(Command->Port,ADL_AT_RSP),arc_shell_common_rspBuffer);
	arc_shell_commonPost(NULL);
}


