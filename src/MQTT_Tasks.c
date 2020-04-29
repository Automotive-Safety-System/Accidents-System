/*
 * MQTT_Tasks.c
 *
 *  Created on: Mar 20, 2020
 *      Author: Hazem
 */


#include "MQTT_Tasks.h"
#include "stm32f4_discovery.h"
#include "../Libraries/Middleware/MQTTPacket/MQTTPacket.h"
#include "../Libraries/Middleware/json/parson.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ESP01.h"
#include "MPU6050.h"
#include "tm_stm32_gps.h"

#include "stm32f4xx_usart.h"


#define CONNECTION_KEEPALIVE_S 60UL


void MQTT_SendDataTask(void * pvParameters){


	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	char buf[200];
	MQTTString topicString = MQTTString_initializer;
	char payload[200]={'\0'};
	int payloadlen ;
	int buflen = sizeof(buf);
	int len=0;
	int flag =0;
	int state = 0;


	JSON_Value *root_value ;
	JSON_Object *root_object ;
	char *serialized_string ;
	extern TM_GPS_t GPS_Data;
	extern TM_GPS_Float_t GPS_Float;
	extern MPU6050_t MY_MPU;
	char num[20]={'\0'};


	for(;;){
		if (ESP_TCPStatus()){
			len =0;
			data.clientID.cstring = "me";
			data.keepAliveInterval = 20;
			data.cleansession = 1;
			if (flag==0){
				len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */
				ESP_SendDataRT(buf,len);
				flag=1;
			}
			else{
				switch(state){
				case 0:
					topicString.cstring = "accident/gps/";

					root_value = json_value_init_object();
					root_object = json_value_get_object(root_value);
					serialized_string = NULL;

					json_object_set_string(root_object, "id", "15");
					TM_GPS_ConvertFloat(GPS_Data.Latitude, &GPS_Float, 6);
					sprintf(payload, "%d.%d", GPS_Float.Integer, GPS_Float.Decimal);
					json_object_set_string(root_object, "latitude", payload);
					memset(payload, 0, 200*sizeof(char));

					TM_GPS_ConvertFloat(GPS_Data.Longitude, &GPS_Float, 6);
					sprintf(payload, "%d.%d", GPS_Float.Integer, GPS_Float.Decimal);
					json_object_set_string(root_object, "longitude", payload);
					memset(payload, 0, 200*sizeof(char));

					TM_GPS_ConvertFloat(GPS_Data.Altitude, &GPS_Float, 6);
					sprintf(payload, "%d.%d", GPS_Float.Integer, GPS_Float.Decimal);
					json_object_set_string(root_object, "altitude", payload);
					memset(payload, 0, 200*sizeof(char));

					json_serialize_to_buffer(root_value,payload,200);
					payloadlen = json_serialization_size(root_value);

					json_free_serialized_string(serialized_string);
					json_value_free(root_value);
					len = MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, payload, payloadlen); /* 2 */
					if (ESP_getSendBufferFree() >= len){
					ESP_SendDataRT(buf,len);
					}
					memset(payload, 0, 200*sizeof(char));
					state++;
					break;

				case 1:
					topicString.cstring = "accident/mpu/";


					root_value = json_value_init_object();
					root_object = json_value_get_object(root_value);
					serialized_string = NULL;

					json_object_set_string(root_object, "id", "15");
					itoa(MY_MPU.Accel_X_RAW,num,10);
					json_object_set_string(root_object, "ax", num);
					memset(num, 0, 20*sizeof(char));
					itoa(MY_MPU.Accel_Y_RAW,num,10);
					json_object_set_string(root_object, "ay", num);
					memset(num, 0, 20*sizeof(char));
					itoa(MY_MPU.Accel_Z_RAW,num,10);
					json_object_set_string(root_object, "az", num);
					memset(num, 0, 20*sizeof(char));

					json_serialize_to_buffer(root_value,payload,200);
					payloadlen = json_serialization_size(root_value);

					json_free_serialized_string(serialized_string);
					json_value_free(root_value);
					len = MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, payload, payloadlen); /* 2 */
					if (ESP_getSendBufferFree() >= len){
					ESP_SendDataRT(buf,len);
					}
					memset(payload, 0, 200*sizeof(char));
					state++;
					break;

				case 2:
					topicString.cstring = "accident/signal/";

					root_value = json_value_init_object();
					root_object = json_value_get_object(root_value);
					serialized_string = NULL;
					//TODO: check first if there is and accident to send this signal
					json_object_set_string(root_object, "id", "15");
					json_object_set_string(root_object, "type", "accident_signal");
					json_object_set_string(root_object, "value", "false");

					json_serialize_to_buffer(root_value,payload,200);
					payloadlen = json_serialization_size(root_value);

					json_free_serialized_string(serialized_string);
					json_value_free(root_value);
					len = MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, payload, payloadlen); /* 2 */
					if (ESP_getSendBufferFree() >= len){
					ESP_SendDataRT(buf,len);
					}
					memset(payload, 0, 200*sizeof(char));
					state = 0;
					break;

				}
			}
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

