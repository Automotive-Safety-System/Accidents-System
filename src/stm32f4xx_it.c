/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Template/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-January-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "stm32f4_discovery.h"
#include "MPU_0.h"


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/


/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}


/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/
void USART2_IRQHandler(void){

	if (USART_GetFlagStatus(USART2, USART_FLAG_TC) != RESET){
		USART_ClearITPendingBit(USART2, USART_IT_TC);
	}

	if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET){
		USART_ReceiverHandler(USART2);
	}

}
void USART3_IRQHandler(void){


	if (USART_GetFlagStatus(USART3, USART_FLAG_TC) != RESET){
		USART_ClearITPendingBit(USART3, USART_IT_TC);
	}

	if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET){
		USART_ReceiverHandler(USART3);
	}

}
void USART4_IRQHandler(void){

	if (USART_GetFlagStatus(UART4, USART_FLAG_TC) != RESET){
		USART_ClearITPendingBit(UART4, USART_IT_TC);
	}

	if (USART_GetFlagStatus(UART4, USART_FLAG_RXNE) != RESET){
		USART_ReceiverHandler(UART4);
	}

}
void USART5_IRQHandler(void){

	if (USART_GetFlagStatus(UART5, USART_FLAG_TC) != RESET){
		USART_ClearITPendingBit(UART5, USART_IT_TC);
	}

	if (USART_GetFlagStatus(UART5, USART_FLAG_RXNE) != RESET){
		USART_ReceiverHandler(UART5);
	}

}

void I2C1_EV_IRQHandler(void)
{

	//HAL_I2C_EV_IRQHandler(&i2c_handle);
}
void I2C1_ER_IRQHandler(void)
{
	//HAL_I2C_ER_IRQHandler(&i2c_handle );
}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
