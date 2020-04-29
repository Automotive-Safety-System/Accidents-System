/*
 * flash_driver.c
 *
 *  Created on: Mar 21, 2020
 *      Author: A
 */

#include "bootloader.h"

uint8_t flag_received=0;

state current_state = FLASH_IDLE;

uint32_t MAX_BUFFER_LENGTH  = 128;


char RxBuffer[128];


 uint32_t len_message = 0 ;

handle_flag current_flag = flag_idle;





/*
 *  to erase any sector you want
 */



void MY_FLASH_EraseSector(uint32_t sector_num)

{
	                          /* unlock flash first */
	                             FLASH_Unlock();


	                          /* Erase the required Flash sector */
	                             FLASH_EraseSector(sector_num, VoltageRange_3);


	                          /* flash lock */
	                             FLASH_Lock();

}



/* function to init flash
 *
 */

void flash_init(){

	 //lock_flash
	  FLASH_Unlock();

	  /* Select flash parallelism x32 */
	  FLASH->CR &= ~FLASH_CR_PSIZE;
	  FLASH->CR |= FLASH_CR_PSIZE_1;

	  /* Write lock bit */
	  FLASH_Lock();

	 // FLASH_Latency_5: FLASH Five Latency cycles

	  FLASH_SetLatency(FLASH_Latency_5);

	    /**
	    * @brief  Enables  the Prefetch Buffer.
	    */
	   FLASH_PrefetchBufferCmd(ENABLE);

	   /**
	     * @brief  Enables the Instruction Cache feature.
	   */
	   FLASH_InstructionCacheCmd(ENABLE);

	    /**
	    * @brief  Enables  the Data Cache feature.
	    */
	   FLASH_DataCacheCmd(ENABLE);

	   /**
	    * @brief  Enables  the specified FLASH interrupts.
	    * @arg FLASH_IT_ERR: FLASH Error Interrupt
	    */

	   FLASH_ITConfig(FLASH_IT_ERR,ENABLE);
}


/*
 *
 *  boot process to didicate
 *  which you want to enter bootloader
 *  or application
 *  this function to
 */


void boot_process(){


				        	switch(RxBuffer[0]){

					       /* WRITE DATA IN FLASH */
					        case '1':

					    	  flash_write();

						      break;



						    /* JUMP TO BOOTLOADER */
					         case '2':

					    	    cmd_jumb();


						  break;



					}

		}


/*
 * function to write data in flash
 */



void flash_write(){

					 volatile uint32_t data = 0;

					  MY_FLASH_EraseSector(sector_11);

					 /* Write data into flash */

					  volatile uint32_t   address = * (uint32_t *) &RxBuffer[1];

	                                   FLASH_Unlock();

	  				for(uint32_t idx=0; idx < 300; idx++)

	  				{
	  				    data = * (uint32_t *) &RxBuffer[5 + (idx * 4)];

	  				    FLASH_ProgramWord(address,data);

	  					address += 4;
	  				}

	                        FLASH_Lock();
	       
}



/*
 *  function to deal with command
 *  if state is ideal
 *  or not
 *
 */


void flash_main()

  {

    /* init some function */
	
	flash_init();

	while(1){


			                 switch (current_state){

                                                         /* check if flash idle */
							  case FLASH_IDLE:

							      /* Go to next state */
							     current_state = FLASH_WAIT_FOR_CMD;

								        break;


							   case FLASH_WAIT_FOR_CMD:


							        /* Check if new message received */
							        if(len_message != 0)

							        {

							          /* Process command */
							          boot_process();

							          /* Check errors */
							          check_errors();

							          /* reset pointer */
							          len_message = 0;

							        }

							        else

								        {
							              /* Nothing received yet */
							            }

							        /* Go to next state */
							        current_state = FLASH_WAIT_FOR_CMD;
							        break;

							   default:
							    break;


			  }


  }
}



/**
 * @brief   Check flash errors
 * @param   none
 * @retval  none
 */

 void check_errors()

{
  if(FLASH_SR_PGSERR == (FLASH->SR & FLASH_SR_PGSERR))
  {
    /* Programming sequence error */

    /* Clear */
    FLASH->SR |= FLASH_SR_PGSERR;
  }
  else if(FLASH_SR_PGPERR == (FLASH->SR & FLASH_SR_PGPERR))
  {
    /* Programming parallelism error */

    /* Clear */
    FLASH->SR |= FLASH_SR_PGPERR;
  }
  else if(FLASH_SR_PGAERR == (FLASH->SR & FLASH_SR_PGAERR))
  {
    /* Programming alignment error */

    /* Clear */
    FLASH->SR |= FLASH_SR_PGAERR;
  }
  else if(FLASH_SR_WRPERR == (FLASH->SR & FLASH_SR_WRPERR))
  {
    /* Write protection error */

    /* Clear */
    FLASH->SR |= FLASH_SR_WRPERR;
  }
  else
  {
      /* No Errors */


  }
}

 /*  enter bootloader mode
  *
  *  or user mode
  */


 void cmd_jumb(){


	       /* Get jump address */

	      volatile uint32_t address = * (uint32_t *) &RxBuffer[1];

	      volatile uint32_t val = * (uint32_t *) address;

	      /* Check if it has valid stack pointer in the RAM */
	      if(0x20000000 == (val & 0x20000000))
	      {

	    	  __disable_irq();

	          /* Reset GPIOB */

	    	  RCC->AHB1RSTR = (RCC_AHB1RSTR_GPIOBRST );

	    	  /* Reset GPIOD */
	    	  RCC->AHB1RSTR = (RCC_AHB1RSTR_GPIODRST );

	    	  /* Release reset */
	    	  RCC->AHB1RSTR = 0;

	    	  /* Reset USART3 */
	    	  RCC->APB1RSTR = RCC_APB1RSTR_USART3RST;

	    	  /* Release reset */
	    	  RCC->APB1RSTR = 0;


	    	 /* Reset RCC */

	    	  RCC_DeInit();

	    	       /* Reset SysTick */
	    	       SysTick->CTRL = 0;
	    	       SysTick->LOAD = 0;
	    	       SysTick->VAL = 0;


	    	       /* Check jump address */
	    	      if(0x1FFF0000 == address)

	    	        {
	    	                /* Enable SYSCFG clock */
	    	                RCC->APB2ENR |= RCC_APB2LPENR_SYSCFGLPEN;

	    	                /* Map address 0x0 to system memory */
	    	                SYSCFG->MEMRMP = SYSCFG_MEMRMP_MEM_MODE_0;
	    	       }
	    	         else
	    	           {
	    	                /* Vector Table Relocation in Internal FLASH */

	    	                __DMB();
	    	                SCB->VTOR = address;
	    	                __DSB();

	    	          }
	    	                 void (*jump_address)(void) = (void *)(*((uint32_t *)(address + 4)));

	    	                         /* Set stack pointer */
	    	                         __set_MSP(val);

	    	                         /* Jump */
	    	                         jump_address();


	    	        }

	               else
	               {


	    	       }

	  }




