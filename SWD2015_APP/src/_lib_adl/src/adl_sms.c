/*
 * adl_sms.c
 *
 * Created: 27/2/2558 13:38:20
 *  Author: pan
 */ 
#include <adl_global.h>
#include "../private/adl_sms_Private.h"

static wm_lst_t ADL_SMSSUBSCR_hList = NULL;
static wm_lst_t ADL_SMSSENDER_hList = NULL;

adl_smssnd_item_t * adl_smsSendListGet(void) {
	if(!ADL_SMSSENDER_hList) return NULL;
	return (adl_smssnd_item_t *) wm_lstGetItem(ADL_SMSSENDER_hList,0);
}

s32 adl_smsSendListFree(adl_smssnd_item_t * free_item) {
	if(!ADL_SMSSENDER_hList) return ERROR;
	if(!free_item)return ERROR;
	return wm_lstDeleteItem(ADL_SMSSENDER_hList,0);
}

adl_smssubscr_item_t * adl_smsSubscrListGet(uint8_t Handle) {
	if(!ADL_SMSSUBSCR_hList)return NULL;
	if(!Handle) return NULL;
	Handle--;
	return (adl_smssubscr_item_t *) wm_lstGetItem(ADL_SMSSUBSCR_hList,Handle);
}

//+CMGR: "REC UNREAD","+31628870634",,"11/01/09,10:26:26+04"<CR><LF>This is text message 1<CR><LF>
bool adl_smsGetParameterString (adl_smsrecv_item_t *dst, const ascii * src) {
	const char *data_str = (const char *)src;
	uint16_t i = 0;
	
	if(!dst) return false;
	
	dst->SmsTel[0] = 0;
	dst->SmsTime[0] = 0;
	dst->SmsText[0] = 0;

	/* Remote number starts from the second header field */
	data_str = strchr(data_str, ',');
	if (data_str == NULL) return FALSE;
	/* Find start of remote number after the opening quote */
	data_str = strchr(data_str, '\"');
	if (data_str == NULL) return FALSE;
	/* Copy the remote number from the SMS header until terminating quote */
	for (i = 0; i < ADL_SMS_TELN_LEN; i++) {
		char current_digit = data_str[i + 1];
		if (current_digit != '\"') dst->SmsTel[i] = current_digit;
		else break;
	}
	/* Ensure remote number is null-terminated */
	dst->SmsTel[i] = '\0';
	/* Forward to get time stamp */
	data_str = strchr(data_str, ',');
	if (data_str == NULL) return FALSE;
	/* Find start of service center time after the opening quote */
	data_str = strchr(data_str, '\"');
	if (data_str == NULL) return FALSE;
	/* Copy the time stamp from the SMS header until terminating quote */
	for (i = 0; i < ADL_SMS_TIME_LEN; i++) {
		char current_digit = data_str[i + 1];
		if (current_digit != '\"') dst->SmsTime[i] = current_digit;
		else break;
	}
	/* Ensure time stamp number is null-terminated */
	dst->SmsTime[i] = '\0';
	/* Forward to get text message */
	data_str = strchr(data_str, '\n');
	if (data_str == NULL) return FALSE;
	/* Copy the text message from the SMS header until terminating*/
	for (i = 0; i < ADL_SMS_TEXT_LEN; i++) {
		char current_digit = data_str[i + 1];
		if (current_digit != '\r') dst->SmsText[i] = current_digit;
		else break;
	}
	/* Ensure text message is null-terminated */
	dst->SmsText[i] = '\0';
	return TRUE;
}

s8 adl_smsSend( u8 Handle, ascii * SmsTel, ascii * SmsText, u8 Mode ) {
	adl_smssnd_item_t *item = NULL;
	if((!ADL_SMSSUBSCR_hList)||(!ADL_SMSSENDER_hList)) return -1;
	if(Handle > wm_lstGetCount(ADL_SMSSUBSCR_hList)) return -1;
	item = (adl_smssnd_item_t*)adl_memGet(sizeof(adl_smssnd_item_t));
	if(item) {
		wm_strcpy(item->SmsTel,SmsTel);
		wm_strcpy(item->SmsText,SmsText);
		item->Mode = Mode;
		item->Handle = Handle;
		wm_lstAddItem(ADL_SMSSENDER_hList,item);
		return 0;
	}
	return -1;
}
s8 adl_smsSubscribe( adl_smsHdlr_f SmsHandler, adl_smsCtrlHdlr_f SmsCtrlHandler, u8 Mode ){
	adl_smssubscr_item_t *item = NULL;
	if(!ADL_SMSSUBSCR_hList)ADL_SMSSUBSCR_hList = wm_lstCreate(WM_LIST_NONE,NULL);
	if(!ADL_SMSSENDER_hList)ADL_SMSSENDER_hList = wm_lstCreate(WM_LIST_NONE,NULL);
	if((!ADL_SMSSUBSCR_hList)||(!ADL_SMSSENDER_hList)) return -1;
	item = (adl_smssubscr_item_t*)adl_memGet(sizeof(adl_smssubscr_item_t));
	if(item) {
		item->SmsCtrl = SmsCtrlHandler;
		item->SmsData = SmsHandler;
		wm_lstAddItem(ADL_SMSSUBSCR_hList,item);
		return wm_lstGetCount(ADL_SMSSUBSCR_hList);//return (Handle > 0)
	}
	return -1;
}