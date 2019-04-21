#include "cgpio.h"
#include "stm32f0xx.h"


typedef struct{
	uint32_t      counter;
	uint32_t      period;
	GPIO_TypeDef* port;
	uint16_t      pin;
}blinky_led_t;

blinky_led_t  heart_beat = {0, 1000, GPIOC,GPIO_PIN_8 };

void blink_led (void)
{
	if (heart_beat.counter < HAL_GetTick ()) {
		heart_beat.counter = HAL_GetTick() + heart_beat.period;
		heart_beat.port->ODR ^=heart_beat.pin;
	}
}
