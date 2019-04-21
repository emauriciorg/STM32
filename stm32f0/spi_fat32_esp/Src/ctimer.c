#include "ctimer.h"
#include "cgpio.h"





extern TIM_HandleTypeDef htim6;

static uint32_t seconds_counter = 5;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == TIM6) {
		if (! (seconds_counter-- ) ) {
			LED_GREEN_TOGGLE();
			seconds_counter = ONE_SECOND_IN_MILLIS;
		}
	}
}


uint8_t ctimer_get_tick(void)
{
	if (seconds_counter == 0) {
		seconds_counter = 5;
		return seconds_counter;
	}

	return !seconds_counter;
}
