#ifndef __ctimer__h_
#define __ctimer__h_
#include "stm32f4xx_hal.h"
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void ctimer_update_pwm(uint32_t pwm_frequency, uint32_t pwm_duty_cycle, uint8_t pwm_channel);
uint8_t ctimer_second_counter_check(void);
#endif /* __ctimer__h_ */
