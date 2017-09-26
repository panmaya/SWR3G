/*
 * conf_fm24clx.h
 *
 * Created: 17/10/2557 14:26:45
 *  Author: pan
 */ 


#ifndef CONF_FM24CLX_H_
#define CONF_FM24CLX_H_

/**
 * \name TWIx pin definitions
 * @{
 */
/*! TWI ID for FMRAM application to use */
#define BOARD_ID_TWI_FMRAM			ID_TWI0

/*! TWI Base for TWI FMRAM application to use */
#define BOARD_BASE_TWI_FMRAM		TWI0
#define BOARD_FM24C_TWI_INSTANCE	TWI0
#define BOARD_FM24C_ADDRESS			0x50

/*! TWI0 Data pin for FMRAM */
#define TWIO_DATA_GPIO				PIO_PA3_IDX
#define TWIO_DATA_FLAG				IOPORT_MODE_MUX_A
/*! TWI0 Clock pin for FMRAM */
#define TWIO_CLK_GPIO				PIO_PA4_IDX
#define TWIO_CLK_FLAG				IOPORT_MODE_MUX_A
#define BOARD_CLK_TWI_FMRAM			TWIO_CLK_GPIO
#define BOARD_CLK_TWI_MUX_FMRAM		TWIO_CLK_FLAG

#endif /* CONF_FM24CLX_H_ */