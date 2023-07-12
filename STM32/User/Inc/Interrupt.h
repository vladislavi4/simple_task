#ifndef __Interrupt_H
/* Private defines -----------------------------------------------------------*/
#define __Interrupt_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef union{
  uint8_t all_flag;
  struct{
    uint8_t idle :1;
    uint8_t rx :1;
    uint8_t tx :1;
  };
}uart_it_flag_t;

//typedef struct{}uart_it_t
/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern uint32_t system_tick;


#endif /* __Interrupt_H */