#ifndef __BOARD_H

/* Private defines -----------------------------------------------------------*/
#define __BOARD_H


/* Includes ------------------------------------------------------------------*/
#include "project_config.h"
#include "ADC.h"
#include "GPIO.h"
#include "I2C.h"
#include "Interrupt.h"
#include "UART.h"
#include "SPI.h"
#include "TIM.h"
#include "RCC.h"

//#include "LIS302DL.h"
#include "BME280.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct{
  uint32_t delay;
  uint32_t counter;
}task_struct_t;
/* Exported constants --------------------------------------------------------*/
enum MsgType_t{
  command = 1,
  request_rd,
  request_wr,
  ack,
};
enum command_t{
  led_on = 1,
  led_off,
};
enum request_rd_t{
  button_pressed = 1,
  button_not_pressed,
  temperature,
  humidity,
  pressure
};

enum request_wr_t{
  reg1 = 1,
  reg2,
  reg3,
};
/* Exported functions prototypes ---------------------------------------------*/
void hardware_init(void);
void board_cycle(void);


#endif  /* __BOARD_H  */