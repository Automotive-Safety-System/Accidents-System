/*
 * MPU_0_Config.h
 *
 *  Created on: Jan 28, 2020
 *      Author: mahmo
 */

#ifndef MPU_0_CONFIG_H_
#define MPU_0_CONFIG_H_

#define I2Cx                          I2C1
#define I2Cx_CLK                      RCC_APB1Periph_I2C1
#define I2Cx_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define I2Cx_SDA_PIN                  GPIO_Pin_9
#define I2Cx_SDA_GPIO_PORT            GPIOB
#define I2Cx_SDA_SOURCE               GPIO_PinSource9
#define I2Cx_SDA_AF                   GPIO_AF_I2C1

#define I2Cx_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define I2Cx_SCL_PIN                  GPIO_Pin_6
#define I2Cx_SCL_GPIO_PORT            GPIOB
#define I2Cx_SCL_SOURCE               GPIO_PinSource6
#define I2Cx_SCL_AF                   GPIO_AF_I2C1


#define I2Cx_EV_IRQn				  I2C1_EV_IRQn
#define I2Cx_ER_IRQn				  I2C1_ER_IRQn

#endif /* MPU_0_CONFIG_H_ */
