/*
 * xmodem.c
 *
 * Created: 9/6/2558 17:52:51
 *  Author: pan
 */
#include <adl_global.h>
#include "crc16.h"
#include "xmodem.h"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 1000
#define MAXRETRANS 25
#define TRANSMIT_XMODEM_1K


static int check(int chk_crc, const unsigned char *buf, int sz) {
	if(chk_crc) {
		unsigned short crc = crc16_ccitt(buf, sz);
		unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
		if (crc == tcrc)
		return 1;
	}
	else {
		unsigned char cks = 0;
		for (int i = 0; i < sz; ++i) {cks += buf[i];}
		if (cks == buf[sz])
		return 1;
	}
	return 0;
}

static void flushinput(freertos_usart_if hUart) {
	uint8_t input_string[32];
	portBASE_TYPE returned_value = 0;
	do { returned_value = freertos_usart_serial_read_packet(hUart, input_string, 32, (3000 / portTICK_RATE_MS));
	}while(returned_value > 0);
}

static int readbyte(freertos_usart_if hUart) {
	uint8_t input_string = 0;
	portBASE_TYPE returned_value = 0;
	returned_value = freertos_usart_serial_read_packet(hUart, &input_string, 1, (1000 / portTICK_RATE_MS));
	if(returned_value > 0) return input_string;
	return -1;
}

int32_t xmodem_receive(freertos_usart_if hUart, FIL *file_object, int32_t destsz) {
	unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
	unsigned char *p;
	int bufsz, crc = 0;
	unsigned char trychar = 'C';
	unsigned char packetno = 1;
	int32_t len = 0;
	int i, c = 0;
	int retry, retrans = MAXRETRANS;
	
	for(;;) {
		
		vTaskDelay(1);
		
		for(retry = 0; retry < 16; ++retry) {
			if(trychar) putchar(trychar);
			c = readbyte(hUart);
			if(c >= 0) {
				switch (c) {
					case SOH:
					bufsz = 128;
					goto start_recv;
					case STX:
					bufsz = 1024;
					goto start_recv;
					case EOT:
					flushinput(hUart);
					putchar(ACK);
					return len; /* normal end */
					case CAN:
					c = readbyte(hUart);
					if (c == CAN) {
						flushinput(hUart);
						putchar(ACK);
						return -1; /* canceled by remote */
					}
					break;
					default:
					break;
				}
			}
			vTaskDelay(2000);
		}
		
		if (trychar == 'C') { trychar = NAK; continue; }
		flushinput(hUart);
		putchar(CAN);
		putchar(CAN);
		putchar(CAN);
		return -2; /* sync error */

		start_recv:
		
		if (trychar == 'C') crc = 1;
		trychar = 0;
		p = xbuff;
		*p++ = c;
		for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
			c = readbyte(hUart);
			if (c < 0) goto reject;
			*p++ = c;
		}

		if (xbuff[1] == (unsigned char)(~xbuff[2])
		&& (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1)
		&& check(crc, &xbuff[3], bufsz)) {
			
			if (xbuff[1] == packetno)	{
				register int32_t count = destsz - len;
				if (count > bufsz) count = bufsz;
				if (count > 0) {
					FRESULT res;
					UINT byte_write = 0;
					int32_t write_ix = 0;
					do {
						res = f_write(file_object, &xbuff[3+write_ix], count - write_ix, &byte_write);
						if(res!= FR_OK){
							flushinput(hUart);
							putchar(CAN);
							putchar(CAN);
							putchar(CAN);
							return -5;
						}else write_ix += byte_write;
					}while(write_ix < count);
					len += count;
				}
				++packetno;
				retrans = MAXRETRANS+1;
			}
			if (--retrans <= 0) {
				flushinput(hUart);
				putchar(CAN);
				putchar(CAN);
				putchar(CAN);
				return -3; /* too many retry error */
			}
			putchar(ACK);
			continue;
		}
		
		reject:
		
		flushinput(hUart);
		putchar(NAK);
	}
}