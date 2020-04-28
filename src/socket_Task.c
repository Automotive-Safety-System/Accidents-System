/*
 * socket_Task.c
 *
 *  Created on: Apr 26, 2020
 *      Author: Hazem
 */
#include "socket_Task.h"
#include "../Libraries/Middleware/json/parson.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ESP01.h"
#include "MPU6050.h"
#include "tm_stm32_gps.h"
#include "stm32f4_discovery.h"

void socket_SendDataTask(void * pvParameters){

	JSON_Value *root_value ;
	JSON_Object *root_object ;
	char *serialized_string ;
	extern TM_GPS_t GPS_Data;
	extern TM_GPS_Float_t GPS_Float;
	extern MPU6050_t MY_MPU;
	char buffer[200]={'\0'};
	char num[20]={'\0'};

	for(;;){
		/************************MPU DATA***********************************/
		root_value = json_value_init_object();
		root_object = json_value_get_object(root_value);
		serialized_string = NULL;

		json_object_set_string(root_object, "id", "15");
		json_object_set_string(root_object, "type", "mpu");
		itoa(MY_MPU.Accel_X_RAW,num,10);
		json_object_set_string(root_object, "ax", num);
		memset(num, 0, 20*sizeof(char));
		itoa(MY_MPU.Accel_Y_RAW,num,10);
		json_object_set_string(root_object, "ay", num);
		memset(num, 0, 20*sizeof(char));
		itoa(MY_MPU.Accel_Z_RAW,num,10);
		json_object_set_string(root_object, "az", num);
		memset(num, 0, 20*sizeof(char));

		json_serialize_to_buffer(root_value,buffer,200);

		if (ESP_getSendBufferFree() >= json_serialization_size(root_value)){
			ESP_SendDataRT(buffer,json_serialization_size(root_value));
		}
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
		memset(buffer, 0, 200*sizeof(char));
		vTaskDelay(pdMS_TO_TICKS(5));


		/*************************************************************************/
		/************************GPS DATA***********************************/
		root_value = json_value_init_object();
		root_object = json_value_get_object(root_value);
		serialized_string = NULL;

		json_object_set_string(root_object, "id", "15");
		json_object_set_string(root_object, "type", "gps");

		TM_GPS_ConvertFloat(GPS_Data.Latitude, &GPS_Float, 6);
		sprintf(buffer, "%d.%d", GPS_Float.Integer, GPS_Float.Decimal);
		json_object_set_string(root_object, "latitude", buffer);
		memset(buffer, 0, 200*sizeof(char));

		TM_GPS_ConvertFloat(GPS_Data.Longitude, &GPS_Float, 6);
		sprintf(buffer, "%d.%d", GPS_Float.Integer, GPS_Float.Decimal);
		json_object_set_string(root_object, "longitude", buffer);
		memset(buffer, 0, 200*sizeof(char));

		TM_GPS_ConvertFloat(GPS_Data.Altitude, &GPS_Float, 6);
		sprintf(buffer, "%d.%d", GPS_Float.Integer, GPS_Float.Decimal);
		json_object_set_string(root_object, "altitude", buffer);
		memset(buffer, 0, 200*sizeof(char));

		json_serialize_to_buffer(root_value,buffer,200);

		if (ESP_getSendBufferFree() >= json_serialization_size(root_value)){
			ESP_SendDataRT(buffer,json_serialization_size(root_value));
		}
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
		memset(buffer, 0, 200*sizeof(char));
		vTaskDelay(pdMS_TO_TICKS(5));


		/*************************************************************************/

		/************************IF ACCIDENT LOG***********************************/
		root_value = json_value_init_object();
		root_object = json_value_get_object(root_value);
		serialized_string = NULL;
		//TODO: check first if there is and accident to send this signal
		json_object_set_string(root_object, "id", "15");
		json_object_set_string(root_object, "type", "accident_signal");

		json_serialize_to_buffer(root_value,buffer,200);

		if (ESP_getSendBufferFree() >= json_serialization_size(root_value)){
			ESP_SendDataRT(buffer,json_serialization_size(root_value));
		}
		json_free_serialized_string(serialized_string);
		json_value_free(root_value);
		memset(buffer, 0, 200*sizeof(char));
		vTaskDelay(pdMS_TO_TICKS(5));

		/*************************************************************************/


		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
