/*
 * gsm_init.h
 *
 *  Created on: Jan 30, 2020
 *      Author: A
 */

#ifndef HAL_GSM_H_
#define HAL_GSM_H_

/* include libraries */
#include "stm32f4xx.h"
#include "string.h"
#include "stm32f4_discovery.h"

/* include all libraries used in freeRtos */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* declare function in this file */

void usart2_init();

void send_command_to_init_gsm(void *parameter);

void SIM900_PutFrame(char *buf);

uint8_t SIM900_GetFrame(void* buf, void *len);

void gsm_call(void *num);

void send_sms(void *num);

void gsm_os_init(void *parameter);


#endif /* HAL_GSM_INIT_H_ */
