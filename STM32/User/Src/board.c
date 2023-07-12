
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//#define dumy 0xFF
/*typedef struct{
    uint8_t head[2];
    uint8_t msg_type;
    uint8_t stop_byte;
}request_t;

typedef struct{
    uint8_t head[2];
    uint8_t msg_type;
    uint8_t data[8];
    uint8_t stop_byte;
}ansv_t;

typedef struct{
    uint8_t head[2];
    uint8_t msg_type;
    union{
      uint8_t raw[8];
      struct{
        uint8_t led_status;
        uint16_t temp;
        uint16_t humi;
        uint16_t pres;
        //uint8_t reserv;
      }data;
    };
    uint8_t stop_byte;
}protocol_t;
*/
typedef struct{
    uint8_t head[2];
    uint8_t msg_type;
    uint8_t data[8];
    uint8_t stop_byte;
}protocol_t;


/* Externed variables --------------------------------------------------------*/
extern uint8_t cnt_r;
extern uint8_t uart_rx[32];
extern uart_it_flag_t uart_it_flag[7];

/* Private variables ---------------------------------------------------------*/
static void GPIO_init(void);

task_struct_t reload_task_time = {0};
task_struct_t test_task_time = {0};
ADC_satus_struct_t ADC_satus = {0};
ADC_setup_struct_t ADC_setup_struct = {0};

uint16_t        ADC_buff[4] = {0};

uint8_t l_cnt = 0;
uint16_t led_br[5] = {0};
uint8_t led_mode[5] = {0};

uint16_t  humi = 0;
uint16_t  temp = 0;
uint16_t pres = 0;

uint8_t i2c_sec = 0;
uint8_t i2c_addr = 0;


//request_t   request = {0};
//ansv_t      ansver = {.head[0] = 0xAA, .head[1] = 0x55, .stop_byte = 0x55};
protocol_t      protocol = {.head[0] = 0xAA, .head[1] = 0x55, .stop_byte = 0x55};
protocol_t      msg  = {0};



void hardware_init(void){
  SystemClock_Config(HSE, _168_MHz);
  GPIO_init();
  UART_init(uart3, 1, 9600);
  BME_init();
  
  test_task_time.delay = 100;
  reload_task_time.delay = 30000;
  
}

void board_cycle(void){
  while(1){
    if(uart_it_flag[3].idle)
      {
        if(uart_rx[0] == 0xAA && uart_rx[1] == 0x55){
          memcpy(&msg, uart_rx, 12);
          if(msg.msg_type == command){
            if(msg.data[0] == led_on)
            {
              GPIOD->ODR |= GPIO_ODR_ODR_12;
              protocol.data[0] = led_on;
            }
            else if(msg.data[0] == led_off)
            {
              GPIOD->ODR &= ~GPIO_ODR_ODR_12;
              protocol.data[0] = led_off;
            }
            else
            {
              sprintf((char*)protocol.data, "ok");
            }
            protocol.msg_type = ack;
          }
          if(msg.msg_type == request_rd){
            if((uint8_t)(GPIOA->IDR & 1))
            {
              protocol.data[0] = button_pressed;
            }else
            {
              protocol.data[0] = button_not_pressed;
            }
            protocol.data[1] = (uint8_t)temp;
            protocol.data[2] = (uint8_t)(temp>>8);
            protocol.data[3] = (uint8_t)humi;
            protocol.data[4] = (uint8_t)(humi>>8);
            protocol.data[5] = (uint8_t)pres;
            protocol.data[6] = (uint8_t)(pres>>8);
            protocol.msg_type = request_rd;
          }
          UART_Send_data(uart3, (uint8_t*)&protocol, sizeof(protocol));
          memset(&protocol.data, 0, 8);
        }
        //memset(&msg, 0, sizeof(msg));
        memset(uart_rx, 0, sizeof(uart_rx));
        cnt_r = 0;
        uart_it_flag[3].idle = 0;
      }
    if (system_tick - test_task_time.counter >= test_task_time.delay){
      temp = BME280_getTemperature(-1);
      humi = BME280_getHumidity(-1);
      pres = (uint16_t)BME280_getPressure();
      test_task_time.counter = system_tick;
    }
    if((uint8_t)(GPIOA->IDR & 1)){
      GPIOD->ODR |= GPIO_ODR_ODR_14;
    }
    else{
      GPIOD->ODR &= ~GPIO_ODR_ODR_14;
    }
    if (system_tick - reload_task_time.counter >= reload_task_time.delay){
      //BME_reload();
      //BME_init();
      reload_task_time.counter = system_tick;
    }
  }
}

void GPIO_init(void)
{
  PIN_init(portA, 0, GPIO_MODE_INPUT, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL);
  PIN_init(portD, 12, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL);
  //PIN_init(portD, 13, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL);
  PIN_init(portD, 14, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL);
  //PIN_init(portD, 15, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL);
}
