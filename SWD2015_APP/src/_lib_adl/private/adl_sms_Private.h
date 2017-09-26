/*
 * adl_sms_Private.h
 *
 * Created: 27/2/2558 17:11:18
 *  Author: pan
 */ 


#ifndef ADL_SMS_PRIVATE_H_
#define ADL_SMS_PRIVATE_H_

#define ADL_SMS_TELN_LEN	13//+66851370781
#define ADL_SMS_TIME_LEN	21//11/01/09,10:26:26+04
#define ADL_SMS_TEXT_LEN	(70*4)//Thai unicode SMS

typedef struct ADL_SMSSUBSCR_ITEM_T {
	adl_smsHdlr_f SmsData;
	adl_smsCtrlHdlr_f SmsCtrl;
}adl_smssubscr_item_t;

typedef struct ADL_SMSSEND_ITEM_T {
	uint8_t Handle;
	char SmsTel[ADL_SMS_TELN_LEN + 1];
	char SmsText[ADL_SMS_TEXT_LEN + 1];
	uint8_t Mode;
}adl_smssnd_item_t;

typedef struct ADL_SMSRECV_ITEM_T {
	uint16_t index;
	char SmsTel[ADL_SMS_TELN_LEN + 1];
	char SmsTime[ADL_SMS_TIME_LEN + 1];
	char SmsText[ADL_SMS_TEXT_LEN + 1];
}adl_smsrecv_item_t;


adl_smssnd_item_t * adl_smsSendListGet(void);
s32 adl_smsSendListFree(adl_smssnd_item_t * free_item);
bool adl_smsGetParameterString (adl_smsrecv_item_t *dst, const ascii * src);
adl_smssubscr_item_t * adl_smsSubscrListGet(uint8_t Handle);



#endif /* ADL_SMS_PRIVATE_H_ */