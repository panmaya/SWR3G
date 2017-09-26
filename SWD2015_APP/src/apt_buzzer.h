/*
 * sound_buzzer.h
 *
 * Created: 2016-10-21 2:31:31 PM
 *  Author: Panmaya
 */ 


#ifndef APT_BUZZER_H_
#define APT_BUZZER_H_

void aptBuzzer_task(void * pvParameters);
void aptBuzzer_alert (u8 loop);
void aptBuzzer_clrbuff(void);
void aptBuzzer_error(void);
void aptBuzzer_ovspeed(void);
void aptBuzzer_qc(void);

#endif /* APT_BUZZER_H_ */