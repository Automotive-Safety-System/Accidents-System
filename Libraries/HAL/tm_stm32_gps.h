
/*
  ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_GPS_H
#define TM_GPS_H 100

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif


#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"

/**
 * @defgroup TM_GPS_Macros
 * @brief    Library private defines without any sense for USER
 * @{
 */

/* Default GPS USART used */
#ifndef GPS_USART

	#define GPS_USART                   USART2

#endif

	/* Maximum number of custom GPGxx values */
#ifndef GPS_CUSTOM_NUMBER
#define GPS_CUSTOM_NUMBER       10
#endif

 /**
 * @}
 */

/**
 * @defgroup TM_GPS_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  GPS Result enumeration
 */
typedef enum {
	TM_GPS_Result_NewData,          /*!< New data are available to operate with */
	TM_GPS_Result_OldData,          /*!< We don't have new data available yet */
	TM_GPS_Result_FirstDataWaiting, /*!< We are waiting for first data from GPS module */
} TM_GPS_Result_t;


/**
 * @brief  Speed conversion enumeration
 * @note   Speed from GPS is in knots, use TM_GPS_ConvertSpeed() to convert to useable value
 */
typedef enum {
	/* Metric */
	TM_GPS_Speed_KilometerPerSecond,  /*!< Convert speed to kilo meters per second */
	TM_GPS_Speed_MeterPerSecond,      /*!< Convert speed to meters per second */
	TM_GPS_Speed_KilometerPerHour,    /*!< Convert speed to kilo meters per hour */
	TM_GPS_Speed_MeterPerMinute,      /*!< Convert speed to meter per minute */
	/* Imperial */
	TM_GPS_Speed_MilePerSecond,       /*!< Convert speed to miles per second */
	TM_GPS_Speed_MilePerHour,         /*!< Convert speed to miles per hour */
	TM_GPS_Speed_FootPerSecond,       /*!< Convert speed to foots per second */
	TM_GPS_Speed_FootPerMinute,       /*!< Convert speed to foots per minute */
	/* For Runners and Joggers */
	TM_GPS_Speed_MinutePerKilometer,  /*!< Convert speed to minutes per kilo meter */
	TM_GPS_Speed_SecondPerKilometer,  /*!< Convert speed to seconds per kilo meter */
	TM_GPS_Speed_SecondPer100Meters,  /*!< Convert speed to seconds per 100 meters */
	TM_GPS_Speed_MinutePerMile,       /*!< Convert speed to minutes per mile */
	TM_GPS_Speed_SecondPerMile,       /*!< Convert speed to seconds per mile */
	TM_GPS_Speed_SecondPer100Yards,   /*!< Convert speed to seconds per 100 yards */
	/* Nautical */
	TM_GPS_Speed_SeaMilePerHour,      /*!< Convert speed to sea miles per hour */
} TM_GPS_Speed_t;


/**
 * @brief  GPS float structure
 * @note   GPS has longitude, latitude, altitude, etc.. in float format.
 *
 *         But, if you want to make a tracker and save data to SD card, there is a problem, because FATFS library breaks %f with sprintf() function.
 *
 *         For that purpose I made a new function TM_GPS_ConvertFloat() which converts float number to an integer and decimal part.
 */
typedef struct {
	int32_t Integer;   /*!< Integer part of float number. */
	uint32_t Decimal;  /*!< Decimal part of float number, in integer format. */
} TM_GPS_Float_t;

/**
 * @brief  Date struct for GPS date
 */
typedef struct {
	uint8_t Date;  /*!< Date in month from GPS. */
	uint8_t Month; /*!< Month from GPS. */
	uint8_t Year;  /*!< Year from GPS. */
} TM_GPS_Date_t;

/**
 * @brief  Time structure for GPS
 */
typedef struct {
	uint8_t Hours;       /*!< Hours from GPS time. */
	uint8_t Minutes;     /*!< Minutes from GPS time. */
	uint8_t Seconds;     /*!< Seconds from GPS time. */
	uint16_t Hundredths; /*!< Hundredths from GPS time. */
} TM_GPS_Time_t;

/**
 * @brief  Satellite in view description structure
 */
typedef struct {
	uint8_t ID;        /*!< SV PRN number */
	uint8_t Elevation; /*!< Elevation in degrees, 90 maximum */
	uint16_t Azimuth;  /*!< Azimuth, degrees from true north, 000 to 359 */
	uint8_t SNR;       /*!< SNR, 00-99 dB (0 when not tracking) */
} TM_GPS_Satellite_t;

/**
 * @brief  Custom NMEA statement and term, selected by user
 */
typedef struct {
	char Statement[7];  /*!< Statement value, including "$" at beginning. For example, "$GPRMC" */
	uint8_t TermNumber; /*!< Term number position inside statement */
	char Value[15];     /*!< Value from GPS receiver at given statement and term number will be stored here.
	                            @note Value will not be converted to number if needed, but will stay as a character */
	uint8_t Updated;    /*!< Updated flag. If this parameter is set to 1, then new update has been made. Meant for private use */
} TM_GPS_Custom_t;

/**
 * @brief  Main GPS data structure
 */
typedef struct {
#ifndef GPS_DISABLE_GPGGA
	float Latitude;                                       /*!< Latitude position from GPS, -90 to 90 degrees response. */
	float Longitude;                                      /*!< Longitude position from GPS, -180 to 180 degrees response. */
	float Altitude;                                       /*!< Altitude above the seain units of meters */
	uint8_t Satellites;                                   /*!< Number of satellites in use for GPS position. */
	uint8_t Fix;                                          /*!< GPS fix; 0: Invalid; 1: GPS Fix; 2: DGPS Fix. */
	TM_GPS_Time_t Time;                                   /*!< Current time from GPS. @ref TM_GPS_Time_t. */
#endif
#ifndef GPS_DISABLE_GPRMC
	TM_GPS_Date_t Date;                                   /*!< Current date from GPS. @ref TM_GPS_Date_t. */
	float Speed;                                          /*!< Speed in knots from GPS. */
	uint8_t Validity;                                     /*!< GPS validation; 1: valid; 0: invalid. */
	float Direction;                                      /*!< Course on the ground in relation to North. */
#endif
#ifndef GPS_DISABLE_GPGSA
	float HDOP;                                           /*!< Horizontal dilution of precision. */
	float PDOP;                                           /*!< Position dilution od precision. */
	float VDOP;                                           /*!< Vertical dilution of precision. */
	uint8_t FixMode;                                      /*!< Current fix mode in use:; 1: Fix not available; 2: 2D; 3: 3D. */
	uint8_t SatelliteIDs[12];                             /*!< Array with IDs of satellites in use.
	                                                           Only first data are valid, so if you have 5 satellites in use, only SatelliteIDs[4:0] are valid */
#endif
#ifndef GPS_DISABLE_GPGSV
	uint8_t SatellitesInView;                             /*!< Number of satellites in view */
	TM_GPS_Satellite_t SatDesc[30];                       /*!< Description of each satellite in view */
#endif
	TM_GPS_Result_t Status;                               /*!< GPS result. This parameter is value of @ref TM_GPS_Result_t */
	TM_GPS_Custom_t* CustomStatements[GPS_CUSTOM_NUMBER]; /*!< Array of pointers for custom GPS NMEA statements, selected by user.
	                                                              You can use @ref GPS_CUSTOM_NUMBER number of custom statements */
	uint8_t CustomStatementsCount;                        /*!< Number of custom GPS statements selected by user */
} TM_GPS_t;

/* Backward compatibility */
typedef TM_GPS_t TM_GPS_Data_t;

/**
 * @brief  GPS Distance and bearing struct
 */
typedef struct {
	float Latitude1;  /*!< Latitude of starting point. */
	float Longitude1; /*!< Longitude of starting point. */
	float Latitude2;  /*!< Latitude of ending point. */
	float Longitude2; /*!< Longitude of ending point. */
	float Distance;   /*!< Distance between 2 points which will be calculated. */
	float Bearing;    /*!< Bearing from start to stop point according to North. */
} TM_GPS_Distance_t;

/**
 * @}
 */

/**
 * @defgroup TM_GPS_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes GPS and USART peripheral
 * @param  *GPS_Data: Pointer to @ref TM_GPS_t structure to set default values
 * @param  baudrate: Specify GPS baudrate for USART. Most common are 9600 or 115200 bauds
 * @note   GPS baudrate can have other values. Check GPS datasheet for proper info.
 * @retval None
 */
void TM_GPS_Init(TM_GPS_t* GPS_Data, uint32_t baudrate);

/**
 * @brief  Update GPS data.
 * @note   This function must be called periodically, as fast as possible.
 *         It basically checks if data is available on GPS USART and parse it to useful data for user.
 * @note   - When you first call this function and there is not available data from GPS, this function will return @ref TM_GPS_Result_FirstTimeWaiting.
 *         - This will be returning all the time we don't have any useful data.
 *         - When first time useful data is received from GPS (everything parsed), @ref TM_GPS_Result_NewData will be returned.
 *         - When we have already new data, next time we call this function, @ref TM_GPS_Result_OldData will be returning until we don't receive new packet of useful data.
 * @note   If you are making GPS logger, then when you receive @ref TM_GPS_Result_NewData it is time to save your data.
 * @param  *GPS_Data: Pointer to working @ref TM_GPS_t structure
 * @retval Returns value of @ref TM_GPS_Result_t structure
 */
TM_GPS_Result_t TM_GPS_Update(TM_GPS_t* GPS_Data);

/**
 * @brief  Converts speed in knots (from GPS) to user selectable speed
 * @param  speedInKnots: float value from GPS module
 * @param  toSpeed: Select to which speed you want conversion from knot. This parameter ca be a value of TM_GPS_Speed_t enumeration.
 * @retval Calculated speed from knots to user selectable format
 */
float TM_GPS_ConvertSpeed(float SpeedInKnots, TM_GPS_Speed_t toSpeed);

/**
 * @brief  Converts float number into integer and decimal part
 * @param  num: Float number to split into 2 parts
 * @param  *Float_Data: Pointer to TM_GPS_Float_t structure where to save result
 * @param  decimals: Number of decimal places for conversion
 * @note   Example: You have number 15.002 in float format.
 *            - You want to split this to integer and decimal part with 6 decimal places.
 *            - Call @ref TM_GPS_ConvertFloat(15.002, &Float_Struct, 6);
 *            - Result will be: Integer: 15; Decimal: 2000 (0.002 * 10^6)
 * @retval None
 */
void TM_GPS_ConvertFloat(float num, TM_GPS_Float_t* Float_Data, uint8_t decimals);

/**
 * @brief  Calculates distance between 2 coordinates on earth and bearing from start to end point in relation to the north
 * @param  *Distance_Data: Pointer to @ref TM_GPS_Distance_t structure with latitude and longitude set values
 * @note   Calculation results will be saved in *Distance_Data @ref TM_GPS_Distance_t structure
 * @retval None
 */
void TM_GPS_DistanceBetween(TM_GPS_Distance_t* Distance_Data);

/**
 * @brief  Adds custom GPG statement to array of user selectable statements.
 *            Array is available to user using @ref TM_GPS_t workign structure
 * @note   Functions uses @ref malloc() function to allocate memory, so make sure you have enough heap memory available.
 * @note   Also note, that your GPS receiver HAVE TO send statement type you use in this function, or
 *            @ref TM_GPS_Update() function will always return that there is not data available to read.
 * @param  *GPS_Data: Pointer to working @ref TM_GPS_t structure
 * @param  *GPG_Statement: String of NMEA starting line address, including "$" at beginning
 * @param  TermNumber: Position in NMEA statement
 * @retval Success status:
 *            - NULL: Malloc() failed or you reached limit of user selectable custom statements:
 *            - > NULL: Function succeded, pointer to @ref TM_GPS_Custom_t structure
 */
TM_GPS_Custom_t * TM_GPS_AddCustom(TM_GPS_t* GPS_Data, char* GPG_Statement, uint8_t TermNumber);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

