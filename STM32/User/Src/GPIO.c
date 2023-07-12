#include "GPIO.h"
#include "RCC.h"

void PIN_init(uint8_t port, uint8_t pin, uint8_t mode, uint8_t speed, uint8_t pu_pd)
{
  GPIO_TypeDef* selected_port;
  RCC_Periph(port, ENABLE);
  switch(port)
  {
    case portA:
      selected_port = GPIOA;
    break;
    case portB:
    selected_port = GPIOB;
    break;
    case portC:
    selected_port = GPIOC;
    break;
    case portD:
      selected_port = GPIOD;
    break;
    case portE:
    selected_port = GPIOE;
    break;
    case portF:
    selected_port = GPIOF;
    break;
    case portG:
      selected_port = GPIOG;
    break;
    case portH:
    selected_port = GPIOH;
    break;
  }
  selected_port->MODER &= ~(3 << (pin * 2));
  if (mode < 4){
    selected_port->MODER |= mode << (pin * 2);
  }
  else{
    if (mode == GPIO_MODE_AF_OD){
      selected_port->MODER |= 2 << (pin * 2);
    }
    else{
      selected_port->MODER |= 1 << (pin * 2);
    }
    selected_port->OTYPER |= (1 << pin);
  }
  selected_port->OSPEEDR &= ~(3 << (pin * 2));
  selected_port->OSPEEDR |= speed << (pin * 2);
  
  selected_port->PUPDR &= ~(3 << (pin * 2));
  selected_port->PUPDR |= pu_pd << (pin * 2);
}

void GPIO_EXTI(uint8_t port, uint8_t pin, _Bool trig_mode, uint8_t pu_pd){
  uint32_t tmp = 0;
  GPIO_TypeDef* selected_port;
  RCC_Periph(port, ENABLE);
  RCC_Periph(syscfg, ENABLE);
  switch(port)
  {
    case portA:
      selected_port = GPIOA;
      tmp = 0;
    break;
    case portB:
      selected_port = GPIOB;
      tmp = 1;
    break;
    case portC:
      selected_port = GPIOC;
      tmp = 2;
    case portD:
      selected_port = GPIOD;
      tmp = 3;
    break;
    case portE:
      selected_port = GPIOE;
      tmp = 4;
    break;
    case portF:
      selected_port = GPIOF;
      tmp = 5;
    break;
    case portG:
      selected_port = GPIOG;
      tmp = 6;
    break;
    case portH:
      selected_port = GPIOH;
      tmp = 7;
    break;
  }
  selected_port->MODER &= ~(3 << (pin * 2));
  //Set up EXTI
  if (pin > 11){
    SYSCFG->EXTICR[3] &= ~(15 << ((pin - 12) * 4));
    SYSCFG->EXTICR[3] |= (tmp << ((pin - 12) * 4));
  }
  if (pin > 7){
    SYSCFG->EXTICR[2] &= ~(15 << ((pin - 8) * 4));
    SYSCFG->EXTICR[2] |= (tmp << ((pin - 8) * 4));
  }
  if (pin > 3){
    SYSCFG->EXTICR[1] &= ~(15 << ((pin - 4) * 4));
    SYSCFG->EXTICR[1] |= (tmp << ((pin - 4) * 4));
  }
  else{
    SYSCFG->EXTICR[0] &= ~(15 << (pin * 4));
    SYSCFG->EXTICR[0] |= (tmp << (pin * 4));
  }
  if(trig_mode){
    EXTI->RTSR &= ~(1 << pin);
    EXTI->RTSR |= (1 << pin);
  }
  else{
    EXTI->FTSR &= ~(1 << pin);
    EXTI->FTSR |= (1 << pin);
  }
  EXTI->IMR |= (1 << pin);
  if(pin >= 10){
    NVIC_EnableIRQ(EXTI15_10_IRQn);
  }
  if(pin >= 5 && pin < 10){
    NVIC_EnableIRQ(EXTI9_5_IRQn);
  }
  if(pin < 5){
    NVIC_EnableIRQ((IRQn_Type)(EXTI0_IRQn+pin));
  }
}
