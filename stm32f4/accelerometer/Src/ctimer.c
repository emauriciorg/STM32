#include "ctimer.h"
#include "cgpio.h"



extern TIM_HandleTypeDef htim9;
uint32_t seconds_counter = 1;

uint8_t ctimer_second_counter_check(void)
{
		if(seconds_counter == 0){
			seconds_counter = 5;
			return 1;
		}
		return 0;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){


	if (htim->Instance == TIM7){
		TOGGLE_RED_LED();
		if (seconds_counter ) seconds_counter--;
	}
	if (htim->Instance == TIM6) {
		HAL_IncTick();
	}
}



void ctimer_update_pwm(uint32_t pwm_frequency, uint32_t pwm_duty_cycle, uint8_t pwm_channel)
{

	if (pwm_channel)
		TIM9->CCR1 = pwm_duty_cycle; //Duty cycle register for TIM9, Channel 1
	else
		TIM9->CCR2 = pwm_duty_cycle; //Duty cycle register for TIM9, Channel 2

	if(pwm_frequency)
		htim9.Init.Period = pwm_frequency;
}
