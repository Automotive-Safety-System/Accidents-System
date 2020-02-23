/*
 * OS.c
 *
 *  Created on: Feb 23, 2020
 *      Author: Hazem
 */
#include "OS.h"
#include "tm_stm32_gps.h"

/*create all tasks here*/
void OS_INIT(void){

	xTaskCreate(GPS_update_task,
				"gps_update",
				200,
				NULL,
				5,
				NULL);

}
