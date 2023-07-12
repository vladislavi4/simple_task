#include "ADC.h"
#include "GPIO.h"
#include "RCC.h"

extern uint16_t ADC_buff[4];

void ADC_init(void)
{
  uint32_t tmp = 0; 
  RCC_Periph(adc1, ENABLE);
  PIN_init(portA, 0, GPIO_MODE_ANALOG, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL);
  PIN_init(portA, 1, GPIO_MODE_ANALOG, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL);
  PIN_init(portA, 2, GPIO_MODE_ANALOG, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL);
  PIN_init(portA, 3, GPIO_MODE_ANALOG, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL);
      //несколько каналов прерывать сканирование на4м канале    прерывать цепочку каналов сканирования        num of chann 4                        
  tmp = ADC_CR1_SCAN; //ADC_CR1_DISCNUM_1 | ADC_CR1_DISCNUM_0 | ADC_CR1_DISCEN | ADC_CR1_SCAN;
  ADC1->CR1 = tmp;
  
  ADC1->CR2 = 0;
  ADC1->CR2 |= ADC_CR2_EOCS;
  
  //100: 84 cycles
  tmp = ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP2_2 | ADC_SMPR2_SMP3_2;
  ADC1->SMPR2 =  tmp;
  
  //общее кол-тво каналов в scan цепочке 4
   tmp = ADC_SQR1_L_1 | ADC_SQR1_L_0;
   ADC1->SQR1 = tmp;
  
  // 1ch = 4th,  2ch = 3rd,  2ch = 2nd, 4ch = 1st
  tmp = (0<<(3*5)) + (1<<(2*5)) + (2<<5) + 3;
  ADC1->SQR3 =  tmp;
  
  //====================================        DMA     ========================
  
   RCC->AHB1ENR    |= RCC_AHB1ENR_DMA2EN;
        DMA2_Stream4->CR&=~ DMA_SxCR_CHSEL;            // 000: channel 0 selected
        DMA2_Stream4->PAR= (uint32_t)&ADC1->DR;     //
        DMA2_Stream4->M0AR=(uint32_t)&ADC_buff[0];        // Массив
        DMA2_Stream4->NDTR=100;                        // Длина буфера
        DMA2_Stream4->CR|= DMA_SxCR_MINC;            // Инкремент адреса
        DMA2_Stream4->CR|= DMA_SxCR_MSIZE_0;        // 16 бит
        DMA2_Stream4->CR|= DMA_SxCR_PSIZE_0;        // 16 бит
        DMA2_Stream4->CR|= DMA_SxCR_CIRC;            // Непрерывный режим
        DMA2_Stream4->CR&=~ DMA_SxCR_DIR;            // 01: peripheral-to-Memory
        DMA2_Stream4->CR|= DMA_SxCR_PL;                // 11: Very high приоритет
        DMA2_Stream4->CR|= DMA_SxCR_TCIE;            // Transfer complete interrupt enable
        DMA2_Stream4->CR|= DMA_SxCR_EN;                 // Вкл. передачу
        NVIC_EnableIRQ(DMA2_Stream4_IRQn);
        NVIC_SetPriority(DMA2_Stream4_IRQn,5);


  
  //============================================================================
  
  ADC1->CR2|=ADC_CR2_ADON;              //enable ADC    
  //ADC1->CR2|=ADC_CR2_SWSTART; 
}

 /*
void ADC_convert(void)
{
  //ADC1->CR2|=ADC_CR2_SWSTART;
  
  while (!(ADC1->SR & ADC_SR_EOC)); //ждем пока первое преобразование завершится
  ADC_satus.channel[0].status.data = ADC1->DR;
  ADC1->CR2|=ADC_CR2_SWSTART;
  while (!(ADC1->SR & ADC_SR_EOC)); //ждем пока первое преобразование завершится
  ADC_satus.channel[1].status.data = ADC1->DR;
  ADC1->CR2|=ADC_CR2_SWSTART;
  while (!(ADC1->SR & ADC_SR_EOC)); //ждем пока первое преобразование завершится
  ADC_satus.channel[2].status.data = ADC1->DR;
  ADC1->CR2|=ADC_CR2_SWSTART;
  while (!(ADC1->SR & ADC_SR_EOC)); //ждем пока первое преобразование завершится
  ADC_satus.channel[3].status.data = ADC1->DR;
  
}
*/