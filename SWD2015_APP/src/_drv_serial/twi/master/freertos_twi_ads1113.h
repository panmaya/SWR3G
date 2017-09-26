/*
 * freertos_twi_ads1113.h
 *
 * Created: 22/10/2557 11:30:55
 *  Author: pan
 */ 


#ifndef FREERTOS_TWI_ADS1113_H_
#define FREERTOS_TWI_ADS1113_H_

#include <_drv_serial/twi/master/common/freertos_twi_master.h>

#define  ARC_RET_ERR_A2D_PARAM							ADL_RET_ERR_PARAM				//-2
#define  ARC_RET_ERR_A2D_UNKNOWN_HDL					ADL_RET_ERR_UNKNOWN_HDL			//-3
#define  ARC_RET_ERR_A2D_ALREADY_SUBSCRIBED				ADL_RET_ERR_ALREADY_SUBSCRIBED	//-4
#define  ARC_RET_ERR_A2D_NOT_SUBSCRIBED					ADL_RET_ERR_NOT_SUBSCRIBED		//-5
#define  ARC_RET_ERR_A2D_BAD_STATE						ADL_RET_ERR_BAD_STATE			//-8

#define  ARC_RET_ERR_A2D_EMPTY_BUFFER					-101
#define  ARC_RET_ERR_A2D_FULL_BUFFER					-102

#define  ARC_RET_ERR_A2D_SUBSCRIBE_I2C_SUBSCRIBE		-121
#define  ARC_RET_ERR_A2D_SUBSCRIBE_A2D_NOT_EXIST		-122
#define  ARC_RET_ERR_A2D_BREAK							-123
#define  ARC_RET_ERR_A2D_WRITE							-124
#define  ARC_RET_ERR_A2D_READ							-125
#define  ARC_RET_ERR_A2D_OUT_OF_RANGE					-126

/* Clock speed to use. */
#define FREERTOS_TWI_ADS1113_CLOCK_HZ					400000// 100KHz,400kHz,max = 3.5MHz

status_code_t freertos_twi_ads1113_InitReg (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in ADC to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in ADC */			uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
);

status_code_t freertos_twi_ads1113_read (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in ADC to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in ADC */			uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
);

freertos_twi_if freertos_twi_ads1113_init(Twi *twi_base);


#endif /* FREERTOS_TWI_ADS1113_H_ */