/*
 * buffer_gpbm.h
 *
 * Created: 2016-12-08 11:47:05 AM
 *  Author: Panmaya
 */ 


#ifndef BUFFER_GPBM_H_
#define BUFFER_GPBM_H_


#define GPBM_LICENSE_CARD_ADDR				0 // 0 -  1  -  2

#define GPBM_LICENSE_TRK3_1_ADDR			3
#define GPBM_LICENSE_TRK3_2_ADDR			4
#define GPBM_LICENSE_TRK3_3_ADDR			5
#define GPBM_LICENSE_TRK3_4_ADDR			6
#define GPBM_LICENSE_TRK3_5_ADDR			7
#define GPBM_LICENSE_TRK3_6_ADDR			8
#define GPBM_LICENSE_TRK3_7_ADDR			9

#define GPBM_LOGIN_LOGOUT_1_ADDR			10 // 10  -  11  - 12   -  13
#define GPBM_LOGIN_LOGOUT_2_ADDR			14 // 14  -  15  - 16   -  17
#define GPBM_LOGIN_LOGOUT_3_ADDR			18 // 18  -  19  - 20   -  21
#define GPBM_LOGIN_LOGOUT_4_ADDR			22 // 22  -  23  - 24   -  25
#define GPBM_LOGIN_LOGOUT_5_ADDR			26 // 26  -  27  - 28   -  29

#define GPBM_LICENSE_TRK3_FIRST_ADDR		GPBM_LICENSE_TRK3_1_ADDR
#define GPBM_LICENSE_TRK3_LAST_ADDR			GPBM_LICENSE_TRK3_7_ADDR

#define GPBM_LOGIN_LOGOUT_FIRST_ADDR		GPBM_LOGIN_LOGOUT_1_ADDR
#define GPBM_LOGIN_LOGOUT_LAST_ADDR			GPBM_LOGIN_LOGOUT_5_ADDR


#define GPBM_LICENSE_TRK3_MAX_ID			((GPBM_LICENSE_TRK3_LAST_ADDR - GPBM_LICENSE_TRK3_FIRST_ADDR) + 1)
#define GPBM_LOGIN_LOGOUT_MAX_ID			(((GPBM_LOGIN_LOGOUT_LAST_ADDR - GPBM_LOGIN_LOGOUT_FIRST_ADDR)/4) + 1)

#define	GPBM_LICENSE_TRK3_SIZE				64
#define GPBM_DRIVER_WORK_HOUR_OFFSET		GPBM_LICENSE_TRK3_SIZE

s32 buffer_gpbm_write_offset(uint8_t addr, uint8_t offset, uint8_t *p_uc, uint16_t len);

//s32 buffer_gpbm_write(uint8_t addr, uint8_t *p_uc, uint16_t len);

//s32 buffer_gpbm_read(uint8_t addr, uint8_t offset, uint8_t *p_uc, uint16_t len);

s32 buffer_gpbm_read_offset(uint8_t addr, uint8_t offset, uint8_t *p_uc, uint16_t len);

s32 buffer_gpbm_erase(uint8_t addr);

bool buffer_gpbm_exist(uint8_t addr);

s32 buffer_gpbm_getsn(uint8_t addr);

s32 buffer_gpbm_create(xSemaphoreHandle sem);

#define buffer_gpbm_write(_a,_p,_l)	buffer_gpbm_write_offset(_a,0,_p,_l)
#define buffer_gpbm_read(_a,_p,_l)	buffer_gpbm_read_offset(_a,0,_p,_l)

#endif /* BUFFER_GPBM_H_ */