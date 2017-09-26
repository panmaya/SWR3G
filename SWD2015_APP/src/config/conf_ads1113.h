/*
 * conf_ads1113.h
 *
 * Created: 12/1/2558 9:34:35
 *  Author: pan
 */ 


#ifndef CONF_ADS1113_H_
#define CONF_ADS1113_H_

/**
* \name TWIx pin definitions
* @{
	*/
/*! TWI ID for A2D application to use */
#define BOARD_ID_TWI_A2D			ID_TWI1

/*! TWI Base for TWI A2D application to use */
#define BOARD_BASE_TWI_A2D			TWI1
#define BOARD_ADS1113_TWI_INSTANCE	TWI1
#define BOARD_ADS1113C_ADDRESS		0x48

/*! TWI1 Data pin for A2D */
#define TWI1_DATA_GPIO				PIO_PB4_IDX
#define TWI1_DATA_FLAG				IOPORT_MODE_MUX_A
/*! TWI1 Clock pin for A2D */
#define TWI1_CLK_GPIO				PIO_PB5_IDX
#define TWI1_CLK_FLAG				IOPORT_MODE_MUX_A
#define BOARD_CLK_TWI_A2D			TWI1_CLK_GPIO
#define BOARD_CLK_TWI_MUX_A2D		TWI1_CLK_FLAG


//config register set
#define I2C_ADS1113_WRITE_REGISTER_SIZE			3
#define I2C_ADS1113_WRITE_REGISTER_DATA_0		0x01              //Config register
#define I2C_ADS1113_WRITE_REGISTER_DATA_1		0x42              //0100 0010   set register
#define I2C_ADS1113_WRITE_REGISTER_DATA_2		0x83              //1000 0011   set register

//config register read
#define I2C_ADS1113_WRITE_CONVERSION_SIZE		1
#define I2C_ADS1113_WRITE_CONVERSION_DATA_0		0x00            //Conversion register

#define I2C_ADS1113_READ_ADC0_SIZE				2
#define I2C_ADS1113_RESISTOR_HIGH				100000//562000
#define I2C_ADS1113_RESISTOR_LOW				12000//30000
#define RESISTOR_IN_ADC							((I2C_ADS1113_RESISTOR_HIGH+I2C_ADS1113_RESISTOR_LOW)/I2C_ADS1113_RESISTOR_LOW)
#define	I2C_ADS1113_INP_IMPEDANCE				RESISTOR_IN_ADC

#endif /* CONF_ADS1113_H_ */