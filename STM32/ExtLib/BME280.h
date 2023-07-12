#ifndef __BME280_H

/* Private defines -----------------------------------------------------------*/
#define __BME280_H

//#define BME_NSS_ENABLE()    (GPIOA->ODR &= ~GPIO_ODR_ODR_4)
//#define BME_NSS_DISABLE()   (GPIOA->ODR |= GPIO_ODR_ODR_4)
#define BME_NSS_ENABLE()    (GPIOD->ODR &= ~GPIO_ODR_ODR_0)
#define BME_NSS_DISABLE()   (GPIOD->ODR |= GPIO_ODR_ODR_0)

#define SEALEVELPRESSURE_HPA (1013.25)
#define SEALEVELPRESSURE_PA (1013250)
//------------------------------------------------
#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#define be24toword(a) ((((a)>>16)&0x000000ff)|((a)&0x0000ff00)|(((a)<<16)&0x00ff0000))
//------------------------------------------------
#define BME280_ADDR 0xEC //BME280 I2C ADDRES (0x76<<1)
#define BME280_REG_ID 0xD0 //BME280 ID REGISTER
#define BME280_ID 0x60 //BME280 I2C ID
#define BME280_REG_SOFTRESET 0xE0 //BME280 SOFT RESET REGISTER
#define BME280_SOFTRESET_VALUE 0xB6 //BME280 SOFT RESET VALUE
#define BME280_REG_CTRL_HUM 0xF2 // Humidity measure control register
#define BME280_REGISTER_STATUS 0XF3 //BME280 STATUS REGISTER
#define BME280_REG_CTRL_MEAS 0xF4 // Control register pressure and temperature measure
#define BME280_STATUS_MEASURING 0X08 //Running conversion
#define BME280_STATUS_IM_UPDATE 0X01 //NVM data copying
#define BME280_REG_CONFIG 0xF5 // Configuration register
#define BME280_REGISTER_PRESSUREDATA 0xF7
#define BME280_REGISTER_TEMPDATA 0xFA
#define BME280_REGISTER_HUMIDDATA 0xFD
//------------------------------------------------
#define BME280_REGISTER_DIG_T1 0x88
#define BME280_REGISTER_DIG_T2 0x8A
#define BME280_REGISTER_DIG_T3 0x8C
#define BME280_REGISTER_DIG_P1 0x8E
#define BME280_REGISTER_DIG_P2 0x90
#define BME280_REGISTER_DIG_P3 0x92
#define BME280_REGISTER_DIG_P4 0x94
#define BME280_REGISTER_DIG_P5 0x96
#define BME280_REGISTER_DIG_P6 0x98
#define BME280_REGISTER_DIG_P7 0x9A
#define BME280_REGISTER_DIG_P8 0x9C
#define BME280_REGISTER_DIG_P9 0x9E
#define BME280_REGISTER_DIG_H1 0xA1
#define BME280_REGISTER_DIG_H2 0xE1
#define BME280_REGISTER_DIG_H3 0xE3
#define BME280_REGISTER_DIG_H4 0xE4
#define BME280_REGISTER_DIG_H5 0xE5
#define BME280_REGISTER_DIG_H6 0xE7
//------------------------------------------------
#define BME280_STBY_MSK 0xE0
#define BME280_STBY_0_5 0x00
#define BME280_STBY_62_5 0x20
#define BME280_STBY_125 0x40
#define BME280_STBY_250 0x60
#define BME280_STBY_500 0x80
#define BME280_STBY_1000 0xA0
#define BME280_STBY_10 0xC0
#define BME280_STBY_20 0xE0
//------------------------------------------------
#define BME280_FILTER_MSK 0x1C
#define BME280_FILTER_OFF 0x00
#define BME280_FILTER_2 0x04
#define BME280_FILTER_4 0x08
#define BME280_FILTER_8 0x0C
#define BME280_FILTER_16 0x10
//------------------------------------------------
#define BME280_OSRS_T_MSK 0xE0
#define BME280_OSRS_T_SKIP 0x00
#define BME280_OSRS_T_x1 0x20
#define BME280_OSRS_T_x2 0x40
#define BME280_OSRS_T_x4 0x60
#define BME280_OSRS_T_x8 0x80
#define BME280_OSRS_T_x16 0xA0
#define BME280_OSRS_P_MSK 0x1C
#define BME280_OSRS_P_SKIP 0x00
#define BME280_OSRS_P_x1 0x04
#define BME280_OSRS_P_x2 0x08
#define BME280_OSRS_P_x4 0x0C
#define BME280_OSRS_P_x8 0x10
#define BME280_OSRS_P_x16 0x14
#define BME280_OSRS_H_MSK 0x07
#define BME280_OSRS_H_SKIP 0x00
#define BME280_OSRS_H_x1 0x01
#define BME280_OSRS_H_x2 0x02
#define BME280_OSRS_H_x4 0x03
#define BME280_OSRS_H_x8 0x04
#define BME280_OSRS_H_x16 0x05
//------------------------------------------------
#define BME280_MODE_MSK 0x03
#define BME280_MODE_SLEEP 0x00
#define BME280_MODE_FORCED 0x01
#define BME280_MODE_NORMAL 0x03
//------------------------------------------------

/* Includes ------------------------------------------------------------------*/
//#include "project_config.h"
#include "board.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void BME_init(void);
//float BME280_getTemperature(int8_t n);
uint16_t BME280_getTemperature(int8_t n);
uint16_t BME280_getHumidity(int8_t n);
uint16_t BME280_getPressure(void);
//float BME280_getHumidity(int8_t n);
//float BME280_getPressure(void);
float fahrenheit(float celsius);
double calculate_humidex(double temperature, double humidity);
double calculate_humidityAbsolute(double temperature, double humidityRelative);

#endif  /* __BME280_H  */