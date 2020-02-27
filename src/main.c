/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "OS.h"
/* Private macro */
/* Private variables */
/* Private function prototypes */
void uart3_init();
void uart2_init();
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
void  vRedLedControllerTask(void *pvParameters)
{

	while(1)
	{
		STM_EVAL_LEDToggle(LED3);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

int main(void){
//	STM_EVAL_LEDInit(LED3);
//
//int i;
//	char ssid[] = "Hazem";
//	char pass[] ="h2326574";
//
//
//	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//
//	  GPIO_InitTypeDef 	gpio_init_struct;
//	  /*configure usart2 rx"PA3" as a floating input*/
//	  gpio_init_struct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
//	  gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
//	  gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
//	  gpio_init_struct.GPIO_OType = GPIO_OType_PP;
//	  gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP ;
//	  GPIO_Init(GPIOA, &gpio_init_struct);
//	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
//	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
//
//	  /*configure NVIC*/
//	    NVIC_InitTypeDef usart2_irq_struct;
//	    usart2_irq_struct.NVIC_IRQChannel = USART2_IRQn;
//	    usart2_irq_struct.NVIC_IRQChannelCmd = ENABLE;
//	    usart2_irq_struct.NVIC_IRQChannelPreemptionPriority = 10;
//	    usart2_irq_struct.NVIC_IRQChannelSubPriority = 10;
//	    NVIC_Init(&usart2_irq_struct);
//
//	  /*configure usart2*/
//	    USART_InitTypeDef 	usart2_init_struct;
//	    usart2_init_struct.USART_BaudRate = 9600;
//	    usart2_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	    usart2_init_struct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
//	    usart2_init_struct.USART_Parity = USART_Parity_No;
//	    usart2_init_struct.USART_StopBits = USART_StopBits_1;
//	    usart2_init_struct.USART_WordLength = USART_WordLength_8b;
//	    USART_Init(USART2, &usart2_init_struct);
//	    USART2->CR1 |=(1<<6);
//
//	  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//	  USART_Cmd(USART2, ENABLE);
//	/* if event has occur */
//
//	// telephone number
//
//
////	send_command_to_init_gsm();
//
//	//send telephone number
//
//	//send_sms(x);
//
//	// send telephone number
//
//	//gsm_call(x);
//	  extern TM_GPS_t GPS_Data;
//	  TM_GPS_Init(&GPS_Data, 9600);
//	  OS_INIT();
//	  vTaskStartScheduler();
gsm_os_init();
/* Infinite loop */
  while (1){
	 // USART_SendString(USART2, ssid,6);
	  //for(i=0 ; i<700000 ; i++);


  }
}



void vApplicationMallocFailedHook( void ){
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName ){
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}

void vApplicationIdleHook( void ){
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 ){
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}



/*setup goes here*/
static void prvSetupHardware( void ){
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

/*dummy refrences for compilation conflicts
 * remove these only when if you need to implement them in main*/
void vApplicationTickHook( void ){}

