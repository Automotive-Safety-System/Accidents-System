/*
 * gsm_init.h
 *
 *  Created on: Jan 30, 2020
 *      Author: A
 */

#ifndef HAL_GSM_MODULE_H_
#define HAL_GSM_MODULE_H_

/* macro definition
 *
 * AT commands
 */

#define SIM900_HAL_Enable   usart2_init

#define  test_connect     "AT"                            //Once the handshake test is successful, it will back to OK

#define  quality_test     "AT+CSQ"                        //Signal quality test, value range is 0-31 , 31 is the best

#define  sim_info         "AT+CCID"                      // get the SIM card number  this tests that the SIM card is found OK

#define  net_reg          "AT+CREG?"                    //Check whether it has registered in the network

#define config_mod        "AT+CMGF=1"                    // Selects SMS message format as text.

#define call              "ATD+20xxxxxxxxxx"             // call a specified number.

/* include libraries */

#include "stm32f4xx.h"
#include "string.h"



/* declaration function */

void usart2_init();

void send_command_to_init_gsm();

void gsm_call(char x[]);

void SIM900_PutFrame(char* buf) ;

uint8_t SIM900_GetFrame(uint8_t* buf, uint8_t len) ;

void send_sms(char x[]);


#endif /* HAL_GSM_INIT_H_ */
