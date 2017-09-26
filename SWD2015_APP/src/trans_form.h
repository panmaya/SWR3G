/*
 * trans_form.h
 *
 *  Created on: 9 ¡.¾. 2555
 *      Author: Pan
 */

#ifndef TRANS_FORM_H_
#define TRANS_FORM_H_
#include "xm0110_compat.h"
#include "apt_tracking.h"
#include "buffer.h"
#include "card_authen.h"

#define TRANSFORM_SWD_HEAD_SIZE	70 /*Estimated*/
#define TRANSFORM_SWD_TAIL_SIZE	50 /*Estimated*/


typedef struct	_TRANSCIVER_FORM_RAW_T {
	u16	size;
	u8	*data;
}trans_form_raw_t;

typedef struct	_TRANSCIVER_FORM_EXTEND_T {
	u16 size;
	u16	code;
	u8	data[1];
}trans_form_extend_t;

typedef struct	_TRANSCIVER_FORM_SWD_HEAD_T {
	u16	size;
	u8	data[TRANSFORM_SWD_HEAD_SIZE];
}trans_form_swd_head_t;

typedef struct	_TRANSCIVER_FORM_SWD_TAIL_T {
	u16	size;
	u8	data[TRANSFORM_SWD_TAIL_SIZE];
}trans_form_swd_tail_t;

typedef struct _TRANSCIVER_FORM_SWD_BASIC_T {
	u8  TempSensor[4];
	U16 RFID;
	u16	ADCIn_0;
	u16	Input;
	u16	Output;
	
	struct {
		u8 Event;
		u16 Value;
	}Status;

	u32	TotalDistance;

	struct {
		float	hDop;
		double	lat;
		double	lon;
		u16		speed;
		u16		course;
		u8		satFixs;
		u8		satUsed;
		u8		hour;
		u8		minute;
		u8		second;
		u8		day;
		u8		month;
		u16		year;
	}GPS;

	trans_form_swd_head_t header;
	trans_form_swd_tail_t endframe;
}trans_form_swd_basic_t;


int transform_to_sharpE(char pData[512], aptTracking_runtime_t *pNVMem, ascii *extend);
int transform_to_sharpL(char pData[512], aptTracking_runtime_t *pNVMem, ascii *extend);

#endif /* TRANS_FORM_H_ */
