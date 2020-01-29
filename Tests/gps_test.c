// inside main insert the following


uart3_init();
   //uart2_init();
  char buffer[40];
  float temp;
  TM_GPS_t GPS_Data;
  TM_GPS_Result_t result;
  TM_GPS_Result_t current;
  TM_GPS_Float_t GPS_Float;
  TM_GPS_Distance_t GPS_Distance;
  TM_GPS_Init(&GPS_Data, 9600);
  //USART_SendString(USART3,"not connected", -1);





  /**************************************************/
  //inside the loop insert the following

  result = TM_GPS_Update(&GPS_Data);

  if ( result == TM_GPS_Result_FirstDataWaiting){
	  USART_SendString(USART3,"not connected", -1);
  }
  else if (result == TM_GPS_Result_NewData) {
  			/* We received new packet of useful data from GPS */
	  	  current = TM_GPS_Result_NewData;
  			if (GPS_Data.Validity){
  				 STM_EVAL_LEDOn(LED5);
  				USART_SendString(USART3,"GPGGA: \n", -1);
  				TM_GPS_ConvertFloat(GPS_Data.Latitude, &GPS_Float, 6);
  				sprintf(buffer, " - Latitude: %d.%d\n", GPS_Float.Integer, GPS_Float.Decimal);
  				USART_SendString(USART3, buffer,-1);

  				/* Longitude */
  				/* Convert float to integer and decimal part, with 6 decimal places */
  				TM_GPS_ConvertFloat(GPS_Data.Longitude, &GPS_Float, 6);
  				sprintf(buffer, " - Longitude: %d.%d\n", GPS_Float.Integer, GPS_Float.Decimal);
  				USART_SendString(USART3, buffer, -1);

  				/* Satellites in use */
  				sprintf(buffer, " - Sats in use: %02d\n", GPS_Data.Satellites);
  				USART_SendString(USART3, buffer, -1);

  				/* Current time */
  	    		sprintf(buffer, " - UTC Time: %02d.%02d.%02d:%02d\n", GPS_Data.Time.Hours, GPS_Data.Time.Minutes, GPS_Data.Time.Seconds, GPS_Data.Time.Hundredths);
  				USART_SendString(USART3, buffer, -1);

  				/* Fix: 0 = invalid, 1 = GPS, 2 = DGPS */
  				sprintf(buffer, " - Fix: %d\n", GPS_Data.Fix);
  				USART_SendString(USART3, buffer, -1);

  				/* Altitude */
  				/* Convert float to integer and decimal part, with 6 decimal places */
  				TM_GPS_ConvertFloat(GPS_Data.Altitude, &GPS_Float, 6);
  				sprintf(buffer, " - Altitude: %3d.%06d\n", GPS_Float.Integer, GPS_Float.Decimal);
  				USART_SendString(USART3, buffer, -1);

  				/**************************************************/
  				USART_SendString(USART3, "GPRMC statement:\n",-1);

  								/* Current date */
  								sprintf(buffer, " - Date: %02d.%02d.%04d\n", GPS_Data.Date.Date, GPS_Data.Date.Month, GPS_Data.Date.Year + 2000);
  								USART_SendString(USART3, buffer,-1);

  								/* Current speed in knots */
  								TM_GPS_ConvertFloat(GPS_Data.Speed, &GPS_Float, 6);
  								sprintf(buffer, " - Speed in knots: %d.%06d\n", GPS_Float.Integer, GPS_Float.Decimal);
  								USART_SendString(USART3, buffer,-1);

  								/* Current speed in km/h */
  								temp = TM_GPS_ConvertSpeed(GPS_Data.Speed, TM_GPS_Speed_KilometerPerHour);
  								TM_GPS_ConvertFloat(temp, &GPS_Float, 6);
  								sprintf(buffer, " - Speed in km/h: %d.%06d\n", GPS_Float.Integer, GPS_Float.Decimal);
  								USART_SendString(USART3, buffer,-1);

  								TM_GPS_ConvertFloat(GPS_Data.Direction, &GPS_Float, 3);
  								sprintf(buffer, " - Direction: %3d.%03d\n", GPS_Float.Integer, GPS_Float.Decimal);
  								USART_SendString(USART3, buffer, -1);
  			}
  			else {
  							/* GPS signal is not valid */
  							USART_SendString(USART3, "New received data haven't valid GPS signal!\n",-1);
  						}
  }

  else if (result == TM_GPS_Result_FirstDataWaiting && current != TM_GPS_Result_FirstDataWaiting) {
  			current = TM_GPS_Result_FirstDataWaiting;
  			USART_SendString(USART3, "Waiting first data from GPS!\n",-1);
  		} else if (result == TM_GPS_Result_OldData && current != TM_GPS_Result_OldData) {
  			current = TM_GPS_Result_OldData;
  			/* We already read data, nothing new was received from GPS */
  			USART_SendString(USART3, "here\n",-1);
  		}

  /***************************************************************/

  //usart 3 funcion

  void uart3_init(){
  	 /*enable clock for gpio port A and usart2 */

  	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  	  GPIO_InitTypeDef 	gpio_init_struct;
  	  /*configure usart2 rx"PA11" as a floating input*/
  	  gpio_init_struct.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
  	  gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
  	  gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  	  gpio_init_struct.GPIO_OType = GPIO_OType_PP;
  	  gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP ;
  	  GPIO_Init(GPIOB, &gpio_init_struct);
  	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
  	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

  	  /*configure NVIC*/
  	    NVIC_InitTypeDef usart3_irq_struct;
  	    usart3_irq_struct.NVIC_IRQChannel = USART3_IRQn;
  	    usart3_irq_struct.NVIC_IRQChannelCmd = ENABLE;
  	    usart3_irq_struct.NVIC_IRQChannelPreemptionPriority = 9;
  	    usart3_irq_struct.NVIC_IRQChannelSubPriority = 9;
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

