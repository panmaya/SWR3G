/**
 * \file
 *
 * \brief NMEA 0183 Parser
 *
 * Copyright (C) 2012-2013 Atmel Corporation. All rights reserved.
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
#ifndef NMEA_H_INCLUDED
#define NMEA_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "conf_nmea.h"

/**
 *  \defgroup asfdoc_gps_nmea_group GPS NMEA 0183 Sentence Parser
 *
 *  NMEA 0183 sentence parser, for the parsing of standard GPS NMEA sentences.
 *
 *  \section asfdoc_gps_nmea_api_overview API Overview
 *  @{
 */

/** Scalar for all fractional parsed values; actual fraction is the integer
 *  fractional value divided by this constant.
 */
#define NMEA_FRACTIONAL_SCALER  100//10000//100000

/**
 * \brief Enum for the different NMEA output sentences
 */
enum nmea_data_type {
#if (CONF_NMEA_GPGGA_ENABLED == true)
	/** Time, position and fix type data */
	NMEA_TYPE_GPGGA,
#endif

#if (CONF_NMEA_GPGSA_ENABLED == true)
	/** GPS receiver operating mode, active satellites used in the position
	 *  solution and DOP values
	 */
	NMEA_TYPE_GPGSA,
#endif

#if (CONF_NMEA_GPVTG_ENABLED == true)
	/** Course and speed information relative to the ground */
	NMEA_TYPE_GPVTG,
#endif

#if (CONF_NMEA_GPZDA_ENABLED == true)
	/** Time and date */
	NMEA_TYPE_GPZDA,
#endif

#if (CONF_NMEA_GPRMC_ENABLED == true)
	/** Recommended minimum */
	NMEA_TYPE_GPRMC,
#endif

#if (CONF_NMEA_GPGSV_ENABLED == true)
	/* Sat. in view*/
	NMEA_TYPE_GPGSV,
#endif

	/** Unknown or malformed input */
	NMEA_TYPE_UNKNOWN,
};


/**
 * \brief Enum for mode 1 GPGSA
 */
enum nmea_gpgsa_mode1 {
	/** Manual, forced to operate in 2D or 3D mode */
	NMEA_GPGSA_MODE1_MANUAL    = 'M',
	/** 2D automatic, allowed to automatically switch 2D/3D */
	NMEA_GPGSA_MODE1_AUTOMATIC = 'A',
};

/**
 * \brief Enum for mode 2 GPGSA
 */
enum nmea_gpgsa_mode2 {
	/** Fix not available */
	NMEA_GPGSA_MODE2_NO_FIX = 1,
	/** 2D */
	NMEA_GPGSA_MODE2_2D     = 2,
	/** 3D */
	NMEA_GPGSA_MODE2_3D     = 3,
};

/**
 * \brief Enum for mode  GPRMC
 */
enum nmea_gprmc_mode {
	/** GPS Status  Valid*/
	NMEA_GPRMC_MODE_VALID  = 'V',
	/** GPS Status  Active*/
	NMEA_GPRMC_MODE_ACTIVE = 'A',
};

/**
 * \brief Enum for the different types of dilution of precision
 */
enum nmea_dop {
	/** Position dilution of precision */
	NMEA_PDOP,
	/** Horizontal dilution of precision */
	NMEA_HDOP,
	/** Vertical dilution of precision */
	NMEA_VDOP,
};

/**
 * \brief Enum for the NMEA GPS indicators
 */
enum nmea_gps_coord_indicator {
	/** North */
	NMEA_GPS_COORD_NORTH = 'N',
	/** South */
	NMEA_GPS_COORD_SOUTH = 'S',
	/** West */
	NMEA_GPS_COORD_WEST  = 'W',
	/** East */
	NMEA_GPS_COORD_EAST  = 'E',
};

/**
 * \brief Structure for satellites in View parameter
 */
struct nmea_gps_inview {
	uint8_t PRN_nb;
	uint16_t Elevation;//degree
	uint16_t Azimuth;//degree
	uint16_t SNR;//Higher is better
};

/**
 * \brief Structure for GPS latitude and longitude coordinates
 */
struct nmea_gps_coord_val {
	/** Indicator (N, S, E, W) */
	enum nmea_gps_coord_indicator indicator;
	/** Degrees */
	uint16_t degrees;
	/** Minutes */
	uint16_t minutes;
	/** Minutes (fractional) */
	uint32_t minutes_frac;
};

/**
 * \brief Structure for GPS coordinates, latitude and longitude
 */
struct nmea_gps_coord {
	/** Latitude */
	struct nmea_gps_coord_val latitude;
	/** Longitude */
	struct nmea_gps_coord_val longitude;
};

/**
 * \brief Structure for GPGGA data
 */
struct nmea_data_gpgga {
	/** UTC hour */
	uint8_t utc_hour;
	/** UTC minute */
	uint8_t utc_minute;
	/** UTC second */
	uint8_t utc_second;
	/** GPS coordinates */
	struct nmea_gps_coord coords;
	/** Position fix indicator */
	uint8_t fix_quality;
	/** Number of satellites used */
	uint8_t satellites;
	/** Altitude */
	int16_t altitude;
};

/**
 * \brief Structure for GPGSA data
 */
struct nmea_data_gpgsa {
	/** Mode 1 - Auto or Manual*/
	enum nmea_gpgsa_mode1 mode_select;
	/** Mode 2 - 2D or 3D */
	enum nmea_gpgsa_mode2 fix_status;
	/** Satellites used on the channels */
	uint8_t satellite[12];
	/** Position dilution of precision */
	uint32_t pdop;
	/** Horizontal dilution of precision */
	uint32_t hdop;
	/** Vertical dilution of precision */
	uint32_t vdop;
};

/**
 * \brief Structure for GPVTG data
 */
struct nmea_data_gpvtg {
	/** Course (integer) */
	uint16_t course;
	/** Course (fractional) */
	uint16_t course_frac;
	/** Magnetic course (integer) */
	uint16_t course_magnetic;
	/** Magnetic course (fractional */
	uint16_t course_magnetic_frac;
	/** Speed, knots (integer) */
	uint16_t speed;
	/** Speed, knots (fractional) */
	uint16_t speed_frac;
	/** Speed, km/h (integer) */
	uint16_t speed_kmh;
	/** Speed, km/h (fractional) */
	uint16_t speed_kmh_frac;
};

/**
 * \brief Structure for GPGSV data
 */
struct nmea_data_gpgsv {
	uint8_t NbSatInViews;
	struct nmea_gps_inview gps_view[64];
};

/**
 * \brief Structure for GPZDA data
 */
struct nmea_data_gpzda {
	/** UTC hour */
	uint8_t utc_hour;
	/** UTC minute */
	uint8_t utc_minute;
	/** UTC second */
	uint8_t utc_second;
	/** UTC day */
	uint8_t utc_day;
	/** UTC month */
	uint8_t utc_month;
	/** UTC year */
	uint16_t utc_year;
};


/**
 * \brief Structure for GPRMC data
 */
struct nmea_data_gprmc {
	/** UTC hour */
	uint8_t utc_hour;
	/** UTC minute */
	uint8_t utc_minute;
	/** UTC second */
	uint8_t utc_second;
	/** UTC day */
	uint8_t utc_day;
	/** UTC month */
	uint8_t utc_month;
	/** UTC year */
	uint16_t utc_year;
	/** GPS coordinates */
	struct nmea_gps_coord coords;
	/** GPS Status */
	enum nmea_gprmc_mode gps_status;
	/** Course (integer) */
	uint16_t course;
	/** Course (fractional) */
	uint16_t course_frac;
	/** Speed, knots (integer) */
	uint16_t speed;
	/** Speed, knots (fractional) */
	uint16_t speed_frac;
};

/**
 * \brief Structure for storing the different types of NMEA data
 */
struct nmea_data {
	/** Data type of the data */
	enum nmea_data_type type;
	/** Union for the different data types */
	//union nmea_type_data {
#if (CONF_NMEA_GPGGA_ENABLED == true)
		struct nmea_data_gpgga gpgga;
#endif
#if (CONF_NMEA_GPGSA_ENABLED == true)
		struct nmea_data_gpgsa gpgsa;
#endif
#if (CONF_NMEA_GPVTG_ENABLED == true)
		struct nmea_data_gpvtg gpvtg;
#endif
#if (CONF_NMEA_GPZDA_ENABLED == true)
		struct nmea_data_gpzda gpzda;
#endif
#if (CONF_NMEA_GPRMC_ENABLED == true)
		struct nmea_data_gprmc gprmc;
#endif
#if (CONF_NMEA_GPGSV_ENABLED == true)
		struct nmea_data_gpgsv gpgsv;
#endif
	//} type_specific;
};

int8_t nmea_parse_input( const char *buffer, uint16_t len, struct nmea_data *data);

uint8_t nmea_compute_checksum(
		const char *buffer);

uint8_t nmea_extract_checksum(
		const char *buffer);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_H_INCLUDED */
