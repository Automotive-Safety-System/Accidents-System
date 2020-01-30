/*
 * gsm_init.c
 *
 *  Created on: Jan 30, 2020
 *      Author: A
 */

#include "gsm_module.h"


void usart2_init(){


	      /*enable clock for gpio port A and usart2 */

		  /* insert the following code inside main(), before the infinite loop*/

		  /*enable clock for gpio port A and usart2 */

		  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		  GPIO_InitTypeDef 	gpio_init_struct;

		  /* set all gpio as 0 */

		  memset(&gpio_init_struct,0,sizeof(gpio_init_struct));

		  /*configure usart2 rx"PA3" as a floating input*/
		  gpio_init_struct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
		  gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
		  gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
		  gpio_init_struct.GPIO_OType = GPIO_OType_PP;
		  gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP ;

		  GPIO_Init(GPIOA, &gpio_init_struct);

		  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
		  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	      /*configure NVIC*/
	      NVIC_InitTypeDef usart2_irq_struct;

		  /* set all gpio as 0 */

		  memset(&usart2_irq_struct,0,sizeof(usart2_irq_struct));

	      usart2_irq_struct.NVIC_IRQChannel = USART2_IRQn;
		  usart2_irq_struct.NVIC_IRQChannelCmd = ENABLE;
		  usart2_irq_struct.NVIC_IRQChannelPreemptionPriority = 0;
		  usart2_irq_struct.NVIC_IRQChannelSubPriority = 10;

	      NVIC_Init(&usart2_irq_struct);

	     /*configure usart2*/

	    USART_InitTypeDef 	usart2_init_struct;

	    /* set all gpio as 0 */

	    memset(&usart2_init_struct,0,sizeof(usart2_init_struct));

	    usart2_init_struct.USART_BaudRate = 9600;
	    usart2_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	    usart2_init_struct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	    usart2_init_struct.USART_Parity = USART_Parity_No;
	    usart2_init_struct.USART_StopBits = USART_StopBits_1;
	    usart2_init_struct.USART_WordLength = USART_WordLength_8b;

	    USART_Init(USART2, &usart2_init_struct);

	    USART2->CR1 |=(1<<6);

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	    USART_Cmd(USART2, ENABLE);


}



void send_command_to_init_gsm(){

	  char data_receive[100];

      uint8_t x;

      uint32_t i=0;

	  //Once the handshake test is successful, it will back to OK

	 SIM900_PutFrame(test_connect);

	 for(i=0; i<1000000;i++);

	  x = SIM900_GetFrame(data_receive,sizeof(data_receive));

	  if(x == 0){

		  printf("error there is no data in buffer");
	  }

	  else{

		  if(strcmp(data_receive, "ok") == 0){

			 printf("connect to network\n");
		  }

		  else{

			  printf("error in connect on network");
		  }
	  }


	    //Signal quality test, value range is 0-31 , 31 is the best

	     SIM900_PutFrame(quality_test);

	  	 for(i=0; i<1000000;i++);

	  	 //Check whether it has registered in the network

	     SIM900_PutFrame(net_reg);

	    for(i=0; i<1000000;i++);



	 }




/**
 * @brief Send a zero terminated string to SIm900.
 *
 * @param buf String to send.
 */
void SIM900_PutFrame(char* buf) {

           uint16_t len = strlen(buf);

          for(int i = 0; i < len; i++) {

	         USART_SendData(USART2,buf[i]);
          }

}



uint8_t SIM900_GetFrame(uint8_t* buf, uint8_t len) {



	   uint8_t size = USART_ReceiveString(USART2,buf,len);

	   if(size <= 0){

		   printf("error no data in buffer\n");
		   return 0;
	   }

	   else{


		   return size ;
		  // printf("length is = %d\n",size)
	   }

	return size;
}

void gsm_call(char x[]){


	/* call gsm module */


	    char y[]="ATD+20";



		uint32_t i=0;

		/* select callmode */

		strcat(y,x);


		SIM900_PutFrame(y);


	 	for(i=0; i<1000000;i++);//delay 20 seconds





}

void send_sms(char x[]){

	char y[]="AT+CMGS=\"+20";

	char m[] = "\"\r\n";

	uint32_t i=0;

	/* select textmode */

	SIM900_PutFrame(config_mod);

 	for(i=0; i<1000000;i++);

	strcat(y,x);


	strcat(y,m);


	SIM900_PutFrame(y);



}
