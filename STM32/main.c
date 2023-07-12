#include "main.h"
#include "board.h"

void main(void)
{
  hardware_init();
  board_cycle();
  while(1){
  }
}

void Error_Handler(void)
{
  //__disable_irq();
  while (1){}
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
