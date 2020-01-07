#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "dial_encoder.h"
#include "stm32f0xx_hal.h"

#define DIAL_A_PIN     GPIO_PIN_0
#define DIAL_B_PIN     GPIO_PIN_1
#define DIAL_ENTER_PIN GPIO_PIN_2


#define DIAL_A_CODE 01
#define DIAL_B_CODE 10

enum dial_directioon{
	DIAL_LEFT  = 0X01,
	DIAL_RIGHT = 0X10
};
struct dial_st{
	uint8_t direction;
	uint8_t dial_first;
	uint8_t  dial_a_pulse;
	uint8_t  dial_b_pulse;

	uint8_t steps;
	uint8_t enter_pressed;
	uint8_t enter_released;
	uint8_t changed_event;
	uint16_t steps_left;
	uint16_t steps_right;
}dial;

/*to do , only rising edge*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == DIAL_ENTER_PIN){
		dial.enter_pressed  = true;
		dial.enter_released = false;
		dial.changed_event  = true;
	}

	if (GPIO_Pin == DIAL_A_PIN){
		dial.dial_a_pulse  = true;
		dial.changed_event = true;
	}
#if 0
	if (GPIO_Pin == DIAL_B_PIN) {
		dial.dial_b_pulse  = true;
		dial.changed_event = true;
	}
#endif

	if(dial.dial_a_pulse && DIAL_B_LEVEL){
		dial.direction = DIAL_LEFT;
		dial.steps_right = 0;
		dial.steps_left++;
	}
	else{
		dial.direction = DIAL_RIGHT;
		dial.steps_right++;
		dial.steps_left = 0;
	}

	dial.dial_a_pulse = 0;
	dial.dial_b_pulse = 0;

	// src 0001 ,A: 0010, then B 1000
	// src 0001 , B: 0100, then 1000
}

void dial_step_count(void){

}


void dial_encoder_init(void)
{

}



void dial_encoder_service(void)
{
	if (!dial.changed_event) {return;}

	if (dial.direction == DIAL_LEFT){
		printf("Lsteps %d",dial.steps_left);
	}
	if (dial.direction == DIAL_RIGHT){
		printf("Rsteps %d",dial.steps_right);

	}
	if(dial.dial_a_pulse) {
		printf("pulse A\r\n");
		dial.dial_a_pulse = false;
	}

	if (dial.dial_b_pulse) {
		printf("pulse B\r\n");
		dial.dial_b_pulse = false;
	}

	if (dial.enter_pressed){
		printf("pulse ENTER\r\n");
		dial.enter_pressed = false;
	}
}
