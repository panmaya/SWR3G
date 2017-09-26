/*
 * swd_event.h
 *
 * Created: 3/25/2015 10:52:37 AM
 *  Author: Pan
 */ 


#ifndef SWD_EVENT_H_
#define SWD_EVENT_H_

#define SWD_IDLE_CODE_VALUE			0x02 /* 2 Minutes */
#define SWD_PARK_CODE_VALUE			0x01 /* 1 Minutes */

#define SWD_REPORT_IG_ON_PERIOD		5 /*Minute*/
#define SWD_REPORT_IG_OFF_PERIOD	5/*Minute*/

typedef enum SWD_EVENT_E {
	SWD_EV10_PTO1_ON		=	0x10,
	SWD_EV11_PTO1_OFF		=	0x11,
	SWD_EV12_PTO2_ON		=	0x12,
	SWD_EV13_PTO2_OFF		=	0x13,
	SWD_EV14_PTO3_ON		=	0x14,
	SWD_EV15_PTO3_OFF		=	0x15,
	SWD_EV30_RUNNING 		=	0x30,
	SWD_EV31_IDLE			=	0x31,
	SWD_EV32_EXIT_IDLE		=	0x32,
	SWD_EV33_IG_OFF			=	0x33,
	SWD_EV34_IG_ON			=	0x34,
	SWD_EV41_OVER_SPEED		=	0x41,
	SWD_EV50_REPORT			=	0x50,
	SWD_EV53_HARD_DE		=	0x53,
	SWD_EV54_HARD_AC		=	0x54,
	SWD_EV55_CARD_LOGIN		=	0x55,
	SWD_EV56_CARD_LOGOUT	=	0x56,
	SWD_EV60_RESET			=	0x60,
	SWD_EV95_BACKUP			=	0x95,
	SWD_EV96_SHUTDOWN		=	0x96,
}swd_event_e;


void swd_event_keep_status(u8 event, u16 casing);
void swd_event_update_report(int *speed_gps,int *speed_odo);
s32 swd_event_InsertCommData(ascii *commData);



#endif /* SWD_EVENT_H_ */