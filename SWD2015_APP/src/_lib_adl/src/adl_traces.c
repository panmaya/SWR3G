/*
 * adl_traces.c
 *
 * Created: 31/10/2557 10:30:55
 *  Author: pan
 */ 
#include <adl_global.h>
#include <stdarg.h>
#include "../private/adl_traces_Private.h"

static bool ADL_TRACE_Tab[] = {
	ADL_TRACES_LEV00_EN,
	ADL_TRACES_LEV01_EN,
	ADL_TRACES_LEV02_EN,
	ADL_TRACES_LEV03_EN,
	ADL_TRACES_LEV04_EN,
	ADL_TRACES_LEV05_EN,
	ADL_TRACES_LEV06_EN,
	ADL_TRACES_LEV07_EN,
	ADL_TRACES_LEV08_EN,
	ADL_TRACES_LEV09_EN,
	ADL_TRACES_LEV10_EN,
	ADL_TRACES_LEV11_EN,
	ADL_TRACES_LEV12_EN,
	ADL_TRACES_LEV13_EN,
	ADL_TRACES_LEV14_EN,
	ADL_TRACES_LEV15_EN,
	ADL_TRACES_LEV16_EN,
	ADL_TRACES_LEV17_EN,
	ADL_TRACES_LEV18_EN,
	ADL_TRACES_LEV19_EN,
	ADL_TRACES_LEV20_EN,
	ADL_TRACES_LEV21_EN,
	ADL_TRACES_LEV22_EN,
	ADL_TRACES_LEV23_EN,
	ADL_TRACES_LEV24_EN,
	ADL_TRACES_LEV25_EN,
	ADL_TRACES_LEV26_EN,
	ADL_TRACES_LEV27_EN,
	ADL_TRACES_LEV28_EN,
	ADL_TRACES_LEV29_EN,
	ADL_TRACES_LEV30_EN,
};

static u32 traceLev = 0;


u32 adl_trcLevGet ( void ) {return traceLev;}
	
void adl_trcLevSet ( u32 level ) {
	traceLev = level;
	for(uint8_t i = 0; i< sizeof(ADL_TRACE_Tab);i++) {
		if(level & (0x00000001<<i)) ADL_TRACE_Tab[i] = 1;
		else ADL_TRACE_Tab[i] = 0;
	}
}

void adl_trcDump ( u32 level, u8 * DumpBuffer, u16 DumpLength ) {}
	
s32 adl_trcPrint ( u32 Level, const ascii *strFormat, ... ){
   va_list arg;
   s32 ret = -1;
   if(ADL_TRACE_Tab[Level]) {
	   char buffer[15] = {"\r\n(ADL: "};
	   char * pBuff = &buffer[0];
	   va_start (arg, strFormat);
	   wm_itoa(Level,buffer+8);
	   strcat(buffer,"= ");
	   while(*pBuff) {
		   putchar(*pBuff);
		   pBuff++;
	   }
	   ret = vfprintf (stdout, strFormat, arg);
	   va_end (arg);
	   putchar(')');
	   
   }
   return ret;
}

