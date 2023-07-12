#ifndef __TIMER_H
/* Private defines -----------------------------------------------------------*/
#define __TIMER_H

#define  OUTPUT_COMPARE_NO_OUTPUT               0x00000000u     /*!< Input Floating Mode                */
#define  OUTPUT_COMPARE_CH_X                    0x00000001u     /*!< Output Push Pull Mode              */
#define  PWM_MODE_CH1                           0x00000002u     /*!< Alternate Function Push Pull Mode  */
#define  PWM_MODE_CH2                           0x00000004u     /*!< Alternate Function Push Pull Mode  */
#define  PWM_MODE_CH3                           0x00000008u     /*!< Alternate Function Push Pull Mode  */
#define  PWM_MODE_CH4                           0x00000016u     /*!< Alternate Function Push Pull Mode  */

/* Includes ------------------------------------------------------------------*/
#include "project_config.h"
/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

void TIMER_init(uint32_t periph_num, uint8_t mode, uint16_t arr, uint16_t psc);

#endif /* __TIMER_H */