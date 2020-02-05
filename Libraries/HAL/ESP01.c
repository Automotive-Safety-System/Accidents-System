/*
 * ESP01.c
 *
 *  Created on: Feb 1, 2020
 *      Author: Hazem
 */


#include "ESP01.h"
#include "ESP01_config.h"
#include "stm32f4xx_usart.h"
#include "stm32f4_discovery.h"
#include "string.h"

void ESP_init(){
	char acknowldge[100]={'\0'};
    uint32_t i = 0;
    uint8_t init_flag = 0;

    /*enable clock for ESP gpio and usart */

	  RCC_APB1PeriphClockCmd(ESP_USART_RCC, ENABLE);
	  RCC_AHB1PeriphClockCmd(ESP_GPIO_RCC, ENABLE);

	  GPIO_InitTypeDef 	gpio_init_struct;

	  /*configure usart rx and tx*/
	  gpio_init_struct.GPIO_Pin = ESP_TX_PIN|ESP_RX_PIN;
	  gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
	  gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	  gpio_init_struct.GPIO_OType = GPIO_OType_PP;
	  gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP ;

	  GPIO_Init(ESP_GPIO, &gpio_init_struct);

	  GPIO_PinAFConfig(ESP_GPIO, ESP_AF_PIN2_SOURCE, ESP_AF_USART);
	  GPIO_PinAFConfig(ESP_GPIO, ESP_AF_PIN1_SOURCE, ESP_AF_USART);

	  /*configure NVIC*/
	  NVIC_InitTypeDef usart_irq_struct;

	  usart_irq_struct.NVIC_IRQChannel = ESP_USART_IRQ;
	  usart_irq_struct.NVIC_IRQChannelCmd = ENABLE;
	  usart_irq_struct.NVIC_IRQChannelPreemptionPriority = ESP_USART_IRQ_PERIORITY;
	  usart_irq_struct.NVIC_IRQChannelSubPriority = ESP_USART_IRQ_PERIORITY;

	  NVIC_Init(&usart_irq_struct);

	  /*configure usart*/

	  USART_InitTypeDef 	usart_init_struct;

	  usart_init_struct.USART_BaudRate = ESP_BAUD_RATE;
	  usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  usart_init_struct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	  usart_init_struct.USART_Parity = USART_Parity_No;
	  usart_init_struct.USART_StopBits = USART_StopBits_1;
	  usart_init_struct.USART_WordLength = USART_WordLength_8b;

	  USART_Init(ESP_USART, &usart_init_struct);

	  ESP_USART->CR1 |=(1<<6);

	  USART_ITConfig(ESP_USART, USART_IT_RXNE, ENABLE);

	  USART_Cmd(ESP_USART, ENABLE);

//TODO : convert the following to a time constrained hand shake in RTOS

	  /*send AT to check if the module connected*/
	 USART_SendString(ESP_USART, "AT\r\n", -1);
	 while(!init_flag){

	  while (!USART_BufferEmpty(ESP_USART)){
	  acknowldge[i] = USART_ReceiveData(ESP_USART);
	  i++;}


		  if(strstr(acknowldge, "OK") != 0){
			 //printf("connected to esp \n");
			 init_flag = 1;
		  }
		  else if(strstr(acknowldge, "ERROR") != 0){
			  //STM_EVAL_LEDOn(LED4);
			  //printf("connection with esp failed");
		  }

	 }

}

// TODO : convert the following to a time constrained tasks
uint8_t ESP_sendBlindCommand(char* command){

	USART_SendString(ESP_USART, command, -1);
	return 1 ;
}

//returns 1 if expected response has been received and 2 if error occurred
uint8_t ESP_sendRequest(char *command, char *expected_response){
	uint32_t i = 0;
	uint8_t response_flag = 0;
	char response[100]={'\0'};
	USART_SendString(ESP_USART, command, -1);
	while(!response_flag){

		  while (!USART_BufferEmpty(ESP_USART)){
			  response[i] = USART_ReceiveData(ESP_USART);
		  i++;}


			  if(strstr(response, expected_response) != 0){
				 response_flag = 1;
			  }
			  else if(strstr(response, "ERROR") != 0){
				  return 2;
			  }
	}
	return response_flag;
}

// TODO : convert the following to a task
uint8_t ESP_readData(char* user_buffer, char delimeter){

	uint32_t i = 0;
	while (!USART_BufferEmpty(ESP_USART)){
			user_buffer[i] = USART_ReceiveData(ESP_USART);
			i++;
	}
	return 1;
}


uint8_t ESP_connectAccessPoint(char* ssid, char* password){
	char command[100] = {'\0'};
	sprintf(command, "AT+CWJAP=\"%s\",\"%s\"%s", ssid, password, "\r\n");
	return ESP_sendRequest(command, "WIFI CONNECTED");

	//TODO
	/*

	 	 if (ESP_sendRequest(command, "WIFI CONNECTED"))
	 	 	 return ESP_WIFI_CONNECTED;

	 	 if(strstr(RESPONSE_BUFFER, "+CWJAP:1"))
		return ESP_CONNECTION_TIMEOUT;
		else if(strstr(RESPONSE_BUFFER, "+CWJAP:2"))
		return ESP_WRONG_PASSWORD;
		else if(strstr(RESPONSE_BUFFER, "+CWJAP:3"))
		return ESP_NOT_FOUND_TARGET_AP;
		else if(strstr(RESPONSE_BUFFER, "+CWJAP:4"))
		return ESP_CONNECTION_FAILED;
		else
		return ESP_JOIN_UNKNOWN_ERROR;*/
}

uint8_t ESP_WIFIMode(uint8_t mode){

	char command[20]={'\0'};
	sprintf(command, "AT+CWMODE=%d%s", mode,"\r\n");
	return ESP_sendRequest(command, "OK");

}

