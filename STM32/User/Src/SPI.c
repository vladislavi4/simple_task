#include "SPI.h"
#include "RCC.h"
#include "GPIO.h"

void SPI_init(uint32_t periph_num, uint8_t mode, uint8_t div)
{
  SPI_TypeDef* selected_spi;
  uint32_t tmp = 0;
  RCC_Periph(periph_num, ENABLE);
   switch(periph_num)
   {
    case spi1:
      selected_spi = SPI1;
      RCC_Periph(portA, ENABLE);
      PIN_init(portA, 4, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_HIGH, GPIO_NOPULL);
      //PIN_init(portE, 3, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_HIGH, GPIO_NOPULL);
      PIN_init(portA, 5, GPIO_MODE_AF, GPIO_SPEED_FREQ_HIGH, GPIO_NOPULL);
      PIN_init(portA, 6, GPIO_MODE_AF, GPIO_SPEED_FREQ_HIGH, GPIO_NOPULL);
      PIN_init(portA, 7, GPIO_MODE_AF, GPIO_SPEED_FREQ_HIGH, GPIO_NOPULL);

      GPIOA->AFR[0] |= (5<<(4*7)) + (5<<(4*6)) + (5<<(4*5));
      
      //tmp = speed;
      //SPI1-> = tmp;
      
      tmp = SPI_CR1_BR_2| SPI_CR1_BR_1 | /*SPI_CR1_CPOL |*/ SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;//baud f/32, cpol =1 (sck in 1 when idle), master mode, software slave select pin;
      selected_spi->CR1 = tmp;
    break;
    
   case spi2:
    break;
   }
   selected_spi->CR1 |= SPI_CR1_SPE;//spi enable
}


uint8_t SPI1_transmit(uint8_t periph_num, uint8_t data)
{
  SPI_TypeDef* selected_spi;
  switch(periph_num)
   {
    case spi1:
      selected_spi = SPI1;
    break;
   case spi2:
     selected_spi = SPI2;
    break;
   }
    //SPI1->DR = 0xFF;
    //(void)(SPI1->DR);
    while(!(selected_spi->SR & SPI_SR_TXE) || selected_spi->SR & SPI_SR_BSY){}
    selected_spi->DR = data;
    while(!(selected_spi->SR & SPI_SR_TXE) || selected_spi->SR & SPI_SR_BSY){}
    return selected_spi->DR;
}

/*8
void GPIO_init(void)
{
  RCC->AHB1ENR |=RCC_AHB1ENR_GPIODEN;  //start clock PORTD
  
  GPIOD->MODER |= GPIO_MODER_MODER15_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER12_0; // Pin 15, 14, 13, 12 - out
  GPIOD->OTYPER &= ~0xF000; // Pin 15, 14, 13, 12 - Output push-pull
  GPIOD->OSPEEDR &= ~0xFF000000; // Pin 15, 14, 13, 12 - Low speed
  
  //    PORT A
  RCC->AHB1ENR |=RCC_AHB1ENR_GPIOAEN;  //start clock PORTD
                  // PA 0-3 - Analog
  GPIOA->MODER |= GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2 | GPIO_MODER_MODER3 | GPIO_MODER_MODER4;
}
*/