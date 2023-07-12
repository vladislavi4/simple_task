#ifndef __RCC_H
/* Private defines -----------------------------------------------------------*/
#define __RCC_H

//#define DISABLE 0
//#define ENABLE  1

/* Includes ------------------------------------------------------------------*/
#include "project_config.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/


enum clk_src_type_t
{
  HSI = 0,
  HSE
};

enum freq_t
{
  _4_MHz = 0,
  _8_MHz,
  _16_MHz,
  _48_MHz,
  _72_MHz,
  _84_MHz,
  _92_MHz,
  _168_MHz
};

enum periph_type
{
  NONE = 0,
  
  tim1,
  tim2,
  tim3,
  tim4,
  tim5,
  tim6,
  tim7,
  tim8,
  tim9,
  tim10,
  tim11,
  tim12,
  tim13,
  tim14,
  iwdg,
  wwdg,
  rtc,
  
  portA,
  portB,
  portC,
  portD,
  portE,
  portF,
  portG,
  portH,
  
  adc1,
  adc2,
  adc3,
  
  i2c1,
  i2c2,
  i2c3,
  
  uart1,
  uart2,
  uart3,
  uart4,
  uart5,
  uart6,
  
  spi1,
  spi2,
  spi3,
  
  dma1,
  dma2,
  
  syscfg,
};


typedef struct{
  _Bool ADC1_clk_en;
  _Bool ADC2_clk_en;
  _Bool ADC3_clk_en;
}ADC_clk_t;

typedef struct{
  _Bool PORT_A_clk_en;
  _Bool PORT_B_clk_en;
  _Bool PORT_C_clk_en;
  _Bool PORT_D_clk_en;
  _Bool PORT_E_clk_en;
  _Bool PORT_F_clk_en;
  _Bool PORT_G_clk_en;
  _Bool PORT_H_clk_en;
}PORT_clk_t;

typedef struct{
  _Bool SPI1_clk_en;
  _Bool SPI2_clk_en;
  _Bool SPI3_clk_en;
  _Bool SPI4_clk_en;
  _Bool SPI5_clk_en;
  _Bool SPI6_clk_en;
}SPI_clk_t;

typedef struct{
  _Bool USART1_clk_en;
  _Bool USART2_clk_en;
  _Bool USART3_clk_en;
  _Bool UART4_clk_en;
  _Bool UART5_clk_en;
  _Bool USART6_clk_en;
  _Bool UART9_clk_en;
  _Bool UART10_clk_en;
}USART_clk_t;

typedef struct{
  _Bool I2C1_clk_en;
  _Bool I2C2_clk_en;
  _Bool I2C3_clk_en;
}I2C_clk_t;

typedef struct{
  _Bool TIM1_clk_en;
  _Bool TIM4_clk_en;
  _Bool TIM8_clk_en;
}TIM_clk_t;

typedef struct{
  _Bool DMA1_clk_en;
  _Bool DMA2_clk_en;
}DMA_clk_t;

typedef struct{
  _Bool SYSCFG_clk_en;
}NVIC_clk_t;


typedef struct{
  ADC_clk_t ADC_clk_status;
  PORT_clk_t PORT_clk_status;
  SPI_clk_t SPI_clk_status;
  USART_clk_t USART_clk_status;
  I2C_clk_t I2C_clk_status;
  TIM_clk_t TIM_clk_status;
  DMA_clk_t DMA_clk_status;
  NVIC_clk_t NVIC_clk_status;
}Clock_struct_t;

void SystemClock_Config(uint8_t clock_type, uint8_t freq);
void RCC_Periph(uint32_t periph, _Bool state);

#endif /* __RCC_H */



















/*enum periph_type
{
  NONE = 0,
//==============================      APB2      ==============================//
  Clock_TIM1,
  Clock_TIM8,
  Clock_USART1,
  Clock_USART6,
  Clock_UART9,
  Clock_UART10,
  Clock_ADC1,
  Clock_ADC2,
  Clock_ADC3,
  Clock_SDIO,
  Clock_SPI1,
  Clock_SPI4,
  Clock_SYSCFG,
  Clock_EXTI,
  Clock_TIM9,
  Clock_TIM10,
  Clock_TIM11,
  Clock_SPI5,
  Clock_SPI6,
  Clock_SAI1,
//==============================      APB2      ==============================//
//==============================      APB1      ==============================//
  Clock_TIM2,
  Clock_TIM3,
  Clock_TIM4,
  Clock_TIM5,
  Clock_TIM6,
  Clock_TIM7,
  Clock_TIM12,
  Clock_TIM13,
  Clock_TIM14,
  Clock_WWDG,
  Clock_SPI2,
  Clock_SPI3,
  
  Clock_USART2,
  Clock_USART3,
  Clock_UART4,
  Clock_UART5,
  
  Clock_I2C1,
  Clock_I2C2,
  Clock_I2C3,
  
  Clock_GPIOA,
  Clock_GPIOB,
  Clock_GPIOC,
  Clock_GPIOD,
  Clock_GPIOE,
  Clock_GPIOF,
  Clock_GPIOG,
  Clock_GPIOH
//==============================      APB1      ==============================//
};*/