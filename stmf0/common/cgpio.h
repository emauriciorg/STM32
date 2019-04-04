#ifndef __CGPIO__H
#define __CGPIO__H
#include "stm32f0xx_hal.h"

#define LED_GREEN_TOGGLE() GPIOC->ODR ^= GPIO_PIN_9
#define LED_RED_TOGGLE() GPIOC->ODR ^= GPIO_PIN_8


#endif /*__CGPIO__H*/
