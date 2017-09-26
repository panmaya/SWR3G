/*
 * adl_CmdStackHandler.c
 *
 * Created: 4/10/2557 13:27:32
 *  Author: pan
 */ 
#include <adl_global.h>
#include <stdarg.h>
#include "../private/adl_CmdStack_Private.h"

static wm_lst_t ADL_CMDSTACK_ATList = NULL;

#define ADL_CMDSTACK_ATLIMIT_STACK	5
static uint8_t ADL_CMDSTACK_ATLimit = 0;

adl_cmdstack_at_item_t * adl_atCmdListGet(void) {
	return (adl_cmdstack_at_item_t *) wm_lstGetItem(ADL_CMDSTACK_ATList,0);
}

s32 adl_atCmdListFree(adl_cmdstack_at_item_t * free_item) {
	s32 ret = -1;
	if(!ADL_CMDSTACK_ATList)return ADL_RET_ERR_PARAM;
	if(!free_item)return ERROR;
	if(free_item->hRspList) wm_lstDestroy(free_item->hRspList);
	ret = wm_lstDeleteItem(ADL_CMDSTACK_ATList,0);
	ADL_CMDSTACK_ATLimit--;
	return ret;
}


s8 adl_atCmdCreate (const ascii *atstr, u16 rspflag, adl_atRspHandler_t rsphdlr, ... ) {
	adl_cmdstack_at_item_t * atstack = NULL;
	if(!atstr) return ADL_RET_ERR_PARAM;
	if(!ADL_CMDSTACK_ATList)return ADL_RET_ERR_PARAM;
	if(ADL_CMDSTACK_ATLimit > ADL_CMDSTACK_ATLIMIT_STACK)return ADL_RET_ERR_PARAM;
	
	atstack = (adl_cmdstack_at_item_t*)adl_memGet(sizeof(adl_cmdstack_at_item_t)+ strlen(atstr) + 2);
	if(!atstack)return -1;
	
	strcpy(atstack->atstr,atstr);
	strcat(atstack->atstr,"\r");
	atstack->rsphdlr = rsphdlr;
	atstack->rspFlag = rspflag;
	atstack->hRspList = NULL;

	if(rsphdlr) {
		va_list ap;
		char * str_target = NULL;
		va_start(ap, rsphdlr);
		str_target = va_arg(ap,char*);
		if(str_target) {
			atstack->hRspList = wm_lstCreate(WM_LIST_NONE,NULL);
			if(atstack->hRspList) {
				do {
					ascii * pBuff = adl_memGet(strlen(str_target));
					if(!pBuff)return -1;
					strcpy(pBuff,str_target);
					wm_lstAddItem(atstack->hRspList, pBuff);
					str_target = va_arg(ap,char*);
				}while(str_target);	
			}
		}
		va_end(ap);
	}
	wm_lstAddItem(ADL_CMDSTACK_ATList, atstack);
	ADL_CMDSTACK_ATLimit++;
	return OK;
}

s8 adl_CmdStackHandler_init(void) {
	if(ADL_CMDSTACK_ATList)return 1;//already subscribed
	ADL_CMDSTACK_ATList = wm_lstCreate(WM_LIST_NONE,NULL);
	return ADL_CMDSTACK_ATList ? OK : ERROR;
}