/*
 * distance_calc.c
 *
 *  Created on: 20 ¡.¾. 2555
 *      Author: Pan
 */

#include "adl_global.h"
#include "math.h"
#include "distance_calc.h"

#define GPS_METERRING_GPS_USE_WGS84_FORMAT	1

static inline double DISTANCE_CALC_Deg2Rad(double Dgree) {
	/*
	PI/180 = (4*atan(1.0)/(360/2)) = 0.0174532925
	*/
	return (Dgree*0.0174532925);
}

#if !GPS_METERRING_GPS_USE_WGS84_FORMAT
// Convert from DM (Decimal Minute) to DD (Decimal Degree)
static double DISTANCE_CALC_DecMin2DecDeg ( double val_100dm ) {
	u32 decimal = 0;
	double floating = 0.0000000;
	u32 Degree = 0;
	Degree = (u32)(val_100dm/100);
	decimal = (u32)(Degree*10000000);
	//floating = (double)((((val_100dm*10000)-decimals)/10000)/60);//0.xxxxxx
	floating = (double)((((val_100dm*100000)-decimal))/6000000.000000);//0.xxxxxx
	return (((double)Degree)+(double)floating);
}
#endif


double distance_calc_lac( distance_calc_coordinate_t p0, distance_calc_coordinate_t p1) {
	double diff_lat = 0.0 , diff_lon = 0.0;
	double arc_circle = 0.0, dist_circle = 0.0;
	static const double two	= 2.0;
	static const double radius = 6378137.0;//meter
	//static const double radius = 6378.137;//Km.
	//static const double radius = 3956.6;//mile

	#if !GPS_METERRING_GPS_USE_WGS84_FORMAT
	// convert DM to DD
	p0.lat = DISTANCE_CALC_DecMin2DecDeg (p0.lat);
	p0.lon = DISTANCE_CALC_DecMin2DecDeg (p0.lon);
	p1.lat = DISTANCE_CALC_DecMin2DecDeg (p1.lat);
	p1.lon = DISTANCE_CALC_DecMin2DecDeg (p1.lon);
	#endif

	// get delta lat,lon in absolute float value
	diff_lat = fabs(p1.lat - p0.lat);
	diff_lon = fabs(p1.lon - p0.lon);

	// convert Degree to Radians
	diff_lat = DISTANCE_CALC_Deg2Rad(diff_lat);
	diff_lon = DISTANCE_CALC_Deg2Rad(diff_lon);

	// Refer to Great Circle Distance Thoery
	arc_circle = ((sin(diff_lat/two))*(sin(diff_lat/two)))+ cos(DISTANCE_CALC_Deg2Rad(p0.lat))*cos(DISTANCE_CALC_Deg2Rad(p1.lat))*((sin(diff_lon/two))*(sin(diff_lon/two)));
	dist_circle = two * atan2(sqrt(arc_circle),sqrt(1-arc_circle));
	return (double) (radius * dist_circle);//convert to meters -> used geoid datum not 20000/PI

}


