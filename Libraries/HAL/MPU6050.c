/*
 * MPU6050.c
 *
 *  Created on: Mar 9, 2020
 *      Author: mahmo
 */
#include <stdio.h>
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include "MPU6050.h"
#include "MPU6050_Config.h"
#include "stm32f4_discovery.h"






/* Default I2C clock */
#define MPU6050_I2C_CLOCK			400000

// Setup MPU6050
#define MPU6050_ADDR 0xD0
#define MPU6050_I_AM				0x68




// offset Values
float    base_x_accel;
float    base_y_accel;
float    base_z_accel;
float    base_x_gyro;
float    base_y_gyro;
float    base_z_gyro;

float    RADIANS_TO_DEGREES ;
//float 	 Last_Read_Time;

float    last_x_angle;  // These are the filtered angles
float    last_y_angle;
float    last_z_angle;


static	I2C_HandleTypeDef I2C_Handle;

MPU6050_t MY_MPU;

static float accelScalingFactor, gyroScalingFactor;


TaskHandle_t xTask_MPU6050_Read_RawData=NULL;

TickType_t Last_Read_Time;
TickType_t Current_Read_Time;

//uint16_t ACC_BUF[3][3];



static void MPU6050_I2C_Config();
static void MPU6050_Config_Init(MPU6050_t *Data_Struct);
static void MPU6050_Init(MPU6050_t *Data_Struct);
static void MPU6050_Get_Accel_Scale();
static void MPU6050_Get_Gyro_Scale();

static void MPU6050_Calibrate();

static void MPU6050_Get_Accel_Angels();

/*      HELPER Function      */
static float get_last_x_angle();
static float get_last_y_angle();
static float get_last_z_angle();
static TickType_t get_last_read_Time(void);
static void Update_Last_Data(TickType_t Time ,float x, float y, float z);


//Fucntion Definitions
//1- i2c Handler
/*
static void MPU6050_StructCpy(MPU6050_t *Data_Struct)
{
	//Copy I2C CubeMX handle to local library
	memcpy(MY_MPU,Data_Struct, sizeof(*Data_Struct));
}
*/


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
	I2C_Handle.Instance=I2Cx;

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
	/*printf("%d\n",Data_Struct->Accel_Full_Scale);
	printf("%d\n",MY_MPU.Accel_Full_Scale);*/

	accelScalingFactor=0;
	gyroScalingFactor=0;


	base_x_accel=-2290;
	base_y_accel=165;
	base_z_accel=960;

	base_x_gyro=222;
	base_y_gyro=82;
	base_z_gyro=24;

	RADIANS_TO_DEGREES = 180/3.14159;
	last_x_angle=0;
	last_y_angle=0;
	last_z_angle=0;
}

static void MPU6050_Init(MPU6050_t *Data_Struct)
{
	uint8_t temp;
	//uint8_t Buffer = 0;;

	// check device ID WHO_AM_I
	I2C_Handle.State=HAL_I2C_STATE_READY;

	HAL_I2C_Mem_Read(&I2C_Handle, MPU6050_ADDR,WHO_AM_I_REG,&temp, 1);

	if (temp == MPU6050_I_AM)  // 0x68 will be returned by the sensor if everything goes well
	{

		// power management register 0X6B we should write all 0's to wake the sensor up
		temp = 0x00;
		HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, PWR_MAGT_1_REG,&temp, 1);

		//10 ms delay



		/*Buffer = Data_Struct ->ClockSource & 0x07; //change the 7th bits of register
		Buffer |= (Data_Struct ->Sleep_Mode_Bit << 6)&0x40;
		//HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, PWR_MAGT_1_REG,&Buffer, 1);
		if(HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, PWR_MAGT_1_REG,&Buffer, 1)==HAL_OK)
		{
			//STM_EVAL_LEDOn(LED3);
		}
*/
		//10 ms delay



		//Set the Digital Low Pass Filter
		/*Buffer = 0;
		Buffer = Data_Struct->CONFIG_DLPF & 0x07;
		HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, CONFIG_REG, &Buffer, 1);
*/
		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g
		temp = 0x00;
		HAL_I2C_Mem_Read(&I2C_Handle,MPU6050_ADDR,GYRO_CONFIG_REG,&temp,1);
		temp = (temp & 0xE7) | (Data_Struct->Gyro_Full_Scale << 3) ;
		if(	HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, GYRO_CONFIG_REG, &temp, 1)==HAL_OK){
			STM_EVAL_LEDOn(LED4);
		}


		temp = 0x00;
		HAL_I2C_Mem_Read(&I2C_Handle,MPU6050_ADDR,ACCEL_CONFIG_REG,&temp,1);
		temp = (temp & 0xE7) | (Data_Struct->Accel_Full_Scale << 3) ;
		if(	HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, ACCEL_CONFIG_REG, &temp, 1)==HAL_OK){
			STM_EVAL_LEDOn(LED5);
		}
	//Set SRD To  1Khz
		/*Buffer = 0x07;
		HAL_I2C_Mem_Write(&I2C_Handle, MPU6050_ADDR, SMPLRT_DIV_REG, &Buffer, 1);*/
		

	}

	//Accelerometer Scaling Factor, Set the Accelerometer and Gyroscope Scaling Factor
	switch (Data_Struct->Accel_Full_Scale)
	{
		case AFS_SEL_2g:
			accelScalingFactor = (2.0f/32768.0f);
			break;

		case AFS_SEL_4g:
			accelScalingFactor = (4.0f/32768.0f);
				break;

		case AFS_SEL_8g:
			accelScalingFactor = (8.0f/32768.0f);
			break;

		case AFS_SEL_16g:
			accelScalingFactor = (16.0f/32768.0f);
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


extern void MPU6050_Module_INIT()
{
	// MPU6050_StructCpy(&Data_Struct);
	 MPU6050_I2C_Config();
	 MPU6050_Config_Init(&MY_MPU);
	 MPU6050_Init(&MY_MPU);
}



void MPU6050_Read_All()
{

	uint8_t Rec_Data[14]={0};
	int16_t temp;


	// Read 14 BYTES of data starting from ACCEL_XOUT_H register
	while(1){
		Current_Read_Time=xTaskGetTickCount();

		if(HAL_I2C_Mem_Read(&I2C_Handle, MPU6050_ADDR, ACCEL_XOUT_H_REG,Rec_Data,14)==HAL_OK){
			//STM_EVAL_LEDToggle(LED6);
		}
		/*temp=ACCEL_XOUT_H_REG;
		HAL_I2C_Master_Transmit(&I2C_Handle,MPU6050_ADDR,&temp,1);
		HAL_I2C_Master_Receive(&I2C_Handle,MPU6050_ADDR,Rec_Data,2);*/

		MY_MPU.Accel_X_RAW= (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);

		MY_MPU.Accel_Y_RAW= (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);

		MY_MPU.Accel_Z_RAW= (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

		temp = (int16_t)(Rec_Data[6] << 8 | Rec_Data [7]);
		MY_MPU.Temperature=temp;

		MY_MPU.Gyro_X_RAW = (int16_t)(Rec_Data[8] << 8 | Rec_Data [9]);

		MY_MPU.Gyro_Y_RAW= (int16_t)(Rec_Data[10] << 8 | Rec_Data [11]);

		MY_MPU.Gyro_Z_RAW= (int16_t)(Rec_Data[12] << 8 | Rec_Data [13]);




/*
		printf("%d	\t",MY_MPU.Accel_X_RAW);

		printf("%d \t",MY_MPU.Accel_Y_RAW);

		printf("%d \t",MY_MPU.Accel_Z_RAW);

		printf("%d \t",MY_MPU.Gyro_X_RAW);

		printf("%d \t",MY_MPU.Gyro_Y_RAW);

		printf("%d \t\n",MY_MPU.Gyro_Z_RAW);

*/

		MPU6050_Get_Accel_Scale(&MY_MPU);
		MPU6050_Get_Gyro_Scale(&MY_MPU);

		vTaskDelay(pdMS_TO_TICKS(100));
	}

}

//10- Get Accel scaled data (g unit of gravity, 1g = 9.81m/s2)
static void MPU6050_Get_Accel_Scale(MPU6050_t *Data_Struct)
{

//	char buf[20];

	//Accel Scale data
	Data_Struct->Ax = ((Data_Struct->Accel_X_RAW+0.0f-base_x_accel)*accelScalingFactor);

	Data_Struct->Ay= ((Data_Struct->Accel_Y_RAW+0.0f-base_y_accel)*accelScalingFactor);
	Data_Struct->Az = ((Data_Struct->Accel_Z_RAW+0.0f-base_z_accel)*accelScalingFactor);


   /* gcvt(Data_Struct->Ax,5,buf);

	printf("%s\t\t",buf);
	gcvt(Data_Struct->Ay,5,buf);

	printf("%s\t\t",buf);
	gcvt(Data_Struct->Az,5,buf);

	printf("%s\t\t",buf);
*/


}
static void MPU6050_Get_Accel_Angels(MPU6050_t *Data_Struct)
{
	//Roll angle
	Data_Struct->Acc_Agnle_X=atan(Data_Struct->Ay/sqrt(pow(Data_Struct->Ax,2)+pow(Data_Struct->Az,2)))*RADIANS_TO_DEGREES;

	//pitch angle
	Data_Struct->Acc_Agnle_Y=atan(Data_Struct->Ax/sqrt(pow(Data_Struct->Ay,2)+pow(Data_Struct->Az,2)))*RADIANS_TO_DEGREES;
	//yaw angle
	Data_Struct->Acc_Agnle_Z=0;
}

//13- Get Gyro scaled data
static void MPU6050_Get_Gyro_Scale(MPU6050_t *Data_Struct)
{
	//char buf[20];
	//Accel Scale data

	Data_Struct->Gx = ((Data_Struct->Gyro_X_RAW+0.0f-base_x_gyro)*gyroScalingFactor);
	Data_Struct->Gy=  ((Data_Struct->Gyro_Y_RAW+0.0f-base_y_gyro)*gyroScalingFactor);
	Data_Struct->Gz = ((Data_Struct->Gyro_Z_RAW+0.0f-base_z_gyro)*gyroScalingFactor);



/*

   gcvt(Data_Struct->Gx,5,buf);

	printf("%s\t\t",buf);
	gcvt(Data_Struct->Gy,5,buf);

	printf("%s\t\t",buf);



	gcvt(Data_Struct->Gz,5,buf);

	printf("%s\t\t\n",buf);
*/



}
static void MPU6050_Get_Gyro_Angels(MPU6050_t *Data_Struct)
{
	float dt;
	//time in second
	//to be checked
	dt=((Current_Read_Time- get_last_read_Time())/configTICK_RATE_HZ);

	Data_Struct->Gyro_Agnle_X=Data_Struct->Gx * dt + get_last_x_angle();
	Data_Struct->Gyro_Agnle_Y=Data_Struct->Gy * dt + get_last_y_angle();
	Data_Struct->Gyro_Agnle_Z=Data_Struct->Gz * dt + get_last_z_angle();

}

static void MPU6050_Get_ComplemtaryFilter_Angels(MPU6050_t *Data_Struct)
{
	//to be changed
	float alpha = 0.96;


	Data_Struct->ComFilter_Angle_X=alpha*(Data_Struct->Gyro_Agnle_X)+(1.0-alpha)*(Data_Struct->Acc_Agnle_X);
	Data_Struct->ComFilter_Angle_Y=alpha*(Data_Struct->Gyro_Agnle_Y)+(1.0-alpha)*(Data_Struct->Acc_Agnle_Y);
	Data_Struct->ComFilter_Angle_Z=Data_Struct->Gyro_Agnle_Z;
}

//to be calibrated on Arduino
static void MPU6050_Calibrate()
{
	  int                   num_readings = 10;
	  float                 x_accel = 0;
	  float                 y_accel = 0;
	  float                 z_accel = 0;
	  float                 x_gyro = 0;
	  float                 y_gyro = 0;
	  float                 z_gyro = 0;
	  uint8_t Rec_Data[14];


	  // Read and average the raw values from the IMU
	  for (int i = 0; i < num_readings; i++) {
		  HAL_I2C_Mem_Read_IT(&I2C_Handle, MPU6050_ADDR, ACCEL_XOUT_H_REG,Rec_Data, 14);
		  x_accel += (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
		  y_accel += (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
		  z_accel += (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
		  x_gyro  +=(int16_t)(Rec_Data[8] << 8 | Rec_Data [9]);
		  y_gyro  += (int16_t)(Rec_Data[10] << 8 | Rec_Data [11]);
		  z_gyro  += (int16_t)(Rec_Data[12] << 8 | Rec_Data [13]);
	  }
	  x_accel /=num_readings;
	  y_accel /=num_readings;
	  z_accel /=num_readings;
	  x_gyro /= num_readings;
	  y_gyro /= num_readings;
	  z_gyro /= num_readings;

	  // Store the raw calibration values globally
	  base_x_accel = x_accel;
	  base_y_accel = y_accel;
	  base_z_accel = z_accel;
	  base_x_gyro = x_gyro;
	  base_y_gyro = y_gyro;
	  base_z_gyro = z_gyro;
}

void HAL_I2C_EVENT_CALLBack()
{
	HAL_I2C_EV_IRQHandler(&I2C_Handle);
}

static TickType_t get_last_read_Time(void){
	return Last_Read_Time;
}


static float get_last_x_angle(void){
	return last_x_angle;
}

static float get_last_y_angle(void){
	return last_y_angle;
}
static float get_last_z_angle(void){
	return last_z_angle;
}

static void Update_Last_Data(TickType_t Time ,float x, float y, float z)
{
	Last_Read_Time=Time;
	last_x_angle=x;
	last_y_angle=y;
	last_z_angle=z;
}


// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

