/*
 * wm_stdio.c
 *
 * Created: 24/9/2557 14:38:54
 *  Author: pan
 */ 
//#include "asf.h"
#include <adl_global.h>

/* reverse:  reverse string s in place */
static void WM_STDIO_FnReverse(char s[]) {
	char c;
	int i, j;
	for(i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
 }
 
 /*
 int wm_atoi(const char * src) {
	 char* end;
	 int res = strtol(src, &end, 10);
	 if (!*end) return res;
	 else return 0;
 }
 */
 
//static void WM_STDIO_RemoveSubstring(char *s,const char *toremove) {
//	 	while( (s=strstr(s,toremove)) ) memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
//}

/***************************************************************************/
/*  Function   : wm_isascii                                                */
/*-------------------------------------------------------------------------*/
/*  Objet      : Check if the character is an ascii one                    */
/*                                                                         */
/*  Return     : Character if ascii, 0 otherwise                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  c                 | x |   |   | Char to check                          */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//ascii wm_isascii ( ascii c );
ascii wm_isascii ( ascii c ) {
	return ((c & (~0x7f)) == 0);
}


/***************************************************************************/
/*  Function   : wm_ishexa                                                 */
/*-------------------------------------------------------------------------*/
/*  Objet      : Check if the character is an hexadecimal one              */
/*                                                                         */
/*  Return     : Character if hexadecimal, 0 otherwise                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  c                 | x |   |   |  Char to check                         */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//ascii wm_ishexa ( ascii c );
ascii wm_ishexa ( ascii c ) {
	return ((c >= '0' && c <= '9') ||(c >= 'a' && c <= 'f') ||(c >= 'A' && c <= 'F'));
}


/***************************************************************************/
/*  Function   : wm_itoa                                                 */
/*-------------------------------------------------------------------------*/
/*  Objet      : Check if the character is an hexadecimal one              */
/*                                                                         */
/*  Return     : Character if hexadecimal, 0 otherwise                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  c                 | x |   |   |  Char to check                         */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//ascii * wm_itoa  ( s32 a, ascii *szBuffer );
ascii* wm_itoa(s32 n, ascii * s) {
	int i, sign;
	
	if ((sign = n) < 0)	/* record sign */
	n = -n;				/* make n positive */
	i = 0;
	do {				/* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
	if (sign < 0)
	s[i++] = '-';
	s[i] = '\0';
	WM_STDIO_FnReverse(s);
	return s;
}

/***************************************************************************/
/*  Function   : wm_isnumstring                                            */
/*-------------------------------------------------------------------------*/
/*  Objet      : Check numeric string                                      */
/*                                                                         */
/*  Return     : TRUE if numeric, FALSE otherwise                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  string            | x | x |   |  String to check                       */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//bool wm_isnumstring ( ascii * string );



/***************************************************************************/
/*  Function   : wm_ishexastring                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : Check if the string is an hexadecimal one                 */
/*                                                                         */
/*  Return     : TRUE if hexadecimal, FALSE otherwise                      */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  string            | x |   |   |  String to check                       */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
bool wm_ishexastring ( ascii * string ) {
	for(;string && *string;string++) {
		switch(*string) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			break;
		default:
			return FALSE;
			break;
		}
	}
	return TRUE;
}



/***************************************************************************/
/*  Function   : wm_isphonestring                                          */
/*-------------------------------------------------------------------------*/
/*  Object     : Check if the string is valid phone number                 */
/*                                                                         */
/*  Return     : TRUE if phone number, FALSE otherwise                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  string            | x |   |   |  String to check                       */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//bool wm_isphonestring ( ascii * string );



/***************************************************************************/
/*  Function   : wm_hexatoi                                                */
/*-------------------------------------------------------------------------*/
/*  Object     : Convert hexadecimal string to u32                         */
/*                                                                         */
/*  Return     : Converted u32, 0 on error                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  src               | x |   |   | String to convert                      */
/*--------------------+---+---+---+----------------------------------------*/
/*  iLen              | x |   |   | String length                          */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
u32 wm_hexatoi ( ascii * src, u16 iLen ) {
	u32 rval = 0;
	ascii *pend = NULL;
	ascii pbuf[9];
	if(iLen && (iLen < 9)) {
		strncpy(pbuf,src,iLen);
		pbuf[iLen] = 0;
		rval = strtoul(pbuf,&pend,16);
	}
	return rval;
}



/***************************************************************************/
/*  Function   : wm_hexatoibuf                                             */
/*-------------------------------------------------------------------------*/
/*  Objet      : Convert hexadecimal string to u8 * buffer                 */
/*                                                                         */
/*  Return     : pointer on dst                                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  dst               |   | x |   | Buffer                                 */
/*--------------------+---+---+---+----------------------------------------*/
/*  src               | x |   |   | String to convert                      */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//u8 * wm_hexatoibuf ( u8 * dst, ascii * src );



/***************************************************************************/
/*  Function   : wm_itohexa                                                */
/*-------------------------------------------------------------------------*/
/*  Objet      : Convert u32 to hexadecimal string                         */
/*                                                                         */
/*  Return     : pointer on dst string                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  dst               |   | X |   |  destination string                    */
/*--------------------+---+---+---+----------------------------------------*/
/*  nb                | x |   |   |  u32 to convert                        */
/*--------------------+---+---+---+----------------------------------------*/
/*  len               | x |   |   |  length of conversion                  */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
ascii * wm_itohexa ( ascii *dst, u32 nb, u8 len )
{
	char myhex[] = { "0123456789ABCDEF" };
	if((dst)&&(len<=8)) {
		u8 i = 0;
		u32 val = 0;
		for(;len;len--){
			val = (nb & (0x0000000F << (i*4))) >> (i*4);
			dst[i] = myhex[val & 0x0000000F];
			i++;
		}
		dst[i]='\0';
	}
	return dst;
}



/***************************************************************************/
/*  Function   : wm_ibuftohexa                                             */
/*-------------------------------------------------------------------------*/
/*  Objet      : Convert a u8* buffer to hexadecimal string                */
/*                                                                         */
/*  Return     : pointer on dst string                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  dst               |   | X |   |  destination string                    */
/*--------------------+---+---+---+----------------------------------------*/
/*  src               | x |   |   |  buffer to convert                     */
/*--------------------+---+---+---+----------------------------------------*/
/*  len               | x |   |   |  len of src buffer                     */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//ascii * wm_ibuftohexa ( ascii * dst, u8 * src, u16 len );



/***************************************************************************/
/*  Function   : wm_strSwitch                                              */
/*-------------------------------------------------------------------------*/
/*  Object     : Switch for strings                                        */
/*                                                                         */
/*  Remark     : The list must finish by NULL.                             */
/*                                                                         */
/*  Return     : u16  0 strTest not find in list                           */
/*                    n strTest was founded row n                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  strTest           | x |   |   |  String to compare                     */
/*--------------------+---+---+---+----------------------------------------*/
/*  ...               | x |   |   |  List of string to compare             */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//u16 wm_strSwitch ( const ascii * strTest, ... );



/***************************************************************************/
/*  Function   : wm_strRemoveCRLF                                          */
/*-------------------------------------------------------------------------*/
/*  Object     : Remove CR(0x0D) LF(0x0A)                                  */
/*                                                                         */
/*  Return     : String without CR/LF                                      */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  dst               |   | x |   |  string format without CR LF           */
/*--------------------+---+---+---+----------------------------------------*/
/*  src               | x |   |   |  string format with    CR LF           */
/*--------------------+---+---+---+----------------------------------------*/
/*  size              | x |   |   |  string size                           */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
ascii * wm_strRemoveCRLF ( ascii * dst, ascii * src, u16 size ) {
	for(;size && *src;size--) {
		if((*src!=0x0D)&&(*src!=0x0A)){
			*dst = *src;
			dst++;
		}
		src++;
	}
	*dst = 0;
	return dst;
}



/***************************************************************************/
/*  Function   : wm_strGetParameterString                                  */
/*-------------------------------------------------------------------------*/
/*  Object     : Returns the parameter string at Position                  */
/*                                                                         */
/*  Return     : ascii * NULL if not found                                 */
/*                       Address of the begin of string parameter          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  dst               |   | x |   |  String to return                      */
/*--------------------+---+---+---+----------------------------------------*/
/*  src               | x |   |   |  String to analyse                     */
/*--------------------+---+---+---+----------------------------------------*/
/*  Position          | x |   |   |  Parameter position                    */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//ascii * wm_strGetParameterString ( ascii * dst, const ascii * src, u8 Position );
ascii * wm_strGetParameterString ( ascii * dst, const u16 dst_len ,const ascii * src, u8 Position ) {
	uint16_t p0 = 0, p1 = 0;
	uint8_t continue_flag = TRUE;
	bool qouat_open = FALSE;
	if((!src) || (!dst) || (Position <= 0))return NULL;
	do{
		switch(src[p1]) {
		case 0:
			Position--;
			continue_flag = FALSE;
			break;
		case '\r':
		case '\n':
			if(p0) {
				Position--;
				continue_flag = FALSE;
			}
			break;
		case '=':
		case ' ':
			if(!p0) p0 = p1;// update first  position
			break;
		case ',':
			
			if(qouat_open && p1) {
				if(src[p1-1]!='"') break;
				else qouat_open = FALSE;
			}
			
			Position--;
			if(Position==0){
				continue_flag = FALSE;
			}else p0 = p1;// update to next  position
			break;
		case '"':
			qouat_open = TRUE;
			break;
		default:
			break;
		}
		if(continue_flag) p1++;
	}while(Position && continue_flag);
	
	p0++;

	if((!Position)&&(p0 < p1)){
		if(src[p0] == '"') {
			p0++;
			p1--;
			if(src[p1] != '"') return NULL;//Wrong format
		}
		else {
			while(src[p0]==' ') { p0++;	}
		}
		
		if(dst_len > (p1-p0)) {
			memcpy(dst,src+p0,p1-p0);
			dst[p1-p0] = 0;
			return dst;	
		}else puts("\r\n strParameter too long\r\n");
	}
	
	return NULL;
}


s16 strSearchToken(ascii *buff,ascii token,u16 number,u16 size) {
	s16 i = 0, count=0;
	while(i<size) {
		if(buff[i]==token) count++;
		if(count>=number) return i;
		if(buff[i]=='\0') return -1;
		i++;
	}
	return -1;
}