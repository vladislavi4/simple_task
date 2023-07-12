/* Includes ------------------------------------------------------------------*/
#include "Interrupt.h"
/* Private variables ---------------------------------------------------------*/
uint32_t system_tick = 0;
uart_it_flag_t uart_it_flag[7] = {0};

/* Exported variables --------------------------------------------------------*/

extern uint8_t uart_rx[32];
extern uint8_t uart_tx[32];
extern uint8_t cnt_r;


void SysTick_Handler (void)
{
  system_tick++;
}
//==============================        UART     ===============================
void USART1_IRQHandler(void) {
  if (USART1->SR & USART_CR1_RXNEIE) { /*следим за состо€нием даннго флага 1 Ч данные пришли, 0- пусто*/
    uart_rx[cnt_r++] = USART1->DR;
    USART1->SR &= ~USART_SR_RXNE;
  }
  if(USART1->SR & USART_SR_IDLE){
    (void)USART1->SR;
    (void)USART1->DR;
    uart_it_flag[1].idle = 1;
  }
}
void USART2_IRQHandler(void) {
  if (USART2->SR & USART_CR1_RXNEIE) { /*следим за состо€нием даннго флага 1 Ч данные пришли, 0- пусто*/
    uart_rx[cnt_r++] = USART2->DR;
    USART2->SR &= ~USART_SR_RXNE;
  }
  if(USART2->SR & USART_SR_IDLE){
    (void)USART2->SR;
    (void)USART2->DR;
    uart_it_flag[2].idle = 1;
  }
}
void USART3_IRQHandler(void) {
  if (USART3->SR & USART_CR1_RXNEIE) { /*следим за состо€нием даннго флага 1 Ч данные пришли, 0- пусто*/
    uart_rx[cnt_r++] = USART3->DR;
    USART3->SR &= ~USART_SR_RXNE;
  }
  if(USART3->SR & USART_SR_IDLE){
    (void)USART3->SR;
    (void)USART3->DR;
    uart_it_flag[3].idle = 1;
  }
}
void USART4_IRQHandler(void) {
  if (UART4->SR & USART_CR1_RXNEIE) { /*следим за состо€нием даннго флага 1 Ч данные пришли, 0- пусто*/
    uart_rx[cnt_r++] = UART4->DR;
    UART4->SR &= ~USART_SR_RXNE;
  }
  if(UART4->SR & USART_SR_IDLE){
    (void)UART4->SR;
    (void)UART4->DR;
    uart_it_flag[4].idle = 1;
  }
}

void EXTI0_IRQHandler(void){
  EXTI->PR |= 1 << 0;
}

