#ifndef __CTIMER_H_
#define __CTIMER_H_


#include <stm32f0xx_hal.h>
#include <stdint.h>


#define ONE_SECOND_IN_MILLIS 1000
#define ONE_MINUTE_IN_SECONDS 60


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
uint8_t ctimer_get_tick(void);


#endif /*__CTIMER_H_*/
