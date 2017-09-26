/*
 * freertos_twi_lsm6ds3.h
 *
 * Created: 2/19/2016 3:40:32 PM
 *  Author: Panmaya
 */ 


#ifndef FREERTOS_TWI_LSM6DS3_H_
#define FREERTOS_TWI_LSM6DS3_H_

#include <_drv_serial/twi/master/common/freertos_twi_master.h>

#define  ARC_RET_ERR_LSM6DS3_PARAM							ADL_RET_ERR_PARAM				//-2
#define  ARC_RET_ERR_LSM6DS3_UNKNOWN_HDL					ADL_RET_ERR_UNKNOWN_HDL			//-3
#define  ARC_RET_ERR_LSM6DS3_ALREADY_SUBSCRIBED				ADL_RET_ERR_ALREADY_SUBSCRIBED	//-4
#define  ARC_RET_ERR_LSM6DS3_NOT_SUBSCRIBED					ADL_RET_ERR_NOT_SUBSCRIBED		//-5
#define  ARC_RET_ERR_LSM6DS3_BAD_STATE						ADL_RET_ERR_BAD_STATE			//-8

#define  ARC_RET_ERR_LSM6DS3_EMPTY_BUFFER					-101
#define  ARC_RET_ERR_LSM6DS3_FULL_BUFFER					-102

#define  ARC_RET_ERR_LSM6DS3_SUBSCRIBE_I2C_SUBSCRIBE		-121
#define  ARC_RET_ERR_LSM6DS3_SUBSCRIBE_LSM6DS3_NOT_EXIST	-122
#define  ARC_RET_ERR_LSM6DS3_BREAK							-123
#define  ARC_RET_ERR_LSM6DS3_WRITE							-124
#define  ARC_RET_ERR_LSM6DS3_READ							-125
#define  ARC_RET_ERR_LSM6DS3_OUT_OF_RANGE					-126

/* Clock speed to use. */
#define FREERTOS_TWI_LSM6DS3_CLOCK_HZ						400000//max = 400KHz

status_code_t freertos_twi_lsm6ds3_write (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in ACCL to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in ACCL */		uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
);

status_code_t freertos_twi_lsm6ds3_read (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in ACCL to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in ACCL */		uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
);

freertos_twi_if freertos_twi_lsm6ds3_init(Twi *twi_base);



#endif /* FREERTOS_TWI_LSM6DS3_H_ */