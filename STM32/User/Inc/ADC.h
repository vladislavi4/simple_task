#ifndef __ADC_H
/* Private defines -----------------------------------------------------------*/
#define __ADC_H

/* Includes ------------------------------------------------------------------*/
#include "project_config.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

typedef struct{
  uint8_t amount;
  union
  {
    uint32_t raw;
    struct{
      uint32_t ch_1     : 4;
      uint32_t ch_2     : 4;
      uint32_t ch_3     : 4;
      uint32_t ch_4     : 4;
      uint32_t ch_5     : 4;
      uint32_t ch_6     : 4;
      uint32_t ch_7     : 4;
      uint32_t ch_8     : 4;
      uint32_t ch_9     : 4;
      uint32_t ch_10    : 4;
      uint32_t ch_11    : 4;
      uint32_t ch_12    : 4;
      uint32_t ch_13    : 4;
      uint32_t ch_14    : 4;
      uint32_t ch_15    : 4;
      uint32_t ch_16    : 4;
    };
  }number_conv;
}ADC_setup_struct_t;

typedef struct{
  union
  {
    uint16_t raw;
    struct{
      uint16_t data        : 12;
      uint16_t ch_num      : 4;
    }status;
  }channel[16];
}ADC_satus_struct_t;


void ADC_init(void);

#endif /* __ADC_H */