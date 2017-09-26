/*
 * adl_CmdStack_Private.h
 *
 * Created: 27/2/2558 17:09:31
 *  Author: pan
 */ 


#ifndef ADL_CMDSTACK_PRIVATE_H_
#define ADL_CMDSTACK_PRIVATE_H_

typedef struct ADL_CMDSTACK_STR_T {
	adl_atRspHandler_t rsphdlr;
	wm_lst_t hRspList;
	u16 rspFlag;
	ascii atstr[1];
}adl_cmdstack_at_item_t;

adl_cmdstack_at_item_t * adl_atCmdListGet(void);
s32 adl_atCmdListFree(adl_cmdstack_at_item_t * free_item);



#endif /* ADL_CMDSTACK_PRIVATE_H_ */