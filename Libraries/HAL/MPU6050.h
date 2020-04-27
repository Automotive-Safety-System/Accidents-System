/*
 * MPU6050.h
 *
 *  Created on: Mar 9, 2020
 *      Author: mahmo
 */

#ifndef MPU6050_H_
#define MPU6050_H_
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

//Define Registers
#define WHO_AM_I_REG			0x75
#define MPU_ADDR				0x68
#define PWR_MAGT_1_REG			0x6B
#define CONFIG_REG				0x1A
#define GYRO_CONFIG_REG			0x1B
#define ACCEL_CONFIG_REG		0x1C
#define SMPLRT_DIV_REG			0x19
#define INT_STATUS_REG			0x3A
#define ACCEL_XOUT_H_REG		0x3B
#define TEMP_OUT_H_REG			0x41
#define GYRO_XOUT_H_REG			0x43
#define FIFO_EN_REG 			0x23
#define INT_ENABLE_REG 			0x38
#define I2CMACO_REG 			0x23
#define USER_CNT_REG			0x6A
#define FIFO_COUNTH_REG 		0x72
#define FIFO_R_W_REG 			0x74




// MPU6050 structure
typedef struct  {

	// Raw Data Acc
	int16_t Accel_X_RAW;
	int16_t Accel_Y_RAW;
	int16_t Accel_Z_RAW;
	
	//scaled data acc
	double Ax;
	double Ay;
	double Az;

	// Raw Data Gyro
	int16_t Gyro_X_RAW;
	int16_t Gyro_Y_RAW;
	int16_t Gyro_Z_RAW;
	
	//scaled data gyro
	double Gx;
	double Gy;
	double Gz;

	float   Temperature;

	//Acc Angels
	float Acc_Agnle_X;
	float Acc_Agnle_Y;
	float Acc_Agnle_Z;

	//Gyro Angels
	float Gyro_Agnle_X;
	float Gyro_Agnle_Y;
	float Gyro_Agnle_Z;

	//Complentary filter Angels
	float ComFilter_Angle_X ;
	float ComFilter_Angle_Y ;
	float ComFilter_Angle_Z ;

	uint8_t ClockSource;
	uint8_t Gyro_Full_Scale;
	uint8_t Accel_Full_Scale;
	uint8_t CONFIG_DLPF;
	uint8_t Sleep_Mode_Bit;

}MPU6050_t;

//2- Clock Source ENUM
enum PM_CLKSEL_ENUM
{
	Internal_8MHz 	= 0x00,
	X_Axis_Ref			= 0x01,
	Y_Axis_Ref			= 0x02,
	Z_Axis_Ref			= 0x03,
	Ext_32_768KHz		= 0x04,
	Ext_19_2MHz			= 0x05,
	TIM_GENT_INREST	= 0x07
};
//3- Gyro Full Scale Range ENUM (deg/sec)
enum gyro_FullScale_ENUM
{
	FS_SEL_250 	= 0x00,
	FS_SEL_500 	= 0x01,
	FS_SEL_1000 = 0x02,
	FS_SEL_2000	= 0x03
};
//4- Accelerometer Full Scale Range ENUM (1g = 9.81m/s2)
enum accel_FullScale_ENUM
{
	AFS_SEL_2g	= 0x00,
	AFS_SEL_4g,
	AFS_SEL_8g,
	AFS_SEL_16g
};
//5- Digital Low Pass Filter ENUM
enum DLPF_CFG_ENUM
{
	DLPF_260A_256G_Hz = 0x00,
	DLPF_184A_188G_Hz = 0x01,
	DLPF_94A_98G_Hz 	= 0x02,
	DLPF_44A_42G_Hz 	= 0x03,
	DLPF_21A_20G_Hz 	= 0x04,
	DLPF_10_Hz 				= 0x05,
	DLPF_5_Hz 				= 0x06
};
//6- e external Frame Synchronization ENUM
enum EXT_SYNC_SET_ENUM
{
	input_Disable = 0x00,
	TEMP_OUT_L		= 0x01,
	GYRO_XOUT_L		= 0x02,
	GYRO_YOUT_L		= 0x03,
	GYRO_ZOUT_L		= 0x04,
	ACCEL_XOUT_L	= 0x05,
	ACCEL_YOUT_L	= 0x06,
	ACCEL_ZOUT_L	= 0x07
};



extern void MPU6050_Module_INIT();
void MPU6050_Read_All();
void HAL_I2C_EVENT_CALLBack(void);


#endif /* MPU6050_H_ */
