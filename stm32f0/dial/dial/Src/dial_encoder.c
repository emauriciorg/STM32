#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "dial_encoder.h"
#include "stm32f0xx_hal.h"

#define DIAL_A_PIN       GPIO_PIN_2
#define DIAL_B_PIN       GPIO_PIN_10
#define DIAL_ENTER_PIN   GPIO_PIN_0

#define DIAL_B_PORT      GPIOB
#define DIAL_A_PORT      GPIOB
#define DIAL_B_LEVEL     DIAL_B_PORT->IDR & DIAL_B_PIN
#define DIAL_A_LEVEL     DIAL_A_PORT->IDR & DIAL_A_PIN

/*app specific macro, used to see a full turn or control the level of something
lets say sound volume, see  dial.steps*/
#define DIAL_STEP_LIMIT 100

enum dial_directioon{
	DIAL_IDLE  = 0X00;
	DIAL_ENTER  = 0X01,
	DIAL_LEFT  = 0X10,
	DIAL_RIGHT = 0X11
};


volatile struct dial_st {
	uint8_t direction;

	uint8_t steps;
	uint16_t steps_left;
	uint16_t steps_right;

	uint8_t enter_pressed;
	uint8_t enter_released;
	uint8_t changed_event;

}dial;

/*to do , only rising edge*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
	HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
	HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);

	if (GPIO_Pin == DIAL_ENTER_PIN) {
		dial.enter_pressed  = true;
		dial.enter_released = false;
		dial.changed_event  = true;
		dial.direction      = DIAL_ENTER;
	}

/*TODO: implement debounce circuit, meanwhile use this digital debounce*/
#ifdef SOFT_DEBOUNCER
	uint32_t delay_isr = 240000;
	while (delay_isr) delay_isr--;
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
#endif /*SOFT_DEBOUNCER*/

	uint8_t dial_level_a = !!DIAL_A_LEVEL;
	uint8_t dial_level_b = !!DIAL_B_LEVEL;

 	if (GPIO_Pin == DIAL_A_PIN){
		dir.rising  = DIAL_LEFT;
		dir.falling = DIAL_RIGHT;
	}

	if (GPIO_Pin == DIAL_B_PIN) {
		dir.rising  = DIAL_RIGHT;
		dir.falling = DIAL_LEFT;
	}

	dial_direction_check_a(dial_level_b , dial_level_a ,dir.rising  , &dial.direction);
	dial_direction_check_a(!dial_level_b, dial_level_a ,dir.falling , &dial.direction);
	dial_step_count(&dial);

	HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

	dial.changed_event = true;
}


void dial_direction_check_a(uint8_t signal_a, uint8_t signal_b, uint8_t r_true, uint8_t *direction)
{
	if ( (signal_a && signal_b) || ((!signal_a) && (!signal_b)) ) {
		*direction = r_true;
	}
}

void dial_step_count(struct dial_st &ptr_dial)
{
	if(ptr_dial->direction == DIAL_RIGHT){
		ptr_dial->steps_left++;
		if (ptr_dial->steps < DIAL_STEP_LIMIT) ptr_dial->steps++;
		if (ptr_dial->steps_right) ptr_dial->steps_right--;
	}else{
		ptr_dial->steps_right++;
		if (ptr_dial->steps_left)  ptr_dial->steps_left--;
		if (ptr_dial->steps) ptr_dial->steps--;
	}
}


void dial_encoder_init(void)
{

}

uint8_t dial_get_direction(void)
{
	return dial.direction;
}

uint8_t dial_get_button_state(void)
{
	return dial.enter_pressed;
}

uint8_t dial_reset_steps(void)
{
	dial.steps_left  = 0;
	dial.steps_right = 0;
}

void dial_encoder_service(void)
{
	if (!dial.changed_event) {return;}

	dial.changed_event = 0;

/*This way when enter is pressed we got the last direcction and steps
if we don't desired such a behavior, we can clean the direction value in the isr
or here , as soon as we read the steps and address*/
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
