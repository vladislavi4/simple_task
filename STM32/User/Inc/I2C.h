#ifndef __I2C_H
/* Private defines -----------------------------------------------------------*/
#define __I2C_H

/* Includes ------------------------------------------------------------------*/
#include "project_config.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void I2C_init(uint8_t periph_num, uint8_t mode, uint8_t speed);
void I2C_Read(uint8_t periph_num, uint8_t slave_addr, uint8_t reg_addr, uint8_t* buff, uint8_t size);
void I2C_Write(uint8_t periph_num, uint8_t slave_addr, uint8_t reg_addr, uint8_t* data, uint8_t size);

#endif /* __I2C_H */