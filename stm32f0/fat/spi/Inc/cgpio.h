#ifndef __CGPIO__H
#define __CGPIO__H
#include "stm32f0xx_hal.h"

#ifdef DISCOVERY_BOARD
	#define LED_GREEN_TOGGLE() GPIOC->ODR ^= GPIO_PIN_9
	#define LED_BLUE_TOGGLE() GPIOC->ODR ^= GPIO_PIN_8
#else
	#define LED_GREEN_TOGGLE() GPIOC->ODR ^= GPIO_PIN_9
	#define LED_BLUE_TOGGLE() GPIOC->ODR ^= GPIO_PIN_8
	#define LED_RED_TOGGLE() GPIOA->ODR ^= GPIO_PIN_5
#endif
#endif /*__CGPIO__H*/
