#include "UART.h"
#include "RCC.h"
#include "GPIO.h"

uint8_t uart_rx[32] = {0};
uint8_t uart_tx[32] = {0};
uint8_t cnt_r = 0;
uint8_t cnt_t = 0;

void UART_init(uint32_t periph_num, uint8_t mode, uint32_t baud)
{
  USART_TypeDef* selected_uart;
  uint32_t tmp = 0;
  RCC_Periph(periph_num, ENABLE);
  switch(periph_num)
  {
  case uart1:
    RCC_Periph(portA, ENABLE);
    PIN_init(portA, 9,  GPIO_MODE_AF, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    PIN_init(portA, 10, GPIO_MODE_AF, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    //GPIOA->AFR[1] |= (4<<0) | (4<<4);
    GPIOA->AFR[1] |= (7<<4) + (7<<8);
    selected_uart = USART1;
    tmp = ((SystemCoreClock / 2) + (baud / 2)) / baud;
    selected_uart->BRR = tmp;
    NVIC_EnableIRQ(USART1_IRQn);
    break;
    
  case uart2:
    RCC_Periph(portA, ENABLE);
    PIN_init(portA, 2, GPIO_MODE_AF, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    PIN_init(portA, 3, GPIO_MODE_AF, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    GPIOA->AFR[0] |= (7<<(4*2)) + (7<<(4*3));
    selected_uart = USART2;
    tmp = ((SystemCoreClock / 4) + (baud / 2)) / baud;
    selected_uart->BRR = tmp;
    NVIC_EnableIRQ(USART2_IRQn);
    break;
  case uart3:
    RCC_Periph(portB, ENABLE);
    PIN_init(portB, 10, GPIO_MODE_AF, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    PIN_init(portB, 11, GPIO_MODE_AF, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    GPIOB->AFR[1] |= (7<<(4*2)) + (7<<(4*3));
    selected_uart = USART3;
    tmp = ((SystemCoreClock / 4) + (baud / 2)) / baud;
    selected_uart->BRR = tmp;
    NVIC_EnableIRQ(USART3_IRQn);
    break;
  case uart4:
    RCC_Periph(portB, ENABLE);
    PIN_init(portA, 0, GPIO_MODE_AF, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    PIN_init(portA, 1, GPIO_MODE_AF, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    GPIOB->AFR[1] |= (7<<(4*0)) + (7<<(4*1));
    selected_uart = UART4;
    tmp = ((SystemCoreClock / 4) + (baud / 2)) / baud;
    selected_uart->BRR = tmp;
    NVIC_EnableIRQ(UART4_IRQn);
    break;
  }
  tmp = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_IDLEIE;
  selected_uart->CR1 = tmp;
  selected_uart->CR1 |= USART_CR1_UE;
}


void UART_Send (uint32_t periph_num, char chr){
  
  USART_TypeDef* selected_uart;
  
  switch(periph_num)
  {
  case uart1:
    selected_uart = USART1;
    break;
  case uart2:
    selected_uart = USART2;
    break;
  case uart3:
    selected_uart = USART3;
    break;
  }
  while (!(selected_uart->SR & USART_SR_TC)); /*Сравниваем регистр статуса с разрядом установки флага об окончании прерывания*/
  selected_uart->DR = chr;
}

void UART_Send_String (uint32_t periph_num, char* str)
{
	uint16_t i = 0;
	while(str[i])
          UART_Send (periph_num, str[i++]);
}

void UART_Send_data (uint32_t periph_num, uint8_t* data, uint16_t len)
{
	for(uint16_t i = 0; i < len; i++)
          UART_Send (periph_num, data[i]);
}