/*
 * distance_calc.h
 *
 *  Created on: 20 ¡.¾. 2555
 *      Author: Pan
 */

#ifndef DISTANCE_CALC_H_
#define DISTANCE_CALC_H_

typedef struct _DISTANCE_CALC_COORDINATE_T {
	double lat;
	double lon;
}distance_calc_coordinate_t;

double distance_calc_lac (distance_calc_coordinate_t p0, distance_calc_coordinate_t p1);

#endif /* DISTANCE_CALC_H_ */
