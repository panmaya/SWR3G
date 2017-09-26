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

#include <compiler.h>
#include <ctype.h>
#include <string.h>
#include "nmea.h"

/** Moves to the next token in a NMEA sentence.
 *
 *  \param[in,out] buffer  Indirect pointer to a buffer containing a NMEA sentence
 */
#define NMEA_NEXT_TOKEN(buffer)      if (**buffer != '\0') { (*buffer)++; }

/** Retrieves the current token in a NMEA sentence.
 *
 *  \param[in,out] buffer  Indirect pointer to a buffer containing a NMEA sentence
 *
 *  \return Current token in the NMEA sentence
 */
#define NMEA_CURRENT_TOKEN(buffer)   ((unsigned char)**buffer)


/**
 * \internal
 * Parse the current integer field value.
 *
 * \param[in, out] buffer      Current location in the NMEA sentence
 * \param[in]      max_length  Maximum length of the read integer
 * \param[out]     out         Location of the parsed integer
 *
 * \return Boolean \c true if the field was parsed successfully.
 */
static bool _nmea_parse_int(
		const char **buffer,
		uint8_t max_length,
		uint32_t *const out)
{
	uint32_t parsed_value = 0;

	while (isdigit(NMEA_CURRENT_TOKEN(buffer)) && max_length--) {
		parsed_value = (parsed_value * 10) + (NMEA_CURRENT_TOKEN(buffer) - '0');
		NMEA_NEXT_TOKEN(buffer);
	}

	*out = parsed_value;
	return true;
}

/**
 * \internal
 * Parse the current fixed point X.Y field value.
 *
 * \param[in, out] buffer          Current location in the NMEA sentence
 * \param[out]     out_whole       Location of the parsed integer portion
 * \param[out]     out_fractional  Location of the parsed fractional portion
 *
 * \return Boolean \c true if the field was parsed successfully.
 */
static bool _nmea_parse_fixed_point(
		const char **buffer,
		uint32_t *const out_whole,
		uint32_t *const out_fractional)
{
	uint32_t parsed_whole      = 0;
	uint32_t parsed_fractional = 0;
	bool     parse_success     = true;

	parse_success &= _nmea_parse_int(buffer, -1, &parsed_whole);
	
	//vPrintf("\r\n _nmea_parse_fixed_point parsed_whole-----> %d\r\n",parsed_whole);

	if (NMEA_CURRENT_TOKEN(buffer) != '.') {
		return false;
	}
	NMEA_NEXT_TOKEN(buffer);

	for (uint32_t i = 10; i <= NMEA_FRACTIONAL_SCALER; i *= 10) {
		parsed_fractional *= 10;

		if (isdigit(NMEA_CURRENT_TOKEN(buffer))) {
			parsed_fractional += (NMEA_CURRENT_TOKEN(buffer) - '0');
		}
		
		NMEA_NEXT_TOKEN(buffer);
		//vPrintf("\r\n _nmea_parse_fixed_point parsed_fractional-----> %d\r\n",parsed_fractional);

	}

	*out_whole      = parsed_whole;
	*out_fractional = parsed_fractional;

	return parse_success;
}

/**
 * \internal
 * Parse the current character field value.
 *
 * \param[in, out] buffer  Current location in the NMEA sentence
 * \param[out]     out     Location of the parsed character
 *
 * \return Boolean \c true if the field was parsed successfully.
 */
static bool _nmea_parse_character(
		const char **buffer,
		char *const out)
{
	char parsed_char   = '\0';
	bool parse_success = false;
	
	//vPrintf("\r\nchar  = %c\r\n",NMEA_CURRENT_TOKEN(buffer));

	if (isupper(NMEA_CURRENT_TOKEN(buffer))) {
		parsed_char   = NMEA_CURRENT_TOKEN(buffer);
		parse_success = true;
	}
	NMEA_NEXT_TOKEN(buffer);

	*out = parsed_char;

	return parse_success;
}

/**
 * \internal
 * Parse sentence until the next comma delimited field.
 *
 * \param[in, out] buffer  Current location in the NMEA sentence
 */
static void _nmea_parse_next_field(
		const char **buffer)
{
	while (NMEA_CURRENT_TOKEN(buffer) != ',') {
		NMEA_NEXT_TOKEN(buffer);
	}

	NMEA_NEXT_TOKEN(buffer);
}


#if (CONF_NMEA_GPGGA_ENABLED == true)
/**
 * \internal
 * \brief Parse GGA output sentence
 *
 * Global Positioning System Fixed Data. Time, Position and fix related data.
 *  - $GPGGA
 *  - UTC Time
 *  - Latitude
 *  - N/S Indicator
 *  - Longitude
 *  - E/W Indicator
 *  - Position Fix Indicator (Fix quality:	0 = invalid
								1 = GPS fix (SPS)
								2 = DGPS fix
								3 = PPS fix
								4 = Real Time Kinematic
								5 = Float RTK
								6 = estimated (dead reckoning) (2.3 feature)
								7 = Manual input mode
								8 = Simulation mode)
 *  - Satellites Used
 *  - Horizontal Dilution of Precision
 *  - MSL Altitude
 *  - Units
 *  - Geoidal Separation
 *  - Units
 *  - Age of Diff Corr
 *  - Checksum
 *  - <CR> <LF>
 *
 * \param[in, out] buffer  NMEA sentence to parse
 * \param[out]     data    Parsed sentence data storage location
 *
 * \return Boolean \c true if the sentence was parsed successfully
 */
static bool _nmea_parse_gpgga(
		const char *buffer,
		struct nmea_data_gpgga *const data)
{
	bool parse_success = true;
	uint32_t temp_int;
	uint32_t temp_int2;
	char temp_char;

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_hour = temp_int;
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_minute = temp_int;
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_second = temp_int;

	
	_nmea_parse_next_field(&buffer);
	
	//vPuts("\r\n GPGGA :latitude\r\n");
	//vPuts(buffer);

	parse_success &= _nmea_parse_fixed_point(&buffer, &temp_int, &temp_int2);
	data->coords.latitude.degrees = temp_int / 100;
	data->coords.latitude.minutes = temp_int % 100;
	data->coords.latitude.minutes_frac = temp_int2;

	_nmea_parse_next_field(&buffer);
	//vPuts("\r\n GPGGA :latitude indy\r\n");
	//vPuts(buffer);
	parse_success &= _nmea_parse_character(&buffer, &temp_char);
	data->coords.latitude.indicator = (enum nmea_gps_coord_indicator)temp_char;

	_nmea_parse_next_field(&buffer);
	//vPuts("\r\n GPGGA :longitude\r\n");
	//vPuts(buffer);
	parse_success &= _nmea_parse_fixed_point(&buffer, &temp_int, &temp_int2);
	data->coords.longitude.degrees = temp_int / 100;
	data->coords.longitude.minutes = temp_int % 100;
	data->coords.longitude.minutes_frac = temp_int2;

	_nmea_parse_next_field(&buffer);
	//vPuts("\r\n GPGGA :longitude indy\r\n");
	//vPuts(buffer);
	parse_success &= _nmea_parse_character(&buffer, &temp_char);
	data->coords.longitude.indicator = (enum nmea_gps_coord_indicator)temp_char;

	_nmea_parse_next_field(&buffer);
	//vPuts("\r\n GPGGA :position fix\r\n");
	//vPuts(buffer);
	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->fix_quality = temp_int;
	//vPrintf("\r\n GPGGA :position fix %d \r\n",temp_int);

	_nmea_parse_next_field(&buffer);
	//vPuts("\r\n GPGGA :satellite\r\n");
	//vPuts(buffer);
	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->satellites = temp_int;

	_nmea_parse_next_field(&buffer);

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->altitude = temp_int;

	//vPuts("\r\n gpgga OK...................\r\n");
	return parse_success;
}
#endif


#if (CONF_NMEA_GPGSA_ENABLED == true)
/**
 * \internal
 * \brief Parse GSA output sentence
 *
 * GNSS DOP and Active Satellites
 *  - $GPGSA
 *  - Mode 1 (Auto selection of 2D or 3D fix (M = manual), (A = 2D automatic))
 *  - Mode 2 (1 = fix not available, 2 = 2D, 3 = 3D)
 *  - Satellite Used 1
 *  - ....
 *  - Satellite Used 12
 *  - Position Dilution of Precision
 *  - Horizontal Dilution of Precision
 *  - Vertical Dilution of Precision
 *  - Checksum
 *  - <CR> <LF>
 *
 * \param[in, out] buffer  NMEA sentence to parse
 * \param[out]     data    Parsed sentence data storage location
 *
 * \return Boolean \c true if the sentence was parsed successfully
 */
static bool _nmea_parse_gpgsa(
		const char *buffer,
		struct nmea_data_gpgsa *const data)
{
	bool parse_success = true;
	uint32_t temp_int;
	char temp_char;
	
	//puts(buffer);

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_character(&buffer, &temp_char);
	data->mode_select = temp_char;

	_nmea_parse_next_field(&buffer);
	//vPuts("\r\n GPGSA :Mode ( 1:no fix , 2:2D , 3:3D )\r\n");
	//vPuts(buffer);
	//parse_success &= _nmea_parse_character(&buffer, &temp_char);
	//data->mode2 = temp_char;
	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->fix_status = temp_int;
	//printf("\r\n\r\n GPGSA :Mode ( 1:no fix , 2:2D , 3:3D )>> %ld\r\n",temp_int);

	_nmea_parse_next_field(&buffer);

	for (uint8_t i = 0; i < sizeof(data->satellite) / sizeof(data->satellite[0]); i++) {
		parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
		data->satellite[i] = temp_int;

		_nmea_parse_next_field(&buffer);
	}

	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->pdop = temp_int;

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->hdop = temp_int;

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->vdop = temp_int;

	//vPuts("\r\n gpgsa OK...................\r\n");
	return parse_success;
}
#endif


#if (CONF_NMEA_GPVTG_ENABLED == true)
/**
 * \internal
 * \brief Parse VTG output sentence
 *
 * Course and speed information relative to the ground.
 *  - $GPTVG
 *  - Course
 *  - Reference
 *  - Speed
 *  - Units
 *  - Speed
 *  - Units
 *  - Mode (A = autonomous, D = differential, E = estimated)
 *  - Checksum
 *  - <CR> <LF>
 *
 * \param[in, out] buffer  NMEA sentence to parse
 * \param[out]     data    Parsed sentence data storage location
 *
 * \return Boolean \c true if the sentence was parsed successfully
 */
static bool _nmea_parse_gpvtg(
		const char *buffer,
		struct nmea_data_gpvtg *const data)
{
	bool parse_success = true;
	uint32_t temp_int;
	uint32_t temp_int2;
	
	//puts(buffer);

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_fixed_point(&buffer, &temp_int, &temp_int2);
	data->course      = temp_int;
	data->course_frac = temp_int2;

	_nmea_parse_next_field(&buffer);

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_fixed_point(&buffer, &temp_int, &temp_int2);
	data->course_magnetic      = temp_int;
	data->course_magnetic_frac = temp_int2;

	_nmea_parse_next_field(&buffer);

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_fixed_point(&buffer, &temp_int, &temp_int2);
	data->speed      = temp_int;
	data->speed_frac = temp_int2;

	_nmea_parse_next_field(&buffer);

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_fixed_point(&buffer, &temp_int, &temp_int2);
	data->speed_kmh      = temp_int;
	data->speed_kmh_frac = temp_int2;
	//vPuts("\r\n gpvtg OK...................\r\n");

	return parse_success;
}
#endif


#if (CONF_NMEA_GPZDA_ENABLED == true)
/**
 * \internal
 * \brief Parse ZDA output sentence
 *
 * Time and date in UTC.
 *  - $GPZDA
 *  - HHMMSS
 *  - DD
 *  - MM
 *  - YYYY
 *  - (Null)
 *  - (Null)
 *  - Checksum
 *  - <CR> <LF>
 *
 * \param[in, out] buffer  NMEA sentence to parse
 * \param[out]     data    Parsed sentence data storage location
 *
 * \return Boolean \c true if the sentence was parsed successfully
 */
static bool _nmea_parse_gpzda(
		const char *buffer,
		struct nmea_data_gpzda *const data)
{
	bool parse_success = true;
	uint32_t temp_int;

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_hour = temp_int;
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_minute = temp_int;
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_second = temp_int;

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->utc_day = temp_int;

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->utc_month = temp_int;

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	data->utc_year = temp_int;
	
	//vPuts("\r\n gpzda OK...................\r\n");
	return parse_success;
}
#endif


#if (CONF_NMEA_GPRMC_ENABLED == true)
/**
 * \internal
 * \brief Parse RMC output sentence
 *
 * Global Positioning System Fixed Data. Time, Position and fix related data.
 *  - $GPRMC
 *  - UTC Time
 *  - Status A=active or V=Void.
 *  - Latitude
 *  - N/S Indicator
 *  - Longitude
 *  - E/W Indicator
 *  - Speed over the ground in knots
 *  - Track angle in degrees True
 *  - Date
 *  -  Magnetic Variation
 *  - Checksum
 *  - <CR> <LF>
 *
 * \param[in, out] buffer  NMEA sentence to parse
 * \param[out]     data    Parsed sentence data storage location
 *
 * \return Boolean \c true if the sentence was parsed successfully
 */
static bool _nmea_parse_gprmc(
		const char *buffer,
		struct nmea_data_gprmc *const data)
{
	bool parse_success = true;
	uint32_t temp_int;
	uint32_t temp_int2;
	char temp_char;

	_nmea_parse_next_field(&buffer);
	//vPuts(buffer);
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_hour = temp_int;
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_minute = temp_int;
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_second = temp_int;
	//vPrintf("\r\nRAW GPSTime = %d:%d:%d\r\n",data->utc_hour,data->utc_minute,data->utc_second);
	
	_nmea_parse_next_field(&buffer);
	
	//vPuts("\r\n Mode\r\n");
	//vPuts(buffer);
	parse_success &= _nmea_parse_character(&buffer, &temp_char);
	data->gps_status = (enum nmea_gprmc_mode)temp_char;
	//vPrintf("\r\n Status = %s\r\n",(enum nmea_gprmc_mode)temp_char);
	
	_nmea_parse_next_field(&buffer);

	_nmea_parse_next_field(&buffer);
	
	_nmea_parse_next_field(&buffer);

	_nmea_parse_next_field(&buffer);
	//vPuts("\r\n Before Speed\r\n");
	//vPuts(buffer);
	
	//vPuts("\r\n Speed\r\n");
	_nmea_parse_next_field(&buffer);
	//vPuts(buffer);
	
	parse_success &= _nmea_parse_fixed_point(&buffer, &temp_int, &temp_int2);
	data->speed = temp_int;
	data->speed_frac = temp_int2;
	//vPrintf("\r\nSpeed Knot= %d.%d\r\n",temp_int,temp_int2);
	
	//vPuts("\r\n Angel\r\n");
	_nmea_parse_next_field(&buffer);
	//vPuts(buffer);
	parse_success &= _nmea_parse_fixed_point(&buffer, &temp_int, &temp_int2);
	data->course = temp_int;
	data->course_frac = temp_int2;
	//vPrintf("\r\nAngle = %d.%d\r\n",temp_int,temp_int2);
	
	//vPuts("\r\n Date\r\n");
	_nmea_parse_next_field(&buffer);
	//vPuts(buffer);
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_day = temp_int;
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_month = temp_int;
	parse_success &= _nmea_parse_int(&buffer, 2, &temp_int);
	data->utc_year = temp_int;
	//vPrintf("\r\nDate = %d/%d/%d\r\n",data->utc_day,data->utc_month,data->utc_year);

	//vPuts("\r\n gprmc OK...................\r\n");
	//vPrintf("\r\n parse_success GPRMC........%d \r\n",parse_success);
	return parse_success;
}
#endif

#if (CONF_NMEA_GPGSV_ENABLED == true)
/**
 * \internal
 * \brief Parse GSV output sentence
 *
 * Satellite in view.
 *  - $GPGSV
 *  - Number of sentence for full data
 *  - Sentence number
 *  - Number of satellite in view
 *  - Satellite PRN number
 *  - Elevation
 *  - Azimuth
 *  - SNR
     ........  Up to 4 satellite per sentence
 
 *  - Checksum
 *  - <CR> <LF>
 *
 * \param[in, out] buffer  NMEA sentence to parse
 * \param[out]     data    Parsed sentence data storage location
 *
 * \return Boolean \c true if the sentence was parsed successfully
 */
static bool _nmea_parse_gpgsv(
		const char *buffer,
		struct nmea_data_gpgsv *const data)
{
	bool parse_success = true;
	uint32_t temp_int;
	//uint8_t numbAllSentence = 0;
	uint8_t sentenceNb = 0;
	static uint8_t tab_idx = 0;
	
	//puts("\r\n");
	//puts(buffer);
	
	_nmea_parse_next_field(&buffer);
	
	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	//numbAllSentence = temp_int;

	_nmea_parse_next_field(&buffer);

	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	sentenceNb = temp_int;
	
	_nmea_parse_next_field(&buffer);
	
	parse_success &= _nmea_parse_int(&buffer, -1, &temp_int);
	if(sentenceNb==1) {
		tab_idx = 0;
		data->NbSatInViews = temp_int;
	}
	
	while ((tab_idx < data->NbSatInViews)&&(*buffer != '\0')&&(*buffer != '*')) {
		_nmea_parse_next_field(&buffer);
		
		_nmea_parse_int(&buffer, -1, &temp_int);
		data->gps_view[tab_idx].PRN_nb = temp_int;
		
		_nmea_parse_next_field(&buffer);
		_nmea_parse_int(&buffer, -1, &temp_int);
		data->gps_view[tab_idx].Elevation = temp_int;
			
		_nmea_parse_next_field(&buffer);
		_nmea_parse_int(&buffer, -1, &temp_int);
		data->gps_view[tab_idx].Azimuth = temp_int;
			
		_nmea_parse_next_field(&buffer);
		_nmea_parse_int(&buffer, -1, &temp_int);
		data->gps_view[tab_idx].SNR = temp_int;
		
		tab_idx++;
	}

	return parse_success;
}
#endif

/**
 * Parses an input NMEA sentence into its component field values.
 *
 * \param[in]  buffer  Input buffer containing the NMEA sentence to parse
 * \param[out] data    Structure where the parsed values are to be stored
 *
 * \return Boolean \c true if the sentence was parsed successfully
 */
int8_t nmea_parse_input(
		const char* buffer,
		uint16_t len,
		struct nmea_data *const data)
{
	/* Verify the computed checksum against the checksum extracted from
	 * the input NMEA sentence to verify they match
	 */
	if (nmea_compute_checksum(buffer) != nmea_extract_checksum(buffer)) {
		return -1;
	}

	if ((buffer[0]!='$')||(buffer[1]!='G')||(len < 6)) {
		/* Do nothing */
	}
	#if (CONF_NMEA_GPGGA_ENABLED == true)
	else if (strncmp(buffer, "$GPGGA", 6) == 0) { //"$GNGGA"
		data->type = NMEA_TYPE_GPGGA;
		return _nmea_parse_gpgga(buffer, &data->gpgga);
	}
	#endif
	#if (CONF_NMEA_GPGSA_ENABLED == true)
	else if (strncmp(buffer, "$GNGSA", 6) == 0) { //"$GPGSA"
		data->type = NMEA_TYPE_GPGSA;
		return _nmea_parse_gpgsa(buffer, &data->gpgsa);
	}
	#endif
	#if (CONF_NMEA_GPVTG_ENABLED == true)
	else if (strncmp(buffer, "$GNVTG", 6) == 0) { //"$GPVTG",
		data->type = NMEA_TYPE_GPVTG;
		return _nmea_parse_gpvtg(buffer, &data->gpvtg);
	}
	#endif
	#if (CONF_NMEA_GPZDA_ENABLED == true)
	else if (strncmp(buffer, "$GNZDA", 6) == 0) { //"$GPZDA",
		data->type = NMEA_TYPE_GPZDA;
		return _nmea_parse_gpzda(buffer, &data->gpzda);
	}
	#endif
	#if (CONF_NMEA_GPRMC_ENABLED == true)
	else if (strncmp(buffer, "$GNRMC", 6) == 0) { //"$GPRMC",
	data->type = NMEA_TYPE_GPRMC;
	return _nmea_parse_gprmc(buffer, &data->gprmc);
	}
	#endif
	#if (CONF_NMEA_GPGSV_ENABLED == true)
	else if (strncmp(buffer, "$GPGSV", 6) == 0) { //"$GPGSV"
		data->type = NMEA_TYPE_GPGSV;
		return _nmea_parse_gpgsv(buffer, &data->gpgsv);
	}
	#endif
	else
	{
		data->type = NMEA_TYPE_UNKNOWN;
		return false;
	}
	return false;
}

/**
 * Computes the correct NMEA checksum value for a given sentence. The input
 * sentence may or may not contain a checksum field already; if one exists
 * it will be ignored for the calculation.
 *
 * \param[in]  buffer  Input buffer containing the NMEA sentence to compute
 *
 * \return Checksum value computed from the given NMEA sentence
 */
uint8_t nmea_compute_checksum(const char *buffer)
{
	uint8_t checksum = 0;

	/* Skip over the leading $ start character if one is present */
	if (*buffer == '$') {
		buffer++;
	}

	/* Compute until either checksum field or end of partial sentence reached */
	while (*buffer != '\0' && *buffer != '*') {
		checksum ^= *(buffer++);
	}

	return checksum;
}

/**
 * \internal
 * Converts an ASCII hex digit (0-9, A-F) into its numerical equivalent.
 *
 * \note The input must be in upper-case only.
 *
 * \param[in]  hex_char  Input HEX character to convert
 *
 * \return Converted value of the HEX digit
 */
static uint8_t _nmea_hex_to_int(
		const signed char hex_char)
{
	if (isupper(hex_char)) {
		return (10 + (hex_char - 'A'));
	}
	else {
		return (hex_char - '0');
	}
}

/**
 * Extracts the NMEA checksum value of a given sentence, from the trailing
 * checksum field.
 *
 * \param[in]  buffer  Input buffer containing the NMEA sentence to extract
 *
 * \return Checksum value extracted from the given NMEA sentence
 */
uint8_t nmea_extract_checksum(const char *buffer)
{
	/* Find checksum field start position */
	char* checksum_pos = strrchr(buffer, '*');

	/* If no checksum field abort */
	if (checksum_pos == NULL) {
		return 0;
	}

	uint8_t checksum;

	/* Convert HEX digits of the checksum to an integer */
	checksum  = _nmea_hex_to_int(checksum_pos[1]) << 4;
	checksum += _nmea_hex_to_int(checksum_pos[2]);

	return checksum;
}
