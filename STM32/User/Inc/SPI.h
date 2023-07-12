#ifndef __SPI_H
/* Private defines -----------------------------------------------------------*/
#define __SPI_H

/* Includes ------------------------------------------------------------------*/
#include "project_config.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
enum spi_clk_divider{
  //div_0 = 0,
  div_2,
  div_4,
  div_8,
  div_16,
  div_32,
  div_64,
  div_128,
  div_256,
};
/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

void SPI_init(uint32_t periph_num, uint8_t mode, uint8_t div);
uint8_t SPI1_transmit(uint8_t periph_num, uint8_t data);

#endif /* __SPI_H */