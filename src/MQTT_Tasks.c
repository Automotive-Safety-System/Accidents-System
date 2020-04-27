/*
 * MQTT_Tasks.c
 *
 *  Created on: Mar 20, 2020
 *      Author: Hazem
 */


#include "MQTT_Tasks.h"
#include "stm32f4_discovery.h"
#include "../Libraries/Middleware/MQTTPacket/MQTTPacket.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ESP01.h"
#include "stm32f4xx_usart.h"


#define CONNECTION_KEEPALIVE_S 60UL


void MQTT_SendDataTask(void * pvParameters){


	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int rc = 0;
	char buf[200];
	char filtered_buf[200];
	MQTTString topicString = MQTTString_initializer;
	char* payload = "pay";
	int payloadlen = strlen(payload);
	int buflen = sizeof(buf);
	int i = 0;
	int temp;
	char response[200];

	for(;;){




					int filtered_buf_len = 0;
					data.clientID.cstring = "me";
					data.keepAliveInterval = 20;
					data.cleansession = 1;
					int len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */
					//ESP_SendDataRT( buf, len);
					topicString.cstring = "mytopic/";
					len += MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, payload, payloadlen); /* 2 */

					len += MQTTSerialize_disconnect(buf + len, buflen - len); /* 3 */
					//MQTTPacket_encode(buf, len);
					// Send PUBLISH to the mqtt broker.
					// TODO: you have the packet but you can't send it :D
					/*	switch (buf[i]){
						case '\0':{ filtered_buf[filtered_buf_len++] = '0';} break;

						default :{filtered_buf[filtered_buf_len++] = buf[i];} break;
						}*/

						ESP_SendDataRT(buf,len);
				/*		char command[20]={'\0'};
						sprintf(command, "AT+CIPSEND=%lu%s", len,"\r\n");
						ESP_sendBlindCommand(command);
						vTaskDelay(pdMS_TO_TICKS(200));
						for(i=0; i<len; i++){
							vTaskDelay(pdMS_TO_TICKS(1));
							USART_SendData(ESP_USART, buf[i]);
						}
						vTaskDelay(pdMS_TO_TICKS(2000));
						USART_ReceiveString(ESP_USART, response ,200);
						unsigned char sessionPresent, connack_rc;
						if ((MQTTDeserialize_connack(&sessionPresent, &connack_rc, response, sizeof(response)) != 1) || (connack_rc != 0)){
							STM_EVAL_LEDOn(LED6);
						}
				 */

					 //ESP_SendDataRT( buf, len);
					// STM_EVAL_LEDOn(LED6);








		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

