/*
 * adl_CmdHandler.c
 *
 * Created: 22/1/2558 11:38:35
 *  Author: pan
 */ 
#include "adl_global.h"
typedef struct ADL_CMDSUBSCR_ITEM_T {
	adl_atCmdHandler_t Cmdhdl;
	uint8_t length;
	u16 Cmdopt;
	char Cmdstr[1];
}adl_cmdsubscr_item_t;

static wm_lst_t ADL_CMDSUBSCR_hList = NULL;

bool adl_atCmdSubscribeExecute( const ascii *str, adl_atPort_e port) {
	uint16_t offset = 0, iList = 0, total_strlen = strlen(str);
	for(offset = 0;str[offset];offset++){if((str[offset]=='=')||(str[offset]=='?'))break;}

	do {//Searching
		adl_cmdsubscr_item_t * pCmd = NULL;
		adl_atCmdPreParser_t * Params = NULL;

		pCmd = wm_lstGetItem(ADL_CMDSUBSCR_hList,iList);
		iList++;
		
		if(!pCmd)return FALSE;//Not Found
		if(pCmd->length != offset)continue;//Length not matched
		if(wm_strnicmp(str, pCmd->Cmdstr, pCmd->length)) continue;//Character not matched
		
		total_strlen = (total_strlen > 256 ) ? 256 : total_strlen;
		Params = adl_memGet(sizeof(adl_atCmdPreParser_t) + 257);
		if(!Params)return FALSE;
		Params->NbPara = 0;
		Params->StrLength = total_strlen;
		Params->Port = port;
		wm_strncpy(Params->StrData,str,total_strlen);
		Params->StrData[total_strlen] = 0;
		Params->ParaList = wm_lstCreate(0,(void*)0);
		switch(str[offset]) {
		case '=':
			if(str[offset+1]=='?') {
				Params->Type = ADL_CMD_TYPE_TEST;
			}else 	Params->Type = ADL_CMD_TYPE_PARA;
			break;
		case '?':	Params->Type = ADL_CMD_TYPE_READ;
			break;
		case 0:		Params->Type = ADL_CMD_TYPE_ACT;
			break;
		default:	Params->Type = ADL_CMD_TYPE_ROOT;
			break;
		}
		
		if(Params->Type & pCmd->Cmdopt)//Command type support
		{
			if(Params->Type == ADL_CMD_TYPE_PARA)
			{
				u16 pc = 0, max_pc = ((pCmd->Cmdopt & 0x00F0)>>4);
				for(;pc <= max_pc ;pc++) {//Get maximum parameters
					ascii buff[257] = {""};
					ascii *p = NULL;
					p = wm_strGetParameterString(buff,sizeof(buff),str,(pc+1));
					if(p && buff[0]) {
						if(pc >= max_pc) {//Parameters more than maximum
							wm_lstDestroy(Params->ParaList);
							adl_memRelease(Params);
							return FALSE;
						}
						p = adl_memGet(257);
						if(!p)return FALSE;
						wm_strcpy(p,buff);
						Params->NbPara++;
						wm_lstAddItem(Params->ParaList,p);
					}	
				}
				
				if(Params->NbPara < (pCmd->Cmdopt & 0x000F))//Parameters less then minimum
				{
					wm_lstDestroy(Params->ParaList);
					adl_memRelease(Params);
					return FALSE;
				}
			}
		}else {//Command type  Not support
			wm_lstDestroy(Params->ParaList);
			adl_memRelease(Params);
			return FALSE;
		}

		pCmd->Cmdhdl(Params);	
		wm_lstDestroy(Params->ParaList);
		adl_memRelease(Params);
		return TRUE;
	}while(1);
	return FALSE;
}

s16 adl_atCmdSubscribe ( const ascii *Cmdstr, adl_atCmdHandler_t Cmdhdl, u16 Cmdopt ) {
	adl_cmdsubscr_item_t *item = NULL;
	if(!ADL_CMDSUBSCR_hList)ADL_CMDSUBSCR_hList = wm_lstCreate(WM_LIST_NONE,NULL);
	item = (adl_cmdsubscr_item_t*)adl_memGet(sizeof(adl_cmdsubscr_item_t)+strlen(Cmdstr));
	if(!item)return -1;
	wm_strcpy(item->Cmdstr,Cmdstr);
	item->Cmdhdl = Cmdhdl;
	item->length = strlen(Cmdstr);
	item->Cmdopt = Cmdopt;
	wm_lstAddItem(ADL_CMDSUBSCR_hList,item);
	return 1;
}