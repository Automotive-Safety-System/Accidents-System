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
/*create all tasks here*/


extern TaskHandle_t handle_gsm_os_init;

extern TaskHandle_t xTask_MPU6050_Read_RawData;

void OS_INIT(void){


/*	xTaskCreate(GPS_update_task,
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
					&handle_gsm_os_init); */

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

/*	  xTaskCreate(MPU6050_Read_All,
			  	  "MPU6050_ReadData",
				  200,
				  NULL,
				  5 ,
				  &xTask_MPU6050_Read_RawData);
*/
}


