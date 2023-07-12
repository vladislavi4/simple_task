#include "RCC.h"

Clock_struct_t Clock_struct = {0};

void SystemClock_Config(uint8_t clock_type, uint8_t freq)
{
  
  SystemInit();  
  
  //quarz
  RCC->CR |= (RCC_CR_HSEON);                    //Enable HSE
  while( !(RCC->CR & RCC_CR_HSERDY) ) {}        //ready to start HSE
  
  //FLASH
  FLASH->ACR |= FLASH_ACR_LATENCY | FLASH_ACR_PRFTEN| FLASH_ACR_ICEN| FLASH_ACR_DCEN;
  
  //PLL - HSE
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; //set HSE as PLL source
  RCC->CR &= ~(RCC_CR_PLLON);                   //disable PLL before changes
  
  //PLL M
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);          //clear all PLLM bits
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_3;           //PLLM = 8 (1000)
  
  //PLL P
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);          //main PLL division PLLP = 2: 00
  
  //PLL N
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);           //clear all PLLN bits with mask
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_3;           //set PLLN = 168 (1010 1000)
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_5;           //for 8 or 16 MHz HSE
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_7;           //
  
  //PLL Q 7 (0111)
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLQ);
  RCC->PLLCFGR |= (RCC_PLLCFGR_PLLQ_0 | RCC_PLLCFGR_PLLQ_2 | RCC_PLLCFGR_PLLQ_2);
  
  //AHB Prescaler
  RCC->CFGR &= ~(RCC_CFGR_HPRE);                //Prescaler 1
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;              //AHB = SYSCLK/1
  
  //APB1 Prescaler 8
  RCC->CFGR &= ~(RCC_CFGR_PPRE1);
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
  
  //APB2 Prescaler 2
  RCC->CFGR &= ~(RCC_CFGR_PPRE2);
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
  
  //PLL enable
  RCC->CR |= RCC_CR_PLLON;                      //enalbe PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0) {}      //wait for PLL is ready
  
  //PLL System
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;                 //PLL selected as system clock
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}      //wait for PLL is used
  
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock/1000000);// = 1 us
  //SysTick_Config(SystemCoreClock/1000);// = 1 ms
}

void RCC_Periph(uint32_t periph, _Bool state)
{
  switch(periph)
  {
    //==============================        ADC     ================================
  case dma1:
    if (Clock_struct.DMA_clk_status.DMA1_clk_en != state)
    {
      Clock_struct.DMA_clk_status.DMA1_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_DMA1EN;
    }
    break;
   case dma2:
    if (Clock_struct.DMA_clk_status.DMA2_clk_en != state)
    {
      Clock_struct.DMA_clk_status.DMA2_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_DMA2EN;
    }
    break;
    
//==============================        ADC     ================================
  case adc1:
    if (Clock_struct.ADC_clk_status.ADC1_clk_en != state)
    {
      Clock_struct.ADC_clk_status.ADC1_clk_en = state;
      RCC->APB2ENR ^= RCC_APB2ENR_ADC1EN;
    }
    break;
    
//============================        SYSCFG     ===============================
    case syscfg:
    if (Clock_struct.NVIC_clk_status.SYSCFG_clk_en != state)
    {
      Clock_struct.NVIC_clk_status.SYSCFG_clk_en = state;
      RCC->APB2ENR ^= RCC_APB2ENR_SYSCFGEN;
    }
    break;
    
//==============================        I2C      ===============================
    case i2c1:
    if (Clock_struct.I2C_clk_status.I2C1_clk_en != state)
    {
      Clock_struct.I2C_clk_status.I2C1_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_I2C1EN;
    }
    break;
    case i2c2:
    if (Clock_struct.I2C_clk_status.I2C1_clk_en != state)
    {
      Clock_struct.I2C_clk_status.I2C2_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_I2C2EN;
    }
    break;
    case i2c3:
    if (Clock_struct.I2C_clk_status.I2C3_clk_en != state)
    {
      Clock_struct.I2C_clk_status.I2C3_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_I2C3EN;
    }
    break;
//==============================        SPI      ===============================
  case spi1:
    if (Clock_struct.SPI_clk_status.SPI1_clk_en != state)
    {
      Clock_struct.SPI_clk_status.SPI1_clk_en = state;
      RCC->APB2ENR ^= RCC_APB2ENR_SPI1EN;
    }
    break;
    case spi2:
    if (Clock_struct.SPI_clk_status.SPI2_clk_en != state)
    {
      Clock_struct.SPI_clk_status.SPI2_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_SPI2EN;
    }
    break;
    case spi3:
    if (Clock_struct.SPI_clk_status.SPI3_clk_en != state)
    {
      Clock_struct.SPI_clk_status.SPI3_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_SPI3EN;
    }
    break;
//==============================        UART     ===============================
  case uart1:
    if (Clock_struct.USART_clk_status.USART1_clk_en != state)
    {
      Clock_struct.USART_clk_status.USART1_clk_en = state;
      RCC->APB2ENR ^= RCC_APB2ENR_USART1EN;
    }
    break;
  case uart2:
    if (Clock_struct.USART_clk_status.USART2_clk_en != state)
    {
      Clock_struct.USART_clk_status.USART2_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_USART2EN;
    }
    break;
  case uart3:
    if (Clock_struct.USART_clk_status.USART3_clk_en != state)
    {
      Clock_struct.USART_clk_status.USART3_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_USART3EN;
    }
    break;
  case uart4:
    if (Clock_struct.USART_clk_status.UART4_clk_en != state)
    {
      Clock_struct.USART_clk_status.UART4_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_UART4EN;
    }
    break;
  case uart5:
    if (Clock_struct.USART_clk_status.UART5_clk_en != state)
    {
      Clock_struct.USART_clk_status.UART5_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_UART5EN;
    }
    break;
  case uart6:
    if (Clock_struct.USART_clk_status.USART6_clk_en != state)
    {
      Clock_struct.USART_clk_status.USART6_clk_en = state;
      RCC->APB2ENR ^= RCC_APB2ENR_USART6EN;
    }
    break;
//==============================        TIM     ================================
  case tim4:
    if (Clock_struct.TIM_clk_status.TIM4_clk_en != state)
    {
      Clock_struct.TIM_clk_status.TIM4_clk_en = state;
      RCC->APB1ENR ^= RCC_APB1ENR_TIM4EN;
    }
    break;
//==============================        GPIO     ===============================
  case portA:
    if (Clock_struct.PORT_clk_status.PORT_A_clk_en != state)
    {
      Clock_struct.PORT_clk_status.PORT_A_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_GPIOAEN;
    }
    break;
    
  case portB:
    if (Clock_struct.PORT_clk_status.PORT_B_clk_en != state)
    {
      Clock_struct.PORT_clk_status.PORT_B_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_GPIOBEN;
    }
    break;
  case portC:
    if (!(Clock_struct.PORT_clk_status.PORT_C_clk_en))
    {
      Clock_struct.PORT_clk_status.PORT_C_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_GPIOCEN;
    }
    break;
  case portD:
    if (!(Clock_struct.PORT_clk_status.PORT_D_clk_en))
    {
      Clock_struct.PORT_clk_status.PORT_D_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_GPIODEN;
    }
    break;
  case portE:
    if (!(Clock_struct.PORT_clk_status.PORT_E_clk_en))
    {
      Clock_struct.PORT_clk_status.PORT_E_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_GPIOEEN;
    }
    break;
  case portF:
    if (!(Clock_struct.PORT_clk_status.PORT_F_clk_en))
    {
      Clock_struct.PORT_clk_status.PORT_F_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_GPIOFEN;
    }
    break;
  case portG:
    if (!(Clock_struct.PORT_clk_status.PORT_G_clk_en))
    {
      Clock_struct.PORT_clk_status.PORT_G_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_GPIOGEN;
    }
    break;
  case portH:
    if (!(Clock_struct.PORT_clk_status.PORT_H_clk_en))
    {
      Clock_struct.PORT_clk_status.PORT_H_clk_en = state;
      RCC->AHB1ENR ^= RCC_AHB1ENR_GPIOHEN;
    }
    break;
    
  }
}
