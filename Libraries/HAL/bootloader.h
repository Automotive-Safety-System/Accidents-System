/*
 * flash_driver.h
 *
 *  Created on: Mar 21, 2020
 *      Author: A
 */

#ifndef HAL_FLASH_DRIVER_H_
#define HAL_FLASH_DRIVER_H_

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_crc.h"
#include "string.h"



typedef enum
{
	DATA_TYPE_8=0,
	DATA_TYPE_16,
	DATA_TYPE_32,
}DataTypeDef;


typedef enum{

	sector_0 = 0,
	sector_1,
	sector_2,
	sector_3,
	sector_4,
	sector_5,
	sector_6,
	sector_7,
	sector_8,
	sector_9,
	sector_10,
	sector_11,


}SECTOR_NUM;

typedef enum{

	 FLASH_IDLE,
	 FLASH_WAIT_FOR_CMD,
	 flash_CMD_RECEIVED,

}state;

/*
 *  this enum is used to handle
 *  flag in idle interrupt
 *  in usart
 */

typedef enum{

	flag_idle = 0,
	flag_set,

}handle_flag;


void MY_FLASH_EraseSector(uint32_t sector_num);
void flash_init();
void boot_process();
void flash_write();
void flash_main();
void check_errors();
void cmd_jumb();

#define CMD_WRITE  0x01
#define CMD_ERASE   43
#define CMD_Jump   0x03
#define CMD_wr_p   33
#define RE_wr_pr   55
#define up_date     2
#define CMD_READ    77


//#define  APP_START_ADDRESS  *((volatile uint32_t*) (0x080E0000 )) //0x08008000 is used to hold image start address to be checked before jump to app




#endif /* HAL_FLASH_DRIVER_H_ */
