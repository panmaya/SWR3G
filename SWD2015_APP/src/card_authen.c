/*
 * card_authen.c
 *
 * Created: 2/2/2559 10:14:22
 *  Author: Pan
 */ 

#include "adl_global.h"
#include "swd_event.h"
#include "apt_tracking.h"
//#include "buffer.h"
#include "ini_config.h"
#include "apt_gateway.h"
#include "misc_config.h"
#include "apt_swd.h"
#include "apt_tracking.h"
#include "vprinter.h"
#include "buffer_Q512Flash.h"
#include "_lib_arcshell/arc_sh_swd.h"
#include "apt_hahd_gps.h"
#include "hdha_odo.h"
#include "card_authen.h"
#include "trans_form.h"
#include "swd_io.h"
#include "buffer_gpbm.h"

#define CARD_AUTHEN_LICENSE_STR_LEN		60

#define CARD_AUTHEN_BUFFER_SIZE			350

#define CARD_AUTHEN_TRACK3_SIZE			GPBM_LICENSE_TRK3_SIZE

#define CARD_AUTHEN_IN					0
#define CARD_AUTHEN_OUT					1


#define CARD_AUTHEN_TRACK3_MAX_ID		GPBM_LICENSE_TRK3_MAX_ID
#define CARD_AUTHEN_LOGING_MAX_ID		GPBM_LOGIN_LOGOUT_MAX_ID

#define CARD_AUTHEN_FORWARD				0
#define CARD_AUTHEN_UNFORWARD			1

static ascii CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_OUT+1][CARD_AUTHEN_BUFFER_SIZE] = {"",""};
static uint8_t  CARD_AUTHEN_hInit = 0;
static int8_t CARD_AUTHEN_T3Tabid = -1;
static int8_t CARD_AUTHEN_SndIndex = -1;
static char CARD_AUTHEN_LoginLogoutBuffer[512];

	
////////////////////////////////////////////////////////////////////////////////////////////////
enum {
	BUS_1_PRIVATE    =  1,  BUS_1_BUSINESS    =  2,
	BUS_2_PRIVATE    =  3,  BUS_2_BUSINESS    =  4,
	BUS_3_PRIVATE    =  5,  BUS_3_BUSINESS    =  6,
	BUS_4_PRIVATE    =  7,  BUS_4_BUSINESS    =  8,
	BUS_5_PRIVATE    =  9,  BUS_5_BUSINESS    = 10,
	BUS_6_PRIVATE    = 11,  BUS_6_BUSINESS    = 12,
	BUS_7_PRIVATE    = 13,  BUS_7_BUSINESS    = 14,
	BUS_UNDEFINED    =  0,
	
	TRUCK_1_PRIVATE  = 15,  TRUCK_1_BUSINESS  = 16,
	TRUCK_2_PRIVATE  = 17,  TRUCK_2_BUSINESS  = 18,
	TRUCK_3_PRIVATE  = 19,  TRUCK_3_BUSINESS  = 20,
	TRUCK_4_PRIVATE  = 21,  TRUCK_4_BUSINESS  = 22,
	TRUCK_5_PRIVATE  = 23,  TRUCK_5_BUSINESS  = 24,
	TRUCK_6_PRIVATE  = 25,  TRUCK_6_BUSINESS  = 26,
	TRUCK_7_PRIVATE  = 27,  TRUCK_7_BUSINESS  = 28,
	TRUCK_8_PRIVATE  = 29,  TRUCK_8_BUSINESS  = 30,
	TRUCK_9_PRIVATE  = 31,  TRUCK_9_BUSINESS  = 32,
	TRUCK_UNDEFINED  =  0,
};
	
/* == VEHICLE TYPE DECISION TABLE == */
static const int BUS_TYPE_DECISION_TABLE[8][4] =
{
	/* BUS'S USAGE TYPE */
	/* BUS'S CONDITION                 |    0: UNDEFINED              |  1: PRIVATE               |  2: NON-REGULAR ROUTE    |  3: REGULAR ROUTE */
	/* CONDITION #UNDEFINED*/ {  BUS_UNDEFINED,       BUS_UNDEFINED,      BUS_UNDEFINED ,         BUS_UNDEFINED  },
	/* CONDITION #1*/         {  BUS_UNDEFINED,       BUS_1_PRIVATE,      BUS_1_BUSINESS,         BUS_1_BUSINESS },
	/* CONDITION #2*/         {  BUS_UNDEFINED,       BUS_2_PRIVATE,      BUS_2_BUSINESS,         BUS_2_BUSINESS },
	/* CONDITION #3*/         {  BUS_UNDEFINED,       BUS_3_PRIVATE,      BUS_3_BUSINESS,         BUS_3_BUSINESS },
	/* CONDITION #4*/         {  BUS_UNDEFINED,       BUS_4_PRIVATE,      BUS_4_BUSINESS,         BUS_4_BUSINESS },
	/* CONDITION #5*/         {  BUS_UNDEFINED,       BUS_5_PRIVATE,      BUS_5_BUSINESS,         BUS_5_BUSINESS },
	/* CONDITION #6*/         {  BUS_UNDEFINED,       BUS_6_PRIVATE,      BUS_6_BUSINESS,         BUS_6_BUSINESS },
	/* CONDITION #7*/         {  BUS_UNDEFINED,       BUS_7_PRIVATE,      BUS_7_BUSINESS,         BUS_7_BUSINESS }
};
static const int TRUCK_TYPE_DECISION_TABLE[10][4] =
{
	/* TRUCK'S USAGE TYPE */
	/* TRUCK'S CONDITION              |  0: UNDEFINED                 |  1: PRIVATE               |  2: NON-REGULAR ROUTE    |  3: REGULAR ROUTE */
	/* CONDITION #UNDEFINED*/ {  TRUCK_UNDEFINED,     TRUCK_UNDEFINED,    TRUCK_UNDEFINED ,       TRUCK_UNDEFINED  },
	/* CONDITION #1*/         {  TRUCK_UNDEFINED,     TRUCK_1_PRIVATE,    TRUCK_1_BUSINESS,       TRUCK_UNDEFINED  },
	/* CONDITION #2*/         {  TRUCK_UNDEFINED,     TRUCK_2_PRIVATE,    TRUCK_2_BUSINESS,       TRUCK_UNDEFINED  },
	/* CONDITION #3*/         {  TRUCK_UNDEFINED,     TRUCK_3_PRIVATE,    TRUCK_3_BUSINESS,       TRUCK_UNDEFINED  },
	/* CONDITION #4*/         {  TRUCK_UNDEFINED,     TRUCK_4_PRIVATE,    TRUCK_4_BUSINESS,       TRUCK_UNDEFINED  },
	/* CONDITION #5*/         {  TRUCK_UNDEFINED,     TRUCK_5_PRIVATE,    TRUCK_5_BUSINESS,       TRUCK_UNDEFINED  },
	/* CONDITION #6*/         {  TRUCK_UNDEFINED,     TRUCK_6_PRIVATE,    TRUCK_6_BUSINESS,       TRUCK_UNDEFINED  },
	/* CONDITION #7*/         {  TRUCK_UNDEFINED,     TRUCK_7_PRIVATE,    TRUCK_7_BUSINESS,       TRUCK_UNDEFINED  },
	/* CONDITION #8*/         {  TRUCK_UNDEFINED,     TRUCK_8_PRIVATE,    TRUCK_8_BUSINESS,       TRUCK_UNDEFINED  },
	/* CONDITION #9*/         {  TRUCK_UNDEFINED,     TRUCK_9_PRIVATE,    TRUCK_9_BUSINESS,       TRUCK_UNDEFINED  }
};
/* == DRIVING LICENSE CARD AUTHORIZATION DECISION TABLE == */
/* [ 0: NOT ALLOWED TO AUTHORIZE , 1: ALLOWED TO AUTHORIZE , 2: ALLOWED TO AUTHORIZE(CONDITIONAL AUTHORIZE) ] */
static const int AUTH_DECISION_TABLE[33][8] =
{
	/* DRIVING LICENSE CARD TYPE (DLT_CODE) */
	/* VEHICLE TYPE: BUS      | 11 | 12 | 13 | 14 | 21 | 22 | 23 | 24 */
	/* UNDEFINED TYPE   */    {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* BUS #1 [PRIVATE] */    {  2,   1,   1,   1,   2,   1,   1,   1  },
	/* BUS #1 [BUSINESS]*/    {  0,   0,   0,   0,   2,   1,   1,   1  },
	/* BUS #2 [PRIVATE] */    {  2,   1,   1,   1,   2,   1,   1,   1  },
	/* BUS #2 [BUSINESS]*/    {  0,   0,   0,   0,   2,   1,   1,   1  },
	/* BUS #3 [PRIVATE] */    {  2,   1,   1,   1,   2,   1,   1,   1  },
	/* BUS #3 [BUSINESS]*/    {  0,   0,   0,   0,   2,   1,   1,   1  },
	/* BUS #4 [PRIVATE] */    {  0,   1,   1,   1,   0,   1,   1,   1  },
	/* BUS #4 [BUSINESS]*/    {  0,   0,   0,   0,   0,   1,   1,   1  },
	/* BUS #5 [PRIVATE] */    {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* BUS #5 [BUSINESS]*/    {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* BUS #6 [PRIVATE] */    {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* BUS #6 [BUSINESS]*/    {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* BUS #7 [PRIVATE] */    {  2,   1,   1,   1,   2,   1,   1,   1  },
	/* BUS #7 [BUSINESS]*/    {  0,   0,   0,   0,   2,   1,   1,   1  },
		
	/* DRIVING LICENSE CARD TYPE (DLT_CODE) */
	/* VEHICLE TYPE: TRUCK    | 11 | 12 | 13 | 14 | 21 | 22 | 23 | 24 */
	/* TRUCK #1 [PRIVATE] */  {  0,   1,   1,   1,   0,   1,   1,   1  },
	/* TRUCK #1 [BUSINESS]*/  {  0,   0,   0,   0,   0,   1,   1,   1  },
	/* TRUCK #2 [PRIVATE] */  {  0,   1,   1,   1,   0,   1,   1,   1  },
	/* TRUCK #2 [BUSINESS]*/  {  0,   0,   0,   0,   0,   1,   1,   1  },
	/* TRUCK #3 [PRIVATE] */  {  0,   1,   1,   1,   0,   1,   1,   1  },
	/* TRUCK #3 [BUSINESS]*/  {  0,   0,   0,   0,   0,   1,   1,   1  },
	/* TRUCK #4 [PRIVATE] */  {  0,   0,   0,   1,   0,   0,   0,   1  },
	/* TRUCK #4 [BUSINESS]*/  {  0,   0,   0,   0,   0,   0,   0,   1  },
	/* TRUCK #5 [PRIVATE] */  {  0,   1,   1,   1,   0,   1,   1,   1  },
	/* TRUCK #5 [BUSINESS]*/  {  0,   0,   0,   0,   0,   1,   1,   1  },
	/* TRUCK #6 [PRIVATE] */  {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* TRUCK #6 [BUSINESS]*/  {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* TRUCK #7 [PRIVATE] */  {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* TRUCK #7 [BUSINESS]*/  {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* TRUCK #8 [PRIVATE] */  {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* TRUCK #8 [BUSINESS]*/  {  0,   0,   0,   0,   0,   0,   0,   0  },
	/* TRUCK #9 [PRIVATE] */  {  0,   0,   1,   1,   0,   0,   1,   1  },
	/* TRUCK #9 [BUSINESS]*/  {  0,   0,   0,   0,   0,   0,   1,   1  }
};
	
static char const LICENSE_CODE_STR [8][5] =
{
	" 11 ",
	" 21 ",
	" 12 ",
	" 22 ",
	" 13 ",
	" 23 ",
	" 14 ",
	" 24 ",
};
	
///////////////////////////////////////////////////////////////////////////////////////////////

static void CARD_AUTHEN_SaveDriverStatus(ascii * scard, bool isLogin) {
	bool isNewDriver = true;

	for(uint8_t i = 0 ; i < CARD_AUTHEN_TRACK3_MAX_ID ; i++) {
		if(aptTracking_Driver[i].id[0]) {
			if(!strncmp(aptTracking_Driver[i].id,scard,strlen(aptTracking_Driver[i].id))) {
			//if(!strcmp(aptTracking_Driver[i].id,scard)) {
				aptTracking_Driver[i].isLogin = isLogin;
				isNewDriver = false;
				break;
			}
		}
	}
	
	if(isNewDriver && isLogin) {
		vPuts("\r\nNew User Login\r\n");
		for(uint8_t i = 0 ; i < CARD_AUTHEN_TRACK3_MAX_ID ; i++) {
			if(!aptTracking_Driver[i].id[0]) {
				vPuts("\r\nSave New Driver\r\n");
				snprintf(aptTracking_Driver[i].id,APT_TRACKING_DRIVER_ID_LEN,"%s",scard);
				aptTracking_Driver[i].isLogin = true;
				break;
			}
		}
	}
}

static ascii * CARD_AUTHEN_REMOVESP_4Track3 ( ascii * dst, ascii * src, u16 size ) {
	bool comma_inserted = false;
	bool char_inserted = false;
	for(;size && *src;size--) {
		if(*src!=0x20) {
			if(comma_inserted) {
				*dst = ',';
				dst++;
			}
			*dst = *src;
			dst++;
			char_inserted = true;
			comma_inserted = false;
			
		}else if(char_inserted) comma_inserted = true;
		src++;
	}
	*dst = 0;
	return dst;
}

static void CARD_AUTHEN_Display_2Serial(char * DRIV_LICENSE_CARD_INFO, bool isLogin, bool CardOK) {
	const char QUESTION_MARK_STR[2] = "?";
	char * strToken1 = NULL;
	char * strToken2 = NULL;
	char * strToken3 = NULL;
	uint8_t TABID = 0;
	bool DRIV_CARD_TYPE_GET = FALSE;
	ascii sTempLicense[CARD_AUTHEN_BUFFER_SIZE] = {""};

	vPuts("\r\n====================================================\r\n");
	if(isLogin) vPrintf("\r\n$DLT> LOGIN  = CARD TYPE %s",CardOK?"CORRECT":"INCORRECT");
	else vPuts("\r\n$DLT> LOGOUT");
	
	/* Copy data to temp buffer for protect no want data changed by strtok*/
	strcpy(sTempLicense,DRIV_LICENSE_CARD_INFO);
	/* get the first token */
	strToken1 = strtok(sTempLicense, QUESTION_MARK_STR);
	vPrintf("\r\n$DLT> TRACK1 = %s",strToken1);
	if(strToken1) { strToken2 = strtok(NULL, QUESTION_MARK_STR); }
	vPrintf("\r\n$DLT> TRACK2 = %s",strToken2);
	if(strToken2) { strToken3 = strtok(NULL, QUESTION_MARK_STR); }
	lvPrintf("\r\n$DLT> TRACK3 = %s",strToken3);
	if(strToken3) {
		strToken3 += 1;
		for(;TABID < 8;TABID++) {
			if(strstr(strToken3,LICENSE_CODE_STR[TABID])) {
				DRIV_CARD_TYPE_GET = TRUE;
				break;
			}
		}
	}
	if(DRIV_CARD_TYPE_GET) {
		switch(TABID) {
		case 0: { vPuts("\r\n$DLT> TYPE   = 11"); }break;//º1
		case 1: { vPuts("\r\n$DLT> TYPE   = 21"); }break;//·1
		case 2: { vPuts("\r\n$DLT> TYPE   = 12"); }break;//º2
		case 3: { vPuts("\r\n$DLT> TYPE   = 22"); }break;//·2
		case 4: { vPuts("\r\n$DLT> TYPE   = 13"); }break;//º3
		case 5: { vPuts("\r\n$DLT> TYPE   = 23"); }break;//·3
		case 6: { vPuts("\r\n$DLT> TYPE   = 14"); }break;//º4
		case 7: { vPuts("\r\n$DLT> TYPE   = 24"); }break;//·4
		default:{ vPuts("\r\n$DLT> TYPE   = --"); }break;
		}
	}else vPuts("\r\n$DLT> TYPE   = UNKNOW");
	vPrintf("\r\n$DLT> SPEC.  = %04d , Carry %s Peoples , Weight %s Kg.",ini_config_utils.DTCUniq.vehicle_type,
	(ini_config_utils.DTCUniq.vehicle_attr & 0x02) ? "<=20" : ">20",
	(ini_config_utils.DTCUniq.vehicle_attr & 0x01) ? "<=3500" : ">3500");
	vPuts("\r\n====================================================\r\n");
}

static bool CARD_AUTHEN_Verify_License(	char * DRIV_LICENSE_CARD_INFO,uint16_t VEH_TYPE,uint8_t VEH_WEIGHT_NUM_PASSENGER ){
	bool AUTH_DECISION_RESULT = FALSE;
	int DRIV_LICENSE_CARD_TYPE = 0, DRIV_AUTH_DECISION = 0;
	bool DRIV_CARD_TYPE_GET = FALSE;
	const char QUESTION_MARK_STR[2] = "?";
	char * strToken1 = NULL;
	char * strToken2 = NULL;
	char * strToken3 = NULL;
	uint8_t TABID = 0;
	int VEH_TYPE_DIGIT_1 , VEH_TYPE_DIGIT_2 , VEH_TYPE_DIGIT_3 , DECIDED_VEH_TYPE = 0;
	ascii sTempLicense[CARD_AUTHEN_BUFFER_SIZE] = {""};
	
	VEH_TYPE_DIGIT_1 = (VEH_TYPE/1000);//STR_VEH_TYPE[0] - '0',// 1 = BUS , 2 = TRUCK
	VEH_TYPE_DIGIT_2 = (VEH_TYPE%1000)/100;//STR_VEH_TYPE[1] - '0',// STANDARD TYPE
	VEH_TYPE_DIGIT_3 = ((VEH_TYPE%1000)%100)/10;//STR_VEH_TYPE[2] - '0',// USAGE TYPE
	
	/* Copy data to temp buffer for protect no want data changed by strtok*/
	strcpy(sTempLicense,DRIV_LICENSE_CARD_INFO);
	/* get the first token */
	strToken1 = strtok(sTempLicense, QUESTION_MARK_STR);
	/* walk through other tokens */
	if(strToken1) { strToken2 = strtok(NULL, QUESTION_MARK_STR); }
	if(strToken2) { strToken3 = strtok(NULL, QUESTION_MARK_STR); }
	if(strToken3) { 
		strToken3 += 1;
		for(;TABID < 8;TABID++) {
			if(strstr(strToken3,LICENSE_CODE_STR[TABID])) {
				DRIV_CARD_TYPE_GET = TRUE;
				break;
			}
		}
	}
	
	if(DRIV_CARD_TYPE_GET) {
		switch(TABID) {
		case 0: { DRIV_LICENSE_CARD_TYPE = 0; vPutsTrace(0,"DRIV. LICENSE ID TYPE = 11"); }break;//º1
		case 1: { DRIV_LICENSE_CARD_TYPE = 4; vPutsTrace(0,"DRIV. LICENSE ID TYPE = 21"); }break;//·1
		case 2: { DRIV_LICENSE_CARD_TYPE = 1; vPutsTrace(0,"DRIV. LICENSE ID TYPE = 12"); }break;//º2
		case 3: { DRIV_LICENSE_CARD_TYPE = 5; vPutsTrace(0,"DRIV. LICENSE ID TYPE = 22"); }break;//·2
		case 4: { DRIV_LICENSE_CARD_TYPE = 2; vPutsTrace(0,"DRIV. LICENSE ID TYPE = 13"); }break;//º3
		case 5: { DRIV_LICENSE_CARD_TYPE = 6; vPutsTrace(0,"DRIV. LICENSE ID TYPE = 23"); }break;//·3
		case 6: { DRIV_LICENSE_CARD_TYPE = 3; vPutsTrace(0,"DRIV. LICENSE ID TYPE = 14"); }break;//º4
		case 7: { DRIV_LICENSE_CARD_TYPE = 7; vPutsTrace(0,"DRIV. LICENSE ID TYPE = 24"); }break;//·4
		default:{                             vPutsTrace(0,"DRIV. LICENSE ID TYPE = UNKNOW"); return (VEH_TYPE==0) ? TRUE :FALSE; }break;
		}
		
		if (VEH_TYPE_DIGIT_1 == 1) DECIDED_VEH_TYPE = BUS_TYPE_DECISION_TABLE [VEH_TYPE_DIGIT_2][VEH_TYPE_DIGIT_3];
		else if (VEH_TYPE_DIGIT_1 == 2) DECIDED_VEH_TYPE = TRUCK_TYPE_DECISION_TABLE [VEH_TYPE_DIGIT_2][VEH_TYPE_DIGIT_3];
		
		DRIV_AUTH_DECISION = AUTH_DECISION_TABLE [DECIDED_VEH_TYPE][DRIV_LICENSE_CARD_TYPE];
		
		switch (DRIV_AUTH_DECISION) {
		case 1: AUTH_DECISION_RESULT = TRUE; break;	
		case 2: if (DRIV_LICENSE_CARD_TYPE == 0 || DRIV_LICENSE_CARD_TYPE == 4) AUTH_DECISION_RESULT = (VEH_WEIGHT_NUM_PASSENGER == 0x03) ? TRUE : FALSE ; break;
		default: break;
		}
	}else vPutsTrace(30,"DRIV. LICENSE UNKNOW");

	return (VEH_TYPE==0) ? TRUE :AUTH_DECISION_RESULT;
}

static s8 CARD_AUTHEN_Save_DriverID(ascii *drive_license) {
	s32 retVal;
	char * strToken1 = NULL;
	char * strToken2 = NULL;
	char * strToken3 = NULL;
	ascii sTempLicense[CARD_AUTHEN_BUFFER_SIZE] = {""};
	const char QUESTION_MARK_STR[2] = "?";
	
	/* Copy data to temp buffer for protect no want data changed by strtok*/
	strcpy(sTempLicense,drive_license);
	/* get the first token */
	strToken1 = strtok(sTempLicense, QUESTION_MARK_STR);
	if(strToken1) { strToken2 = strtok(NULL, QUESTION_MARK_STR); }
	if(strToken2) { strToken3 = strtok(NULL, QUESTION_MARK_STR); }
	if(strToken3) {
		ascii sTrack3Buffer[CARD_AUTHEN_TRACK3_SIZE] = {""};
		strToken3 += 1;
		CARD_AUTHEN_REMOVESP_4Track3(sTrack3Buffer,strToken3,strlen(strToken3));
		if((sTrack3Buffer[0]>='0')&&(sTrack3Buffer[0]<='9')
		&&(sTrack3Buffer[1]>='0')&&(sTrack3Buffer[1]<='9')) {
			strcpy(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],drive_license);
			buffer_gpbm_write(GPBM_LICENSE_CARD_ADDR,(u8*)CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],CARD_AUTHEN_BUFFER_SIZE);
			CARD_AUTHEN_T3Tabid = CARD_AUTHEN_T3Tabid >= (CARD_AUTHEN_TRACK3_MAX_ID-1) ? 0 : CARD_AUTHEN_T3Tabid+1;
			buffer_gpbm_write(CARD_AUTHEN_T3Tabid + GPBM_LICENSE_TRK3_FIRST_ADDR,(u8*)sTrack3Buffer,CARD_AUTHEN_TRACK3_SIZE);
			vPrintfTrace((30,"TRACK3 = %s",sTrack3Buffer));
		}else return -1;
	}
	return 0;
}

static void CARD_AUTHEN_Del_DriverID(void) {
	memset(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],0,CARD_AUTHEN_BUFFER_SIZE);
	buffer_gpbm_write(GPBM_LICENSE_CARD_ADDR,(u8*)CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],CARD_AUTHEN_BUFFER_SIZE);
	vPutsTrace(0,"DRIV. LICENSE ID DELETE");
}

static void CARD_AUTHEN_Logout(ascii *cardData, uint8_t R_value) {
	if(!CARD_AUTHEN_hInit) return;
	if(strlen(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN]) <= CARD_AUTHEN_LICENSE_STR_LEN) return;//Never Login
	CARD_AUTHEN_Display_2Serial(cardData,0,0);
	strcpy(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_OUT],cardData);
	aptTracking_backward_runtime(1);// backward 1 second
	aptTracking_Runtime.Status.Event = SWD_EV56_CARD_LOGOUT;
	aptTracking_Runtime.Status.R_Val = R_value;
	aptTracking_Runtime.Status.S_Val = 0;
	
	ascii * strhexBuff = &CARD_AUTHEN_LoginLogoutBuffer[0];//adl_memGet(512);
	int ret_size = transform_to_sharpE(strhexBuff, &aptTracking_Runtime,CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_OUT]);
	
	CARD_AUTHEN_SndIndex = CARD_AUTHEN_SndIndex >= (CARD_AUTHEN_LOGING_MAX_ID-1) ? 0 : CARD_AUTHEN_SndIndex+1;
	buffer_gpbm_write((CARD_AUTHEN_SndIndex*4) + GPBM_LOGIN_LOGOUT_FIRST_ADDR,(u8*)strhexBuff,ret_size+1);
		
	aptTracking_backward_runtime(0);// restore to real time
	CARD_AUTHEN_Del_DriverID();
	vPutsTrace(0,"DRIV. LICENSE ID LOGOUT");
}

static void CARD_AUTHEN_Login(ascii *cardData,  bool cardOK) {
	if(!CARD_AUTHEN_hInit) return;
	CARD_AUTHEN_Display_2Serial(cardData,1,cardOK);
	aptTracking_Runtime.Status.Event = SWD_EV55_CARD_LOGIN;
	aptTracking_Runtime.Status.R_Val = 0;
	aptTracking_Runtime.Status.S_Val = 0;
	
	ascii * strhexBuff = &CARD_AUTHEN_LoginLogoutBuffer[0];//adl_memGet(512);
	int ret_size = transform_to_sharpE(strhexBuff, &aptTracking_Runtime,CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN]);
	
	CARD_AUTHEN_SndIndex = CARD_AUTHEN_SndIndex >= (CARD_AUTHEN_LOGING_MAX_ID-1) ? 0 : CARD_AUTHEN_SndIndex+1;
	buffer_gpbm_write((CARD_AUTHEN_SndIndex*4) + GPBM_LOGIN_LOGOUT_FIRST_ADDR,(u8*)strhexBuff,ret_size+1);

	vPutsTrace(30,"DRIV. LICENSE ID LOGIN");
}

u8 card_authenLicenseID(ascii *cardData) {
	bool bLogoutOnly = FALSE;
	bool bCardTypeOK = FALSE;
	
	if(!CARD_AUTHEN_hInit) return 0;
	
	if(((ini_config_utils.DTCUniq.optionmask & 0xE0)>>6) == 0) vPutsTrace(0,"CARD READER OPTION DISABLE");
	else {
		if(!cardData) {
			
			CARD_AUTHEN_SaveDriverStatus(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],false);
			CARD_AUTHEN_Logout(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],CARD_AUTHEN_FORWARD);
			return 0;
		}
		if(!swdio_get_ig_status()) return 0;
		
		if(strlen(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN])> CARD_AUTHEN_LICENSE_STR_LEN) {// Login existing
			
			CARD_AUTHEN_SaveDriverStatus(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],false);

			if(!strcmp(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],cardData)) {
				bLogoutOnly = TRUE; // Same card = Logout Only
				CARD_AUTHEN_Logout(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],CARD_AUTHEN_FORWARD);
			}
			else {
				bLogoutOnly = FALSE;// Other card = Logout and Then Login
				CARD_AUTHEN_Logout(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],CARD_AUTHEN_UNFORWARD);
			}
		}
			
		if(bLogoutOnly) {
			aptSwd_submit_licensecard_alert(TRUE);
		}
		else {
			if(CARD_AUTHEN_Save_DriverID(cardData) >= 0 ) {
				bCardTypeOK = CARD_AUTHEN_Verify_License (	cardData,
				ini_config_utils.DTCUniq.vehicle_type,
				ini_config_utils.DTCUniq.vehicle_attr & 0x03);
				
				if(bCardTypeOK) {
					aptSwd_submit_licensecard_alert(FALSE);
					CARD_AUTHEN_SaveDriverStatus(cardData,true);
				}
				else aptSwd_submit_licensecard_alert(TRUE);
				CARD_AUTHEN_Login(cardData, bCardTypeOK);
			}
		}
	}	
	return 0;	
}
 
ascii * card_authenGetTrack3(int8_t index) {
	static ascii sTrack3Buffer[CARD_AUTHEN_TRACK3_SIZE] = {""};
	memset(sTrack3Buffer,0,CARD_AUTHEN_TRACK3_SIZE);
	if((index < 0)||(index >= CARD_AUTHEN_TRACK3_MAX_ID)) return sTrack3Buffer;// Logout Or Error
	buffer_gpbm_read(index + GPBM_LICENSE_TRK3_FIRST_ADDR,(u8*)sTrack3Buffer,CARD_AUTHEN_TRACK3_SIZE);
	return sTrack3Buffer;
}

int8_t card_authenGetTrack3Tabid(void) {
	if(!CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN][0])return -1;// Stay in Logout
	return CARD_AUTHEN_T3Tabid;
}

void card_authenClean(void) {
	for(uint8_t i = 0; i < CARD_AUTHEN_LOGING_MAX_ID ; i++) buffer_gpbm_erase((i*4)+GPBM_LOGIN_LOGOUT_FIRST_ADDR);
	vPutsTrace(30,"DRIV. Logging All Clear");
	for(uint8_t i = 0; i < CARD_AUTHEN_TRACK3_MAX_ID ; i++) buffer_gpbm_erase(i+GPBM_LICENSE_TRK3_FIRST_ADDR);
	vPutsTrace(30,"DRIV. Track3 All Clear");
	memset(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],0,CARD_AUTHEN_BUFFER_SIZE);
	buffer_gpbm_erase(GPBM_LICENSE_CARD_ADDR);
	vPutsTrace(30,"DRIV. License Clear");
}

ascii* card_authenInit(void) {
	u32 lastupdate_time = 0;
	buffer_gpbm_read(GPBM_LICENSE_CARD_ADDR,(u8*)CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],CARD_AUTHEN_BUFFER_SIZE);
	lvPutsLabel(30,"LICENSE> ",CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN],strlen(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN]));
	if(strlen(CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN])>CARD_AUTHEN_LICENSE_STR_LEN) {
		for(uint8_t i = 0; i < CARD_AUTHEN_TRACK3_MAX_ID ; i++) {
			u32 exist_time = buffer_gpbm_getsn(i+GPBM_LICENSE_TRK3_FIRST_ADDR);
			if(exist_time && (exist_time >= lastupdate_time)) {
				lastupdate_time = exist_time;
				CARD_AUTHEN_T3Tabid = i;//Find Newest T3 Record ID
			}
			
			//Boot-up all driver ID
			buffer_gpbm_read_offset(i+GPBM_LICENSE_TRK3_FIRST_ADDR,GPBM_DRIVER_WORK_HOUR_OFFSET,(u8*)&aptTracking_Driver[i],sizeof(aptTracking_driver_t));
			vPrintfTrace((30,"\r\nDRIV-%d: %s",i,aptTracking_Driver[i].id));
	
		}
		if(CARD_AUTHEN_T3Tabid >= 0) {
			ascii sTrack3Buffer[CARD_AUTHEN_TRACK3_SIZE] = {""};
			buffer_gpbm_read(CARD_AUTHEN_T3Tabid + GPBM_LICENSE_TRK3_FIRST_ADDR,(u8*)sTrack3Buffer,CARD_AUTHEN_TRACK3_SIZE);
			lvPutsLabel(30,"TRACK3> ",sTrack3Buffer,strlen(sTrack3Buffer));
		}
	}
	
	lastupdate_time = 0;
	
	for(uint8_t i = 0; i < CARD_AUTHEN_LOGING_MAX_ID ; i++) {
		u32 exist_time = buffer_gpbm_getsn((i*4)+GPBM_LOGIN_LOGOUT_FIRST_ADDR);
		if(exist_time && (exist_time >= lastupdate_time)) {
			lastupdate_time = exist_time;
			CARD_AUTHEN_SndIndex = i;//Find Newest Send Record ID
		}
		
	}

	CARD_AUTHEN_hInit = 1;
	return CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN][0]?CARD_AUTHEN_LicenseBuffer[CARD_AUTHEN_IN]:NULL;
}