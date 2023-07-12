#include "TIM.h"
#include "RCC.h"
#include "GPIO.h"


void TIMER_init(uint32_t periph_num, uint8_t mode, uint16_t arr, uint16_t psc)
{
  TIM_TypeDef* selected_tim;
  uint32_t tmp  = 0;
  uint32_t tmp1 = 0;
  RCC_Periph(periph_num, ENABLE);
  switch(periph_num)
  {
  case tim4:
    selected_tim = TIM4;
    if (mode & PWM_MODE_CH1){
      PIN_init(portD, 12,  GPIO_MODE_AF, GPIO_SPEED_FREQ_MEDIUM, GPIO_NOPULL);
      GPIOD->AFR[1] |= (2<<(4*4));
    }
    if (mode & PWM_MODE_CH2){
      PIN_init(portD, 13,  GPIO_MODE_AF, GPIO_SPEED_FREQ_MEDIUM, GPIO_NOPULL);
      GPIOD->AFR[1] |= (2<<(4*5));
    }
    if (mode & PWM_MODE_CH3){
      PIN_init(portD, 14,  GPIO_MODE_AF, GPIO_SPEED_FREQ_MEDIUM, GPIO_NOPULL);
      GPIOD->AFR[1] |= (2<<(4*6));
    }
    if (mode & PWM_MODE_CH4){
      PIN_init(portD, 15,  GPIO_MODE_AF, GPIO_SPEED_FREQ_MEDIUM, GPIO_NOPULL);
      GPIOD->AFR[1] |= (2<<(4*7));
    }
    break;
  }
  selected_tim->CCMR1 = 0;
  if (mode & PWM_MODE_CH1){
    tmp |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;  // pryamoy PWM
    tmp1 |= TIM_CCER_CC1E;
  }
  if (mode & PWM_MODE_CH2){
    tmp |=  TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;  // pryamoy PWM
    tmp1 |= TIM_CCER_CC2E;
  }
  selected_tim->CCMR1 = tmp;
  selected_tim->CCMR2 = 0;
  if (mode & PWM_MODE_CH3){
    tmp |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;  // pryamoy PWM
    tmp1 |= TIM_CCER_CC3E;
  }
  if (mode & PWM_MODE_CH4){
    tmp |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4PE;  // pryamoy PWM
    tmp1 |= TIM_CCER_CC4E;
  }
  selected_tim->CCMR2 |= tmp;
  selected_tim->CCER |= tmp1;
  selected_tim->PSC = psc;//168-1;
  selected_tim->ARR = arr;
  selected_tim->CR1 |= TIM_CR1_CEN;
}

/*
GPIOD->AFR[1] |= (2<<(4*4)) + (2<<(4*5)) + (2<<(4*6)) + (2<<(4*7));
  TIM4->CR1 = 0;
  TIM4->CCER = 0;
  tmp = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE | TIM_CCMR1_OC1PE;  // pryamoy PWM
  TIM4->CCMR1 = tmp;
  //TIM4->CCMR1 = TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;		// pryamoy PWM
  //TIM4->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;		// pryamoy PWM
  tmp = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC3PE | TIM_CCMR2_OC4PE;  // pryamoy PWM
  TIM4->CCMR2 = tmp;
  tmp = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
  TIM4->CCER = tmp;
  TIM4->ARR = 100;
  TIM4->CCR1 = 10;
  TIM4->CCR2 = 10;
  TIM4->CCR3 = 10;
  TIM4->CCR4 = 10;
*/


