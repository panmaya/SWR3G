/**
 * \file
 *
 * \brief API driver for component FM24CXX.
 *
 * Copyright (c) 2011 - 2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef FM24CXX_H_INCLUDED
#define FM24CXX_H_INCLUDED

#include "twi_master.h"
#include "conf_fm24clx.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

//! Checking board configuration of the FM24C FMRAM
#if !defined(BOARD_FM24C_TWI_INSTANCE)
# warning The FM24C TWI instance has not been defined. Using default settings.
# define BOARD_FM24C_TWI_INSTANCE 0 /* TWI instance (TWI0) */
#endif
#if !defined(BOARD_FM24C_ADDRESS)
# warning The FM24C TWI address has not been defined. Using default settings.
# define BOARD_FM24C_ADDRESS 0 /* TWI eeprom address (0x50u) */
#endif
#if !defined(BOARD_CLK_TWI_FMRAM)
# warning The FM24C TWI CLK pin index has not been defined. Using default.
# define BOARD_CLK_TWI_FMRAM 0 /* TWI eeprom clock pin index */
#endif
#if !defined(BOARD_CLK_TWI_MUX_FMRAM)
# warning The FM24C TWI CLK peripheral has not been defined. Using default.
# define BOARD_CLK_TWI_MUX_FMRAM 0 /* TWI eeprom clock pin peripheral */
#endif

/* FM24CXX internal address length */
#define FM24C_MEM_ADDR_LEN   2

/* Return value for accessing FM24CXX */
#define FM24C_WRITE_SUCCESS     0
#define FM24C_WRITE_FAIL        1
#define FM24C_READ_SUCCESS      0
#define FM24C_READ_FAIL         1

void fm24cxx_reset(void);
#if(1)
uint32_t fm24cxx_write_byte(uint32_t u32_address, uint8_t uc_value);
uint32_t fm24cxx_write_continuous(uint32_t u32_start_address,
		uint16_t u16_length, uint8_t const *p_wr_buffer);
uint32_t fm24cxx_read_byte(uint32_t u32_address, uint8_t *p_rd_byte);
uint32_t fm24cxx_read_continuous(uint32_t u32_start_address, uint16_t u16_length,
		uint8_t *p_rd_buffer);
uint32_t fm24cxx_write_page(uint32_t u32_page_address,
		uint32_t u32_page_size, uint8_t const *p_wr_buffer);
uint32_t fm24cxx_read_page(uint32_t u32_page_address,
		uint32_t u32_page_size, uint8_t *p_rd_buffer);
uint32_t fm24cxx_fill_pattern(uint32_t u32_start_address,
		uint32_t u32_end_address, uint8_t u8_pattern);
#endif
/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* FM24CXX_H_INCLUDED */
