/*
 * vprinter.c
 *
 * Created: 19/5/2558 19:12:13
 *  Author: pan
 */ 
#include <adl_global.h>
#include <stdarg.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "queue.h"

/* Demo program include files. */
#include "vprinter.h"
#include "apt_debug_port.h"

#define VPRINTER_Q_SIZE			40
#define VPRINTER_MAX_STR		80
#define vPRINT_MAX_SLINE		5//"99999"

#define VPRINTER_TIMESTAMP		0

static xQueueHandle xPrintQueue;

static volatile uint16_t xPrint_QNumber = 0;

static bool vPrintTrace_En = FALSE;

char _vPrinter_FileBuff[vPRINT_FSIZE+1] = {""};
uint16_t _vPrinter_LineNumb = 0;

static unsigned char vPrintTrace_Tab[32] = {
	vPRINT_TRACES00_EN,
	vPRINT_TRACES01_EN,
	vPRINT_TRACES02_EN,
	vPRINT_TRACES03_EN,
	vPRINT_TRACES04_EN,
	vPRINT_TRACES05_EN,
	vPRINT_TRACES06_EN,
	vPRINT_TRACES07_EN,
	vPRINT_TRACES08_EN,
	vPRINT_TRACES09_EN,
	vPRINT_TRACES10_EN,
	vPRINT_TRACES11_EN,
	vPRINT_TRACES12_EN,
	vPRINT_TRACES13_EN,
	vPRINT_TRACES14_EN,
	vPRINT_TRACES15_EN,
	vPRINT_TRACES16_EN,
	vPRINT_TRACES17_EN,
	vPRINT_TRACES18_EN,
	vPRINT_TRACES19_EN,
	vPRINT_TRACES20_EN,
	vPRINT_TRACES21_EN,
	vPRINT_TRACES22_EN,
	vPRINT_TRACES23_EN,
	vPRINT_TRACES24_EN,
	vPRINT_TRACES25_EN,
	vPRINT_TRACES26_EN,
	vPRINT_TRACES27_EN,
	vPRINT_TRACES28_EN,
	vPRINT_TRACES29_EN,
	vPRINT_TRACES30_EN,
	vPRINT_TRACES30_EN+1,//Reserved
};

static char *vPrint_pInterBuff = NULL;

#if(VPRINTER_TIMESTAMP)
static adl_rtcTime_t tstmp;
static uint16_t counter = 0;
#define vPRINT_HEAD_BUFFER_SIZE (2 /*\r\n*/ + 24 /*YYYY-MM-DD HH:MM:SS+msc */ + vPRINT_FSIZE + 5 /*, Ln */ + vPRINT_MAX_SLINE + 3 /* ~ */ + VPRINTER_MAX_STR + 1 /*NULL*/)
#else
#define vPRINT_HEAD_BUFFER_SIZE (2 /*\r\n*/ + vPRINT_FSIZE + 5 /*, Ln */ + vPRINT_MAX_SLINE + 3 /* ~ */ + VPRINTER_MAX_STR + 1 /*NULL*/)
#endif



/*-----------------------------------------------------------*/

void vPrintInit( void ) {
	/* Create the queue on which errors will be reported. */
	xPrintQueue = xQueueCreate( VPRINTER_Q_SIZE, ( unsigned portBASE_TYPE ) sizeof( char * ) );
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE _vPrintf ( uint8_t lev, const char *strFormat, ... ) {
	if(xPrint_QNumber >= VPRINTER_Q_SIZE) return 0;
	if(vPrintTrace_Tab[lev] && vPrintTrace_En && xPrintQueue) {
		va_list arg;
		char * pBuff = NULL;
		uint16_t flen = strlen(_vPrinter_FileBuff);
		pBuff = adl_memGet(vPRINT_HEAD_BUFFER_SIZE);
		if(!pBuff) return 0;
	
		#if(VPRINTER_TIMESTAMP)
		adl_rtcGetTime(&tstmp);
		counter = counter >= 999 ? 0 : counter + 1;
		flen = sprintf(pBuff,"\r\n%04d-%02d-%02d %02d:%02d:%02d+%03d %s, Ln %d ~ ",
																	tstmp.Year,tstmp.Month,tstmp.Day,
																	tstmp.Hour,tstmp.Minute,tstmp.Second,counter,
																	_vPrinter_FileBuff,_vPrinter_LineNumb);
		#else
		flen = sprintf(pBuff,"\r\n%s, Ln %d ~ ",_vPrinter_FileBuff,_vPrinter_LineNumb);
		#endif
		
		va_start(arg, strFormat);
		vsnprintf(pBuff + flen, VPRINTER_MAX_STR, strFormat, arg);
		va_end(arg);
		
		xPrint_QNumber++;
		return xQueueSend( xPrintQueue, ( void * ) &pBuff, 0 );
	}
	return 0;
}

signed portBASE_TYPE _vPuts ( uint8_t lev, const char * file, uint32_t line, const char *strFormat) {
	if(xPrint_QNumber >= VPRINTER_Q_SIZE) return 0;
	if(vPrintTrace_Tab[lev] && vPrintTrace_En && xPrintQueue) {
		char * pBuff = NULL;
		uint16_t flen , slen = 0;
		flen = strlen(file);
		slen = strlen(strFormat);
		pBuff = adl_memGet(vPRINT_HEAD_BUFFER_SIZE);
		if(!pBuff) return 0;
		
		#if(VPRINTER_TIMESTAMP)
		adl_rtcGetTime(&tstmp);
		counter = counter >= 999 ? 0 : counter + 1;
		flen = sprintf(pBuff,"\r\n%04d-%02d-%02d %02d:%02d:%02d+%03d %s, Ln %d ~ ",
																			tstmp.Year,tstmp.Month,tstmp.Day,
																			tstmp.Hour,tstmp.Minute,tstmp.Second,counter,
																			file,(int)line);
		#else
		flen = sprintf(pBuff,"\r\n%s, Ln %d ~ ",file, (int)line);
		#endif

		if(slen >= VPRINTER_MAX_STR) slen = VPRINTER_MAX_STR -1;
		memcpy(pBuff + flen, strFormat, slen);
		pBuff[slen+flen] = 0;
		
		xPrint_QNumber++;
		return xQueueSend( xPrintQueue, ( void * ) &pBuff, 0 );
	}
	return 0;
}


signed portBASE_TYPE vPrintf ( const char *strFormat, ... ) {
	if(xPrint_QNumber >= VPRINTER_Q_SIZE) return 0;
	if((!vPrintTrace_En)  || (!xPrintQueue)) return 0;
	va_list arg;
	char * pBuff = adl_memGet(VPRINTER_MAX_STR + 1);
	if(!pBuff) return 0;
	va_start(arg, strFormat);
	vsnprintf(pBuff,VPRINTER_MAX_STR, strFormat, arg);
	va_end(arg);
	
	xPrint_QNumber++;
	return xQueueSend( xPrintQueue, ( void * ) &pBuff, 0 );
}

signed portBASE_TYPE lvPrintf ( const char *strFormat, ... ) {
	if(xPrint_QNumber >= VPRINTER_Q_SIZE) return 0;
	if((!vPrintTrace_En)  || (!xPrintQueue)) return 0;
	va_list arg;
	char * pBuff = adl_memGet((VPRINTER_MAX_STR*3) + 1);
	if(!pBuff) return 0;
	va_start(arg, strFormat);
	vsnprintf(pBuff,(VPRINTER_MAX_STR*2), strFormat, arg);
	va_end(arg);
	
	xPrint_QNumber++;
	return xQueueSend( xPrintQueue, ( void * ) &pBuff, 0 );
}

signed portBASE_TYPE vPuts ( const char *strFormat) {
	if(xPrint_QNumber >= VPRINTER_Q_SIZE) return 0;
	if((!vPrintTrace_En)  || (!xPrintQueue)) return 0;
	char * pBuff = adl_memGet(VPRINTER_MAX_STR + 1);
	uint16_t slen = strlen(strFormat);
	if(!pBuff) return 0;
	if(slen >= VPRINTER_MAX_STR) slen = VPRINTER_MAX_STR -1;
	memcpy(pBuff,strFormat,slen);
	pBuff[slen] = 0;
	
	xPrint_QNumber++;
	return xQueueSend( xPrintQueue, ( void * ) &pBuff, 0 );
}

/*-----------------------------------------------------------*/

bool vPrint( void ) {
	char *pBuff = NULL;
	if((!vPrintTrace_En)  || (!xPrintQueue)) return FALSE;
	
	if(vPrint_pInterBuff) {
		//while(!uart_is_tx_buf_empty(CONSOLE_UART));
		aptDbgPort_puts(vPrint_pInterBuff);
		//while(!uart_is_tx_buf_empty(CONSOLE_UART));
		adl_memRelease(vPrint_pInterBuff);
	}
	
	if( xQueueReceive( xPrintQueue, &pBuff, 0 ) == pdPASS ) {
		//while(!uart_is_tx_buf_empty(CONSOLE_UART));
		//puts(pBuff);
		aptDbgPort_puts(pBuff);
		//while(!uart_is_tx_buf_empty(CONSOLE_UART));
		adl_memRelease(pBuff);
		
		if(xPrint_QNumber) xPrint_QNumber--;
		
		return TRUE;
	}
	
	return FALSE;
}

void vPrintEnable(void) {
	//memset(vPrintTrace_Tab,0,sizeof(vPrintTrace_Tab));
	vPrintTrace_En = TRUE;
}

void vPrintDisable(void) {
	//memset(vPrintTrace_Tab,0,sizeof(vPrintTrace_Tab));
	vPrintTrace_En = FALSE;
}

void vPrintTraceSet(uint8_t lev, bool ison) {
	if(lev==0xFF) memset(vPrintTrace_Tab,ison,sizeof(vPrintTrace_Tab));
	else if(lev < sizeof(vPrintTrace_Tab)) vPrintTrace_Tab[lev] = ison;
	else;
}

bool vPrintTraceGet(uint8_t lev) {
	if(lev==0xFF) return vPrintTrace_Tab[31];
	else if(lev < sizeof(vPrintTrace_Tab)) return vPrintTrace_Tab[lev];
	else return 0;
}


void vPutsIntermediat(const char *strFormat ) {
	//if(vPrintTrace_Tab[31]) puts(strFormat);
	if(vPrintTrace_Tab[31]) {
		//if(!vPrint_pInterBuff) vPrint_pInterBuff = adl_memGet(strlen(strFormat));
		//if(vPrint_pInterBuff) strcpy(vPrint_pInterBuff,strFormat);
		aptDbgPort_puts(strFormat);
		//puts(strFormat);
	}
}

signed _lvPutsTag(uint8_t lev,const char * Tag, const char * strFormat, uint16_t size, const char * file, uint32_t line) {
	if(vPrintTrace_Tab[lev] && vPrintTrace_En && xPrintQueue) {
		uint16_t TagLen = strlen(Tag);
		if(TagLen >= VPRINTER_MAX_STR)return 0;
		char stampBuff[150] = {""};
		uint16_t flen = 0;
		
		#if(VPRINTER_TIMESTAMP)
		adl_rtcGetTime(&tstmp);
		flen = sprintf(stampBuff,"\r\n%04d-%02d-%02d %02d:%02d:%02d %s, Ln %d ~ %s",
								tstmp.Year,tstmp.Month,tstmp.Day,tstmp.Hour,tstmp.Minute,tstmp.Second, file ,(int) line , Tag);
		#else
		flen = sprintf(stampBuff,"\r\n%s, Ln %d ~ %s",file ,(int) line , Tag);
		#endif
		
		while(size && (xPrint_QNumber < VPRINTER_Q_SIZE)) {
			uint16_t printSize  = (size >= (VPRINTER_MAX_STR - TagLen)) ? (VPRINTER_MAX_STR - TagLen) : size;
			char * pBuff = adl_memGet(vPRINT_HEAD_BUFFER_SIZE);
			if(!pBuff) return 0;
			memcpy(pBuff, stampBuff, flen);
			char * p = pBuff + flen;
			for(;printSize && *strFormat;printSize--){
				switch(*strFormat){
				case '\r':
				case '\n':
					*p = '.';
					break;
				default:
					*p = *strFormat;
					break;
				}
				p++;
				strFormat++;
				size--;
			}
			*p = 0;
			xPrint_QNumber++;
			xQueueSend( xPrintQueue, ( void * ) &pBuff, 0 );
			if(!(*strFormat)) return 0;
		}
	}
	return 0;
}


