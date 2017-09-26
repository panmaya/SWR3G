/*
 * arc_afec.c
 *
 * Created: 23/3/2558 14:33:16
 *  Author: pan
 */ 
#include <adl_global.h>
#include "apt_afec.h"

/** Reference voltage for AFEC in mv. */
#define APT_AFEC_VOLT_REF					(3300)

/** The maximal digital value */
#define MAX_DIGITAL_12_BIT					(4095)
#define MAX_DIGITAL_16_BIT					(65535)
#define APT_AFEC_RESOLUTION					MAX_DIGITAL_12_BIT

/** R-input divider */
#define APT_AFEC_VIN_DIVIDER_CAL(r1,r2)		(((r1+r2)*10)/(r2*10))
#define APT_AFEC_47V_DIVIDER				APT_AFEC_VIN_DIVIDER_CAL(100,6.8)
#define APT_AFEC_6V0_DIVIDER				APT_AFEC_VIN_DIVIDER_CAL(56.2,56.2)

//Result_mV = ((Vref+ - Vref-)/2^n_steps * Result) + Vref-
#define APT_AFEC_VIN_CAL(_adc,_Div)			((float)(APT_AFEC_VOLT_REF * _adc * _Div) / APT_AFEC_RESOLUTION )

static enum afec_channel_num ch_list[] = {
	AFEC_CHANNEL_0,
	AFEC_CHANNEL_1,
	AFEC_CHANNEL_2,
	AFEC_CHANNEL_3,
};

static uint32_t vdss_47V_raw = 0;
static uint32_t vbat_4V2_raw = 0;
static uint32_t vreg_5V0_raw = 0;
static uint32_t voil_47V_raw = 0;


#if(0)
/**
 * \brief Configure to trigger AFEC by TIOA output of timer.
 */
static void configure_tc_trigger(void)
{
	uint32_t ul_div = 0;
	uint32_t ul_tc_clks = 0;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	/* Enable peripheral clock. */
	pmc_enable_periph_clk(ID_TC0);
	
	/* TIOA configuration */
	//gpio_configure_pin(PIN_TC0_TIOA0, PIN_TC0_TIOA0_FLAGS);

	/* Configure TC for a 1Hz frequency and trigger on RC compare. */
	tc_find_mck_divisor(1, ul_sysclk, &ul_div, &ul_tc_clks, ul_sysclk);
	tc_init(TC0, 0, ul_tc_clks | TC_CMR_CPCTRG | TC_CMR_WAVE |
			TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET);

	TC0->TC_CHANNEL[0].TC_RA = (ul_sysclk / ul_div) / 2;
	TC0->TC_CHANNEL[0].TC_RC = (ul_sysclk / ul_div) / 1;

	/* Start the Timer. */
	tc_start(TC0, 0);
}
#endif

#if(0)
/**
 * \brief AFEC1 DRDY interrupt callback function.
 */
static void afec00_data_ready(void)
{
	/** AFEC sample data */
	vdss_47V_raw = afec_get_latest_value(AFEC0);
	vss_external = ((vdss_47V_raw * APT_AFEC_VOLT_REF) / iAdcResolution)* APT_AFEC_47V_DIVIDER;
}

static void afec01_data_ready(void)
{
	/** AFEC sample data */
	vbat_4V2_raw = afec_get_latest_value(AFEC0);
	vbat_4V02 = ((vbat_4V2_raw * APT_AFEC_VOLT_REF) / iAdcResolution)* APT_AFEC_47V_DIVIDER;
}

static void afec02_data_ready(void)
{
	voil_47V_raw = afec_get_latest_value(AFEC0);
	voil_external = ((voil_47V_raw * APT_AFEC_VOLT_REF) / iAdcResolution)* APT_AFEC_47V_DIVIDER;
}

static void afec03_data_ready(void)
{
	vreg_5V0_raw = afec_get_latest_value(AFEC0);
	vreg_5v0 = ((vreg_5V0_raw * APT_AFEC_VOLT_REF) / iAdcResolution)* APT_AFEC_47V_DIVIDER;
}
#endif

static void afec0_eoc_0(void) { vdss_47V_raw = afec_channel_get_value(AFEC0, AFEC_CHANNEL_0);
}

static void afec0_eoc_1(void) { vbat_4V2_raw = afec_channel_get_value(AFEC0, AFEC_CHANNEL_1);
}

static void afec0_eoc_2(void) { voil_47V_raw = afec_channel_get_value(AFEC0, AFEC_CHANNEL_2);
}

static void afec0_eoc_3(void) { vreg_5V0_raw = afec_channel_get_value(AFEC0, AFEC_CHANNEL_3);
}


float aptAfec_get(uint8_t ch) {
	switch(ch) {
	case APT_AFEC_EXT_POWER_CH: return APT_AFEC_VIN_CAL(vdss_47V_raw,APT_AFEC_47V_DIVIDER); break;
	case APT_AFEC_BAT_POWER_CH: return APT_AFEC_VIN_CAL(vbat_4V2_raw,APT_AFEC_6V0_DIVIDER); break;
	case APT_AFEC_5V0_POWER_CH: return APT_AFEC_VIN_CAL(vreg_5V0_raw,APT_AFEC_6V0_DIVIDER); break;
	case APT_AFEC_EXT_INPUT_CH: return APT_AFEC_VIN_CAL(voil_47V_raw,APT_AFEC_47V_DIVIDER); break;
	default: return 0; break;
	}
	return 0;
}

uint32_t aptAfec_getRaw(uint8_t ch) {
	switch(ch) {
	case APT_AFEC_EXT_POWER_CH: return vdss_47V_raw; break;
	case APT_AFEC_BAT_POWER_CH: return vbat_4V2_raw; break;
	case APT_AFEC_5V0_POWER_CH: return vreg_5V0_raw; break;
	case APT_AFEC_EXT_INPUT_CH: return voil_47V_raw; break;
	default: return 0; break;
	}
	return 0;
}

float aptAfec_ConvertRaw(uint8_t ch, uint32_t rawADC) {
	float	vDevider = 0.0;
	switch(ch) {
	case APT_AFEC_EXT_POWER_CH: vDevider = APT_AFEC_47V_DIVIDER; break;
	case APT_AFEC_BAT_POWER_CH: vDevider = APT_AFEC_6V0_DIVIDER; break;
	case APT_AFEC_5V0_POWER_CH: vDevider = APT_AFEC_6V0_DIVIDER; break;
	case APT_AFEC_EXT_INPUT_CH: vDevider = APT_AFEC_47V_DIVIDER; break;
	default: return 0; break;
	}
	return  APT_AFEC_VIN_CAL(rawADC,vDevider);
}

void aptAfec_SetupADC(void) {
	static struct afec_config afec_cfg;
	static struct afec_ch_config afec_ch_cfg;

	afec_enable(AFEC0);
	afec_get_config_defaults(&afec_cfg);
	
	afec_cfg.resolution = AFEC_12_BITS;
	afec_cfg.mck = sysclk_get_cpu_hz();
	afec_cfg.afec_clock = 6000000UL;
	afec_cfg.startup_time = 4;
	afec_cfg.settling_time = 0;
	afec_cfg.tracktim = 2;
	afec_cfg.transfer = 1;
	afec_cfg.anach = true;
	afec_cfg.useq = false;
	afec_cfg.tag = true;
	afec_cfg.stm = true;
	afec_cfg.ibctl = 1;
	
	afec_init(AFEC0, &afec_cfg);
	
	afec_ch_get_config_defaults(&afec_ch_cfg);
	
	afec_ch_cfg.diff = false;
	afec_ch_cfg.gain = 1;
	
	
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_0, &afec_ch_cfg);
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_1, &afec_ch_cfg);
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_2, &afec_ch_cfg);
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_3, &afec_ch_cfg);
	
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_0, 0x800);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_1, 0x800);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_2, 0x800);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_3, 0x800);
	
	afec_set_trigger(AFEC0, AFEC_TRIG_SW);
	
	afec_channel_enable(AFEC0, AFEC_CHANNEL_0);
	afec_channel_enable(AFEC0, AFEC_CHANNEL_1);
	afec_channel_enable(AFEC0, AFEC_CHANNEL_2);
	afec_channel_enable(AFEC0, AFEC_CHANNEL_3);
	
	afec_configure_sequence(AFEC0, ch_list, 4);
	
	afec_set_callback(AFEC0, AFEC_INTERRUPT_EOC_0, afec0_eoc_0, 6);
	afec_set_callback(AFEC0, AFEC_INTERRUPT_EOC_1, afec0_eoc_1, 6);
	afec_set_callback(AFEC0, AFEC_INTERRUPT_EOC_2, afec0_eoc_2, 6);
	afec_set_callback(AFEC0, AFEC_INTERRUPT_EOC_3, afec0_eoc_3, 6);
	
	afec_start_calibration(AFEC0);
	//EOCAL bit in AFE_ISR register is set after the calibration is finished, it is checked with:
	while((afec_get_interrupt_status(AFEC0) & AFEC_ISR_EOCAL) != AFEC_ISR_EOCAL){vTaskDelay(1);}	
}

/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         UART RX Task                           */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
void aptAfec_task(void * pvParameters) {
	static uint32_t * pWdCount = NULL;
	pWdCount = (uint32_t*)pvParameters;
	
	aptAfec_SetupADC();
	
	for(;;) {
		
		*pWdCount = 0;//Clear watchdog
		
		afec_start_software_conversion(AFEC0);
		do { 
			vTaskDelay(10);
		}while(afec_get_interrupt_status(AFEC0) & (1 << AFEC_CHANNEL_3));
	}
}