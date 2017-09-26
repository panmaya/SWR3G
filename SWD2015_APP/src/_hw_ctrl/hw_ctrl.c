/*
 * hw_ctrl.c
 *
 * Created: 20/3/2558 14:28:12
 *  Author: pan
 */ 
#include <adl_global.h>
#include "hw_ctrl.h"

void hw_ctrl_a2d_ResetBus(void)
{
	uint32_t i;
	/* MEM reset
	 * a) Clock up to 9 cycles (use 100K)
	 * b) look for SDA high in each cycle while SCL is high and then
	 * c) Create a start condition as SDA is high
	 */
	/* - Enable pin output mode */
	ioport_set_pin_dir(BOARD_CLK_TWI_A2D, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(BOARD_CLK_TWI_A2D, 1);
	ioport_enable_pin(BOARD_CLK_TWI_A2D);
	for (i = 0; i < 10; i ++) {
		delay_us(5);
		ioport_set_pin_level(BOARD_CLK_TWI_A2D, 0);
		delay_us(5);
		ioport_set_pin_level(BOARD_CLK_TWI_A2D, 1);
	}
	/* - Restore pin peripheral mode */
	ioport_set_pin_mode(BOARD_CLK_TWI_A2D, BOARD_CLK_TWI_MUX_A2D);
	ioport_disable_pin(BOARD_CLK_TWI_A2D);
	/* - Start condition will do on R/W start */
}

void hw_ctrl_gps_ModuleOn(void){
	/* Configure the pins connected to GPS  ON/OFF control
	 * by toggle OFF --> ON --> OFF
	 */
	
	ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_ON);
	delay_ms(600);
	ioport_set_pin_level(PIN_ONOFF_GPS, PIN_ONOFF_GPS_OFF);
}

void hw_ctrl_fram_ResetBus(void) {
	/* Reset TWI FMRAM state to release TWI */
	fm24cxx_reset();
}

void hw_ctrl_jtag_Disable(void) {
	MATRIX->CCFG_SYSIO |= (1 << 4) | (1 << 5);// disable JTAG
}

void hw_ctrl_3v3_Power(bool bEnable) {
	/* Configure the pins connected to 3.3v enable power
	 * default initial state to high (ON).
	 */
	ioport_set_pin_dir(PIN_EN_3V3PER, IOPORT_DIR_OUTPUT);
	if(bEnable) ioport_set_pin_level(PIN_EN_3V3PER, PIN_EN_3V3PER_ON);
	else ioport_set_pin_level(PIN_EN_3V3PER, PIN_EN_3V3PER_OFF);
}

void hw_ctrl_3v6_Reg(bool bExtPower) {	
	/* Configure the pins connected to 3.6v enable power
	 * default initial state to high (ON).
	 */
	ioport_set_pin_dir(PIN_EN_3V6MAN_REG, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_EN_3V6MAN_UNREG, IOPORT_DIR_OUTPUT);
	
	if(bExtPower) {
	ioport_set_pin_level(PIN_EN_3V6MAN_REG, PIN_EN_3V6MAN_ON);
	ioport_set_pin_level(PIN_EN_3V6MAN_UNREG, PIN_EN_3V6MAN_OFF);
	}
	else {
		ioport_set_pin_level(PIN_EN_3V6MAN_REG, PIN_EN_3V6MAN_OFF);//PIO_PA30_IDX,0
		ioport_set_pin_level(PIN_EN_3V6MAN_UNREG, PIN_EN_3V6MAN_ON);//PIO_PA29_IDX,1
	}
}

void hw_ctrl_at45_Reset(bool bEnable) {
	/* Configure the pins connected to AT45DB Flash reset control
	 * default initial state to high (ON).
	 */
	ioport_set_pin_dir(PIN_RST_AT45DB, IOPORT_DIR_OUTPUT);
	if(!bEnable) ioport_set_pin_level(PIN_RST_AT45DB, PIN_RST_AT45DB_ON);
	else ioport_set_pin_level(PIN_RST_AT45DB, PIN_RST_AT45DB_OFF);
}

void hw_ctrl_fram_Protect(bool bEnable) {
	/* Configure the pins connected to FM24clx FRAM Write Protect control
	 * default initial state to Low (OFF).
	 */
	ioport_set_pin_dir(PIN_WP_FM24CL, IOPORT_DIR_OUTPUT);
	if(!bEnable) ioport_set_pin_level(PIN_WP_FM24CL, PIN_WP_FM24CL_OFF);
	else  ioport_set_pin_level(PIN_WP_FM24CL, PIN_WP_FM24CL_ON);
}
	

void hw_ctrl_gps_Reset(bool bEnable) {
	/* Configure the pins connected to GPS  Reset control
	 * default initial state to Low (OFF).
	 */
	ioport_set_pin_dir(PIN_RST_GPS, IOPORT_DIR_OUTPUT);
	if(!bEnable) ioport_set_pin_level(PIN_RST_GPS, PIN_RST_GPS_OFF);
	else ioport_set_pin_level(PIN_RST_GPS, PIN_RST_GPS_ON);
}

void hw_ctrl_gps_Enable(bool bEnable) {
	/* Configure the pins connected to GPS  Enable control
	 * default initial state to High (ON).
	 */
	ioport_set_pin_dir(PIN_EN_GPS, IOPORT_DIR_OUTPUT);
	if(bEnable) ioport_set_pin_level(PIN_EN_GPS, PIN_EN_GPS_ON);
	else ioport_set_pin_level(PIN_EN_GPS, PIN_EN_GPS_OFF);
}

void hw_ctrl_gps_AntPower(bool bEnable) {
	/* Configure the pins connected to GPS  EXT Antenna control
	 * default initial state to High (ON).
	 */
	ioport_set_pin_dir(PIN_EXTANT_GPS, IOPORT_DIR_OUTPUT);
	if(bEnable) ioport_set_pin_level(PIN_EXTANT_GPS, PIN_EXTANT_GPS_ON);
	else ioport_set_pin_level(PIN_EXTANT_GPS, PIN_EXTANT_GPS_OFF);
}

bool hw_ctrl_gps_Wakeup(void) {
	//ioport_set_pin_dir(PIN_WAKEUP_GPS, IOPORT_DIR_INPUT);
	return ioport_get_pin_level(PIN_WAKEUP_GPS);
}
	
void hw_ctrl_gsm_Reset(bool bEnable) {
	/* Configure the pins connected to GSM  Reset control
	 * default initial state to Low (OFF).
	 */
	ioport_set_pin_dir(PIN_RST_GSM, IOPORT_DIR_OUTPUT);
	if(!bEnable) ioport_set_pin_level(PIN_RST_GSM, PIN_RST_GSM_OFF);
	else ioport_set_pin_level(PIN_RST_GSM, PIN_RST_GSM_ON);
}
	
void hw_ctrl_gsm_Power(bool bEnable) {
	/* Configure the pins connected to Power On control
	 * default initial state to Low (OFF).
	 */
	ioport_set_pin_dir(PIN_PWRON_GSM, IOPORT_DIR_OUTPUT);
	if(bEnable)ioport_set_pin_level(PIN_PWRON_GSM, PIN_PWRON_GSM_ON);
	else ioport_set_pin_level(PIN_PWRON_GSM, PIN_PWRON_GSM_OFF);
}

void hw_ctrl_p5v_Power(bool bEnable) {
	/* Configure the pins connected to external 5v enable power
	 * default initial state to high (ON).
	 */
	ioport_set_pin_dir(PIN_EN_P5VPERMAN, IOPORT_DIR_OUTPUT);
	if(bEnable) ioport_set_pin_level(PIN_EN_P5VPERMAN, PIN_EN_P5VPERMAN_ON);
	else ioport_set_pin_level(PIN_EN_P5VPERMAN, PIN_EN_P5VPERMAN_OFF);
}

void hw_ctrl_wd_Toggle(void) {
	//static bool logic = 0;
	//if(logic)ioport_set_pin_level(PIN_WD_EXT, PIN_WD_EXT_ON);
	//else ioport_set_pin_level(PIN_WD_EXT, PIN_WD_EXT_OFF);
	//logic = !logic;
	ioport_toggle_pin_level(PIN_WD_EXT);
}

void hw_ctrl_wd_Disable(void) {
	ioport_set_pin_dir(PIN_WD_EXT, IOPORT_DIR_INPUT);
}

void hw_ctrl_wd_Enable(void) {
	ioport_set_pin_dir(PIN_WD_EXT, IOPORT_DIR_OUTPUT);
}