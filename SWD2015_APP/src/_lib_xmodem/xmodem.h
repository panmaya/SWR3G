/*
 * xmodem.h
 *
 * Created: 9/6/2558 17:53:05
 *  Author: pan
 */ 


#ifndef XMODEM_H_
#define XMODEM_H_

#include "../_drv_serial/usart/freertos_usart_serial.h"

int32_t xmodem_receive(freertos_usart_if hUart, FIL *file_object, int32_t destsz);


#endif /* XMODEM_H_ */