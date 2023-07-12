#ifndef __GPIO_H
/* Private defines -----------------------------------------------------------*/
#define __GPIO_H

/* Includes ------------------------------------------------------------------*/
#include "project_config.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/



#define  GPIO_MODE_INPUT                        0x00000000u     /*!< Input Floating Mode                */
#define  GPIO_MODE_OUTPUT_PP                    0x00000001u     /*!< Output Push Pull Mode              */
#define  GPIO_MODE_AF                           0x00000002u     /*!< Alternate Function Push Pull Mode  */
#define  GPIO_MODE_ANALOG                       0x00000003u     /*!< Analog Mode                        */
  
#define  GPIO_MODE_OUTPUT_OD                    0x00000004u     /*!< Output Open Drain Mode             */
#define  GPIO_MODE_AF_OD                        0x00000005u     /*!< Alternate Function Open Drain Mode */
  
#define  GPIO_NOPULL                            0x00000000u     /*!< No Pull-up or Pull-down activation */
#define  GPIO_PULLUP                            0x00000001u     /*!< Pull-up activation                 */
#define  GPIO_PULLDOWN                          0x00000002u     /*!< Pull-down activation               */

#define GPIO_SPEED_FREQ_LOW                     0u              /*!< Low speed                          */
#define GPIO_SPEED_FREQ_MEDIUM                  1u              /*!< Medium speed                       */
#define GPIO_SPEED_FREQ_HIGH                    2u              /*!< High speed                         */
#define GPIO_SPEED_FREQ_VERY_HIGH               3u              /*!< Very High speed                    */

#define  FALLING_TRIGGER_MODE                   0x00000000u     /*!< Output Open Drain Mode             */
#define  RISING_TRIGGER_MODE                    0x00000001u     /*!< Alternate Function Open Drain Mode */

typedef struct{ 
  uint8_t port;
  union
  {
    uint8_t raw;
    struct{
      uint8_t mode        : 2;
      uint8_t type        : 1;
      uint8_t Pup_Pdwn    : 2;
      uint8_t speed       : 2;
      uint8_t reserved    : 1;
    }settings;
  }pin[16];
}GPIO_satus_struct_t;

void PIN_init(uint8_t port, uint8_t pin, uint8_t mode, uint8_t speed, uint8_t pu_pd);
void GPIO_EXTI(uint8_t port, uint8_t pin, _Bool trig_mode, uint8_t pu_pd);


#endif /* __GPIO_H */