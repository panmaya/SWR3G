/*
 * ini_config.c
 *
 *  Created on: 27 Á.¤. 2555
 *      Author: Pan
 */

#include "adl_global.h"
#include "ini_config.h"
#include "misc_config.h"
#include "feature.h"

static bool ini_config_ptrRef_configChanged = FALSE;

ini_config_net_t	ini_config_net = {	{"www.dtac.co.th","","",""},
										{SWD_SERVER_DEFAULT_IP,SWD_SERVER_DEFAULT_PORT},
										{"210.213.3.37","anonymous","1234","new.dwl"},
};


ini_config_utils_t	ini_config_utils = {
											{SWD_OVER_SPEED_DEFAULT,2},// OverSpeed
											{APT_PARK_SAMPLING_DEFAULT,APT_DRIVE_SAMPLING_DEFAULT},//Sampling Drive mode
											{0,2},//Input1
											{0,2},//Input2							
											{0,2},//Input3
											{0,2},//Input4
											{0,0},//Output1
											{0,0},//Output2
											{"","","",""},//SMS Server , User1-4, SMS-FeatureMask
											{FALSE,FALSE},//CtrlRequest
											{SWD_IDLE_SPEED_DEFAULT,120},//Idling speed < 7  , amount 120 seconds 
											{	"","","",FALSE,SWD_DEFAULT_EMAIL,"000",
												{SWD_HA_DEFAULT_VAL,SWD_HD_DEFAULT_VAL},{FALSE,0}/*HAHD Calibrate*/,0x00/*Option Mask*/,0x00/*VEH TYPE*/,0x00/*VEH_ATTR*/
											}
};

bool ini_config_isUpdate(void) {return ini_config_ptrRef_configChanged;}
void ini_config_setUpdate(void) { ini_config_ptrRef_configChanged  = TRUE; }

ascii* ini_config_strParameter(ascii* pStr, const u16 buffsize ,const ascii *cmd, u16 nb){
	ascii sDummy[200] = {"+RESP: "};
	wm_strcpy(sDummy+7, arc_shell_execute(cmd));
	if(!wm_strGetParameterString(pStr,buffsize,sDummy,nb))pStr[0] = 0;
	return pStr;
}

double ini_config_atof_strParameter(const ascii *cmd, u16 nb) {
	ascii sDummy[200] = {"+RESP: "};
	ascii pStr[64] = {""};
	wm_strcat(sDummy+7, arc_shell_execute(cmd));
	wm_strGetParameterString(pStr,sizeof(pStr),sDummy,nb);
	return atof(pStr);
}

u16 ini_config_atoi_strParameter(const ascii *cmd, u16 nb) {
	ascii sDummy[200] = {"+RESP: "};
	ascii pStr[64] = {""};
	wm_strcat(sDummy+7, arc_shell_execute(cmd));
	wm_strGetParameterString(pStr,sizeof(pStr),sDummy,nb);
	return wm_atoi(pStr);
}

u16 ini_config_atol_strParameter(const ascii *cmd, u16 nb) {
	ascii sDummy[200] = {"+RESP: "};
	ascii pStr[64] = {""};
	wm_strcat(sDummy+7, arc_shell_execute(cmd));
	wm_strGetParameterString(pStr,sizeof(pStr),sDummy,nb);
	return atol(pStr);
}

double ini_config_aDMS2i_strParameter( const ascii *cmd, u16 nb) {
	ascii sDummy[200] = {"+RESP: "};
	ascii pStr[64] = {""};
	s8    Signed = 1;
	double m_fraction = 0.0, s_fraction = 0.0;
	wm_strcat(sDummy+7, arc_shell_execute(cmd));
	wm_strGetParameterString(pStr,sizeof(pStr),sDummy,nb);
	switch(pStr[7]){
	case 'N':
	case 'E':
		Signed = 1;
		break;
	case 'S':
	case 'W':
		Signed = (-1);
		break;
	default:
		break;
	}
	pStr[7] = 0;
	s_fraction = (atoi(pStr+5)/3600.0);
	pStr[5] = 0;
	m_fraction = (atoi(pStr+3)/60.0);
	pStr[3] = 0;
	return ((atoi(pStr) + m_fraction + s_fraction)*Signed);
}

bool ini_config_loadRecursive( uint8_t state ) {
	ascii pStr[64] = {""};
	switch(state){
	case 0:
		wm_strcpy(ini_config_utils.DTCUniq.sCompany		,	arc_shell_execute(	"AT#COMP?"		));
		wm_strcpy(ini_config_utils.DTCUniq.sCarID		,	arc_shell_execute(	"AT#BOXID?"		));
		wm_strcpy(ini_config_utils.DTCUniq.sSIMNumb		,	arc_shell_execute(	"AT#SIMNUM?"	));
		wm_strcpy(ini_config_utils.DTCUniq.sEmail		,	arc_shell_execute(	"AT#EMAIL?"		));
		wm_strcpy(ini_config_utils.DTCUniq.sFwName		,	arc_shell_execute(	"AT#SWDFM?"		));
		wm_strcpy(ini_config_net.PPP.APN				,	arc_shell_execute(	"AT#APNSERV?"	));
		wm_strcpy(ini_config_net.PPP.User				,	arc_shell_execute(	"AT#APNUN?"		));
		wm_strcpy(ini_config_net.PPP.PassW				,	arc_shell_execute(	"AT#APNPW?"		));
		wm_strcpy(ini_config_net.PPP.SIMPin				,	arc_shell_execute(	"AT#CPIN?"		));
		wm_strcpy(ini_config_net.TCPServer1.IP			,	arc_shell_execute(	"AT#TCPSERV?"	));
		wm_strcpy(ini_config_utils.AdminPhone[0]		,	arc_shell_execute(	"AT#SMSUSER1?"	));
		wm_strcpy(ini_config_utils.AdminPhone[1]		,	arc_shell_execute(	"AT#SMSUSER2?"	));
		wm_strcpy(ini_config_utils.AdminPhone[2]		,	arc_shell_execute(	"AT#SMSUSER3?"	));
		wm_strcpy(ini_config_utils.AdminPhone[3]		,	arc_shell_execute(	"AT#SMSUSER4?"	));
		break;
	case 1:
		wm_strcpy(ini_config_net.DOTAServer.IP			,	ini_config_strParameter(pStr,sizeof(pStr),	"AT#DOTA?"	,2));
		wm_strcpy(ini_config_net.DOTAServer.User		,	ini_config_strParameter(pStr,sizeof(pStr),	"AT#DOTA?"	,3));
		wm_strcpy(ini_config_net.DOTAServer.PassW		,	ini_config_strParameter(pStr,sizeof(pStr),	"AT#DOTA?"	,4));
		wm_strcpy(ini_config_net.DOTAServer.FileN		,	ini_config_strParameter(pStr,sizeof(pStr),	"AT#DOTA?"	,5));
		ini_config_utils.CtrlRequest.bDota				=	ini_config_atoi_strParameter(	"AT#DOTA?"		,1);
		ini_config_utils.Sampling.DriveMode				=	ini_config_atoi_strParameter(	"AT#SAMPLING?"	,2);
		ini_config_utils.Idling.Value					=	ini_config_atoi_strParameter(	"AT#IDLESPEED?"	,1);
		ini_config_utils.Idling.Delay					=	ini_config_atoi_strParameter(	"AT#IDLESPEED?"	,2);
		ini_config_utils.OverSpeed.Value				=	ini_config_atoi_strParameter(	"AT#OVERSPEED?"	,1);
		ini_config_utils.OverSpeed.Delay				=	ini_config_atoi_strParameter(	"AT#OVERSPEED?"	,2);
		ini_config_net.TCPServer1.Port					=	wm_atoi(arc_shell_execute(		"AT#TCPPORT?"	));
		ini_config_utils.CtrlRequest.bHttpFile			=	wm_atoi(arc_shell_execute(		"AT#HTTPFILE?"	));
		ini_config_utils.DTCUniq.bAlertOversp			=	wm_atoi(arc_shell_execute(		"AT#OVERSPW?"	));
		break;
	case 2:
		ini_config_utils.Output1.Logic					=	ini_config_atoi_strParameter(	"AT#OUTPUT1?"  ,1);
		ini_config_utils.Output1.Delay					=	ini_config_atoi_strParameter(	"AT#OUTPUT1?"  ,2);
		ini_config_utils.Output2.Logic					=	ini_config_atoi_strParameter(	"AT#OUTPUT2?"  ,1);
		ini_config_utils.Output2.Delay					=	ini_config_atoi_strParameter(	"AT#OUTPUT2?"  ,2);
		ini_config_utils.DTCUniq.Harshing.HA			=	ini_config_atoi_strParameter(	"AT#SETHD?"		,1);
		ini_config_utils.DTCUniq.Harshing.HD			=	ini_config_atoi_strParameter(	"AT#SETHD?"		,2);
		ini_config_utils.DTCUniq.HAHDCalibrate.already	=	ini_config_atoi_strParameter(	"AT#HDCAL?"		,1);
		ini_config_utils.DTCUniq.HAHDCalibrate.calValue	=	ini_config_atof_strParameter(	"AT#HDCAL?"		,2);
		ini_config_utils.DTCUniq.optionmask				=	ini_config_atoi_strParameter(	"AT#SWDOPT?"	,1);
		ini_config_utils.DTCUniq.vehicle_type			=	ini_config_atoi_strParameter(	"AT#VEHTYPE?"	,1);
		ini_config_utils.DTCUniq.vehicle_attr			=	ini_config_atoi_strParameter(	"AT#VEHATTR?"	,1);
		break;
	default:
		ini_config_ptrRef_configChanged  = FALSE;
		return FALSE;
		break;
	}
	return TRUE;
}
