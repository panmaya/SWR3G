/*
 * buffer_gpbm.c
 *
 * Created: 2016-12-08 11:46:40 AM
 *  Author: Panmaya
 */ 
#include "adl_global.h"
#include "buffer_gpbm.h"
#include "buffer.h"

/*
*##########################################################
|                                 F-RAM 24C16                                                |
*##########################################################
###########################################################
/                              General Purpose F-RAM                                                  /
###########################################################
*+------------------- +-------------------------------+----------------------+----------
*| 0   ------>     |   FULL LICENSE [350]  |    Reserve [34]
*+-------------------+--------------------------------+-------------------+----------
*+----------------+---------------------------------+-------------------------+----------------------+
*| 1   ------>   |     SN-A[4]       |   TRACK3.1[64]   |    Reserve [60]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 2   ------>   |     SN-A[4]       |   TRACK3.2[64]   |    Reserve [60]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 3   ------>   |     SN-A[4]      |   TRACK3.3[64]   |    Reserve [60]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 4   ------>   |     SN-A[4]       |   TRACK3.4[64]   |    Reserve [60]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 5   ------>   |     SN-A[4]      |   TRACK3.5[64]   |    Reserve [60]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 6   ------>   |     SN-A[4]       |   TRACK3.6[64]   |    Reserve [60]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 7   ------>   |    SN-A[4]       |   TRACK3.7[64]   |    Reserve [60]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 8   ------>   |   SN-B[4]    |      Login - Logout[508]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 9  ------>   |    SN-B[4]    |      Login - Logout[508]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 10   ------>   |   SN-B[4]    |      Login - Logout[508]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 11   ------>   |    SN-B[4]    |      Login - Logout[508]
*+---------------+--------------------------------+--------------------------+----------------------+
*+----------------+---------------------------------+-------------------------+----------------------+
*| 12   ------>   |   SN-B[4]    |      Login - Logout[508]
*+---------------+--------------------------------+--------------------------+----------------------+
*/

#define _BUFFER_GPBM_CAPACITY		(8192 /*8K BYTE for fm24cl16*/ / 2) /*Half reserved for the other purpose*/
#define	_BUFFER_GPBM_START_ADDR		_BUFFER_GPBM_CAPACITY

static xSemaphoreHandle buff_sem = NULL;
static const portTickType sem_block_wait = portMAX_DELAY;//5000UL / portTICK_RATE_MS;
static uint8_t* pBuff_stack = &buffer_accessCentralStack[0];
static uint32_t GPBM_iSNA = 0;
static uint32_t GPBM_iSNB = 0;
static uint8_t  GPBM_NULL = 0;

/* Erase*/
s32 buffer_gpbm_erase(uint8_t addr) {
	s32 ret = ERROR;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		memset(pBuff_stack,0,128);
		if(addr == GPBM_LICENSE_CARD_ADDR) {
			for(uint8_t i = addr; i < (addr+3) ; i++) drv_fram24c16_Write((i * 128) + _BUFFER_GPBM_START_ADDR,128, pBuff_stack);
		}
		else if((addr >= GPBM_LICENSE_TRK3_FIRST_ADDR)&&(addr <= GPBM_LICENSE_TRK3_LAST_ADDR)) {
			drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR,128, pBuff_stack);
		}
		else if((addr >= GPBM_LOGIN_LOGOUT_FIRST_ADDR)&&(addr <= GPBM_LOGIN_LOGOUT_LAST_ADDR)) {
			for(uint8_t i = addr; i < (addr+4) ; i++) drv_fram24c16_Write((i * 128) + _BUFFER_GPBM_START_ADDR,128, pBuff_stack);
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nGPBM Erase Sem Locked\r\n");
	return ret;
}

/* Write*/
s32 buffer_gpbm_write_offset(uint8_t addr, uint8_t offset, uint8_t *p_uc, uint16_t len) {
	s32 ret = ERROR;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if((addr>=GPBM_LICENSE_TRK3_FIRST_ADDR)&&(addr <=GPBM_LOGIN_LOGOUT_LAST_ADDR)) {
			if(addr <= GPBM_LICENSE_TRK3_LAST_ADDR) {
				GPBM_iSNA++;
				
				memcpy(pBuff_stack,(u8*)&GPBM_iSNA,4);
				len = len > (123-offset) ? (123-offset) : len;
				memcpy(pBuff_stack+4,(u8*)p_uc,len);
				pBuff_stack[4+len] = GPBM_NULL;
				drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + offset, 4+len, (u8*)pBuff_stack);
				
				//ret = drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + offset,4, (u8*)&GPBM_iSNA);
				//len = len > (123-offset) ? (123-offset) : len;
				//ret = drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + 4 + offset, len, p_uc);
				//ret = drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + 4 + len + offset, 1, &GPBM_NULL);
			}
			else {
				GPBM_iSNB++;
				
				memcpy(pBuff_stack,(u8*)&GPBM_iSNB,4);
				len = len > (507-offset) ? (507-offset) : len;
				memcpy(pBuff_stack+4,(u8*)p_uc,len);
				pBuff_stack[4+len] = GPBM_NULL;
				drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + offset, 4+len, (u8*)pBuff_stack);
				
				//ret = drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + offset,4, (u8*)&GPBM_iSNB);
				//len = len > (507-offset) ? (507-offset) : len;
				//ret = drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + 4 + offset, len, p_uc);
				//ret = drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + 4 + len + offset, 1, &GPBM_NULL);
			}
		}
		else {
			len = len > (383-offset) ? (383-offset) : len;
			memcpy(pBuff_stack,(u8*)p_uc,len);
			pBuff_stack[len] = GPBM_NULL;
			drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + offset, len, (u8*)pBuff_stack);
			
			//len = len > (383-offset) ? (383-offset) : len;
			//ret = drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + offset, len , p_uc);
			//ret = drv_fram24c16_Write((addr * 128) + _BUFFER_GPBM_START_ADDR + len + offset, 1, &GPBM_NULL);
		}
		
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nGPBM Write Sem Locked\r\n");
	return ret;
}

/* Read Offset*/
s32 buffer_gpbm_read_offset(uint8_t addr, uint8_t offset, uint8_t *p_uc, uint16_t len) {
	s32 ret = ERROR;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if((addr>=GPBM_LICENSE_TRK3_FIRST_ADDR)&&(addr <=GPBM_LOGIN_LOGOUT_LAST_ADDR)) {
			if(addr <= GPBM_LICENSE_TRK3_LAST_ADDR) {
				ret = drv_fram24c16_Read((addr * 128) + _BUFFER_GPBM_START_ADDR + 4 + offset,len > (124-offset) ? (124-offset) : len, p_uc);
			}
			else {
				ret = drv_fram24c16_Read((addr * 128) + _BUFFER_GPBM_START_ADDR + 4 + offset,len > (508-offset) ? (508-offset) : len, p_uc);
			}
		}else ret = drv_fram24c16_Read((addr * 128) + _BUFFER_GPBM_START_ADDR + offset,len > (384-offset) ? (384-offset) : len, p_uc);
		
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nGPBM Read Sem Locked\r\n");
	return ret;
}



/* Get Serial Number */
s32 buffer_gpbm_getsn(uint8_t addr){
	uint32_t buf = 0x00;
	s32 ret = ERROR;
	if((addr<GPBM_LICENSE_TRK3_FIRST_ADDR)||(addr > GPBM_LOGIN_LOGOUT_LAST_ADDR))return ERROR;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		ret = drv_fram24c16_Read((addr * 128) + _BUFFER_GPBM_START_ADDR,4, (u8*)&buf);
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nGPBM Get Serial Sem Locked\r\n");
	return buf;
}

/* Check Item exist */
bool buffer_gpbm_exist(uint8_t addr) {
	uint8_t buf = 0x00;
	s32 ret = ERROR;
	if(addr > GPBM_LOGIN_LOGOUT_LAST_ADDR)return ERROR;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		ret = drv_fram24c16_Read((addr * 128) + _BUFFER_GPBM_START_ADDR,1, (u8*)&buf);
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nGPBM Exist Sem Locked\r\n");
	return buf;
}


/* Initial */
s32 buffer_gpbm_create(xSemaphoreHandle sem) {
	s32 ret = ERROR;
	if(!buff_sem) {
		buff_sem = sem;
		ret = drv_fram24c16_Subscribe(0);
		uint32_t tmp = 0;
		for(uint8_t i = GPBM_LICENSE_TRK3_FIRST_ADDR; i <= GPBM_LICENSE_TRK3_LAST_ADDR ; i++) {
			tmp = buffer_gpbm_getsn(i);
			if(tmp > GPBM_iSNA) GPBM_iSNA = tmp;//Find latest Track3 Serial Number
		}
		for(uint8_t i = GPBM_LOGIN_LOGOUT_FIRST_ADDR; i <= GPBM_LOGIN_LOGOUT_LAST_ADDR ; i+=4) {
			tmp = buffer_gpbm_getsn(i);
			if(tmp > GPBM_iSNB) GPBM_iSNB = tmp;//Find latest logging Serial Number
		}
	}
	return ret;
}