/*
 * ESP01_config.h
 *
 *  Created on: Feb 1, 2020
 *      Author: Hazem
 */

#ifndef HAL_ESP01_CONFIG_H_
#define HAL_ESP01_CONFIG_H_

#include "stm32f4xx.h"

#define	ESP_USART					USART2
#define ESP_GPIO					GPIOA
#define ESP_TX_PIN					GPIO_Pin_2
#define ESP_RX_PIN					GPIO_Pin_3
#define ESP_USART_RCC				RCC_APB1Periph_USART2
#define ESP_GPIO_RCC				RCC_AHB1Periph_GPIOA
#define ESP_AF_PIN1_SOURCE			GPIO_PinSource2
#define ESP_AF_PIN2_SOURCE			GPIO_PinSource3
#define ESP_AF_USART				GPIO_AF_USART2
#define ESP_USART_IRQ				USART2_IRQn
#define ESP_USART_IRQ_PERIORITY		10
#define ESP_BAUD_RATE				115200

#endif /* HAL_ESP01_CONFIG_H_ */
