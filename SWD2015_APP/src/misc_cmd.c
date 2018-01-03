/*
 * misc_cmd.c
 *
 * Created: 7/6/2016 3:29:42 PM
 *  Author: Panmaya
 */ 

#include "adl_global.h"
#include "apt_tracking.h"
#include "apt_gateway.h"
#include "apt_swd.h"
#include "fat_system.h"
#include "apt_gps_data.h"
#include "asf.h"
#include "apt_afec.h"
#include "ini_config.h"
#include "buffer.h"
#include "vprinter.h"
#include "_lib_arcnethl/arc_hlgsm.h"
#include "_lib_arcnethl/arc_hltcp.h"
#include "apt_afec.h"
#include "apt_card.h"
#include "card_authen.h"
#include "../_lib_bootld/boot_trigger.h"
#include "misc_config.h"
#include "xm0110_compat.h"
#include "feature.h"
#include "apt_debug_port.h"
#include "misc_cmd.h"
#include "error.h"
#include "buffer_gpbm.h"

static bool misc_cmd_isCommandWdreset = false;

bool misc_cmd_wdreset(void) {
	return misc_cmd_isCommandWdreset;
}

static void misc_wdReset_cmdHandler (adl_atCmdPreParser_t * Params) {
	misc_cmd_isCommandWdreset = true;
}

static void misc_gpbmDelete_cmdHandler (adl_atCmdPreParser_t * Params) {
	buffer_gpbm_erase(GPBM_LICENSE_CARD_ADDR);
	buffer_gpbm_erase(GPBM_LICENSE_TRK3_1_ADDR);
	buffer_gpbm_erase(GPBM_LICENSE_TRK3_2_ADDR);
	buffer_gpbm_erase(GPBM_LICENSE_TRK3_3_ADDR);
	buffer_gpbm_erase(GPBM_LICENSE_TRK3_4_ADDR);
	buffer_gpbm_erase(GPBM_LICENSE_TRK3_5_ADDR);
	buffer_gpbm_erase(GPBM_LICENSE_TRK3_6_ADDR);
	buffer_gpbm_erase(GPBM_LICENSE_TRK3_7_ADDR);
	buffer_gpbm_erase(GPBM_LOGIN_LOGOUT_1_ADDR);
	buffer_gpbm_erase(GPBM_LOGIN_LOGOUT_2_ADDR);
	buffer_gpbm_erase(GPBM_LOGIN_LOGOUT_3_ADDR);
	buffer_gpbm_erase(GPBM_LOGIN_LOGOUT_4_ADDR);
	buffer_gpbm_erase(GPBM_LOGIN_LOGOUT_5_ADDR);
	puts("\r\n@GPBMDEL: OK\r\n");
}

static void misc_gpbmWrite_cmdHandler (adl_atCmdPreParser_t * Params) {
	char tmp[256] = {""};
	strcpy(tmp,"GPBM_LICENSE_CARD_ADDR = 08513707810");
	buffer_gpbm_write(GPBM_LICENSE_CARD_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LICENSE_TRK3_1_ADDR = 111111111111111111111111110");
	buffer_gpbm_write(GPBM_LICENSE_TRK3_1_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LICENSE_TRK3_2_ADDR = 2222222220");
	buffer_gpbm_write(GPBM_LICENSE_TRK3_2_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LICENSE_TRK3_3_ADDR = 3333333333333333330");
	buffer_gpbm_write(GPBM_LICENSE_TRK3_3_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LICENSE_TRK3_4_ADDR = 4444444444444444444440");
	buffer_gpbm_write(GPBM_LICENSE_TRK3_4_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LICENSE_TRK3_5_ADDR = 5555555555550");
	buffer_gpbm_write(GPBM_LICENSE_TRK3_5_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LICENSE_TRK3_6_ADDR = 66666666666666660");
	buffer_gpbm_write(GPBM_LICENSE_TRK3_6_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LICENSE_TRK3_7_ADDR = 77770");
	buffer_gpbm_write(GPBM_LICENSE_TRK3_7_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LOGIN_LOGOUT_1_ADDR = aaaaaaaaaaaaa0");
	buffer_gpbm_write(GPBM_LOGIN_LOGOUT_1_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LOGIN_LOGOUT_2_ADDR = bbbbbbbbbbbbbbbb0");
	buffer_gpbm_write(GPBM_LOGIN_LOGOUT_2_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LOGIN_LOGOUT_3_ADDR = cccccc0");
	buffer_gpbm_write(GPBM_LOGIN_LOGOUT_3_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LOGIN_LOGOUT_4_ADDR = ddddddddddddddddddddd0");
	buffer_gpbm_write(GPBM_LOGIN_LOGOUT_4_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	strcpy(tmp,"GPBM_LOGIN_LOGOUT_5_ADDR = ee0");
	buffer_gpbm_write(GPBM_LOGIN_LOGOUT_5_ADDR, (uint8_t*)tmp, strlen(tmp) + 1 );
	puts("\r\n@GPBMWRT: OK\r\n");
}

static void misc_gpbmInfo_cmdHandler (adl_atCmdPreParser_t * Params) {
	char tmp[256] = {""};
	uint32_t sn = 0;
	buffer_gpbm_read(GPBM_LICENSE_CARD_ADDR, (uint8_t*)tmp, 256 );
	puts("\r\n");
	puts(tmp);
	buffer_gpbm_read(GPBM_LICENSE_TRK3_1_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LICENSE_TRK3_1_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LICENSE_TRK3_2_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LICENSE_TRK3_2_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LICENSE_TRK3_3_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LICENSE_TRK3_3_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LICENSE_TRK3_4_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LICENSE_TRK3_4_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LICENSE_TRK3_5_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LICENSE_TRK3_5_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LICENSE_TRK3_6_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LICENSE_TRK3_6_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LICENSE_TRK3_7_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LICENSE_TRK3_7_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LOGIN_LOGOUT_1_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LOGIN_LOGOUT_1_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LOGIN_LOGOUT_2_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LOGIN_LOGOUT_2_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LOGIN_LOGOUT_3_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LOGIN_LOGOUT_3_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LOGIN_LOGOUT_4_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LOGIN_LOGOUT_4_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	buffer_gpbm_read(GPBM_LOGIN_LOGOUT_5_ADDR, (uint8_t*)tmp, 256 );
	sn = buffer_gpbm_getsn(GPBM_LOGIN_LOGOUT_5_ADDR);
	printf("\r\n%ld - %s",sn,tmp);
	puts("\r\n@GPBMINF: OK\r\n");
}

static void misc_pwrReset_cmdHandler (adl_atCmdPreParser_t * Params) {
	puts("\r\n@PWRST: OK\r\n");
	vTaskDelay(100);
	ioport_set_pin_dir(PIN_EN_5VPDOWN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EN_5VPDOWN, IOPORT_PIN_LEVEL_HIGH);
}

static void misc_fdisk_cmdHandler(adl_atCmdPreParser_t * Params) {
	puts("\r\n@FDISK: OK\r\n");
	gpbr_write(GPBR_FDISK_ID,1);//Format disk
	gpbr_write(GPBR_DFRAG_ID,0);//Don't Clear FRAM
	gpbr_write(GPBR_CLRBF_ID,0);//Don't Clear Buffer
	gpbr_write(GPBR_RECFG_ID,0);//Don't Re Config
	gpbr_write(GPBR_RESET_ID,0);//Reset Now
	software_reset();
}

static void misc_dfrag_cmdHandler(adl_atCmdPreParser_t * Params) {
	puts("\r\n@DFRAG: OK\r\n");
	gpbr_write(GPBR_FDISK_ID,1);//Format disk
	gpbr_write(GPBR_DFRAG_ID,0);//Don't Clear FRAM
	gpbr_write(GPBR_CLRBF_ID,1);//Clear Buffer
	gpbr_write(GPBR_RECFG_ID,1);//Re Config
	gpbr_write(GPBR_RESET_ID,1);//Reset When IG ON
}

static void misc_clrbf_cmdHandler(adl_atCmdPreParser_t * Params) {
	puts("\r\n@CLRBF: OK\r\n");
	gpbr_write(GPBR_FDISK_ID,0);//Don't Format disk
	gpbr_write(GPBR_DFRAG_ID,1);//Clear FRAM
	gpbr_write(GPBR_CLRBF_ID,1);//Clear Buffer
	gpbr_write(GPBR_RECFG_ID,1);//Re Config
	gpbr_write(GPBR_RESET_ID,1);//Reset When IG ON
}

static void misc_recfg_cmdHandler(adl_atCmdPreParser_t * Params) {
	puts("\r\n@RECFG: OK\r\n");
	gpbr_write(GPBR_FDISK_ID,0);//Don't Format disk
	gpbr_write(GPBR_DFRAG_ID,0);//Don't Clear FRAM
	gpbr_write(GPBR_CLRBF_ID,0);//Don't Clear Buffer
	gpbr_write(GPBR_RECFG_ID,1);//Re Config
	gpbr_write(GPBR_RESET_ID,1);//Reset When IG ON
}


static void misc_versionGet_cmdHandler(adl_atCmdPreParser_t * Params) {
	printf("\r\n@VERSION: %03d\r\nOK\r\n",SWD2015_FW_VERSION);
}

static void misc_vPrint_cmdHandler (adl_atCmdPreParser_t * Params) {
	uint8_t printlev = 0;
	uint8_t onoff = 0;
	switch(Params->Type) {
		case ADL_CMD_TYPE_PARA:
		printlev = atoi(ADL_GET_PARAM(Params,0)) ;
		onoff = atoi(ADL_GET_PARAM(Params,1));
		if(onoff == 1)printTraceOn = AUTO_TRACEON_CLOSE_TIME;
		else if (onoff == 2)printTraceOn = 0;//Alway Debug
		vPrintTraceSet (printlev,onoff);
		break;
		default:
		break;
	}
	puts("\r\n@VPRINT: OK\r\n");
}

static void misc_DLTVehInit_cmdHandler (adl_atCmdPreParser_t * Params) {
	u8 temp = 0x00;
	ascii StingCmd[20] = {""};
	switch(Params->Type) {
		case ADL_CMD_TYPE_PARA:
		ini_config_utils.DTCUniq.vehicle_type = ( atol(ADL_GET_PARAM(Params,0)) );
		temp = (atoi(ADL_GET_PARAM(Params,1)) & 0x01);
		temp = (temp << 1);
		temp |= (atoi(ADL_GET_PARAM(Params,2)) & 0x01);
		ini_config_utils.DTCUniq.vehicle_attr &= (~0x03);
		ini_config_utils.DTCUniq.vehicle_attr |= temp;
		wm_sprintf(StingCmd,"AT#VEHTYPE=\"%d\"",ini_config_utils.DTCUniq.vehicle_type);
		arc_shell_execute( StingCmd);
		wm_sprintf(StingCmd,"AT#VEHATTR=\"%d\"",ini_config_utils.DTCUniq.vehicle_attr);
		arc_shell_execute( StingCmd);
		default:
		break;
	}
	printf("\r\n@VHT: %04d %d, %d\r\nOK\r\n",ini_config_utils.DTCUniq.vehicle_type,
	(ini_config_utils.DTCUniq.vehicle_attr & 0x02) ? 1 : 0,
	(ini_config_utils.DTCUniq.vehicle_attr & 0x01) ? 1 : 0);
}

static void misc_DLTCardInit_cmdHandler (adl_atCmdPreParser_t * Params) {
	u8 temp = 0x00;
	ascii StingCmd[20] = {""};
	switch(Params->Type) {
		case ADL_CMD_TYPE_PARA:
		temp = ( atol(ADL_GET_PARAM(Params,0)) );
		ini_config_utils.DTCUniq.optionmask &= (~0xE0);
		ini_config_utils.DTCUniq.optionmask |= (temp << 6);
		wm_sprintf(StingCmd,"AT#SWDOPT=\"%d\"",ini_config_utils.DTCUniq.optionmask);
		arc_shell_execute( StingCmd);
		default:
		break;
	}
	temp = ((ini_config_utils.DTCUniq.optionmask & 0xE0)>>6) & 0x03;
	printf("\r\n@DLT: %02d (%s)\r\nOK\r\n",temp,temp==0?"NOT USED":temp==1?"DLT SPEC":temp==2?"NORMAL":"UN-DEFINED");
}


void misc_cmd_Subscribe(void) {
	adl_atCmdSubscribe("AT@VERSION", misc_versionGet_cmdHandler, ADL_CMD_TYPE_ACT|0x00);
	adl_atCmdSubscribe("AT@WDRST", misc_wdReset_cmdHandler, ADL_CMD_TYPE_ACT|0x00);
	adl_atCmdSubscribe("AT@PWRST", misc_pwrReset_cmdHandler, ADL_CMD_TYPE_ACT|0x00);
	adl_atCmdSubscribe("AT@FDISK", misc_fdisk_cmdHandler, ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT|0x00);
	adl_atCmdSubscribe("AT@DFRAG", misc_dfrag_cmdHandler, ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT|0x00);
	adl_atCmdSubscribe("AT@CLRBF", misc_clrbf_cmdHandler, ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT|0x00);
	adl_atCmdSubscribe("AT@RECFG", misc_recfg_cmdHandler, ADL_CMD_TYPE_READ|ADL_CMD_TYPE_ACT|0x00);
	adl_atCmdSubscribe("AT@VPRINT", misc_vPrint_cmdHandler, ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST|0x22);
	adl_atCmdSubscribe("AT@VHT", misc_DLTVehInit_cmdHandler, ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST|0x33);
	adl_atCmdSubscribe("AT@DLT", misc_DLTCardInit_cmdHandler, ADL_CMD_TYPE_PARA|ADL_CMD_TYPE_READ|ADL_CMD_TYPE_TEST|0x11);
	adl_atCmdSubscribe("AT@GPBMINF", misc_gpbmInfo_cmdHandler, ADL_CMD_TYPE_ACT|0x00);
	adl_atCmdSubscribe("AT@GPBMWRT", misc_gpbmWrite_cmdHandler, ADL_CMD_TYPE_ACT|0x00);
	adl_atCmdSubscribe("AT@GPBMDEL", misc_gpbmDelete_cmdHandler, ADL_CMD_TYPE_ACT|0x00);
}