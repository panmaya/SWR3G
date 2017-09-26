/*
 * arc_shell_common.c
 *
 * Created: 10/12/2016 11:30:35 AM
 *  Author: Panmaya
 */ 
#include "adl_global.h"
#include "arc_shell_common.h"

/*extren*/ char arc_shell_common_rspBuffer[ARC_SHELL_MAX_CMD_LEN] = {""};

static arc_shell_postCtx ARC_SHELL_POST_CTX_Cbk = (void*)0;

void arc_shell_commonPost(ascii *str) { if(ARC_SHELL_POST_CTX_Cbk) ARC_SHELL_POST_CTX_Cbk(); }
	
void arc_shell_commonSubscribe(arc_shell_postCtx cbk) { ARC_SHELL_POST_CTX_Cbk = cbk; }

void arc_shell_commonToUpper(char * dst, char * src) {
	src += 2;// Exclude "AT"
	for(;*src && dst;src++) {
		switch(*src){
		case '=':
		case '?':
		case '\r':
		case '\n':
			*dst = ':';
			*(dst+1)= ' ';
			*(dst+2) = 0;
			return;
		break;
		}
		*dst = (char)toupper(*src);
		dst++;
	}
	if(dst) *dst = 0;
}
