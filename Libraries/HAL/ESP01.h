/*
 * ESP01.h
 *
 *  Created on: Feb 1, 2020
 *      Author: Hazem
 */

#ifndef HAL_ESP01_H_
#define HAL_ESP01_H_

#include <stdint.h>
#define	ESP_USART					USART2
#define ESP_MODE_STATION				1
#define ESP_MODE_ACCESSPOINT			2
#define ESP_MODE_BOTH_STA				3


enum ESP_JOINAP_STATUS {
	ESP_WIFI_CONNECTED,
	ESP_CONNECTION_TIMEOUT,
	ESP_WRONG_PASSWORD,
	ESP_NOT_FOUND_TARGET_AP,
	ESP_CONNECTION_FAILED,
	ESP_JOIN_UNKNOWN_ERROR
};


void ESP_init();
uint8_t ESP_sendBlindCommand(char* command);
uint8_t ESP_sendRequest(char *command, char *expected_response);
uint8_t ESP_connectAccessPoint(char* ssid, char* password);
void ESP_connectAccessPointTask(void * pvParameters);
uint8_t ESP_WIFIMode(uint8_t mode);
uint8_t ESP_ConnectionMode(uint8_t mode);
uint8_t ESP_ApplicationMode(uint8_t mode);
int ESP_TCPStatus(void);
uint8_t ESP_StartTCP(char* Domain, char* Port);
void ESP_StartTCPTask(void * pvParameters);
uint8_t ESP_ReadData(char* user_buffer, char delimeter);
int ESP_ReadDataRT(unsigned char *address, unsigned int maxbytes);
void ESP_ReadDataTask(void * pvParameters);
uint8_t ESP_SendData(uint32_t length, char* data);
int ESP_SendDataRT(unsigned char *address, unsigned int bytes);
void ESP_SendDataTask(void * pvParameters);
int ESP_getSendBufferFree();
#endif /* HAL_ESP01_H_ */
