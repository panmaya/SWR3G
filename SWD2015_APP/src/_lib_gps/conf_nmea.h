/**
 * \file
 *
 * \brief NMEA 0183 Parser configuration.
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

#ifndef CONF_NMEA_H_INCLUDED
#define CONF_NMEA_H_INCLUDED

/** If \c true, GPGGA sentence parsing is enabled. If disabled, these sentences
 *  will return an unknown sentence error if fed to the NMEA parser.
 */
#define CONF_NMEA_GPGGA_ENABLED        true

/** If \c true, GPGSA sentence parsing is enabled. If disabled, these sentences
 *  will return an unknown sentence error if fed to the NMEA parser.
 */
#define CONF_NMEA_GPGSA_ENABLED        true

/** If \c true, GPVTG sentence parsing is enabled. If disabled, these sentences
 *  will return an unknown sentence error if fed to the NMEA parser.
 */
#define CONF_NMEA_GPVTG_ENABLED        true

/** If \c true, GPZDA sentence parsing is enabled. If disabled, these sentences
 *  will return an unknown sentence error if fed to the NMEA parser.
 */
#define CONF_NMEA_GPZDA_ENABLED        true

/** If \c true, GPRMC sentence parsing is enabled. If disabled, these sentences
 *  will return an unknown sentence error if fed to the NMEA parser.
 */
#define CONF_NMEA_GPRMC_ENABLED        true

/** If \c true, GPGSV sentence parsing is enabled. If disabled, these sentences
 *  will return an unknown sentence error if fed to the NMEA parser.
 */
#define CONF_NMEA_GPGSV_ENABLED        true//false

#endif /* CONF_NMEA_H_INCLUDED */
