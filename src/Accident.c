/*
 * Accident.c
 *
 *  Created on: May 4, 2020
 *      Author: Hazem
 */

#include "tm_stm32_gps.h"



void Accident_task(void * pvParameters){
	float speed = 0;
	extern TM_GPS_t GPS_Data;


	for(;;){
			/* Current speed in km/h */
			speed = TM_GPS_ConvertSpeed(GPS_Data.Speed, TM_GPS_Speed_KilometerPerHour);

	}
}
