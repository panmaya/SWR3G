/*
 * freertos_twi_fm24c.c
 *
 * Created: 22/10/2557 11:30:22
 *  Author: pan
 */ 
#include <adl_global.h>
#include "../freertos_twi_fm24c.h"

static freertos_twi_if hTwiIf;
static const portTickType max_block_time_ticks = 500 /*portMAX_DELAY *// portTICK_RATE_MS;

#ifdef TWI_FRAM24C16_USED
/*
 * FM24C16 I2C F-RAM Factory's structure*
 *
 * +<---------------------------------------------  2K Bytes ----------------------------------------------------->+
 * +-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
 * |<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|<-265 Bytes->|
 * |-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
 * | ADDR = 0x50 | ADDR = 0x51 | ADDR = 0x52 | ADDR = 0x53 | ADDR = 0x54 | ADDR = 0x55 | ADDR = 0x56 | ADDR = 0x57 |
 * +---------------------------------------------------------------------------------------------------------------+
*/

#define TWI_FRAM24C16_SIZE_PER_PAGE		256
#define	TWI_FRAM24C16_MAX_FRAM_PAGE		8
#define TWI_FRAM24C16_CAPACITY_SIZE		(TWI_FRAM24C16_SIZE_PER_PAGE * TWI_FRAM24C16_MAX_FRAM_PAGE)
#define TWI_FRAM24C16_1ST_I2C_ADDR		0x50
#define TWI_FRAM24C16_1ST_MEM_ADDR		0x5000
#define TWI_FRAM24C16_MODE_WRITE		1
#define TWI_FRAM24C16_MODE_READ			0

typedef enum TWI_FRAM24C16_MODE_E {
	TWI_FRAM24C16_READ,
	TWI_FRAM24C16_WRITE
}twi_fram24cl16_mode_e;


/*-----------------------------------------------------------*/

static status_code_t FREERTOS_TWI_FM24C_ACCESS (
/* [IN] The file pointer for the I2C channel */					freertos_twi_if			fd,
/* [IN] The address in FRAM to write to */					uint32_t				accessAddr,
/* [IN] The array of characters are to be written in FRAM */		uint8_t	*				ptrData,
/* [IN] Number of bytes in that buffer */					uint32_t				accessLen,
													twi_fram24cl16_mode_e 	mode
)
{
	uint8_t blog_Addr, iRamPage = 0;
	uint32_t nByte2Operate = 0, iPtrData = 0;
	uint32_t iOffsetEndPage = 0,iRemainSpaceOnPage = 0;
	int32_t iRetVal = (-1);
	twi_packet_t twi_parameters;
	const portTickType max_block_time_ticks = 400UL / portTICK_RATE_MS;
	
	if((!ptrData)||(accessLen==0))return iRetVal;
	if((accessAddr + accessLen) > TWI_FRAM24C16_CAPACITY_SIZE)return iRetVal;
	accessAddr += TWI_FRAM24C16_1ST_MEM_ADDR;
	while(accessLen) {
		blog_Addr = (uint8_t)((accessAddr & 0x000000FF));
		iOffsetEndPage   = ((accessAddr & 0x0000FF00) | 0xFF);
		iRamPage = (accessAddr - TWI_FRAM24C16_1ST_MEM_ADDR)/256;
		iRemainSpaceOnPage = (iOffsetEndPage-accessAddr)+ 1;
		nByte2Operate = (iRemainSpaceOnPage >= accessLen) ? accessLen : iRemainSpaceOnPage;
		
		twi_parameters.chip = TWI_FRAM24C16_1ST_I2C_ADDR + iRamPage;
		twi_parameters.buffer = (uint8_t*)ptrData+iPtrData;
		twi_parameters.length = nByte2Operate;
		twi_parameters.addr[0] = blog_Addr;
		twi_parameters.addr_length = 1;
		
		if(mode==TWI_FRAM24C16_WRITE) {
			//puts("\r\nwriting\r\n");
			iRetVal = freertos_twi_write_packet(fd, &twi_parameters,max_block_time_ticks);	
			//printf("\r\n wrote %ld byte",iRetVal);
		}else iRetVal = freertos_twi_read_packet(fd, &twi_parameters,max_block_time_ticks);
		
		if(iRetVal == STATUS_OK) {
			iPtrData   += nByte2Operate;
			accessAddr += nByte2Operate;
			accessLen  -= nByte2Operate;
		}else break;
	}
	return iRetVal;
}
#endif


status_code_t freertos_twi_fm24c_write (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in FRAM to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in FRAM */			uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
)
{
	status_code_t retCode = STATUS_OK;
	twi_packet_t write_parameters;
	
	#ifdef TWI_FRAM24C16_USED
		#if(0)
		/* The internal FMRAM address being written to is calculated from the page number. */
		write_parameters.chip = BOARD_FM24C_ADDRESS + 0;//manaul page  = 0 - 7;
		write_parameters.buffer = buffer;
		write_parameters.length = n;
		write_parameters.addr[0] = (addr & 0x000000FF);
		write_parameters.addr_length = 1;//FM24CL16
		#else
		return FREERTOS_TWI_FM24C_ACCESS (fd, addr, buffer, n, TWI_FRAM24C16_WRITE);
		#endif
	#endif //TWI_FRAM24C16_USED

	#ifdef TWI_FRAM24C64_USED
	/* The internal FMRAM address being written to is calculated from the page number. */
	write_parameters.chip = BOARD_FM24C_ADDRESS;
	write_parameters.buffer = buffer;
	write_parameters.length = n;
	write_parameters.addr_length = 2;//FM24CL64
	//write_parameters.addr[1] = (addr & 0x000000FF);
	//write_parameters.addr[0] = (addr & 0x0000FF00) >> 8;
	write_parameters.addr[0] = (uint8_t)((addr >> 8) & 0xff);
	write_parameters.addr[1] = (uint8_t)(addr & 0xff);
	
	#endif


	/* The blocking API is being used.  Other tasks will execute while the write operation is in progress. */
	retCode = freertos_twi_write_packet(fd, &write_parameters,max_block_time_ticks);
	if (retCode != STATUS_OK) printf("\r\nFRAM Addr %lu Write %ld\r\n",addr,(int32_t)retCode);

	/* Wait for the FMRAM to complete programming its internal memory.
	Note this is a very crude way of doing this, the FMRAM status
	register could be polled to determine when it was finished. */
	//vTaskDelay(INTERNAL_WRITE_DELAY);
	//vTaskDelay(100);
	return retCode;
}

status_code_t freertos_twi_fm24c_read (
/* [IN] The file pointer for the I2C channel */						freertos_twi_if		fd,
/* [IN] The address in FRAM to write to */						uint32_t			addr,
/* [IN] The array of characters are to be written in FRAM */			uint8_t	*			buffer,
/* [IN] Number of bytes in that buffer */						uint32_t			n
)
{
	status_code_t retCode = STATUS_OK;
	twi_packet_t write_parameters;
	
	#ifdef TWI_FRAM24C16_USED
		#if(0)
		/* The internal FMRAM address being written to is calculated from the page number. */
		write_parameters.chip = BOARD_FM24C_ADDRESS + 0;//manaul page  = 0 - 7;
		write_parameters.buffer = buffer;
		write_parameters.length = n;
		write_parameters.addr[0] = (addr & 0x000000FF);
		write_parameters.addr_length = 1;//FM24CL16
		#else
		return FREERTOS_TWI_FM24C_ACCESS (fd, addr, buffer, n, TWI_FRAM24C16_READ);
		#endif
	#endif //TWI_FRAM24C16_USED

	#ifdef TWI_FRAM24C64_USED
	/* The internal FMRAM address being written to is calculated from the page number. */
	write_parameters.chip = BOARD_FM24C_ADDRESS;
	write_parameters.buffer = buffer;
	write_parameters.length = n;
	write_parameters.addr_length = 2;//FM24CL64
	//write_parameters.addr[1] = (addr & 0x000000FF);
	//write_parameters.addr[0] = (addr & 0x0000FF00) >> 8;
	write_parameters.addr[0] = (uint8_t)((addr >> 8) & 0xff);
	write_parameters.addr[1] = (uint8_t)(addr & 0xff);
	
	#endif


	/* The blocking API is being used.  Other tasks will execute while the write operation is in progress. */
	retCode = freertos_twi_read_packet(fd, &write_parameters,max_block_time_ticks);
	if (retCode != STATUS_OK) printf("\r\nFRAM Addr %lu Read %ld\r\n",addr,(int32_t)retCode);

	/* Wait for the FMRAM to complete programming its internal memory.
	Note this is a very crude way of doing this, the FMRAM status
	register could be polled to determine when it was finished. */
	//vTaskDelay(INTERNAL_WRITE_DELAY);
	//vTaskDelay(100);
	return retCode;
}


freertos_twi_if freertos_twi_fm24c_init(Twi *twi_base) {
	//static freertos_twi_if hTwiIf;
	const freertos_peripheral_options_t blocking_driver_options = {
		NULL,											/* This peripheral does not need a receive buffer, so this parameter is just set to NULL. */
		0,												/* There is no Rx buffer, so the rx buffer size is not used. */
		FREERTOS_TWI_FM24C_INTERRUPT_PRIORITY,	/* The priority used by the TWI interrupts. */
		TWI_I2C_MASTER,									/* Communicating with the FMRAM requires the TWI to be configured as an I2C master. */
		(USE_TX_ACCESS_MUTEX | USE_RX_ACCESS_MUTEX | WAIT_TX_COMPLETE | WAIT_RX_COMPLETE)	/* The blocking driver is to be used, so WAIT_TX_COMPLETE and WAIT_RX_COMPLETE are set. */
	};
	/* Initialize the FreeRTOS driver for blocking operation.  The
	peripheral clock is configured in this function call. */
	hTwiIf = freertos_twi_master_init(twi_base,&blocking_driver_options);
	/* Check the port was initialized successfully. */
	configASSERT(hTwiIf);
	/* Configure the TWI bus parameters.  Do this after calling
	freertos_twi_master_init(). */
	twi_set_speed(twi_base, FREERTOS_TWI_FM24C_CLOCK_HZ, sysclk_get_cpu_hz());
	return hTwiIf;
}

