/*
 * adl_audio.c
 *
 * Created: 17/2/2558 9:50:16
 *  Author: pan
 */ 
#include "adl_global.h"

/** Initial duty cycle value */
#define ADL_AUDIO_INIT_DUTY		50

static pwm_channel_t ADL_AUDIO_PWM_hChannel = {
		/* Channel */
		.channel = PIN_PWM_BUZZER_CHANNEL,
		/* Use PWM clock A as source clock */
		.ul_prescaler = PWM_CMR_CPRE_CLKA,
		/* Period is left-aligned */
		.alignment = PWM_ALIGN_LEFT,
		/* Output waveform starts at a low level */
		.polarity = PWM_LOW,
		/* Duty cycle value of output waveform */
		.ul_duty = 0,
};


s32 adl_audioStop (s32 audioHandle) {
	/* Disable PWM channels for Buzzer */
	pwm_channel_disable(PWM, PIN_PWM_BUZZER_CHANNEL);
	/* Duty cycle value of output waveform */
	ADL_AUDIO_PWM_hChannel.ul_duty = 0;
	/* Initialize channel instance*/
	pwm_channel_init(PWM, &ADL_AUDIO_PWM_hChannel);
	/* Enable PWM channels for BUZZER */
	pwm_channel_enable(PWM, PIN_PWM_BUZZER_CHANNEL);
	return OK;
}


s32 adl_audioTonePlay (s32 audioHandle, u16 Frequency1, s8 Gain1, u16 Frequency2, s8 Gain2, u32 Duration) {
	/* Disable PWM channels for Buzzer */
	pwm_channel_disable(PWM, PIN_PWM_BUZZER_CHANNEL);
	/* Set PWM clock A*/
	pwm_clock_t clock_setting = {
		.ul_clka = Frequency1 * Duration,
		.ul_clkb = 0, /* (clock B is not used) */
		.ul_mck = sysclk_get_cpu_hz()/*96000000 or system clock*/
	};
	/*Setup clock for PWM module:*/
	pwm_init(PWM, &clock_setting);
	/* Period value of output waveform */
	ADL_AUDIO_PWM_hChannel.ul_period = Duration;
	/* Duty cycle value of output waveform */
	ADL_AUDIO_PWM_hChannel.ul_duty = ADL_AUDIO_INIT_DUTY;
	/* Initialize channel instance*/
	pwm_channel_init(PWM, &ADL_AUDIO_PWM_hChannel);
	/* Enable PWM channels for buzzer*/	
	pwm_channel_enable(PWM, PIN_PWM_BUZZER_CHANNEL);
	return OK;
}

s32 adl_audioSubscribe ( adl_audioResources_e audioResource, adl_audioEventHandler_f audioEventHandler, adl_audioResourceOption_e Options ) {
	switch(audioResource) {
	case ADL_AUDIO_BUZZER:
		/* Enable PWM peripheral clock */
		pmc_enable_periph_clk(ID_PWM);
		/* Disable PWM channels for Buzzer */
		pwm_channel_disable(PWM, PIN_PWM_BUZZER_CHANNEL);
		/* Initialize channel instance*/
		pwm_channel_init(PWM, &ADL_AUDIO_PWM_hChannel);
		/* Disable channel counter event interrupt */
		pwm_channel_disable_interrupt(PWM, PIN_PWM_BUZZER_CHANNEL, 0);
		NVIC_DisableIRQ(PWM_IRQn);
		NVIC_ClearPendingIRQ(PWM_IRQn);
		NVIC_SetPriority(PWM_IRQn, 9);
		NVIC_EnableIRQ(PWM_IRQn);
		pwm_channel_enable(PWM, PIN_PWM_BUZZER_CHANNEL);
		break;
	default:
		return ERROR;
		break;
	}
	return OK;
}