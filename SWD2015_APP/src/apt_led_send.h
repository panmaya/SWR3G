/*
 * led_tcpsend.h
 *
 * Created: 2016-10-21 2:50:38 PM
 *  Author: Panmaya
 */ 


#ifndef APT_LED_SEND_H_
#define APT_LED_SEND_H_


void aptLedSend_task(void * pvParameters);
void aptLedSend_set(bool status);
void aptLedSend_igoff(void);
void aptLedSend_sending(void);
void aptLedSend_server (bool status);
void aptLedSend_clrbuffer(void);


#endif /* APT_LED_SEND_H_ */