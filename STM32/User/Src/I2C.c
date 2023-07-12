#include "I2C.h"
#include "RCC.h"
#include "GPIO.h"


void I2C_init(uint8_t periph_num, uint8_t mode, uint8_t speed)
{
  I2C_TypeDef* selected_i2c;
  //uint32_t tmp = 0;
  
  RCC_Periph(periph_num, ENABLE);
  switch(periph_num)
  {
  case i2c1:
    //pin init
    PIN_init(portB, 6, GPIO_MODE_AF_OD, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    PIN_init(portB, 7, GPIO_MODE_AF_OD, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);
    GPIOB->AFR[1] |= (4<<0) | (4<<4);  // Bits (3:2:1:0) = 0:1:0:0 --> AF4 for pin PB8;  Bits (7:6:5:4) = 0:1:0:0 --> AF4 for pin PB9
    
    selected_i2c = I2C1;
    break;
  case i2c2://I2C2:
    break;
  case i2c3://I2C3:
    //pin init
    PIN_init(portA, 8, GPIO_MODE_AF_OD, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);//scl
    PIN_init(portC, 9, GPIO_MODE_AF_OD, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_PULLUP);//sda
    GPIOA->AFR[1] |= (4<<0);  // Bits (3:2:1:0) = 0:1:0:0 --> AF4 for pin PB8;  Bits (7:6:5:4) = 0:1:0:0 --> AF4 for pin PB9
    GPIOC->AFR[1] |= (4<<4);
    
    selected_i2c = I2C3;
    break;
  }
  //tmp = 0;
  //I2C init:
  // Reset the I2C
  selected_i2c->CR1 |= I2C_CR1_SWRST;
  selected_i2c->CR1 &= ~I2C_CR1_SWRST;
  
  selected_i2c->CR1 &= ~I2C_CR1_SMBUS;
  
  //????????? ??????? ???????????? ??????
  selected_i2c->CR2 &= ~I2C_CR2_FREQ;
  selected_i2c->CR2 |= 42; // Fclk1=168/4=42MHz 
  
  //????????????? I2C, standart mode, 100 KHz duty cycle 1/2	
  selected_i2c->CCR &= ~(I2C_CCR_FS | I2C_CCR_DUTY);
  //?????? ??????? ?????? ?????? SCL ?? ??????? 10 000nS/(2* TPCLK1) 
  selected_i2c->CCR |= 208; //10 000ns/48ns = 208
  
  //Standart_Mode = 1000nS, Fast_Mode = 300nS, 1/42MHz = 24nS
  selected_i2c->TRISE = 42; //(1000nS/24nS)+1
  
  //???????? ??????
  selected_i2c->CR1 |= I2C_CR1_PE;
}

void I2C_Write(uint8_t periph_num, uint8_t slave_addr, uint8_t reg_addr, uint8_t* data, uint8_t size)
{
  I2C_TypeDef* selected_i2c;
  uint8_t cnt = 0;
  switch(periph_num)
  {
  case i2c1:
    selected_i2c = I2C1;
    break;
  case i2c3:
    selected_i2c = I2C3;
    break;
  }
  //????????
  selected_i2c->CR1 |= I2C_CR1_START;		
  while(!(selected_i2c->SR1 & I2C_SR1_SB)){};
  (void) selected_i2c->SR1;
  
  //???????? ????? ??????????
  selected_i2c->DR = slave_addr;
  while(!(selected_i2c->SR1 & I2C_SR1_ADDR)){};
  (void) selected_i2c->SR1;
  (void) selected_i2c->SR2;
  
  //???????? ????? ????????
  selected_i2c->DR = reg_addr;	
  while(!(selected_i2c->SR1 & I2C_SR1_TXE)){};	
  
  //????? ??????
  while(size)//
  {//
    selected_i2c->DR = data[cnt];// ([cnt])
    //while(!(selected_i2c->SR1 & I2C_SR1_TXE)){};//
    while(!(selected_i2c->SR1 & I2C_SR1_BTF)){};
    cnt++;//
    size--;//
  }//
  //while(!(selected_i2c->SR1 & I2C_SR1_BTF)){};//
  selected_i2c->CR1 |= I2C_CR1_STOP;		
}

static void I2C_ReadByte(uint8_t periph_num, uint8_t slave_addr, uint8_t reg_addr, uint8_t* buff, uint8_t size)
{
  I2C_TypeDef* selected_i2c;
  uint8_t cnt = 0;
  switch(periph_num)
  {
  case i2c1:
    selected_i2c = I2C1;
    break;
  case i2c3:
    selected_i2c = I2C3;
    break;
  }
  //uint8_t data;
  //стартуем
  selected_i2c->CR1 |= I2C_CR1_START;
  while(!(selected_i2c->SR1 & I2C_SR1_SB)){};
  (void) selected_i2c->SR1;
  
  //передаем адрес устройства	
  selected_i2c->DR = slave_addr;//I2C_ADDRESS(ADR,I2C_MODE_WRITE)
  while(!(selected_i2c->SR1 & I2C_SR1_ADDR)){};
  (void) selected_i2c->SR1;
  (void) selected_i2c->SR2;
  
  //передаем адрес регистра
  selected_i2c->DR = reg_addr;
  while(!(selected_i2c->SR1 & I2C_SR1_TXE)){};	
  selected_i2c->CR1 |= I2C_CR1_STOP;
  
  //рестарт!!!
  selected_i2c->CR1 |= I2C_CR1_START;
  while(!(selected_i2c->SR1 & I2C_SR1_SB)){};
  (void) selected_i2c->SR1;
  
  //передаем адрес устройства, но теперь для чтения
  selected_i2c->DR = slave_addr | 1;//I2C_ADDRESS(ADR,I2C_MODE_READ)
  while(!(selected_i2c->SR1 & I2C_SR1_ADDR)){};
  (void) selected_i2c->SR1;
  (void) selected_i2c->SR2;
  
  //читаем	
  //selected_i2c->CR1 &= ~I2C_CR1_ACK;
  //while(!(selected_i2c->SR1 & I2C_SR1_RXNE)){};
  //data = selected_i2c->DR;	
  //selected_i2c->CR1 |= I2C_CR1_STOP;
  
  while(size)//
  {//
    selected_i2c->CR1 &= ~I2C_CR1_ACK;
    while(!(selected_i2c->SR1 & I2C_SR1_RXNE)){};
    buff[cnt] = selected_i2c->DR;
    cnt++;//
    size--;//
    if(size>1){
      selected_i2c->CR1 |= I2C_CR1_ACK;
    }
  }//
  selected_i2c->CR1 |= I2C_CR1_STOP;
} 	


void I2C_Read(uint8_t periph_num, uint8_t slave_addr, uint8_t reg_addr, uint8_t* buff, uint8_t size){
  for(uint8_t i = 0; i<size; i++){
    I2C_ReadByte(periph_num, slave_addr, reg_addr++, &buff[i], 1);
  }
}

