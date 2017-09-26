/*
 * swd_register.c
 *
 * Created: 26/3/2558 11:07:56
 *  Author: pan
 */ 
#include "adl_global.h"
#include "swd_register.h"

/*
	//FEATURE NAME Support
	
	$A1B2#SWDFM="000",SPN="077160451",SPNB="104090174531",SIM1="0877160451",STON="non12@",STOS="ia12.com",
	SEV1="203.154.243.12",PSEV1="700",TSK="60",PSD="1",OSPD="110",TID="120",SUD1="T",APN1="www.dtac.co.th",
	COMP="DTC",TRP="1800",PASS="pass",DSP="0",
	RS232="0",CARD="0",SEC="0",HA="12",HD="24",GEOF="0",FLM="0",CAN="0",GSEN="0",GSENX="0",GSENY="0",GSENZ="0",
	ROAM="0",SVW="0",HDW="0",VTNFW="0",SFO="0",HAT="0",HDT="0",LBS="0",TEMP="0",HC="0",RS2="115200",ANR="N",
	ANS="N",DEG="0",CT1="N",CT2="N",CT3="N",PTB="N",TPS="0",HDOP="3",CTM="0",PTO1="1",PTO2="1",ODO="0",PROD="N",
	VHW="0",PGN="0",VHT="0",VHD="0",VTL="0",!oKoK
*/
static void _swd_register_cfg_unpack(swd_register_cfg_t* cfg, char* data) {
	static const char cmd_tab[][10] = {"SWDFM=","SPN=","SPNB=","SIM1=","STON=","STOS=","SEV1=","PSEV1=","TSK=",
										"PSD=","OSPD=","TID=","SUD1=","APN1=","COMP=","TRP=","PASS=","DSP=",
										"RS232=","CARD=","SEC=","HA=","HD=","GEOF=","FLM=","CAN=","GSEN=","GSENX=","GSENY=","GSENZ=",
										"ROAM=","SVW=","HDW=","VTNFW=","SFO=","HAT=","HDT=","LBS=","TEMP=","HC=","RS2=","ANR=",
										"ANS=","DEG=","CT1=","CT2=","CT3=","PTB=","TPS=","HDOP=","CTM=","PTO1=","PTO2=","ODO=","PROD=",
										"VHW=","PGN=","VHT=","VHD=","VTL=","SPKG=","MICG",""};
	
	for(uint8_t itab = 0;cmd_tab[itab][0];itab++) {
		char *ptr = strstr((char*)data,cmd_tab[itab]);
		if(ptr) {
			uint16_t sos = 0,eos = 0;
			sos = eos = strlen(cmd_tab[itab]) +1;//add 1 for exclude 1st quote
			while((*(ptr + eos) != '"')&&(*(ptr + eos) != '\0')) { eos++; }

			char buffer[65] = {""};
			strncpy(buffer,ptr + sos,eos-sos);
			buffer[eos-sos] = 0;
			
			switch(itab) {
			case 0://"SWDFM="
				strcpy(cfg->sFwName, buffer);
				break;
			case 1://"SPN="
				break;
			case 2://"SPNB="
				strcpy(cfg->sCarID, buffer);
				break;
			case 3://"SIM1="
				strcpy(cfg->sSIMNumb, buffer);
				break;
			case 4://"STON="
				strcpy(cfg->sEmail, buffer);
				strcpy(cfg->recall, buffer);
				break;
			case 5://"STOS="
				strcat(cfg->sEmail, buffer);
				strcat(cfg->recall, buffer);
				break;
			case 6://"SEV1="
				strcpy(cfg->tcprecservall, buffer);
				break;
			case 7://"PSEV1="
				strcpy(cfg->tcprecservport, buffer);
				break;
			case 8://"TSK="
				cfg->set_samp = atoi(buffer);
				break;
			case 9://"PSD="
				cfg->set_point = atoi(buffer);
				break;
			case 10://"OSPD="
				cfg->set_over = atoi(buffer);
				break;
			case 11://,"TID="
				cfg->set_idle = atoi(buffer);
				break;
			case 12://,"SUD1="
				if(ptr[strlen(cmd_tab[itab]) +1]== 'T') cfg->bAlertOversp = 1;
				else cfg->bAlertOversp = 0;
				break;
			case 13://"APN1="
				strcpy(cfg->apnall, buffer);
				break;
			case 14://"COMP="
				strcpy(cfg->sCompany, buffer);
				break;
			case 15://"TRP="
				break;
			case 16://"PASS="
				break;
			case 17://"DSP="
				cfg->set_delay = atoi(buffer);
				break;
			case 18://"RS232"
				break;
			case 19://"CARD"
				cfg->optionmask &= ~(3 << 7);// Clear bit 7:6
				cfg->optionmask |= ((atoi(buffer) << 6) & 0xE0);
				break;
			case 20://"SEC"
				cfg->optionmask &= ~(1 << 4);// Clear bit 4
				cfg->optionmask |= ((atoi(buffer) << 4) & 0x10);
				break;
			case 21://"HA="
				cfg->HA= atoi(buffer);
				break;
			case 22://"HD="
				cfg->HD = atoi(buffer);
				break;
			case 23://"GEOF="
				break;
			case 24://"FLM="
				break;
			case 25://"CAN="
				cfg->optionmask &= ~(1 << 3);// Clear bit 3
				cfg->optionmask |= ((atoi(buffer) << 3) & 0x04);
				break;
			case 26://"GSEN="
				break;
			case 27://"GSENX="
				break;
			case 28://"GSENY="
				break;
			case 29://"GSENZ="
				break;
			case 30://"ROAM="
				break;
			case 31://"SVW="
				break;
			case 32://"HDW="
				break;
			case 33://"VTNFW="
				break;
			case 34://"SFO="
				break;
			case 35://"HAT="
				break;
			case 36://"HDT="
				break;
			case 37://"LBS="
				break;
			case 38://"TEMP="
				cfg->optionmask &= ~(1 << 5);// Clear bit 5
				cfg->optionmask |= ((atoi(buffer) << 5) & 0x20);
				break;
			case 39://"HC="
				break;
			case 40://"RS2="
				break;
			case 41://"ANR="
				break;
			case 42://"ANS="
				break;
			case 43://"DEG="
				break;
			case 44://"CT1="
				break;
			case 45://"CT2="
				break;
			case 46://"CT3="
				break;
			case 47://"PTB="
				break;
			case 48://"TPS="
				break;
			case 49://"HDOP="
				break;
			case 50://"CTM="
				break;
			case 51://"PTO1="
				break;
			case 52://"PTO2="
				break;
			case 53://"ODO="
				break;
			case 54://"PROD=",
				break;
			case 55://"VHW="
				cfg->vehicle_attr &= ~(1 << 0);// Clear bit 0
				cfg->vehicle_attr |= (((atoi(buffer) > 3500 ? 1 : 0) << 0) & 0x01);
				break;
			case 56://"PGN="
				cfg->vehicle_attr &= ~(1 << 1);// Clear bit 1
				cfg->vehicle_attr |= (((atoi(buffer) > 20 ? 1 : 0) << 1) & 0x02);
				break;
			case 57://"VHT="
				cfg->vehicle_type = atoi(buffer);
				break;
			case 58://"VHD="
				cfg->vehicle_attr &= ~(1 << 2);// Clear bit 2
				cfg->vehicle_attr |= ((atoi(buffer) << 2) & 0x04);
				break;
			case 59://"VTL="
				cfg->vehicle_attr &= ~(1 << 3);// Clear bit 3
				cfg->vehicle_attr |= ((atoi(buffer) << 3) & 0x08);
				break;
			case 60://"SPKG="
				cfg->spk_gain = atoi(buffer);
				break;
			case 61://"MICG="
				cfg->mic_gain = atoi(buffer);
				break;
			default:
				break;
			}
		}
	}
}

uint8_t swd_register_make_sndframe(char* dst, const char* str_ccid, const char* str_sim) {
	#if(!SWD_REGISTER_HARDCODE)
	strcpy(dst,"#A,V=931,S=");
	strcpy(dst+11,str_ccid);
	strcat(dst,",");
	strcat(dst,str_sim);
	strcat(dst,",$?000000000");
	#else
	strcpy(dst,"#A,V=931,S=8966181407449140161,D,$?000000000");
	#endif
	return strlen(dst);
}

bool swd_register_cfg_decode(swd_register_cfg_t * cfg, uint8_t* data, uint16_t length) {
	char * pStr = NULL;
	
	pStr = memmem(data,length,"!oKoK",5);
	
	if(!pStr) {
		vPutsTrace(30, "decode DMS Tailer Failed");
		return FALSE;
	}else *pStr = 0;
	
	pStr = memmem(data,length,"$A1B2#",6);
	
	if(!pStr) {
		vPutsTrace(30, "decode DMS Header Failed");
		return FALSE;
	}
	
	_swd_register_cfg_unpack(cfg,pStr);

	return TRUE;
}