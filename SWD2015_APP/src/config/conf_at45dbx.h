/*
 * conf_at45dbx.h
 *
 * Created: 14/10/2557 16:59:12
 *  Author: pan
 */ 


#ifndef CONF_AT45DBX_H_
#define CONF_AT45DBX_H_

#include "board.h"

#define CONF_AT45DBX_Mhz(_f)		(_f * 1000000)

//! Connect AT45DBx driver to spi master service
#define AT45DBX_USES_SPI_MASTER_SERVICE

//! Select the SPI module AT45DBX is connected to
#define AT45DBX_SPI_MODULE          SPI

//! Size of AT45DBX data flash memories to manage.
#define AT45DBX_MEM_SIZE           AT45DBX_2MB//AT45DBX_2MB//AT45DBX_500KB

//! Number of AT45DBX components to manage.
#define AT45DBX_MEM_CNT             1

//! SPI master speed in Hz.
#define AT45DBX_SPI_MASTER_SPEED    CONF_AT45DBX_Mhz(12)//CONF_AT45DBX_Mhz(10/*16*//*18*/)//work from 0-28 ,max of at45db = 20MHz , max of sam4 = 120 MHz

//! First chip select used by AT45DBX components on the SPI module instance.
#define AT45DBX_CS                  0


#ifdef __FREERTOS__
#define AT45DBX_HAL_SPI_USED_RTOS_PDC	0
#endif


#endif /* CONF_AT45DBX_H_ */