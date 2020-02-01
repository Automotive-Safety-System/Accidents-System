/*
 * gsm_module_config.h
 *
 *  Created on: Feb 1, 2020
 *      Author: Hazem
 */

#ifndef HAL_GSM_CONFIG_H_
#define HAL_GSM_CONFIG_H_

#define	GSM_USART					USART2
#define GSM_GPIO					GPIOA
#define GSM_TX_PIN					GPIO_Pin_2
#define GSM_RX_PIN					GPIO_Pin_3
#define GSM_USART_RCC				RCC_APB1Periph_USART2
#define GSM_GPIO_RCC				RCC_AHB1Periph_GPIOA
#define GSM_AF_PIN1_SOURCE			GPIO_PinSource2
#define GSM_AF_PIN2_SOURCE			GPIO_PinSource3
#define GSM_AF_USART				GPIO_AF_USART2
#define GSM_USART_IRQ				USART2_IRQn
#define GSM_USART_IRQ_PERIORITY		10
#define GSM_BAUD_RATE				9600



#endif /* HAL_GSM_CONFIG_H_ */
