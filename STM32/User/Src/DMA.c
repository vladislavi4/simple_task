#include "DMA.h"
#include "RCC.h"
#include "GPIO.h"

void DMA_init(uint32_t periph_num, uint8_t chan_num)
{
  //uint32_t tmp = 0;
  
  if (periph_num == dma2)
  {
    RCC_Periph(periph_num, ENABLE);
   switch(chan_num)
   {
    case channel_0:
        DMA2_Stream4->CR&=~ DMA_SxCR_CHSEL;            // 000: channel 0 selected
        DMA2_Stream4->PAR= (uint32_t)&ADC1->DR;     //
        //DMA2_Stream4->M0AR=(uint32_t)&ADC_buff[0];        // Массив
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
    break;
   }
  }
}
