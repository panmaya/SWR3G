/*
 * freertos_twi_fm24c.h
 *
 * Created: 22/10/2557 11:30:55
 *  Author: pan
 */ 


#ifndef FREERTOS_TWI_FM24C_H_
#define FREERTOS_TWI_FM24C_H_

#include <_drv_serial/twi/master/common/freertos_twi_master.h>

#define TWI_FRAM24C64_USED
//#define TWI_FRAM24C16_USED

#define  ARC_RET_ERR_FRAM_PARAM							ADL_RET_ERR_PARAM				//-2
#define  ARC_RET_ERR_FRAM_UNKNOWN_HDL					ADL_RET_ERR_UNKNOWN_HDL			//-3
#define  ARC_RET_ERR_FRAM_ALREADY_SUBSCRIBED			ADL_RET_ERR_ALREADY_SUBSCRIBED	//-4
#define  ARC_RET_ERR_FRAM_NOT_SUBSCRIBED				ADL_RET_ERR_NOT_SUBSCRIBED		//-5
#define  ARC_RET_ERR_FRAM_BAD_STATE						ADL_RET_ERR_BAD_STATE			//-8

#define  ARC_RET_ERR_FRAM_EMPTY_BUFFER					-101
#define  ARC_RET_ERR_FRAM_FULL_BUFFER					-102

#define  ARC_RET_ERR_FRAM_SUBSCRIBE_I2C_SUBSCRIBE		-121
#define  ARC_RET_ERR_FRAM_SUBSCRIBE_FRAM_NOT_EXIST		-122
#define  ARC_RET_ERR_FRAM_BREAK							-123
#define  ARC_RET_ERR_FRAM_WRITE							-124
#define  ARC_RET_ERR_FRAM_READ							-125
#define  ARC_RET_ERR_FRAM_OUT_OF_RANGE					-126

/* Clock speed to use. */
#define FREERTOS_TWI_FM24C_CLOCK_HZ						400000//100000//400000//(sam4 not support 1MHz)

#define FREERTOS_TWI_FM24C_INTERRUPT_PRIORITY			(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)

status_code_t freertos_twi_fm24c_write (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in FRAM to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in FRAM */			uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
);

status_code_t freertos_twi_fm24c_read (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in FRAM to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in FRAM */			uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
);

freertos_twi_if freertos_twi_fm24c_init(Twi *twi_base);


#endif /* FREERTOS_TWI_FM24C_H_ */