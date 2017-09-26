/*
 * test_qc.c
 *
 * Created: 7/6/2016 3:19:11 PM
 *  Author: Panmaya
 */ 


#include "adl_global.h"
#include "apt_tracking.h"
#include "apt_gateway.h"
#include "apt_swd.h"
#include "fat_system.h"
#include "apt_gps_data.h"
#include "asf.h"
#include "apt_afec.h"
#include "ini_config.h"
#include "buffer.h"
#include "vprinter.h"
#include "_lib_arcnethl/arc_hlgsm.h"
#include "_lib_arcnethl/arc_hltcp.h"
#include "apt_afec.h"
#include "apt_card.h"
#include "card_authen.h"
#include "../_lib_bootld/boot_trigger.h"
#include "misc_config.h"
#include "xm0110_compat.h"
#include "feature.h"
#include "apt_debug_port.h"
#include "test_qc.h"

void test_qc(void *pvParameters) {
	static u8 gpo_mask = 0x01;
	static u8 led_mask = 0x01;
	static u8 p5vman = 0;
	
	p5vman = p5vman ? 0 : 1;
	hw_ctrl_p5v_Power(p5vman);
	
	ioport_set_pin_level(LED_MCU, led_mask & 0x01 ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
	ioport_set_pin_level(LED_GSM, led_mask & 0x02 ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
	ioport_set_pin_level(LED_OPT, led_mask & 0x04 ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
	ioport_set_pin_level(LED_GPS, led_mask & 0x08 ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
	ioport_set_pin_level(LED_SEND, led_mask & 0x10 ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
	ioport_set_pin_level(LED_IG,  led_mask & 0x20 ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
	ioport_set_pin_level(LED_PTO, led_mask & 0x40 ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
	
	led_mask = (led_mask == 0x40) ? 0x01 : led_mask << 1;
	
	vPuts("\r\n\r\n");
	
	#if(1)
	//uint8_t timeout = 0;
	uint8_t buffer = 0x00;
	lis3dhData_t  g_data = {0,0,0};
	/* Check the status register until a new X/Y/Z sample is ready */
	do
	{
		freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_STATUS_REG_AUX,(uint8_t*)&buffer,1);
		//if(timeout > LIS3DH_POLL_TIMEOUT) break;
		//vTaskDelay(100);
		//timeout++;
		break;
	} while (!(buffer & LIS3DH_STATUS_REG_ZYXDA));

	// /* Check if the data is new or not (data has changed) */
	// if (buffer & LIS3DH_STATUS_REG_ZYXOR)
	// {
	//   /* New data is available */
	// }

	/* For now, always read data even if it hasn't changed */
	uint8_t gbuff[6];
	freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_OUT_X_L,(uint8_t*)&gbuff[0],1);
	freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_OUT_X_H,(uint8_t*)&gbuff[1],1);
	freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_OUT_Y_L,(uint8_t*)&gbuff[2],1);
	freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_OUT_Y_H,(uint8_t*)&gbuff[3],1);
	freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_OUT_Z_L,(uint8_t*)&gbuff[4],1);
	freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_OUT_Z_H,(uint8_t*)&gbuff[5],1);
	
	/* Shift values to create properly formed integer (low byte first) */
	g_data.x = (int16_t)(((uint16_t)gbuff[1]) << 8 | gbuff[0]);
	g_data.y = (int16_t)(((uint16_t)gbuff[3]) << 8 | gbuff[2]);
	g_data.z = (int16_t)(((uint16_t)gbuff[5]) << 8 | gbuff[4]);

	lvPrintf("\r\n#MCU,ID:%s,WD:%d,CREG:%d,CSQ:%d,FRAM:%d,GX:%d,GY:%d,GZ:%d,OAT:%d,@",
	arc_netimei,
	WDOG_TestPassed,
	arc_netsys.bGsmRegistered,
	arc_netsys.signLev,
	FRAM_TestPassed,
	g_data.x,
	g_data.y,
	g_data.z,
	arc_netsys.modemAppVersion);
	#endif
	
	#if(1)
	lvPrintf("\r\n#ADC,OOVP:%.2f,ADCIN:%.2f,4V2:%.2f,3v6:%.2f,5V0:%.2f,@",
	aptAfec_get(APT_AFEC_EXT_POWER_CH)/1000,
	aptAfec_get(APT_AFEC_EXT_INPUT_CH)/1000,
	aptAfec_get(APT_AFEC_BAT_POWER_CH)/1000,
	aptAfec_get(APT_AFEC_3V6_POWER_CH)/1000,
	aptAfec_get(APT_AFEC_5V0_POWER_CH)/1000);
	#endif
	
	#if(1)
	ioport_set_pin_level(EXT_OUT1,		gpo_mask & 0x01 ? LED_INACTIVE_LEVEL : LED_ACTIVE_LEVEL);
	ioport_set_pin_level(EXT_OUT2,		gpo_mask & 0x02 ? LED_INACTIVE_LEVEL : LED_ACTIVE_LEVEL);
	ioport_set_pin_level(EXT_OPT1,		gpo_mask & 0x04 ? LED_INACTIVE_LEVEL : LED_ACTIVE_LEVEL);
	ioport_set_pin_level(PIN_GPIO_0,	gpo_mask & 0x08 ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
	ioport_set_pin_level(PIN_GPIO_1,	gpo_mask & 0x10 ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
	
	lvPrintf("\r\n#IO,IG:%d,IN1:%d,IN2:%d,CLR:%d,OUT1:%d,OUT2:%d,OPT1:%d,MCU1:%d,MCU2:%d,@",
	ioport_get_pin_level(EXT_IG),
	ioport_get_pin_level(EXT_IN1),
	ioport_get_pin_level(EXT_IN2),
	ioport_get_pin_level(EXT_CLRBUF),
	gpo_mask & 0x01 ? 1 :0,
	gpo_mask & 0x02 ? 1 :0,
	gpo_mask & 0x04 ? 1 :0,
	gpo_mask & 0x08 ? 1 :0,
	gpo_mask & 0x10 ? 1 :0);
	
	gpo_mask = (gpo_mask == 0x10) ? 0x01 : gpo_mask << 1;
	#endif
	
	#if(1)
	gps_pvtInfo_t gpsinfo;
	aptGpsData_getInfo(&gpsinfo);
	lvPrintf("\r\n#GPS,FIX:%d,ANT:%d,SV:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,@",
	gpsinfo.tPositionInfo.eFixState,1,
	gpsinfo.tSatellitesInfo.satInViewInfo[0].statsInViewSNR,
	gpsinfo.tSatellitesInfo.satInViewInfo[1].statsInViewSNR,
	gpsinfo.tSatellitesInfo.satInViewInfo[2].statsInViewSNR,
	gpsinfo.tSatellitesInfo.satInViewInfo[3].statsInViewSNR,
	gpsinfo.tSatellitesInfo.satInViewInfo[4].statsInViewSNR,
	gpsinfo.tSatellitesInfo.satInViewInfo[5].statsInViewSNR,
	gpsinfo.tSatellitesInfo.satInViewInfo[6].statsInViewSNR,
	gpsinfo.tSatellitesInfo.satInViewInfo[7].statsInViewSNR,
	gpsinfo.tSatellitesInfo.satInViewInfo[8].statsInViewSNR,
	gpsinfo.tSatellitesInfo.satInViewInfo[9].statsInViewSNR);
	#endif
}