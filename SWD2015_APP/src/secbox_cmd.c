/*
 * secbox_cmd.c
 *
 * Created: 2016-11-14 10:07:54 AM
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
#include "secbox_cmd.h"
#include "swd_event.h"

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------


SWD Individual Command


--------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void secbox_cmd_Sec(ascii * cmdstr, const uint16_t len) {
	//AT#SEC="0000","000E","000F","000D","0000","FFFF","FFFF","0000","0000","0000"
	
	u16 ioValue =  0x0000;

	if((len!=76)||(cmdstr[len-1]!= '"')) {
		puts("\r\nERR: AT#SEC\r\n");
		return;
	}

	if(cmdstr[12]=='"') {
		ioValue = wm_hexatoi(cmdstr +8 ,4);
		aptTracking_Runtime.digGPI &= 0x00FF;// Clear bit 15:8
		aptTracking_Runtime.digGPI |= ((ioValue << 8)&0xFF00);//OR bit 15:8
	}
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------


SWD Individual Command


--------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void secbox_cmd_Comm(ascii * cmdstr, const uint16_t len) {
	//AT#COMM="SECDATA,0F030A10,0E070424,00000000000000000................................................"

	if((len > 12 )&&(cmdstr[len-1]== '"')) {
		cmdstr[len-1] = 0;// remove ' " '
		s32 insert_nb = swd_event_InsertCommData(cmdstr + 9);
		if(insert_nb < 0){
			if(insert_nb == (-2))vPuts("\r\n#COMM: FULL\r\n");
			else vPrintf("\r\n#COMM: ER%d\r\n",insert_nb);
		}else vPrintf("\r\n#COMM: OK\r\nDATA KEEP: %ld\r\n",insert_nb);
	}else puts("\r\nERR: AT#COMM\r\n");

}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------


SWD Individual Command


--------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void secbox_cmd_Card(ascii * cmdstr, const uint16_t len) {
	//AT#CARD="NAME................................."
	
	if((len > 60)&&(cmdstr[len-1]== '"')) {
		cmdstr[len-1] = 0;// remove ' " '
		card_authenLicenseID(cmdstr + 9);// remove AT#CARD="
	}else puts("\r\nERR: AT#CARD\r\n");
}


/*
-------------------------------------------------------------------------------------------------------------------------------------------------------


SWD Individual Command


--------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void secbox_cmd_Temp(ascii * cmdstr, const uint16_t len) {
	//AT#TEMP="00FF"
	//AT#TEMP="00FF","0000"
	//AT#TEMP="00E5","0002","0003","0004"
	if(((len!=35)&&(len!=14)&&(len!=21))||(cmdstr[len-1]!= '"')) {
		puts("\r\nERR: AT#TEMP\r\n");
		return;
	}

	aptTracking_tempsensor_timeout = 1;
	
	if(cmdstr[13]=='"') aptTracking_Runtime.TempSensor[0] = wm_hexatoi(cmdstr + 9 ,4);
	else {
		aptTracking_Runtime.TempSensor[0] = 0xFF;
		aptTracking_Runtime.TempSensor[1] = 0xFF;
		aptTracking_Runtime.TempSensor[2] = 0xFF;
		aptTracking_Runtime.TempSensor[3] = 0xFF;
		return;
	}
	
	if(len <= 14) return;
	
	if(cmdstr[20]=='"') aptTracking_Runtime.TempSensor[0] = wm_hexatoi(cmdstr + 16 ,4);
	else {
		aptTracking_Runtime.TempSensor[1] = 0xFF;
		aptTracking_Runtime.TempSensor[2] = 0xFF;
		aptTracking_Runtime.TempSensor[3] = 0xFF;
		return;
	}
	
	if(len <= 21) return;
	
	if(cmdstr[27]=='"') aptTracking_Runtime.TempSensor[0] = wm_hexatoi(cmdstr + 23 ,4);
	else {
		aptTracking_Runtime.TempSensor[2] = 0xFF;
		aptTracking_Runtime.TempSensor[3] = 0xFF;
		return;
	}
	
	if(cmdstr[34]=='"') aptTracking_Runtime.TempSensor[0] = wm_hexatoi(cmdstr + 30 ,4);
	else {
		aptTracking_Runtime.TempSensor[3] = 0xFF;
		return;
	}
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------


SWD Individual Command


--------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void secbox_cmd_Rfid(ascii * cmdstr, const uint16_t len) {
	//AT#RFID="9CB55","0","0"
	
	u16 io_itemp = 0x00;
	u32 rfid_18bit = 0x00000;
	
	if((len!=23)||(cmdstr[len-1]!= '"')) {
		puts("\r\nERR: AT#RFID\r\n");
		return;
	}
	
	if(cmdstr[14]=='"') {
		rfid_18bit = wm_hexatoi(cmdstr + 9 ,5);
		if(rfid_18bit!=0x0000) {
			aptTracking_rfid_timeout = 1;
			aptTracking_Runtime.RFID = rfid_18bit & 0x3FF;//Change to 10 bit for sharpN
		}
	}else return;
	
	if(cmdstr[22]=='"') {
		io_itemp = wm_hexatoi(cmdstr + 21 ,1);
		aptTracking_Runtime.digGPI &= 0xFF0F;// Clear bit 7:4
		aptTracking_Runtime.digGPI |= ((io_itemp << 4)&0x00F0);//OR bit 7:4
	}
}
