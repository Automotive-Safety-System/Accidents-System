/*
 * Accident.c
 *
 *  Created on: May 4, 2020
 *      Author: Hazem
 */
#include <stdio.h>
#include "tm_stm32_gps.h"
#include "MPU6050.h"
#include "stm32f4_discovery.h"
#include "queue.h"
#include "semphr.h"


// Accident Fliping Constants
#define MAX_FLIPPING_Angle  (float)(30.0)
#define MIN_FLIPPING_Angle  (float)(-30.0)

#define MAX_Positive_ACC_Decleration	(float)(3.0)
#define MAX_Negative_ACC_Decleration    (float)(-3.0)

#define ACCIDENT_DECETED_FLIPPING 	2

#define ACCIDENT_DECETED_Crash		3


extern xQueueHandle Accident_Detection_Queue;
extern xSemaphoreHandle Accident_Detection_semphr;


static float Last_Acc_X ;
static float Last_Acc_Y ;
static float Last_Acc_Z ;


static void Update_last_ACC_Values(void);


void Accident_task(void * pvParameters){
	float speed = 0;
	extern TM_GPS_t GPS_Data;

	int8_t temp;
	portBASE_TYPE xStatus;

	float current_Angle_X;
	float current_Angle_Y;
	float current_Angle_Z;

	float current_ACC_X;
	float current_ACC_Y;
	float current_ACC_Z;

	Last_Acc_X=0;
	Last_Acc_Y=0;
	Last_Acc_Z=0;
	//char buf[20];
	xSemaphoreTake(Accident_Detection_semphr, portMAX_DELAY);
	while(1)
	{

		xSemaphoreTake(Accident_Detection_semphr, portMAX_DELAY);

		printf("Accident Task\n");
			//Current speed in km/h
		speed = TM_GPS_ConvertSpeed(GPS_Data.Speed, TM_GPS_Speed_KilometerPerHour);
		current_Angle_X=Get_Angle_X();
		current_Angle_Y=Get_Angle_Y();
		current_Angle_Z=Get_Angle_Z();

		current_ACC_X=Get_ACC_X();
		current_ACC_Y=Get_ACC_Y();
		current_ACC_Z=Get_ACC_Z();



		/* check Accident rolling over */
		if((current_Angle_X>=MAX_FLIPPING_Angle)||(current_Angle_X<=MIN_FLIPPING_Angle)
				||(current_Angle_Y>=MAX_FLIPPING_Angle)||(current_Angle_Y<=MIN_FLIPPING_Angle)
				||(current_Angle_Z>=MAX_FLIPPING_Angle)||(current_Angle_Z<=MIN_FLIPPING_Angle))
		{
			if(speed<24)
			{

				STM_EVAL_LEDOn(LED3);

				printf("Accident Detected Flipping\n");
				temp=ACCIDENT_DECETED_FLIPPING;

				/* queue between app task and accident task*/
				xStatus= xQueueSend(Accident_Detection_Queue,&temp,0);

				if(xStatus)
				{
					//call app task
				}
			}
			else {
				printf("false accident alarm\n");
			}

		}
		/* check Accident decleration*/
		else if(((Last_Acc_X-current_ACC_X)>=MAX_Positive_ACC_Decleration)||((Last_Acc_X-current_ACC_X)<=MAX_Negative_ACC_Decleration)
			||((Last_Acc_Y-current_ACC_Y)>=MAX_Positive_ACC_Decleration)||((Last_Acc_Y-current_ACC_Y)<=MAX_Negative_ACC_Decleration)
			||((Last_Acc_Z-current_ACC_Z)>=MAX_Positive_ACC_Decleration)||((Last_Acc_Z-current_ACC_Z)<=MAX_Negative_ACC_Decleration))
		{

			STM_EVAL_LEDOn(LED4);

			printf("Accident Detected Crash\n");
			temp=ACCIDENT_DECETED_Crash;

			/* queue between app task and accident task*/
			xStatus= xQueueSend(Accident_Detection_Queue,&temp,0);

			if(xStatus){
				//call app task
			}
		}

		//temp1=Last_Acc_X-current_ACC_X;

/*
		gcvt(Last_Acc_X,5,buf);

		printf("last:%s\t\t",buf);

		gcvt(current_ACC_X,5,buf);

		printf("current:%s\t\t",buf);

		temp=Last_Acc_X-current_ACC_X>=MAX_Positive_ACC_Decleration;
		printf("	%d \n",temp);

*/

		Update_last_ACC_Values();


	}
}


static void Update_last_ACC_Values(void){
	Last_Acc_X=Get_ACC_X();
	Last_Acc_Y=Get_ACC_Y();
	Last_Acc_Z=Get_ACC_Z();
}
