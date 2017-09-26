/*
 * gsensor.c
 *
 * Created: 19/10/2558 17:27:30
 *  Author: Pan
 */ 
#include <adl_global.h>
#include "apt_gsensor.h"
#include "misc_config.h"

#define LIS3DH_SENSITIVITY_2G  (0.001F)
#define LIS3DH_SENSITIVITY_4G  (0.002F)
#define LIS3DH_SENSITIVITY_8G  (0.004F)
#define LIS3DH_SENSITIVITY_16G (0.012F)

void aptGsensor_init(void) {
	bool    _lis3dhInitialised = false;
	uint8_t _lis3dhMeasurementRange = 2;        /* +/-2, 4, 8 or 16 g */
	
	uint8_t WriteBuff[2] = {0};
	uint8_t ReadBuff[2] = {0};
	
	TWI1_hBus = freertos_twi_ls3dh_init(BOARD_BASE_TWI_LIS3DH);

	freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_WHO_AM_I,(uint8_t*)ReadBuff,1);
	//vPrintf("\r\nLIS3D: Chip ID = 0x%02x\r\n",ReadBuff[0]);
	if(ReadBuff[0] == 0x33) {
		WriteBuff[0] = LIS3DH_CTRL_REG1_DATARATE_50HZ /* Normal mode, 50Hz */ | LIS3DH_CTRL_REG1_XYZEN /* Enable X, Y and Z */;
		/* Set data rate and power mode, and enable X/Y/Z */
		freertos_twi_ls3dh_write(TWI1_hBus,LIS3DH_REGISTER_CTRL_REG1,(uint8_t*)WriteBuff,1);
		
		/* Enable block update and set range to +/-2G */
		WriteBuff[0] = LIS3DH_CTRL_REG4_BLOCKDATAUPDATE  /* Enable block update */ | LIS3DH_CTRL_REG4_SCALE_2G /* +/-2G measurement range */;
		freertos_twi_ls3dh_write(TWI1_hBus,LIS3DH_REGISTER_CTRL_REG4,(uint8_t*)WriteBuff,1);
		
		/* Make sure the measurement range is updated here if you change it */
		_lis3dhMeasurementRange = 2;
		_lis3dhInitialised = true;
		
		freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_CTRL_REG1,(uint8_t*)ReadBuff,1);
		//vPrintf("\r\nLIS3D: CTRL_REG1 = 0x%02x\r\n",ReadBuff[0]);
		freertos_twi_ls3dh_read(TWI1_hBus,LIS3DH_REGISTER_CTRL_REG4,(uint8_t*)ReadBuff,1);
		//vPrintf("\r\nLIS3D: CTRL_REG4 = 0x%02x\r\n",ReadBuff[0]);
	}
	
	freertos_twi_lsm6ds3_read(TWI1_hBus,LSM6DS3_ACC_GYRO_WHO_AM_I_REG,(uint8_t*)ReadBuff,1);
	if(ReadBuff[0] == 0x69) {
		vPrintf("\r\nLSM6DS3: IC Exist ID = 0x%02x\r\n",ReadBuff[0]);
	}else vPrintf("\r\nLSM6DS3: IC Not Found\r\n");
}

#if(0)

static freertos_twi_if /*FRam_hBus,*/ TWI1_hBus;
#if(0)
static void twi_a2d_task(void *pvParameters)
{
	uint8_t ReadBuff[I2C_ADS1113_READ_ADC0_SIZE] = {0};
	uint8_t InitCode[I2C_ADS1113_WRITE_REGISTER_SIZE] = {
		I2C_ADS1113_WRITE_REGISTER_DATA_0,
		I2C_ADS1113_WRITE_REGISTER_DATA_1,
		I2C_ADS1113_WRITE_REGISTER_DATA_2
	};
	
	UNUSED(pvParameters);
	
	vPuts("\r\nA2D Initial Reg. \r\n");
	freertos_twi_ads1113_InitReg(TWI1_hBus,I2C_ADS1113_WRITE_REGISTER_DATA_0,(uint8_t*)InitCode+1,2);
	//puts("\r\nA2D Ch0 Selecting\r\n");
	//InitCode[0] = I2C_ADS1113_WRITE_CONVERSION_DATA_0;
	//freertos_twi_ads1113_InitReg(TWI1_hBus,0x00,(uint8_t*)InitCode,I2C_ADS1113_WRITE_CONVERSION_SIZE);
	
	for(;;) {
		uint16_t vAdc = 0x0000;
		freertos_twi_ads1113_read(TWI1_hBus,I2C_ADS1113_WRITE_CONVERSION_DATA_0,(uint8_t*)ReadBuff,I2C_ADS1113_READ_ADC0_SIZE);
		vAdc |= ReadBuff[0];
		vAdc <<= 8;
		vAdc |= ReadBuff[1];
		//printf("\r\n A2D CH0 = %.3f\r\n",(float)((iAdc*4.096)/65535)*RESISTOR_IN_ADC);
		//Calculattion ->    (Vin / Vref) * (65535) = adc;
		//So              ->    Vin = (adc * Vref) / 65535;
		//But Vin = Vext * (R-devider2 / (R-devider1 + R-devider2))
		//So             ->     Vext = ((adc * Vref) / 65535) * ((R-devider1 + R-devider2)/ R-devider2);
		
		//printf("\r\n A2D CH0 = %.3f\r\n",(float)((vAdc*3.578)/65535)*I2C_ADS1113_INP_IMPEDANCE);
		vTaskDelay(100);
	}
	

	/* Delete task */
	vTaskDelete(NULL);
}
#endif

static void gyro_read(void) {
	//int i, fifoCount, j;
	//unsigned long timeNow, timeNew = 0, timeSamp, ledOffTime;
	//uint8_t fifoBuffer[8];
	//int16_t AngleX, AngleY, AngleZ, AccX, AccY, AccZ = 0;
	int16_t RateX, RateY, RateZ = 0;
	//int32_t SumGX = 0, SumGY = 0, SumGZ = 0, SumAX = 0, SumAY = 0, SumAZ = 0;
	//uint16_t Count = 0;
	//uint8_t WriteBuff[2] = {0,0};
	uint8_t ReadBuff[2] = {0,0};

	#ifdef BMI055_USE_FIFO_READ
	fifoCount = bmg160.getFifoCount() & 0x7F;
	for (i = 0; i < fifoCount / 8; i++)
	{
		bmg160.getFifoBuffer(fifoBuffer);
		for (j = 0; j < 6; j++) {
			Serial.print(" ");
			Serial.print(fifoBuffer[j], HEX);
		}
		Serial.println("");
		RateX = fifoBuffer[1];
		RateX <<= 8;
		RateX |= fifoBuffer[0];
		RateY = fifoBuffer[3];
		RateY <<= 8;
		RateY |= fifoBuffer[2];
		RateZ = fifoBuffer[5];
		RateZ <<= 8;
		RateZ |= fifoBuffer[4];
		SumGX += RateX;
		SumGY += RateY;
		SumGZ += RateZ;
		Count++;
	}
	#endif
	
	#ifdef BMI055_USE_DIRECT_READ
	freertos_twi_bmi055_gyr_read(TWI1_hBus,BMI055_GYR_X_AXIS_LSB_REG,(uint8_t*)ReadBuff,2);
	RateX = ReadBuff[1];
	RateX <<= 8;
	RateX |= ReadBuff[0];
	freertos_twi_bmi055_gyr_read(TWI1_hBus,BMI055_GYR_Y_AXIS_LSB_REG,(uint8_t*)ReadBuff,2);
	RateY = ReadBuff[1];
	RateY <<= 8;
	RateY |= ReadBuff[0];
	freertos_twi_bmi055_gyr_read(TWI1_hBus,BMI055_GYR_Z_AXIS_LSB_REG,(uint8_t*)ReadBuff,2);
	RateZ = ReadBuff[1];
	RateZ <<= 8;
	RateZ |= ReadBuff[0];
	
	//SumGX += RateX;
	//SumGY += RateY;
	//SumGZ += RateZ;
	
	
	vPrintf("\r\nGYRO XYZ: %04d , %04d , %04d",RateX,RateY,RateZ);
	#endif
}

static void twi_gyro_task(void *pvParameters)
{
	uint8_t WriteBuff[2] = {0};
	uint8_t ReadBuff[2] = {0};
	
	UNUSED(pvParameters);
	
	vPuts("\r\nGyro Initial Reg. \r\n");
	freertos_twi_bmi055_gyr_read(TWI1_hBus,BMI055_GYR_CHIP_ID_REG,(uint8_t*)ReadBuff,1);
	vPrintf("\r\nGYRO: ID = 0X%02X\r\n",ReadBuff[0]);
	if(ReadBuff[0] == 0x0F) {
		WriteBuff[0] = BMI055_GYR_RANGE;
		freertos_twi_bmi055_gyr_write(TWI1_hBus,BMI055_GYR_RANGE_REG,(uint8_t*)WriteBuff,1);
		WriteBuff[0] = BMI055_GYR_UR_BW_200HZ_23HZ;
		freertos_twi_bmi055_gyr_write(TWI1_hBus,BMI055_GYR_UR_BW_SEL_REG,(uint8_t*)WriteBuff,1);
		#ifdef BMI055_USE_FIFO_READ
		WriteBuff[0] = 0xB2;
		freertos_twi_bmi055_gyr_write(TWI1_hBus,BMI055_GYR_FIFO_REG0,(uint8_t*)WriteBuff,1);
		WriteBuff[0] = 0x80;
		freertos_twi_bmi055_gyr_write(TWI1_hBus,BMI055_GYR_FIFO_REG1,(uint8_t*)WriteBuff,1);
		#endif
		
		for(;;) {
			gyro_read();
			vTaskDelay(1000);
		}
	}
	
	/* Delete task */
	vTaskDelete(NULL);
}


static void  twi1_bus_init(void) {
	//TWI1_hBus = freertos_twi_ads1113_init(BOARD_BASE_TWI_A2D);
	TWI1_hBus = freertos_twi_bmi055_init(BOARD_BASE_TWI_GYRO);
}

void twi1_dev_gyro_init(void) {
	twi1_bus_init();
	/* Create the task as described above. */
	//xTaskCreate(twi_gyro_task, (const signed char *const) "gyro", 1024, (void *) 0, (tskIDLE_PRIORITY + 3),NULL);
}

void twi1_dev_a2d_init(void) {
	//twi1_bus_init();
	/* Create the task as described above. */
	//xTaskCreate(twi_a2d_task, (const signed char *const) "adc", 1024, (void *) 0, (tskIDLE_PRIORITY + 3),NULL);
}
#endif