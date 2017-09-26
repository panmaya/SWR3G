/*
 * conf_bmg160.h
 *
 * Created: 13/1/2558 14:51:56
 *  Author: pan
 */ 


#ifndef CONF_BMI055_H_
#define CONF_BMI055_H_


/**
* \name TWIx pin definitions
* @{
	*/
/*! TWI ID for GYRO application to use */
#define BOARD_ID_TWI_GYRO			ID_TWI1

/*! TWI Base for TWI GYRO application to use */
#define BOARD_BASE_TWI_GYRO			TWI1
#define BOARD_BMI055_TWI_INSTANCE	TWI1
// SDO 1 and 2 pulled to GND
#define BOARD_BMI055_GYRO_ADDRESS	0x68 
#define BOARD_BMI055_ACCEL_ADDRESS	0x18
// SDO 1 and 2 pulled to GND
//#define BOARD_BMI055_GYRO_ADDRESS		0x69
//#define BOARD_BMI055_ACCEL_ADDRESS	0x19


/*! TWI1 Data pin for GYRO */
#define TWI1_DATA_GPIO				PIO_PB4_IDX
#define TWI1_DATA_FLAG				IOPORT_MODE_MUX_A
/*! TWI1 Clock pin for GYRO */
#define TWI1_CLK_GPIO				PIO_PB5_IDX
#define TWI1_CLK_FLAG				IOPORT_MODE_MUX_A
#define BOARD_CLK_TWI_GYRO			TWI1_CLK_GPIO
#define BOARD_CLK_TWI_MUX_GYRO		TWI1_CLK_FLAG


#define BMI055_NAME							"BMI055"

// Register for GYRO
#define BMI055_GYR_HW_ADDR					BOARD_BMI055_GYRO_ADDRESS
#define BMI055_GYR_CHIP_ID_REG				0x00
//#define BMI055_GYR_RESERV1_REG				0x01
#define BMI055_GYR_X_AXIS_LSB_REG			0x02
#define BMI055_GYR_X_AXIS_MSB_REG			0x03
#define BMI055_GYR_Y_AXIS_LSB_REG			0x04
#define BMI055_GYR_Y_AXIS_MSB_REG			0x05
#define BMI055_GYR_Z_AXIS_LSB_REG			0x06
#define BMI055_GYR_Z_AXIS_MSB_REG			0x07
//#define BMI055_GYR_RESERV2_REG				0x08
#define BMI055_GYR_INT0_REG					0x09
#define BMI055_GYR_INT1_REG					0x0A
#define BMI055_GYR_INT2_REG					0x0B
#define BMI055_GYR_INT3_REG					0x0C
//#define BMI055_GYR_RESERV3_REG				0x0D
#define BMI055_GYR_FIFO_REG					0x0E
#define BMI055_GYR_RANGE_REG				0x0F
#define BMI055_GYR_UR_BW_SEL_REG			0x10
#define BMI055_GYR_MODE_CTRL_REG			0x11
#define BMI055_GYR_RESET_REG				0x14

#define BMI055_GYR_OFC1_REG					0x36
#define BMI055_GYR_OFC2_REG					0x37
#define BMI055_GYR_OFC3_REG					0x38
#define BMI055_GYR_OFC4_REG					0x39
#define BMI055_GYR_TRIMGP0_REG				0x3A
#define BMI055_GYR_FIFO_REG0				0x3D
#define BMI055_GYR_FIFO_REG1				0x3E
#define BMI055_GYR_LAST_REG					0x3F


#define BMI055_GYR_RANGE_MASK				0x07
#define BMI055_GYR_RANGE_2000				0x00
#define BMI055_GYR_RANGE_1000				0x01
#define BMI055_GYR_RANGE_500				0x02
#define BMI055_GYR_RANGE_250				0x03
#define BMI055_GYR_RANGE_125				0x04


#define BMI055_GYR_UR_BW_100HZ_32HZ			0x07
#define BMI055_GYR_UR_BW_200HZ_64HZ			0x06
#define BMI055_GYR_UR_BW_100HZ_12HZ			0x05
#define BMI055_GYR_UR_BW_200HZ_23HZ			0x04
#define BMI055_GYR_UR_BW_400HZ_47HZ			0x03
#define BMI055_GYR_UR_BW_1000HZ_116HZ		0x02
#define BMI055_GYR_UR_BW_2000HZ_230HZ		0x01
#define BMI055_GYR_UR_BW_2000HZ_583HZ		0x00

#define BMI055_GYR_MODE_SUSPEND				0x80
#define BMI055_GYR_MODE_DEEPSUSPEND			0x20

#define BMI055_GYR_RESET					0xB6
#define BMI055_GYR_SOC						0x31

#define BMI055_GYR_FIFO_READOUT				0x3F
#define BMI055_GYR_FIFO_COUNT				0x0E


#define BMI055_GYR_RANGE					BMI055_GYR_RANGE_250

#if(BMI055_GYR_RANGE == BMI055_GYR_RANGE_500)
#define BMI055_GYR_SCALE					50
#endif
#if(BMI055_GYR_RANGE == BMI055_GYR_RANGE_250)
#define BMI055_GYR_SCALE					250
#endif
#if(BMI055_GYR_RANGE == BMI055_GYR_RANGE_125)
#define BMI055_GYR_SCALE					125
#endif


//#define BMI055_USE_FIFO_READ
#define BMI055_USE_DIRECT_READ

#endif /* CONF_BMI055_H_ */