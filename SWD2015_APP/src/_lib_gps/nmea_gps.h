/*
 * gps.h
 *
 * Created: 22/9/2557 10:18:52
 *  Author: pan
 */ 


#ifndef GPS_H_
#define GPS_H_
#include "nmea.h"


/** Multiplication factor of processed linear GPS latitude and longitude
 *  coordinates. Divide by this constant to obtain the integer number of
 *  degrees, modulus this value to get the fractional number of degrees.
 */
#define GPS_POS_DEGREES_SCALER         100000


/** Structure for a set of processed GPS coordinate values, after parsing them
 *  out from an attached GPS module.
 */
struct gps_position_data
{
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
	
	/* Position information */
	uint8_t	iFixState;
	uint8_t iFixMode ;
	bool	validStatus;
	/* Position parameters */
	double		latitude;
	double		longitude;
	//float		altitudeEll;
	//float		altitudeMsl;
	/* Movement parameters */
	float		speed;
	float		course;
	//float		verticalVelocity;
	/* Cartesian coordinates */
	//double		xCoord;
	//double		yCoord;
	//double		zCoord;
	/* Precision parameters */
	float			hDop;
	float			vDop;
	float			pDop;
	//float			horizontalPositionError;
	//float			verticalPositionError;
	//float			horizontalSpeedError;
	//float			verticalSpeedError;
};


#endif /* GPS_H_ */