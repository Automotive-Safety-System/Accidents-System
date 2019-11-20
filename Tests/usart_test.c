/* insert the following code inside main(), before the infinite loop*/

/*enable clock for gpio port A and usart2 */
  RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_AHB1Periph_GPIOA , ENABLE);

  GPIO_InitTypeDef 	gpio_init_struct;
  /*configure usart2 rx"PA3" as a floating input*/
  gpio_init_struct.GPIO_Pin = GPIO_Pin_3 ;
  gpio_init_struct.GPIO_Mode = GPIO_Mode_IN;
  gpio_init_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &gpio_init_struct);

  /*configure usart2 tx"PA2" as alternative function in the pushpull mode*/
  gpio_init_struct.GPIO_Pin = GPIO_Pin_2 ;
  gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
  gpio_init_struct.GPIO_OType = GPIO_OType_PP;
  gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &gpio_init_struct);

  /*configure usart2*/
  USART_InitTypeDef 	usart2_init_struct;
  usart2_init_struct.USART_BaudRate = 9600;
  usart2_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usart2_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  usart2_init_struct.USART_Parity = USART_Parity_No;
  usart2_init_struct.USART_StopBits = USART_StopBits_1;
  usart2_init_struct.USART_WordLength = USART_WordLength_8b;

  USART_Init(USART2, &usart2_init_struct);

  USART_SendData(USART2, 'h');
