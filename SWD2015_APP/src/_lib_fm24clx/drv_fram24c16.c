/*
 * drv_fram24c16.c
 *
 *  Created on: 21 ¾.¤. 2554
 *      Author: Pan
 */

#include "adl_global.h"
#include "drv_fram24c16.h"

/*
 * FM24C16 I2C F-RAM Factory's structure*
 *
 * +<---------------------------------------------  2K Bytes ----------------------------------------------------->+
 * +-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
 * |<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|
 * |-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
 * | ADDR = 0x50 | ADDR = 0x51 | ADDR = 0x52 | ADDR = 0x53 | ADDR = 0x54 | ADDR = 0x55 | ADDR = 0x56 | ADDR = 0x57 |
 * +---------------------------------------------------------------------------------------------------------------+
*/

static freertos_twi_if FRam_hBus = NULL;

s32 drv_fram24c16_Read(u32 addr, u32 size, u8* ptrData) {
	status_code_t retCode = STATUS_OK;
	retCode = freertos_twi_fm24c_read(FRam_hBus,addr,(uint8_t*)ptrData,size);
	return (retCode == STATUS_OK) ? OK : ERROR;
}

s32 drv_fram24c16_Write(u32 addr, u32 size, u8* ptrData) {
	status_code_t retCode = STATUS_OK;
	retCode = freertos_twi_fm24c_write(FRam_hBus,addr,(uint8_t*)ptrData,size);
	return (retCode == STATUS_OK) ? OK : ERROR;
}


s32 drv_fram24c16_Subscribe(u16 size) {
	if(!FRam_hBus) FRam_hBus = freertos_twi_fm24c_init(BOARD_BASE_TWI_FMRAM);
	return OK;
}



