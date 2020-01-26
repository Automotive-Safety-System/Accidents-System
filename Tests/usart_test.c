void uart3_init(){
	  /*enable clock for gpio port B and usart2  RX->PB11 , TX->PB10*/

	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	  GPIO_InitTypeDef 	gpio_init_struct;
	  /*configure usart2 rx"PA3" as a floating input*/
	  gpio_init_struct.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	  gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
	  gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	  gpio_init_struct.GPIO_OType = GPIO_OType_PP;
	  gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP ;
	  GPIO_Init(GPIOA, &gpio_init_struct);
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART3);
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_USART3);

	  /*configure NVIC*/
	    NVIC_InitTypeDef usart3_irq_struct;
	    usart2_irq_struct.NVIC_IRQChannel = USART3_IRQn;
	    usart2_irq_struct.NVIC_IRQChannelCmd = ENABLE;
	    usart2_irq_struct.NVIC_IRQChannelPreemptionPriority = 10;
	    usart2_irq_struct.NVIC_IRQChannelSubPriority = 10;
	    NVIC_Init(&usart3_irq_struct);

	  /*configure usart2*/
	    USART_InitTypeDef 	usart3_init_struct;
	    usart3_init_struct.USART_BaudRate = 9600;
	    usart3_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	    usart3_init_struct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	    usart3_init_struct.USART_Parity = USART_Parity_No;
	    usart3_init_struct.USART_StopBits = USART_StopBits_1;
	    usart3_init_struct.USART_WordLength = USART_WordLength_8b;
	    USART_Init(USART3, &usart3_init_struct);
	    USART3->CR1 |=(1<<6);

	  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	  USART_Cmd(USART3, ENABLE);
}


/* insert the following code inside main(), before the infinite loop*/

/*enable clock for gpio port A and usart2  RX->PA3 , TX->PA2*/

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_InitTypeDef 	gpio_init_struct;
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
    usart2_irq_struct.NVIC_IRQChannel = USART2_IRQn;
    usart2_irq_struct.NVIC_IRQChannelCmd = ENABLE;
    usart2_irq_struct.NVIC_IRQChannelPreemptionPriority = 10;
    usart2_irq_struct.NVIC_IRQChannelSubPriority = 10;
    NVIC_Init(&usart2_irq_struct);

  /*configure usart2*/
    USART_InitTypeDef 	usart2_init_struct;
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

  /* write the following code in main to receive data*/

  uint8_t c;
		//USART_SendString(USART2, hi, 14);
//  USART_SendData(USART2, '1');
  while(!USART_BufferEmpty(USART2)){
  c = USART_ReceiveData(USART2);
  if (c!= 0)
	  USART_SendData(USART2, c);
  }
