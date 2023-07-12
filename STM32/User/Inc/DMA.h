#ifndef __DMA_H
/* Private defines -----------------------------------------------------------*/
#define __DMA_H

/* Includes ------------------------------------------------------------------*/
#include "project_config.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
enum dma_chan{
  channel_0 = 0,
  channel_1,
  channel_2,
  channel_3,
  channel_4,
  channel_5,
  channel_6,
  channel_7,
};
/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

void DMA_init(uint32_t periph_num, uint8_t chan_num);

#endif /* __DMA_H */