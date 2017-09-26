/*
 * freertos_twi_ads1113.c
 *
 * Created: 22/10/2557 11:30:22
 *  Author: pan
 */ 
#include <adl_global.h>
#include "../freertos_twi_ads1113.h"

static freertos_twi_if freertos_twi_ads1113_hBus;

status_code_t freertos_twi_ads1113_InitReg (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in ADC to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in ADC */			uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
)
{

	twi_packet_t write_parameters;
	const portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	write_parameters.chip = BOARD_ADS1113C_ADDRESS;
	write_parameters.buffer = buffer;
	write_parameters.length = n;

	/* The internal ADC address being written to is calculated from the page
	number. */
	write_parameters.addr[0] = addr;
	write_parameters.addr_length = 1;

	/* The blocking API is being used.  Other tasks will execute while the
	write operation is in progress. */
	if (freertos_twi_write_packet(fd, &write_parameters,max_block_time_ticks) != STATUS_OK) {
		printf("\r\nADS1113 Error write\r\n");
	}

	/* Wait for the ADC to complete programming its internal memory.
	Note this is a very crude way of doing this, the ADC status
	register could be polled to determine when it was finished. */
	//vTaskDelay(INTERNAL_WRITE_DELAY);
	//vTaskDelay(100);
	return STATUS_OK;
}

status_code_t freertos_twi_ads1113_read (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in ADC to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in ADC */			uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
)
{

	twi_packet_t write_parameters;
	const portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	write_parameters.chip = BOARD_ADS1113C_ADDRESS;
	write_parameters.buffer = buffer;
	write_parameters.length = n;

	/* The internal ADC address being written to is calculated from the page
	number. */
	write_parameters.addr[0] = addr;
	write_parameters.addr_length = 1;

	/* The blocking API is being used.  Other tasks will execute while the
	write operation is in progress. */
	if (freertos_twi_read_packet(fd, &write_parameters,max_block_time_ticks) != STATUS_OK) {
		printf("\r\nADS1113 Error read\r\n");
	}

	/* Wait for the ADC to complete programming its internal memory.
	Note this is a very crude way of doing this, the ADC status
	register could be polled to determine when it was finished. */
	//vTaskDelay(INTERNAL_WRITE_DELAY);
	//vTaskDelay(100);
	return STATUS_OK;
}


freertos_twi_if freertos_twi_ads1113_init(Twi *twi_base) {
	//static freertos_twi_if freertos_twi_ads1113_hBus;
	const freertos_peripheral_options_t blocking_driver_options = {
		NULL,											/* This peripheral does not need a receive buffer, so this parameter is just set to NULL. */
		0,												/* There is no Rx buffer, so the rx buffer size is not used. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2,	/* The priority used by the TWI interrupts. */
		TWI_I2C_MASTER,									/* Communicating with the ADC requires the TWI to be configured as an I2C master. */
		(USE_TX_ACCESS_MUTEX | USE_RX_ACCESS_MUTEX | WAIT_TX_COMPLETE | WAIT_RX_COMPLETE)	/* The blocking driver is to be used, so WAIT_TX_COMPLETE and WAIT_RX_COMPLETE are set. */
	};
	/* Initialize the FreeRTOS driver for blocking operation.  The
	peripheral clock is configured in this function call. */
	freertos_twi_ads1113_hBus = freertos_twi_master_init(twi_base,&blocking_driver_options);
	/* Check the port was initialized successfully. */
	configASSERT(freertos_twi_ads1113_hBus);
	/* Configure the TWI bus parameters.  Do this after calling
	freertos_twi_master_init(). */
	twi_set_speed(twi_base, FREERTOS_TWI_ADS1113_CLOCK_HZ, sysclk_get_cpu_hz());
	return freertos_twi_ads1113_hBus;
}

