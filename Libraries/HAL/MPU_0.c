/*
 * MPU.c
 *
 *  Created on: Dec 6, 2019
 *      Author: mahmo
 */


/**
 * @brief   GPIO Configure, PB6 -> I2C1_SCL, PC7 -> I2C1_SDA
 * @note
 * @param   None
 * @retval  None
 */
/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "MPU_0.h"
#include "MPU_0_Config.h"
#include "stm32f4xx_i2c.h"
/*Private functions */

/*
 * problem of passin the handle to handler of stm32f4xx_it.c
 * */

static uint8_t MPU6050_ByteRead(i2c_handle_t *handle, uint8_t address, uint8_t reg);
static void MPU6050_ByteWrite(i2c_handle_t *handle, uint8_t address, uint8_t reg, uint8_t data);
static void MPU6050_ReadMulti(i2c_handle_t *handle, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count);


#define I2C_TRANSMITTER_MODE   ((uint8_t)0x00)
#define I2C_RECEIVER_MODE      ((uint8_t)0x01)

//static uint32_t I2C_Timeout;
#define I2C_TIMEOUT_MAX		20000
i2c_handle_t i2c_handle;

/**
 * @brief   I2C1  GPIO  Configuration
 * @note
 * @param   None
 * @retval  None
 */
static void MPU6050_I2C_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	// Configure the I2C event priority to be edited
	NVIC_InitStructure.NVIC_IRQChannel =I2Cx_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// Configure the I2C event priority
	NVIC_InitStructure.NVIC_IRQChannel                   = I2Cx_ER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
static void MPU6050_I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
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
}
/*
 * @brief   I2C1  Configuration
 * @note
 * @param   None
 * @retval  None

*/
static void MPU6050_I2C_Config(void)
{

	 //RCC Configuration
	 /* I2C Peripheral clock enable*/
	RCC_APB1PeriphClockCmd(I2Cx_CLK, ENABLE);

	// Set the I2C structure parameters
	i2c_handle.Instance=I2Cx;

	i2c_handle.Init.I2C_ClockSpeed = MPU6050_I2C_CLOCK;
	i2c_handle.Init.I2C_Ack = I2C_Ack_Enable;
	i2c_handle.Init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c_handle.Init.I2C_Mode = I2C_Mode_I2C;
	i2c_handle.Init.I2C_OwnAddress1 = 0x00;
	i2c_handle.Init.I2C_DutyCycle = I2C_DutyCycle_2;

	// Initialize I2C
	I2C_Init(i2c_handle.Instance, &i2c_handle.Init);
}

/**
 * @brief   I2C1  Init
 * @note
 * @param   None
 * @retval  None
 */

extern void I2C1_Init(void){
	MPU6050_I2C_Config();
	MPU6050_I2C_GPIO_Config();
	MPU6050_I2C_NVIC_Config();
}
/**
 * @brief  Initializes MPU6050 and I2C peripheral
 * @param  *DataStruct: Pointer to empty @ref TM_MPU6050_t structure
 * @param   DeviceNumber: MPU6050 has one pin, AD0 which can be used to set address of device.
 *          This feature allows you to use 2 different sensors on the same board with same library.
 *          If you set AD0 pin to low, then this parameter should be TM_MPU6050_Device_0,
 *          but if AD0 pin is high, then you should use TM_MPU6050_Device_1
 *
 *          Parameter can be a value of @ref TM_MPU6050_Device_t enumeration
 * @param  AccelerometerSensitivity: Set accelerometer sensitivity. This parameter can be a value of @ref TM_MPU6050_Accelerometer_t enumeration
 * @param  GyroscopeSensitivity: Set gyroscope sensitivity. This parameter can be a value of @ref TM_MPU6050_Gyroscope_t enumeration
 * @retval Status:
 *            - TM_MPU6050_Result_t: Everything OK
 *            - Other member: in other cases
 */

//
//extern MPU6050_Result_t MPU6050_Init(MPU6050_t* DataStruct, MPU6050_Device_t DeviceNumber, MPU6050_Accelerometer_t AccelerometerSensitivity, MPU6050_Gyroscope_t GyroscopeSensitivity) {
//	uint8_t temp1,temp3;
//	uint8_t	temp2[2];
//	// Format I2C address
//	DataStruct->Address = MPU6050_I2C_ADDR | (uint8_t)DeviceNumber;
//
//
//	// Check if device is connected
//	if (!I2C_IsDeviceConnected(&i2c_handle, DataStruct->Address)) {
//		// Return error
//		return MPU6050_Result_DeviceNotConnected;
//	}
//
//	 //Check who I am
//	/*if (I2C_Read(MPU6050_I2C, DataStruct->Address, MPU6050_WHO_AM_I) != MPU6050_I_AM) {
//		// Return error
//		return MPU6050_Result_DeviceInvalid;
//	}*/
//
//
//	temp1=MPU6050_WHO_AM_I;
//	hal_i2c_master_tx(&i2c_handle,DataStruct->Address,&temp1,1);
//
//	hal_i2c_master_rx(&i2c_handle,DataStruct->Address,&temp3,1);
//
//
//	if(temp3!=MPU6050_I_AM)
//	{

//		return MPU6050_Result_DeviceInvalid;
//	}
//	STM_EVAL_LEDOn(LED6);
////	MPU6050_I2C_ByteWrite(&i2c_handle, DataStruct->Address, 0x00,MPU6050_PWR_MGMT_1);
//
//
//	// Wakeup MPU6050
//	//I2C_Write(MPU6050_I2C, DataStruct->Address, MPU6050_PWR_MGMT_1, 0x00);
//	temp2[0]=MPU6050_PWR_MGMT_1;
//	temp2[1]=0x00;
//	hal_i2c_master_tx(&i2c_handle,DataStruct->Address,temp2,2);
//
//	 //Config accelerometer
//
//	temp1=MPU6050_ACCEL_CONFIG;
//	hal_i2c_master_tx(&i2c_handle,DataStruct->Address,&temp1,1);
//	hal_i2c_master_rx(&i2c_handle,DataStruct->Address,&temp1,1);
//	temp2[0]=MPU6050_ACCEL_CONFIG;
//	temp2[1]= (temp1 & 0xE7) | (uint8_t)AccelerometerSensitivity << 3;
//	hal_i2c_master_tx(&i2c_handle,DataStruct->Address,temp2,2);
//
////	 Config gyroscope
//
//	temp1=MPU6050_GYRO_CONFIG;
//	hal_i2c_master_tx(&i2c_handle,DataStruct->Address,&temp1,1);
//	hal_i2c_master_rx(&i2c_handle,DataStruct->Address,&temp1,1);
//	temp2[0]=MPU6050_GYRO_CONFIG;
//	temp2[1]= (temp1 & 0xE7) | (uint8_t)AccelerometerSensitivity << 3;
//	hal_i2c_master_tx(&i2c_handle,DataStruct->Address,temp2,2);
//
//	// Set sensitivities for multiplying gyro and accelerometer data
//	switch (AccelerometerSensitivity) {
//		case MPU6050_Accelerometer_2G:
//			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_2;
//			break;
//		case MPU6050_Accelerometer_4G:
//			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_4;
//			break;
//		case MPU6050_Accelerometer_8G:
//			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_8;
//			break;
//		case MPU6050_Accelerometer_16G:
//			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_16;
//		default:
//			break;
//	}
//
//	switch (GyroscopeSensitivity) {
//		case MPU6050_Gyroscope_250s:
//			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_250;
//			break;
//		case MPU6050_Gyroscope_500s:
//			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_500;
//			break;
//		case MPU6050_Gyroscope_1000s:
//			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_1000;
//			break;
//		case MPU6050_Gyroscope_2000s:
//			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_2000;
//		default:
//			break;
//	}
//
//	// Return OK
//	return MPU6050_Result_Ok;
//}

extern MPU6050_Result_t MPU6050_Init(MPU6050_t* DataStruct, MPU6050_Device_t DeviceNumber, MPU6050_Accelerometer_t AccelerometerSensitivity, MPU6050_Gyroscope_t GyroscopeSensitivity) {
	uint8_t temp;

	/* Format I2C address */
	DataStruct->Address = MPU6050_I2C_ADDR | (uint8_t)DeviceNumber;


	/* Check if device is connected */
	if (!I2C_IsDeviceConnected(&i2c_handle, DataStruct->Address)) {
		/* Return error */
		return MPU6050_Result_DeviceNotConnected;
	}

	/* Check who I am */
	if (MPU6050_ByteRead(&i2c_handle, DataStruct->Address, MPU6050_WHO_AM_I) != MPU6050_I_AM) {
		/* Return error */
		return MPU6050_Result_DeviceInvalid;
	}

	/* Wakeup MPU6050 */
	MPU6050_ByteWrite(&i2c_handle, DataStruct->Address, MPU6050_PWR_MGMT_1, 0x00);

	/* Config accelerometer */
	temp = MPU6050_ByteRead(&i2c_handle, DataStruct->Address, MPU6050_ACCEL_CONFIG);
	temp = (temp & 0xE7) | (uint8_t)AccelerometerSensitivity << 3;
	MPU6050_ByteWrite(&i2c_handle, DataStruct->Address, MPU6050_ACCEL_CONFIG, temp);

	/* Config gyroscope */
	temp = MPU6050_ByteRead(&i2c_handle, DataStruct->Address, MPU6050_GYRO_CONFIG);
	temp = (temp & 0xE7) | (uint8_t)GyroscopeSensitivity << 3;
	MPU6050_ByteWrite(&i2c_handle, DataStruct->Address, MPU6050_GYRO_CONFIG, temp);

	/* Set sensitivities for multiplying gyro and accelerometer data */
	switch (AccelerometerSensitivity) {
		case MPU6050_Accelerometer_2G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_2;
			break;
		case MPU6050_Accelerometer_4G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_4;
			break;
		case MPU6050_Accelerometer_8G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_8;
			break;
		case MPU6050_Accelerometer_16G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_16;
		default:
			break;
	}

	switch (GyroscopeSensitivity) {
		case MPU6050_Gyroscope_250s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_250;
			break;
		case MPU6050_Gyroscope_500s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_500;
			break;
		case MPU6050_Gyroscope_1000s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_1000;
			break;
		case MPU6050_Gyroscope_2000s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_2000;
		default:
			break;
	}

	/* Return OK */
	return MPU6050_Result_Ok;
}
/**
 * @brief  Reads accelerometer, gyroscope and temperature data from sensor with interrupt
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure to store data to
 * @retval Member of @ref TM_MPU6050_Result_t:
 *            - TM_MPU6050_Result_Ok: everything is OK
 *            - Other: in other cases
 */
/*
extern MPU6050_Result_t MPU6050_ReadAll(MPU6050_t* DataStruct) {
	uint8_t data[14];
	int16_t temp;
	 Read full raw data, 14bytes

	temp =MPU6050_ACCEL_XOUT_H;
	hal_i2c_master_tx(&i2c_handle,DataStruct->Address,(uint8_t *)&temp,1);
	hal_i2c_master_rx(&i2c_handle,DataStruct->Address,data,14);

	 Format accelerometer data
	DataStruct->Accelerometer_X = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Accelerometer_Y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Accelerometer_Z = (int16_t)(data[4] << 8 | data[5]);

	 Format temperature
	temp = (data[6] << 8 | data[7]);
	DataStruct->Temperature = (float)((float)((int16_t)temp) / (float)340.0 + (float)36.53);

	 Format gyroscope data
	DataStruct->Gyroscope_X = (int16_t)(data[8] << 8 | data[9]);
	DataStruct->Gyroscope_Y = (int16_t)(data[10] << 8 | data[11]);
	DataStruct->Gyroscope_Z = (int16_t)(data[12] << 8 | data[13]);


	 Return OK
	return MPU6050_Result_Ok;
}*/
 /**
  * @brief  Reads accelerometer, gyroscope and temperature data from sensor
  * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure to store data to
  * @retval Member of @ref TM_MPU6050_Result_t:
  *            - TM_MPU6050_Result_Ok: everything is OK
  *            - Other: in other cases
  */

 extern MPU6050_Result_t MPU6050_ReadAll(MPU6050_t* DataStruct) {
 	uint8_t data[14];
 	int16_t temp;

 	/* Read full raw data, 14bytes */
 	MPU6050_ReadMulti(&i2c_handle, DataStruct->Address, MPU6050_ACCEL_XOUT_H, data, 14);

 	/* Format accelerometer data */
 	DataStruct->Accelerometer_X = (int16_t)(data[0] << 8 | data[1]);
 	DataStruct->Accelerometer_Y = (int16_t)(data[2] << 8 | data[3]);
 	DataStruct->Accelerometer_Z = (int16_t)(data[4] << 8 | data[5]);

 	/* Format temperature */
 	temp = (data[6] << 8 | data[7]);
 	DataStruct->Temperature = (float)((float)((int16_t)temp) / (float)340.0 + (float)36.53);

 	/* Format gyroscope data */
 	DataStruct->Gyroscope_X = (int16_t)(data[8] << 8 | data[9]);
 	DataStruct->Gyroscope_Y = (int16_t)(data[10] << 8 | data[11]);
 	DataStruct->Gyroscope_Z = (int16_t)(data[12] << 8 | data[13]);
 	if(data[1]==0){
 		STM_EVAL_LEDOn(LED3);
 	}
 	/* Return OK */
 	return MPU6050_Result_Ok;
 }
/*

static void MPU6050_I2C_ByteWrite(i2c_handle_t *handle,u8 slaveAddr, u8 pBuffer, u8 writeAddr)
{

	// Send START condition
	I2C_GenerateSTART(handle->Instance, ENABLE);
	 //Test on EV5 and clear it
	while(!I2C_CheckEvent(handle->Instance, I2C_EVENT_MASTER_MODE_SELECT));

	 //Send MPU6050 address for write
	I2C_Send7bitAddress(handle->Instance, slaveAddr, I2C_Direction_Transmitter);
	// Test on EV6 and clear it
	while(!I2C_CheckEvent(handle->Instance, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	// Send the MPU6050's internal address to write to *
	I2C_SendData(handle->Instance, writeAddr);
	// Test on EV8 and clear it
	while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	 //Send the byte to be written
	if (pBuffer!=0)
		I2C_SendData(handle->Instance, pBuffer);
	 //Test on EV8_2 and clear it
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	// Send STOP condition
	I2C_GenerateSTOP(I2C1, ENABLE);


}
static uint8_t MPU6050_I2C_ByteRead(i2c_handle_t *handle,u8 slaveAddr, u8 writeAddr)
{
	uint8_t data;

	 //Send START condition
	I2C_GenerateSTART(handle->Instance, ENABLE);

	// Test on EV5 and clear it
	while(!I2C_CheckEvent(handle->Instance, I2C_EVENT_MASTER_MODE_SELECT));

	// Enable ACK
	// Send MPU6050 address for write
	I2C_Send7bitAddress(handle->Instance, slaveAddr, I2C_Direction_Transmitter);
	// Test on EV6 and clear it
	while(!I2C_CheckEvent(handle->Instance, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	// Send the MPU6050's internal address to Read from
	I2C_SendData(handle->Instance, writeAddr);

	I2C_GenerateSTOP(handle->Instance,ENABLE);
	 //Send START condition
	I2C_GenerateSTART(handle->Instance, ENABLE);
	 //Test on EV5 and clear it

	while(!I2C_CheckEvent(handle->Instance, I2C_EVENT_MASTER_MODE_SELECT));

	 //Send MPU6050 address for write
	I2C_Send7bitAddress(handle->Instance, slaveAddr, I2C_Direction_Receiver);
	 //Test on EV6 and clear it
	while(!I2C_CheckEvent(handle->Instance, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));


	data=I2C_ReceiveData(handle->Instance);

	return data;
}
*/
/**
 * @brief  Reads single byte from slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to read from
 * @retval Data from slave
 */
static uint8_t MPU6050_ByteRead(i2c_handle_t *handle, uint8_t address, uint8_t reg)
{
	uint8_t received_data;
	I2C_Start(handle, address, I2C_TRANSMITTER_MODE);
	I2C_WriteData(handle, reg);
	I2C_GenerateSTOP(handle->Instance,ENABLE);
	I2C_Start(handle, address, I2C_RECEIVER_MODE);
	received_data = I2C_ReadNack(handle);
	return received_data;
}

/**
 * @brief  Writes single byte to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  data: data to be written
 * @retval None
 */
static void MPU6050_ByteWrite(i2c_handle_t *handle, uint8_t address, uint8_t reg, uint8_t data)
{
	I2C_Start(handle, address, I2C_TRANSMITTER_MODE);
	I2C_WriteData(handle, reg);
	I2C_WriteData(handle, data);
	I2C_GenerateSTOP(handle->Instance,ENABLE);
}

/**
 * @brief  Reads multi bytes from slave
 * @param  *I2Cx: I2C used
 * @param  uint8_t address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  uint8_t reg: register to read from
 * @param  uint8_t *data: pointer to data array to store data from slave
 * @param  uint8_t count: how many bytes will be read
 * @retval None
 */
void MPU6050_ReadMulti(i2c_handle_t *handle,uint8_t address,uint8_t reg, uint8_t* data, uint16_t count)
{

	I2C_Start(handle->Instance, address, I2C_TRANSMITTER_MODE);
	I2C_WriteData(handle, reg);
	//TM_I2C_Stop(I2Cx);
	I2C_Start(handle, address, I2C_RECEIVER_MODE);
	while (count--) {
		if (!count) {
			/* Last byte */
			*data++ = I2C_ReadNack(handle);
		} else {
			*data++ = I2C_ReadAck(handle);
		}
	}
}
