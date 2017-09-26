/*
 * trans_form.c
 *
 *  Created on: 9 ¡.¾. 2555
 *      Author: Pan
 */
#include "adl_global.h"
#include "trans_form.h"
#include "math.h"
#include "misc_config.h"
#include "apt_swd.h"
#include "apt_tracking.h"
#include "buffer.h"
#include "card_authen.h"
#include "feature.h"

#define TRANSFORM_ADC_16BIT		0
#define TRANSFORM_ADC_12BIT		1

#if(0)
static double TRANS_FORM_2_DEGxMIN_DEC(double lac) {
	u16 Deg, Min;
	double Sec = 0.0, dummyValue = 0.0;
	/* Convert Location to llll.ll format -> degrees|minutes.decimal */
	Deg = (u16)lac;
	dummyValue = fabs( (lac - Deg) * 60.0 );
	Min = (u16)dummyValue;
	//Sec = fabs( (dummyValue - Min) * 60.0 );
	Sec = fabs((dummyValue - Min));
	return (double)((Deg*1000000)+(Min*10000)+ (Sec*10000));
}
#endif

static u16 TRANS_FORM_2_DEG(double lac) {
	/* Convert Location to llll.ll format -> degrees|minutes.decimal */
	return (u16)lac;

}
static u16 TRANS_FORM_2_MIN(double lac) {
	u16 Deg, Min;
	double dummyValue = 0.0;
	/* Convert Location to llll.ll format -> degrees|minutes.decimal */
	Deg = (u16)lac;
	dummyValue = fabs( (lac - Deg) * 60.0 )*1000;
	Min = dummyValue;
	//Sec = fabs( (dummyValue - Min) * 60.0 );
	//Sec = fabs((dummyValue - Min));
	return (u16)Min;
	//return (u16)dummyValue*1000;
}

static void encode_sharpN(ascii *dst,trans_form_swd_basic_t *src) {
	s32 longdeg,longmin,latmin;
	s8  latdeg;
	u8  input19 = 0x00,input22 = 0x00;
	u16 iTemp;
	u32 wTemp;
	u8 tmpbuff[30];
	
	strcpy(dst,"*N");
	dst += 2;
		
	tmpbuff[0]=(u8)src->Status.Event;
	tmpbuff[1]=(u8)(src->GPS.day/10)*16+ src->GPS.day%10 + 0x20;
	tmpbuff[2]=(u8)(src->GPS.month/10)*16+ src->GPS.month%10 + 0x20;
	iTemp=src->GPS.year;
	iTemp-= 2000;
	tmpbuff[3]=(u8)(iTemp/10)*16+ iTemp%10 + 0x20;
	tmpbuff[4]=(u8)(src->GPS.hour/10)*16+ src->GPS.hour%10 + 0x20;
	tmpbuff[5]=(u8)(src->GPS.minute/10)*16+ src->GPS.minute%10 + 0x20;
	tmpbuff[6]=(u8)(src->GPS.second/10)*16+ src->GPS.second%10 + 0x20;
	longdeg	=	(s32)TRANS_FORM_2_DEG(src->GPS.lon);// Longitude
	longmin	=	(s32)TRANS_FORM_2_MIN(src->GPS.lon);
	latdeg	=	(s32)TRANS_FORM_2_DEG(src->GPS.lat);
	latmin	=	(s32)TRANS_FORM_2_MIN(src->GPS.lat);
		

	tmpbuff[7]=(u8)(longdeg>>8) &0xFF;
	tmpbuff[8]=(u8)(longdeg) &0xFF;
	tmpbuff[9]=(u8)(latmin>>8) &0xFF;
	tmpbuff[10]=(u8)(latmin) &0xFF;
	tmpbuff[11]=(u8)(longmin>>8) &0xFF;
	tmpbuff[12]= (u8)(longmin) &0xFF;
	tmpbuff[13]=(u8)(latdeg) &0xFF;
	tmpbuff[14]= (u8)src->GPS.speed;
	wTemp=src->TotalDistance;
	wTemp/=100;
	iTemp=wTemp;
	tmpbuff[15]=(u8)(iTemp>>8) &0xFF;
	tmpbuff[16]=(u8)(iTemp) &0xFF;
	tmpbuff[17]= (u8)(src->Status.Value>>8) &0xFF;
	tmpbuff[18]= (u8)src->Status.Value &0xFF;
		
	#if(0) // ALL MSB frame form
	+------------+----------------+----------------+------+------+
	| STATUS GPS | RFID-MSB(10:9) |  OIL-MSB(10:9) | IG   | PTO1 |
	|------------+----------------+----------------+------+------+
	|     XX     |       XX       |       XX       |  X   |   X  |
	+------------+----------------+----------------+------+------+
	#endif
		
	#if TRANSFORM_ADC_16BIT	
	//Convert ADC 16 bits to 10 bits ; compatible old  model of SWR
	u16 adc_10bitsVal = src->ADCIn_0 & 0x7FFF;
	adc_10bitsVal >>= 5;
	adc_10bitsVal &= 0x03FF;
		
	/* ALL MSB frame format */
	input19 = (src->RFID>>8)&0x03;
	input19 <<= 2;
	input19 |= (adc_10bitsVal>>8)&0x03;//Addition OIL MSB(10:9)
	input19 <<= 1;
	input19 |= ((src->Input) & 0x01);//Addition IG
	input19 <<= 1;
	input19 |= ((src->Input>>1)&0x01);//Addition PTO1
	///////////////////////////
		
	tmpbuff[19]=(u8)input19;
	tmpbuff[20]=src->RFID & 0xFF;//driver ID LSB
	tmpbuff[21]=adc_10bitsVal & 0xFF;//Oil LSB
	#else
	//Convert ADC 12 bits to 10 bits ; compatible old  model of SWR
	u16 adc_10bitsVal = src->ADCIn_0 & 0x07FF;
	adc_10bitsVal >>= 1;
	adc_10bitsVal &= 0x03FF;
		
	/* ALL MSB frame format */
	input19 = (src->RFID>>8)&0x03;//RFID(Driver ID) MSB(10:9)
	input19 <<= 2;
	input19 |= (adc_10bitsVal>>8)&0x03;//Addition OIL MSB(10:9)
	input19 <<= 1;
	input19 |= ((src->Input) & 0x01);//Addition IG
	input19 <<= 1;
	input19 |= ((src->Input>>1)&0x01);//Addition PTO1
	///////////////////////////
		
	tmpbuff[19]=(u8)input19;
	tmpbuff[20]=src->RFID & 0xFF;//RFID(Driver ID) LSB
	tmpbuff[21]=adc_10bitsVal & 0xFF;//Oil LSB
	#endif
		
	#if(0) // SWR_ 028 Pack Form
	+------------------+---------+-----------+------+------+
	| RFID_EMER_BUTTON | RESERVE |  RFID_IO  | PTO3 | PTO2 |
	|------------------+---------+-----------+------+------+
	|        X         |    0    |    XXXX   |  X   |   X  |
	+------------------+---------+-----------+------+------+
	#endif
		
	input22 = (uint8_t)((src->Input>>4)&0x0F);//Addition RFID_IO
	input22 <<= 1;
	input22 |= (uint8_t)((src->Input>>3)&0x01);//Addition PTO3
	input22 <<= 1;
	input22 |= (uint8_t)((src->Input>>2)&0x01);//Addition PTO2
	tmpbuff[22] = input22;
		
	if(swd_feature_isEnable(FEAT_TEMP_4_SENSOR)) {
		tmpbuff[23]=src->TempSensor[3];
		tmpbuff[24]=src->TempSensor[2];
		tmpbuff[25]=src->TempSensor[1];
		tmpbuff[26]=src->TempSensor[0];
	}
	else {
		tmpbuff[23]=0x00;//ODE Speed
		tmpbuff[24]=(uint8_t)((src->Input>>8)&0xFF);//Addition SEC_IO
		tmpbuff[25]=0x00;//RPM
		tmpbuff[26]=src->TempSensor[0];//TEMP
	}
		
	//send data end here
	tmpbuff[27]=0x00;
	tmpbuff[28]=0x00;
	tmpbuff[29]=0x00;		

	
	for(u8 i=0;i<30;i++) wm_itohexa(dst + (i*2),tmpbuff[i],2);//dst_len += sprintf(dst + dst_len,"%02X",tmpbuff[i]);
}

static const char *byte_to_binary(int x) {
	#if(0) //GPI format
		+------------+-----------+------+------+------+----+
		|   SEC_IO   |  RFID_IO  | PTO3 | PTO2 | PTO1 | IG |
		|------------+-----------+------+------+------+----+
		| XXXX XXXX  |   XXXX    |   X  |   X  |  X   |  X |
		+------------+-----------+------+------+------+----+
	#endif
	static char b[11];
	int z = 1;
	memset(b,0,sizeof(b));
	strcat(b, ((x & z) == z) ? "1" : "0");
	z = 2;
	strcat(b, ((x & z) == z) ? "1" : "0");
	
	strcat(b, "000000");
	
	z = 8;
	strcat(b, ((x & z) == z) ? "1" : "0");
	z = 4;
	strcat(b, ((x & z) == z) ? "1" : "0");
	#if(0) //Frame format
	+-----+-------+------+------+------+------+------+------+------+-------+
	|  IG |  PTO1 | CANc | Geof | exPw | None | None | None | PTO3 |  PTO2 |
	|-------------+------+------+------+------+------+------+------+-------+
	|  X  |   X   |   X  |   X  |  X   |   0  |  0   |   0  |  X   |   X   |
	+-----+-------+------+------+------+------+------+------+
	#endif
	return b;
}

int transform_to_sharpE(char pData[512], aptTracking_runtime_t *pNVMem, ascii *extend) {
	/*
	#E,T01100000V010113,SMARTi3G020153R07,8966002913900410677,0,|*R,16/01/19,01:32:01,12,30,60,25871,A,190116013201,14.150052,101.88657,
	177,0.78,10,65,0,235.39156,25057,388,10000000,538,0000,11949,0,165378,0,25876,000,5,7,0,0,0,0,0,@!,,#!,@#,,##,@%,,#%,@$,,,,,,,#$,0HF,$#@
	*/
	return snprintf(	pData, 512 ,
					"#E,%s%d%d%d" /*Operator Code*//*Uart-1 Baud rate = 115200*/ /*Uart-2 Baud rate = 9600*//*Card Type*/
					"%d%d%d%d%d" /*Temp Sensor Enable*//*SEC enable*//*CAN enable*//*Buzzer enable*//*Portable*/
					"V%03d%s," /*PSOC version*/
					"SWR3G%02d%02d%02d," /*GSM Firmware Version*/
					"%s,%ld," /*Serial SIM*//*Buffer*/
					"|*R,%02d/%02d/%02d,%02d:%02d:%02d," /*RTC-Date*//*RTC-Time*/
					"%d,%02X,%d,%d,%c," /*CSQ*//*Status-Code*//*R-Value*//*Status -Value*/ /*GPS Status*/
					"%02d%02d%d%02d%02d%02d," /*GPS-Date*//*GPS-Time*/
					"%.5f,%.5f,%d,%.2f,%d,%d," /*LAT*//*LON*//*COURSE*//*HDOP*//*SAT*//*SPEED*/
					"%d,%d,%d,%d,%s,"/*RPM*/ /*Distance/1000*/ /*Ext_Voltage*/ /*Vbatt/10*/ /*IO*/
					"%d,%d00,%ld,%d," /*V-Fuel/10*/ /*Temperature*/ /*Trip drive time*/ /*Card time*/
					"%ld,%ld,%d,0," /*Trip drive distance*/ /*Distance per day*/ /*DriveTime per day*/
					"%d,%d,%d,0,"  /*IG on Counter*/ /*Idle Counter*/ /*Over speed Counter*/ 
					"%d,0,0," /*PTO1 counter*/ /*PTO2 counter*/ /*PTO3 counter*/
					"@!,%d,#!,@#,%d,##,@%%,%s,#%%,@$," /*DTC_CELL*/ /*DTC_OBD*/ /*DBUG*/
					"%s,%d,%d,%d,%d,%04X,#$,0HF,$#@", /*extend*/ /*MAXSpeedPerMin*/ /*OVSPCountPerMin*/ /*HACountPerMin*/ /*HDCountPerMin*/ /*Hour Meter*/
					
					aptSwd_get_sim_operator()/*Operator Code*/,1/*Uart-1 Baud rate = 115200*/,0/*Uart-2 Baud rate = 9600*/,(ini_config_utils.DTCUniq.optionmask >> 6)&0x03/*Card Type*/,
					(ini_config_utils.DTCUniq.optionmask >> 5)&0x01/*Temp Sensor Enable*/,(ini_config_utils.DTCUniq.optionmask >> 4)&0x01/*SEC enable*/,
					(ini_config_utils.DTCUniq.optionmask >> 3)&0x01/*CAN enable*/,1/*Buzzer enable*/,0/*Portable*/,
					SWD2015_FW_VERSION,ini_config_utils.DTCUniq.sFwName/*PSOC version*/,
					//ibuild_year-2000 ,ibuild_month,ibuild_day,/*GSM Firmware Version*/
					17 ,8,24,/*GSM Firmware Version*/
					aptSwd_get_sim_ccid()/*Serial SIM*/,apt_buffer_cpe_nbItem()/*Buffer*/,
					(pNVMem->tmRTC.year >= 2000) ? pNVMem->tmRTC.year - 2000 : 0,pNVMem->tmRTC.month,pNVMem->tmRTC.day/*RTC-Date*/,pNVMem->tmRTC.hour,pNVMem->tmRTC.minute,pNVMem->tmRTC.second/*RTC-Time*/,
					arc_netsys.signLev/*CSQ*/,pNVMem->Status.Event/*Status-Code*/,pNVMem->Status.R_Val/*Status-Value*/,
					pNVMem->Status.S_Val/*Status -Value2*/,pNVMem->GPS.fixDimnt ? 'A': 'V'/*GPS Status*/,
					pNVMem->GPS.day,pNVMem->GPS.month,(pNVMem->GPS.year >= 2000) ? pNVMem->GPS.year - 2000 : 0/*GPS-Date*/,pNVMem->GPS.hour,pNVMem->GPS.minute,pNVMem->GPS.second/*GPS-Time*/,
					pNVMem->GPS.lat/*LAT*/,pNVMem->GPS.lon/*LON*/,pNVMem->GPS.course/*COURSE*/,pNVMem->GPS.hDop/*HDOP*/,pNVMem->GPS.satUsed/*SAT*/,pNVMem->HarshSpeed/*SPEED*/,
					pNVMem->RPM_Frequency/*RPM*/,(int)pNVMem->TotalDistance/1000/*Distance*/,(int)pNVMem->MAIN_Voltage/*Ext_Voltage*/,(int)pNVMem->BATT_Voltage/10/*Vbatt*/,byte_to_binary(pNVMem->digGPI)/*IO*/,
					pNVMem->Fuel_Voltage/10/*Fuel*/,pNVMem->TempSensor[0]/*Temperature*/,pNVMem->TripTime/*Trip drive time*/,0/*Card time*/,
					pNVMem->TripDistance/*Trip drive distance*/,pNVMem->DistancePerDay/1000/*Distance per day*/,pNVMem->DriveTimePerDay/*DriveTime per day*/,
					pNVMem->IGON_Counter/*IG on Counter*/,pNVMem->IDLE_Counter/*Idle Counter*/,pNVMem->OVSP_Counter/*Over speed Counter*/,
					pNVMem->PTO_Counter/*PTO1 counter*/,
					0/*DTC_CELL*/,0/*DTC_OBD*/,"NORMAL"/*DTC_DBUG*/,
					extend ? extend :"err-card", pNVMem->MAXSpeedPerMin,pNVMem->OVSPCountPerMin,pNVMem->HACountPerMin,pNVMem->HDCountPerMin,0);
}


int transform_to_sharpL(char pData[512], aptTracking_runtime_t *pNVMem, ascii *extend) {
	static trans_form_swd_basic_t transFormData;

	transFormData.ADCIn_0		=	pNVMem->adcIN.ch2;
	transFormData.GPS.course	=	pNVMem->GPS.course;
	transFormData.GPS.lat		=	pNVMem->GPS.lat;
	transFormData.GPS.lon		=	pNVMem->GPS.lon;
	transFormData.GPS.day		=	pNVMem->tmRTC.day;
	transFormData.GPS.month		=	pNVMem->tmRTC.month;
	transFormData.GPS.year		=	pNVMem->tmRTC.year;
	transFormData.GPS.hour		=	pNVMem->tmRTC.hour;
	transFormData.GPS.minute	=	pNVMem->tmRTC.minute;
	transFormData.GPS.second	=	pNVMem->tmRTC.second;
	transFormData.GPS.hDop		=	pNVMem->GPS.hDop;
	transFormData.GPS.satUsed	=	pNVMem->GPS.satUsed;
	transFormData.GPS.speed		=	pNVMem->HarshSpeed;
	transFormData.Input			=	pNVMem->digGPI;
	transFormData.Output		=	pNVMem->digGPO;
	transFormData.TempSensor[0]	=	pNVMem->TempSensor[0];
	transFormData.TempSensor[1]	=	pNVMem->TempSensor[1];
	transFormData.TempSensor[2]	=	pNVMem->TempSensor[2];
	transFormData.TempSensor[3]	=	pNVMem->TempSensor[3];
	transFormData.RFID			=	pNVMem->RFID;
	transFormData.Status.Event	=	pNVMem->Status.Event;
	transFormData.Status.Value  =	pNVMem->Status.R_Val;
	transFormData.TotalDistance	=	pNVMem->TotalDistance;

	switch(pNVMem->GPS.fixDimnt) {
	case GPS_FIX_NO_POS:
		transFormData.GPS.satFixs = 0;
		break;
	case GPS_FIX_2D:
	case GPS_FIX_DIFF_2D:
		transFormData.GPS.satFixs = 2;
		break;
	case GPS_FIX_3D:
	case GPS_FIX_DIFF_3D:
		transFormData.GPS.satFixs = 3;
		break;
	default:
		break;
	}

	int ret = snprintf((char*)pData,512,"#L%03d:%s:",SWD2015_FW_VERSION,aptSwd_get_sim_ccid());
	encode_sharpN(pData + ret, &transFormData);
	pData[56 + ret] = '\0';
	wm_strcat(pData, "*L");
	if(extend)wm_strcat(pData, extend );
	wm_strcat(pData, "**08/" );
	wm_strcat(pData, ini_config_utils.DTCUniq.sEmail );
	wm_strcat(pData, "?" );
	return strlen(pData);
}
