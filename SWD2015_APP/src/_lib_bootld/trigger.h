/**
 * \file
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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

#ifndef TRIGGER_H_INCLUDED
#define TRIGGER_H_INCLUDED

#include "regions.h"

/** All supported trigger modes */
typedef enum trigger_modes {
	TRIGGER_BOOT,        /**< Nothing, just run app */
	TRIGGER_LOAD,        /**< Load FW (from host) and update */
	TRIGGER_LOAD_FILE,   /**< Load FW (from file) and update */
	TRIGGER_UPDATE,      /**< Update loaded FW */
	TRIGGER_UPDATE_FILE, /**< Update from a file (in storage) */
	TRIGGER_SLEEP,		 /**<  Nothing, Just sleep */
	TRIGGER_DOTA,		/**< Load FW (from SL) and update */
	TRIGGER_NUM_MAX
} trigger_modes_t;
/** String to describe trigger modes */
static const char *trigger_modes_str[] = {
	"BOOT", "LOAD", "LOAD-FILE", "UPDATE", "UPDATE-FILE", "SLEEP" , "DOTA-OK"
};
/**
 * Return trigger modes description string
 * \param mode Trigger mode
 * \return string of specified mode
 */
__always_inline
static const char* trigger_get_mode_str(enum trigger_modes mode)
{
	return trigger_modes_str[mode];
}

enum trigger_modes trigger_poll(const struct regions_info *info);

#endif /* #ifndef TRIGGER_H_INCLUDED */

