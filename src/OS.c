/*
 * OS.c
 *
 *  Created on: Feb 23, 2020
 *      Author: Hazem
 */

#include "OS.h"

#include "tm_stm32_gps.h"
#include "GSM.h"
#include "ESP01.h"
#include "MPU6050.h"
#include "MQTT_Tasks.h"
#include "socket_Task.h"

#include "MPU6050.h"
#include "Accident.h"
#include "task.h"

#include "queue.h"
#include "semphr.h"
#include "stm32f4_discovery.h"
/*create all tasks here*/


extern TaskHandle_t handle_gsm_os_init;

TaskHandle_t xTask_MPU6050_Read_RawData;

TaskHandle_t xTask_AccidentDetection_Declration;

xQueueHandle Accident_Detection_Queue;
xSemaphoreHandle Accident_Detection_semphr;




void OS_INIT(void){

//STM_EVAL_LEDOn(LED3);
	 Accident_Detection_Queue=xQueueCreate( 1, sizeof( unsigned int ) );
	 vSemaphoreCreateBinary( Accident_Detection_semphr );

	if((Accident_Detection_Queue!=NULL)&&(Accident_Detection_semphr!=NULL)){
		xTaskCreate(GPS_update_task,
					"gps_update",
					200,
					NULL,
					5,
					NULL);

		xTaskCreate(gsm_os_init,
					"gsm_os_init",
					200,
					NULL,
					5,
					&handle_gsm_os_init);

		xTaskCreate(ESP_connectAccessPointTask,
					"esp_connectAP",
					200,
					NULL,
					5,
					NULL);

		xTaskCreate(ESP_StartTCPTask,
					"esp_connectTCP",
					200,
					NULL,
					5,
					NULL);

		xTaskCreate(ESP_SendDataTask,
					"esp_sendData",
					200,
					NULL,
					5,
					NULL);

		xTaskCreate(ESP_ReadDataTask,
					"esp_receiveData",
					200,
					NULL,
					5,
					NULL);

		xTaskCreate(MPU6050_Read_All,
					"MPU6050_ReadData",
					200,
					NULL,
					3 ,
					&xTask_MPU6050_Read_RawData);


		xTaskCreate(MQTT_SendDataTask,
					"Pub",
					400,
					NULL,
					5,
					NULL);



		xTaskCreate(socket_SendDataTask,
					"Pub",
					200,
					NULL,
					5,
					NULL);



		xTaskCreate(Accident_task,
					  "Accident_Decleration",
					  200,
					  NULL,
					  4 ,
					  &xTask_AccidentDetection_Declration);

	}


}
