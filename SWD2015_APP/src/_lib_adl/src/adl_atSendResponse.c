/*
 * adl_atSendResponse.c
 *
 * Created: 24/9/2557 13:37:12
 *  Author: pan
 */ 
#include "adl_global.h"
#include "vprinter.h"
//#include "asf.h"


s32 adl_atSendResponse ( u16 Type, const ascii *Text ) {
	puts((char*)Text);
	return 0;
}