/*
 * MPU6050.c
 *
 *  Created on: Mar 9, 2020
 *      Author: mahmo
 */

#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include "MPU6050.h"
#include "MPU6050_Config.h"





/* Default I2C clock */
#define MPU6050_I2C_CLOCK			400000

// Setup MPU6050
#define MPU6050_ADDR 0xD0
#define MPU6050_I_AM				0x68




static	I2C_HandleTypeDef I2C_Handle;

static MPU6050_t* MY_MPU;

TaskHandle_t xTask_MPU6050_Read_RawData=NULL;

static float accelScalingFactor, gyroScalingFactor;


static void MPU6050_StructCpy(MPU6050_t *Data_Struct);
static void MPU6050_I2C_Config();
static void MPU6050_Config_Init(MPU6050_t *Data_Struct);
static void MPU6050_Init(MPU6050_t *Data_Struct);



static void MPU6050_StructCpy(MPU6050_t *Data_Struct)
{
	//Copy I2C CubeMX handle to local library
	memcpy(MY_MPU,Data_Struct, sizeof(*Data_Struct));
}



static void MPU6050_I2C_Config()
{

	/*
	 * GPIO Configration
	 * */
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	//SDA GPIO clock enable
	RCC_AHB1PeriphClockCmd(I2Cx_SDA_GPIO_CLK, ENABLE);

	  //GPIO Configuration
	// Configure I2C SCL pin
	RCC_AHB1PeriphClockCmd(I2Cx_SCL_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = I2Cx_SCL_PIN ;
	// Select Alternate function mode
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	 //Select output speed very high
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	 //Select output type open drain
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	 //Select pull-up
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	//now all the values are passed to the GPIO_Init()
	GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);

	//Configure I2C SDA pin
	GPIO_InitStruct.GPIO_Pin = I2Cx_SDA_PIN;
	GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);
	 //The I2C1_SCL and I2C1_SDA pins are now connected to their AF
	 /* so that the USART1 can take over control of the
	 * pins
	*/
	GPIO_PinAFConfig(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_SOURCE, I2Cx_SCL_AF);
	GPIO_PinAFConfig(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_SOURCE, I2Cx_SDA_AF);

	/*
	 * I2C Configration
	 * */

	 //RCC Configuration
	 /* I2C Peripheral clock enable*/
	RCC_APB1PeriphClockCmd(I2Cx_CLK, ENABLE);

	// Set the I2C structure parameters
	I2C_Handle.Instance=MPU6050_I2C;

	I2C_Handle.Init.I2C_ClockSpeed = MPU6050_I2C_CLOCK;
	I2C_Handle.Init.I2C_Ack = I2C_Ack_Enable;
	I2C_Handle.Init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Handle.Init.I2C_Mode = I2C_Mode_I2C;
	I2C_Handle.Init.I2C_OwnAddress1 = 0x00;
	I2C_Handle.Init.I2C_DutyCycle = I2C_DutyCycle_2;

	// Initialize I2C
	I2C_Init(I2C_Handle.Instance, &I2C_Handle.Init);

	/*
	 * NVIC Configration
	 * */
		// Configure the I2C event priority to be edited
	NVIC_InitStructure.NVIC_IRQChannel =I2Cx_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// Configure the I2C event priority
	NVIC_InitStructure.NVIC_IRQChannel                   = I2Cx_ER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	I2C_Cmd(I2C_Handle.Instance,ENABLE);

}

static void MPU6050_Config_Init(MPU6050_t *Data_Struct)
{
	Data_Struct->Accel_Full_Scale = AFS_SEL_2g;
	Data_Struct->ClockSource = Internal_8MHz;
	Data_Struct->CONFIG_DLPF = DLPF_184A_188G_Hz;
	Data_Struct->Gyro_Full_Scale = FS_SEL_250;
	Data_Struct->Sleep_Mode_Bit = 0;  //1: sleep mode, 0: normal mode
}

static void MPU6050_Init(MPU6050_t *Data_Struct)
{
	uint8_t check;
	uint8_t Buffer = 0;;

	// check device ID WHO_AM_I
	I2C_Handle.State=HAL_I2C_STATE_READY;

	HAL_I2C_Mem_Read(&I2C_Handle, MPU6050_ADDR,WHO_AM_I_REG,&check, 1);

	if (check == MPU6050_I_AM)  // 0x68 will be returned by the sensor if everything goes well
	{

		// power management register 0X6B we should write all 0's to wake the sensor up
		Buffer = 0x08;
		HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, PWR_MAGT_1_REG,&Buffer, 1);
		//10 ms delay
	//	for(check=0;check<1000;check++);


		Buffer = Data_Struct ->ClockSource & 0x07; //change the 7th bits of register
		Buffer |= (Data_Struct ->Sleep_Mode_Bit << 6) &0x40;
		HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, PWR_MAGT_1_REG,&Buffer, 1);
		//10 ms delay
		
	//	for(check=0;check<1000;check++);


		//Set the Digital Low Pass Filter
		Buffer = 0;
		Buffer = Data_Struct->CONFIG_DLPF & 0x07;
		HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, CONFIG_REG, &Buffer, 1);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g
		Buffer = 0;
		Buffer = (Data_Struct->Gyro_Full_Scale << 3) & 0x18;
		HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, GYRO_CONFIG_REG, &Buffer, 1);

		// Set Gyroscopic configuration in GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> � 250 �/s
		Buffer = 0;
		Buffer = (Data_Struct->Accel_Full_Scale << 3) & 0x18;
		HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, ACCEL_CONFIG_REG, &Buffer, 1);
		//Set SRD To  1Khz
		Buffer = 0x07;
		HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, SMPLRT_DIV_REG, &Buffer, 1);
		
		//STM_EVAL_LEDOn(2);
	}

	//Accelerometer Scaling Factor, Set the Accelerometer and Gyroscope Scaling Factor
	switch (Data_Struct->Accel_Full_Scale)
	{
		case AFS_SEL_2g:
			accelScalingFactor = (2000.0f/32768.0f);
			break;

		case AFS_SEL_4g:
			accelScalingFactor = (4000.0f/32768.0f);
				break;

		case AFS_SEL_8g:
			accelScalingFactor = (8000.0f/32768.0f);
			break;

		case AFS_SEL_16g:
			accelScalingFactor = (16000.0f/32768.0f);
			break;

		default:
			break;
	}
	//Gyroscope Scaling Factor
	switch (Data_Struct->Gyro_Full_Scale)
	{
		case FS_SEL_250:
			gyroScalingFactor = 250.0f/32768.0f;
			break;

		case FS_SEL_500:
				gyroScalingFactor = 500.0f/32768.0f;
				break;

		case FS_SEL_1000:
			gyroScalingFactor = 1000.0f/32768.0f;
			break;

		case FS_SEL_2000:
			gyroScalingFactor = 2000.0f/32768.0f;
			break;

		default:
			break;
	}

}

extern void MPU6050_Module_INIT(MPU6050_t *Data_Struct)
{
	 MPU6050_StructCpy(Data_Struct);
	 MPU6050_I2C_Config();
	 MPU6050_Config_Init(MY_MPU);
	 MPU6050_Init(MY_MPU);
}

extern void MPU6050_Read_All()
{

	uint8_t Rec_Data[14];
	int16_t temp;

	while(1){
	// Read 14 BYTES of data starting from ACCEL_XOUT_H register

	HAL_I2C_Mem_Read_IT(&I2C_Handle, MPU6050_ADDR, ACCEL_XOUT_H_REG,Rec_Data, 14);
	MY_MPU->Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);

	MY_MPU->Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	MY_MPU->Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
	temp = (int16_t)(Rec_Data[6] << 8 | Rec_Data [7]);
	MY_MPU->Temperature=temp;
	MY_MPU->Gyro_X_RAW = (int16_t)(Rec_Data[8] << 8 | Rec_Data [9]);
	MY_MPU->Gyro_Y_RAW = (int16_t)(Rec_Data[10] << 8 | Rec_Data [11]);
	MY_MPU->Gyro_Z_RAW = (int16_t)(Rec_Data[12] << 8 | Rec_Data [13]);
//	printf("Accel_X: %d\n",MY_MPU->Accel_X_RAW);
//	printf("Accel_Y: %d\n",MY_MPU->Accel_Y_RAW);
//	printf("Accel_Z :%d\n",MY_MPU->Accel_Z_RAW );
//	printf("GYR_X: %d\n",MY_MPU->Gyro_X_RAW);
//	printf("GYR_Y: %d\n",MY_MPU->Gyro_Y_RAW);
//	printf("GYR_Z: %d\n",MY_MPU->Gyro_Z_RAW);
	vTaskDelay(pdMS_TO_TICKS(100));

	}

}
//10- Get Accel scaled data (g unit of gravity, 1g = 9.81m/s2)
void MPU6050_Get_Accel_Scale()
{


}

////11- Get Accel calibrated data
//void MPU6050_Get_Accel_Cali()
//{
//
//}
////13- Get Gyro scaled data
//void MPU6050_Get_Gyro_Scale()
//{
//
//}
////14- Accel Calibration
//void _Accel_Cali()
//{
//
//}

void HAL_I2C_EVENT_CALLBack()
{
	HAL_I2C_EV_IRQHandler(&I2C_Handle);
}
