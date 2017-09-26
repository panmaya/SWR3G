/*
 * arc_afec.h
 *
 * Created: 23/3/2558 14:33:36
 *  Author: pan
 */ 


#ifndef APT_AFEC_H_
#define APT_AFEC_H_

#define APT_AFEC_EXT_POWER_CH	0
#define APT_AFEC_BAT_POWER_CH	1
#define APT_AFEC_3V6_POWER_CH	2
#define APT_AFEC_5V0_POWER_CH	3
#define APT_AFEC_EXT_INPUT_CH	4

void aptAfec_task(void * pvParameters);
float aptAfec_get(uint8_t ch);
uint32_t aptAfec_getRaw(uint8_t ch);
float aptAfec_ConvertRaw(uint8_t ch, uint32_t rawADC);

#endif /* APT_AFEC_H_ */