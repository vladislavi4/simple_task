#ifndef __UART_H
/* Private defines -----------------------------------------------------------*/
#define __UART_H

/* Includes ------------------------------------------------------------------*/
#include "project_config.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/


void UART_init(uint32_t periph_num, uint8_t mode, uint32_t baud);
void UART_Send_String (uint32_t periph_num, char* str);
void UART_Send (uint32_t periph_num, char chr);
void UART_Send_data (uint32_t periph_num, uint8_t* data, uint16_t len);

#endif /* __UART_H */