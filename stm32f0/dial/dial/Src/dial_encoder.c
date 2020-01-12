#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "dial_encoder.h"
#include "stm32f0xx_hal.h"

#define DIAL_A_PIN     GPIO_PIN_0
#define DIAL_B_PIN     GPIO_PIN_1
#define DIAL_ENTER_PIN GPIO_PIN_2
#define DIAL_B_PORT    GPIOB
#define DIAL_A_PORT    GPIOB
#define DIAL_B_LEVEL DIAL_B_PORT->IDR & DIAL_B_PIN
#define DIAL_A_LEVEL DIAL_A_PORT->IDR & DIAL_A_PIN

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
		return;
	}
	uint32_t dial_level_a =DIAL_A_LEVEL;
	uint32_t dial_level_b =DIAL_B_LEVEL;

	if(GPIO_Pin == DIAL_B_PIN){
		return;
	}


 	if (GPIO_Pin == DIAL_A_PIN){
		dial.dial_a_pulse  = true;
	}

	if (GPIO_Pin == DIAL_B_PIN) {
		dial.dial_a_pulse = 0;
		dial.dial_b_pulse = 0;
		return;
		dial.dial_b_pulse  = true;
		dial.changed_event = true;
	}

	if(dial.dial_a_pulse &&
	((DIAL_B_LEVEL && dial_level_a) ||(!DIAL_B_LEVEL && !dial_level_a))){
		dial.direction = DIAL_LEFT;
		if (dial.steps_right) dial.steps_right--;
		dial.steps_left++;
	}else{
		dial.direction = DIAL_RIGHT;
		dial.steps_right++;
		if (dial.steps_left)  dial.steps_left--;
	}
	if(dial.dial_b_pulse &&
	((DIAL_B_LEVEL && dial_level_a) ||(!DIAL_B_LEVEL && !dial_level_a))){

	dial.changed_event = true;
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

	dial.changed_event = 0;

	if (dial.direction == DIAL_LEFT){
		printf("Lsteps %d\r\n",dial.steps_left);
	}
	if (dial.direction == DIAL_RIGHT){
		printf("Rsteps %d\r\n",dial.steps_right);

	}
	if (dial.enter_pressed){
		printf("pulse ENTER\r\n");
		dial.enter_pressed = false;
	}
}
