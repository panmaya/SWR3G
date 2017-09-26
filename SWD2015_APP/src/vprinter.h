/*
 * vprinter.h
 *
 * Created: 19/5/2558 19:12:40
 *  Author: pan
 */ 


#ifndef VPRINTER_H_
#define VPRINTER_H_

#define vPRINT_FSIZE	64
extern char _vPrinter_FileBuff[vPRINT_FSIZE+1];
extern uint16_t _vPrinter_LineNumb;


#define vPRINT_TRACES00_EN	1
#define vPRINT_TRACES01_EN	0
#define vPRINT_TRACES02_EN	0
#define vPRINT_TRACES03_EN	0
#define vPRINT_TRACES04_EN	0
#define vPRINT_TRACES05_EN	1
#define vPRINT_TRACES06_EN	1
#define vPRINT_TRACES07_EN	1
#define vPRINT_TRACES08_EN	0
#define vPRINT_TRACES09_EN	0
#define vPRINT_TRACES10_EN	0
#define vPRINT_TRACES11_EN	0
#define vPRINT_TRACES12_EN	0
#define vPRINT_TRACES13_EN	0
#define vPRINT_TRACES14_EN	0
#define vPRINT_TRACES15_EN	0
#define vPRINT_TRACES16_EN	0
#define vPRINT_TRACES17_EN	0
#define vPRINT_TRACES18_EN	0
#define vPRINT_TRACES19_EN	0
#define vPRINT_TRACES20_EN	0
#define vPRINT_TRACES21_EN	0
#define vPRINT_TRACES22_EN	0
#define vPRINT_TRACES23_EN	0
#define vPRINT_TRACES24_EN	0
#define vPRINT_TRACES25_EN	0
#define vPRINT_TRACES26_EN	0
#define vPRINT_TRACES27_EN	0
#define vPRINT_TRACES28_EN	0
#define vPRINT_TRACES29_EN	0
#define vPRINT_TRACES30_EN	1


void vPrintInit( void );
signed portBASE_TYPE _vPrintf ( uint8_t lev, const char *strFormat, ... );
signed portBASE_TYPE _vPuts ( uint8_t lev, const char * file, uint32_t line, const char *strFormat);

signed portBASE_TYPE vPrintf( const char *strFormat, ... );
signed portBASE_TYPE lvPrintf ( const char *strFormat, ... );
signed portBASE_TYPE vPuts( const char *str );
signed _lvPutsTag( uint8_t lev, const char * Tag, const char * strFormat, uint16_t size, const char * file, uint32_t line);


#define vPrintfTrace(_X_)	do { _vPrinter_LineNumb = __LINE__;\
								strncpy(_vPrinter_FileBuff,__FILE__,vPRINT_FSIZE);\
								_vPrinter_FileBuff[vPRINT_FSIZE] = '\0';\
								_vPrintf _X_;\
							}while(0)
							
#define vPutsTrace(_l,_X_)		_vPuts(_l,__FILE__,__LINE__,_X_)

#define lvPutsLabel(_l,_lab,_s,_n) _lvPutsTag(_l,_lab,_s,_n,__FILE__,__LINE__)

bool vPrint( void );

void vPrintEnable(void);
void vPrintDisable(void);
bool vPrintTraceGet(uint8_t lev);
void vPrintTraceSet(uint8_t lev, bool ison);

void vPutsIntermediat(const char *strFormat);

#endif /* VPRINTER_H_ */