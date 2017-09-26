/*
 * adl_hlCtrl.h
 *
 * Created: 20/2/2558 15:28:20
 *  Author: pan
 */ 


#ifndef APT_GSM_UART_H_
#define APT_GSM_UART_H_

/* The size of the buffer provided to the UART driver
for storage of received  bytes. */
#define APT_GSM_UART_BUFFER_SIZE		1024

void aptGsmUart_Flush(void);
int32_t aptGsmUart_Read(uint8_t** Data, uint16_t nByte ,uint16_t timeout_ms);
int32_t aptGsmUart_Write(uint8_t* Data, uint16_t DataLen);
void aptGsmUart_task(void * );


#endif /* APT_GSM_UART_H_ */