/*
 * ESP01.c
 *
 *  Created on: Feb 1, 2020
 *      Author: Hazem
 */


#include "ESP01.h"
#include "ESP01_config.h"
#include "stm32f4xx_usart.h"
#include "tm_stm32_buffer.h"
#include "stm32f4_discovery.h"
#include "string.h"

#define TCP_CONNECTED			1
#define TCP_NOT_CONNECTED		0
#define AVAILABLE  				0
#define SENDING					1
#define RECEIVING				2

uint8_t ESP_connection_status = ESP_CONNECTION_TIMEOUT;
uint8_t ESP_tcp_status = TCP_NOT_CONNECTED ;
uint8_t resource_allowed = AVAILABLE;

uint8_t ESP_Send_Buffer[ESP_SEND_BUFFER_SIZE];
TM_BUFFER_t ESP_Send_Buffer_obj = {ESP_SEND_BUFFER_SIZE, 0, 0, ESP_Send_Buffer, 0, '\n'};

uint8_t ESP_Receive_Buffer[ESP_RECEIVE_BUFFER_SIZE];
TM_BUFFER_t ESP_Receive_Buffer_obj = {ESP_RECEIVE_BUFFER_SIZE, 0, 0, ESP_Receive_Buffer, 0, '\n'};

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



	  /*send AT to check if the module connected*/
	 USART_SendString(ESP_USART, "AT\r\n", -1);
	 while(!init_flag){

	  while (!USART_BufferEmpty(ESP_USART)){
	  acknowldge[i] = USART_ReceiveData(ESP_USART);
	  i++;}


		  if(strstr(acknowldge, "OK") != 0){
				//STM_EVAL_LEDOn(LED3);

			 //printf("connected to esp \n");
			 init_flag = 1;
			  STM_EVAL_LEDOn(LED4);

		  }
		  else if(strstr(acknowldge, "ERROR") != 0){
			  //printf("connection with esp failed");
		  }

	 }
	 ESP_WIFIMode(1);
	 ESP_ConnectionMode(0);
	 ESP_ApplicationMode(0);

}


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

uint8_t ESP_connectAccessPoint(char* ssid, char* password){
	char command[100] = {'\0'};
	sprintf(command, "AT+CWJAP=\"%s\",\"%s\"%s", ssid, password, "\r\n");
	return ESP_sendRequest(command, "WIFI CONNECTED");


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


/* task is checking the ESP_connection_status
 * if the status is ESP_CONNECTION_TIMEOUT which is
 * the not connected status by default, task tries to connect
 * again after 10000 ms, if any other error occurred you can check
 * that error by reading the status, fix the proplem
 * then set the status to ESP_CONNECTION_TIMEOUT
 * the task should try connecting again within 500+10000 ms*/
void ESP_connectAccessPointTask(void * pvParameters){
	char command[100] = {'\0'};
	sprintf(command, "AT+CWJAP=\"%s\",\"%s\"%s", SSID, PASS, "\r\n");
	char response[100]={'\0'};
	uint8_t i = 0;
	char c;
	//USART_SendString(ESP_USART, command, -1);
	for (;;){
		//i = 0;
	//return ESP_sendRequest(command, "WIFI CONNECTED");
		if (ESP_connection_status == ESP_CONNECTION_TIMEOUT){
			USART_SendString(ESP_USART, command, -1);
			vTaskDelay(pdMS_TO_TICKS(10000));
			while (!USART_BufferEmpty(ESP_USART)){
				if ((c = USART_ReceiveData(ESP_USART))!=0){
					response[i] = c;
					i++;
				}
			}
			if(strstr(response, "WIFI CONNECTED") != 0){
				ESP_connection_status = ESP_WIFI_CONNECTED;
				STM_EVAL_LEDOn(LED3);
			}
			else if(strstr(response, "+CWJAP:1") != 0){
				ESP_connection_status = ESP_CONNECTION_TIMEOUT;
			}
			else if(strstr(response, "+CWJAP:2") != 0){
				ESP_connection_status = ESP_WRONG_PASSWORD;
			}
			else if(strstr(response, "+CWJAP:3") != 0){
				ESP_connection_status = ESP_NOT_FOUND_TARGET_AP;
			}
			else if(strstr(response, "+CWJAP:4") != 0){
				ESP_connection_status = ESP_CONNECTION_FAILED;
			}
			else
				ESP_connection_status = ESP_JOIN_UNKNOWN_ERROR;
		}
		vTaskDelay(pdMS_TO_TICKS(500));

	}
}
uint8_t ESP_WIFIMode(uint8_t mode){
	//mode = 1 for station
	char command[20]={'\0'};
	sprintf(command, "AT+CWMODE=%d%s", mode,"\r\n");
	return ESP_sendRequest(command, "OK");

}

uint8_t ESP_ConnectionMode(uint8_t mode){
// mode = 0 if single connection
	char command[20]={'\0'};
	sprintf(command, "AT+CIPMUX=%d%s", mode,"\r\n");
	return ESP_sendRequest(command, "OK");

}

uint8_t ESP_ApplicationMode(uint8_t mode){
// mode = 0 if non transparent mode
	char command[20]={'\0'};
	sprintf(command, "AT+CIPMODE=%d%s", mode,"\r\n");
	return ESP_sendRequest(command, "OK");

}


uint8_t ESP_StartTCP(char* Domain, char* Port){
	uint8_t startResponse;
	char command[100]={'\0'};

	sprintf(command, "AT+CIPSTART=\"TCP\",\"%s\",%s%s", Domain, Port, "\r\n");

	startResponse = ESP_sendRequest(command, "CONNECT");
	return startResponse;
}

/*this task connects automatically to the web server and port
 * which are defined in ESP01_config.h
 * if for any reason the TCP connection went down,
 * just set the ESP_tcp_status = TCP_NOT_CONNECTED */
void ESP_StartTCPTask(void * pvParameters){

	char command[100]={'\0'};
	char response[100]={'\0'};
	sprintf(command, "AT+CIPSTART=\"TCP\",\"%s\",%s%s", ADDRESS, PORT, "\r\n");
	uint8_t i = 0;
	char c;

	for(;;){
		if(ESP_connection_status == ESP_WIFI_CONNECTED && ESP_tcp_status == TCP_NOT_CONNECTED){
			USART_SendString(ESP_USART, command, -1);
			vTaskDelay(pdMS_TO_TICKS(5000));
			while (!USART_BufferEmpty(ESP_USART)){
				if ((c = USART_ReceiveData(ESP_USART))!=0){
					response[i] = c;
					i++;
				}
			}
			if(strstr(response, "CONNECT") != 0){
				ESP_tcp_status = TCP_CONNECTED;
				STM_EVAL_LEDOn(LED5);
			}
		}
		vTaskDelay(500);
	}

}

int ESP_TCPStatus(void){
	if (ESP_connection_status == ESP_WIFI_CONNECTED && ESP_tcp_status == TCP_CONNECTED)
		return 1;
	else
		return 0;
}

uint8_t ESP_ReadData(char* user_buffer, char delimeter){
//add flag to check first if we expect data
//read data after the form  +IPD, length:data_here
	uint32_t i = 0;
	while (!USART_BufferEmpty(ESP_USART)){
			user_buffer[i] = USART_ReceiveData(ESP_USART);
			i++;
	}
	return 1;
}

/*receives string from the receiving buffer
 * The whole receiving process is handled then by RTOS
 * if you are using RTOS, start the receiving task and use
 * this API to receive data.
 * @Parameters:address: the reference to the user buffer,
 * 			   maxbytes: refers to the maximum length expected by the user.
 *
 * @returns: the true length have been received.
 * 			   */

int ESP_ReadDataRT(unsigned char *address, unsigned int maxbytes){
	char c;
	unsigned int i = 0;
	for(i=0 ; i<maxbytes ; i++){
		if(TM_BUFFER_GetFull(&ESP_Receive_Buffer_obj) == 0){
			return i;
		}
		TM_BUFFER_Read(&ESP_Receive_Buffer_obj, &c, 1);
		address[i] = c;
	}
	return i;
}

void ESP_ReadDataTask(void * pvParameters){

	char response[100]={'\0'};
	char* response_ptr;
	uint8_t i = 0;
	char length_string[20]={'\0'};
	int32_t length = 0;
	char c;
	for(;;){

		if (ESP_connection_status == ESP_WIFI_CONNECTED && ESP_tcp_status == TCP_CONNECTED && resource_allowed == AVAILABLE){

			if (!USART_BufferEmpty(ESP_USART)){
				vTaskDelay(pdMS_TO_TICKS(100));
				resource_allowed = RECEIVING;
				while(!USART_BufferEmpty(ESP_USART)){
					if ((c = USART_ReceiveData(ESP_USART))!=0){
						response[i] = c;
						i++;



					}
				}


				i = 0;
				if(strstr(response, "+IPD") != 0){
					response_ptr = strstr(response, "+IPD,") +5;


					while (response_ptr[i] != ':'){
							length_string[i] = response_ptr[i];
							i++;
						}
					i++;
					length = atol(length_string);


					while (i < 100){
							TM_BUFFER_Write(&ESP_Receive_Buffer_obj, &response_ptr[i], 1);
							i++;
							length--;
							if (length == 0 ){
								resource_allowed = AVAILABLE;
								break;
							}
						}
					memset(response, 0, 100*sizeof(char));

				}
			}

		}



		vTaskDelay(pdMS_TO_TICKS(2));
	}
}

uint8_t ESP_SendData(uint32_t length, char* data){

	char command[20]={'\0'};
	sprintf(command, "AT+CIPSEND=%lu%s", length,"\r\n");
	ESP_sendBlindCommand(command);
	if (ESP_sendRequest("AT+CIPSTATUS", "STATUS:3") == 1){
		ESP_sendBlindCommand(data);
	}

	return 1;
}

/*sends string to the sending buffer
 * The whole sending process is handled then by RTOS
 * if you are using RTOS, start the sending task and use
 * this API to send data.
 * @Parameters:address: the reference to the string,
 * 			   bytes: refers to the length.
 *
 * @returns: the true length have been sent.
 * 			   */
int ESP_SendDataRT(unsigned char *address, unsigned int bytes){
	unsigned int i = 0;
//TODO use semaphores to handle the shared buffer
	for(i=0 ; i<bytes ; i++){
		if (TM_BUFFER_GetFull(&ESP_Send_Buffer_obj) == (ESP_SEND_BUFFER_SIZE - 1)){
			return i;
		}
		TM_BUFFER_Write(&ESP_Send_Buffer_obj, &address[i], 1);
	}
	return i;
}

void ESP_SendDataTask(void * pvParameters){

	char response[100]={'\0'};
	char command[20]={'\0'};
	uint16_t i = 0;
	int length ;
	char length_string[20];
	char c;


	for(;;){
		if (ESP_connection_status == ESP_WIFI_CONNECTED && ESP_tcp_status == TCP_CONNECTED && (resource_allowed== AVAILABLE||SENDING)){

			if(TM_BUFFER_GetFull(&ESP_Send_Buffer_obj) != 0){

				resource_allowed = SENDING;
				i = 0;
				length = TM_BUFFER_GetFull(&ESP_Send_Buffer_obj);
				itoa(length, length_string, 20);
				memset(command, 0, 20*sizeof(char));
				sprintf(command, "AT+CIPSEND=%d%s", length,"\r\n");
				ESP_sendBlindCommand(command);
				vTaskDelay(pdMS_TO_TICKS(50));

				for (i=0 ; i<length ; i++){
					resource_allowed = SENDING;
					TM_BUFFER_Read(&ESP_Send_Buffer_obj, &c, 1);
					USART_SendData(ESP_USART, c);
				}
				TM_BUFFER_Reset(&ESP_Send_Buffer_obj);
				//USART_SendData(ESP_USART, '\r');
				//USART_SendData(ESP_USART, '\n');

				vTaskDelay(pdMS_TO_TICKS(1000));

				//if (!USART_BufferEmpty(ESP_USART)){

		}

			else if (resource_allowed == SENDING){
				while (!USART_BufferEmpty(ESP_USART)){
					if ((c = USART_ReceiveData(ESP_USART))!=0){
						response[i] = c;
						i++;
					}
				}

				if(strstr(&response[40], "SEND OK\r\n") != 0)
									resource_allowed = AVAILABLE;
				}
			}


	}
	vTaskDelay(pdMS_TO_TICKS(100));

}


int ESP_getSendBufferFree(){
	return TM_BUFFER_GetFree(&ESP_Send_Buffer_obj);
}
