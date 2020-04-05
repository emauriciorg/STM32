#if 1
#include <stdint.h>
#include <string.h>
#include <stdio.h>
//#include "stm32f1xx_hal.h"
// #include "ws2812b.h"

// #define TIMER_HANDLER htim3
// #define TIMER_HANDLER_PTR &htim3
// #define PWM_CHANNEL TIM_CHANNEL_4
// extern TIM_HandleTypeDef TIMER_HANDLER;




#define W1 52
#define W0 26
//	uint32_t uhTimerPeriod = (uint32_t)((SystemCoreClock / 800000) - 1); //to get
#define INVALID_DIGIT_CONVERTION 0xff
#define PWM_PERIOD  W0
#define BLINK_PERIOD_LED ((uint32_t)10)
//4 IS ZERO 8 IS 1

// uint8_t color_table[] = {
	//
// }
static uint32_t ascii_to_to_hex(uint8_t *stream_pointer, uint8_t convertion_type);
static uint32_t dbg_has_arguments(uint8_t *msg);
void set_pwm_entry_color(uint8_t color, uint16_t *color_ptr);

#define TOTAL_LEDS 9

uint16_t led_table[12][24] ={
	{
		W0,W0,W0,W0,W1,W1,W1,W1,
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W0,W0,W0,W0,W0,
	},
	{
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W0,W1,W1,W1,W1,
		W0,W0,W0,W0,W0,W0,W0,W0,
	},
	{
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W1,W1,W1,W1,W1,
	},
	{
		W0,W0,W0,W0,W1,W1,W1,W1,
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W0,W1,W1,W1,W1,
	},
	{
		W0,W0,W0,W0,W1,W1,W0,W1,
		W0,W1,W0,W0,W1,W0,W0,W0,
		W0,W0,W0,W1,W0,W1,W0,W0,
	},
	{
		W0,W0,W0,W1,W0,W0,W0,W0,
		W0,W0,W0,W1,W0,W0,W0,W0,
		W0,W0,W0,W1,W0,W0,W0,W0,
	},
	{
		W0,W0,W0,W1,W0,W0,W0,W0,
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W1,W0,W0,W0,W0,
	},
	{
		W0,W0,W0,W0,W1,W1,W1,W1,
		W0,W0,W1,W1,W0,W0,W0,W0,
		W0,W0,W0,W0,W1,W1,W1,W1,
	},
	{
		W0,W0,W0,W1,W0,W0,W0,W0,
		W0,W0,W0,W0,W1,W0,W0,W0,
		W0,W0,W0,W1,W0,W1,W1,W0,
	},
	{
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	{
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	{
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	}
};

void start_led_sequence(void)
{
	// HAL_TIM_Base_Start(TIMER_HANDLER_PTR);
	// HAL_TIM_PWM_Start_DMA(TIMER_HANDLER_PTR, PWM_CHANNEL, (uint32_t *)led_table, sizeof(led_table)/2);
}

void stop_led_sequence(void)
{

}
void set_color(uint32_t color_rgb, uint16_t led_id)
{
	if (led_id >= TOTAL_LEDS) return;
	uint8_t red_color   = (color_rgb & 0xff0000)>>16;
	uint8_t green_color = (color_rgb & 0x00ff00)>>8;
	uint8_t blue_color  = (color_rgb & 0x0000ff);
	set_pwm_entry_color(red_color  , &led_table[led_id][0]);
	set_pwm_entry_color(green_color, &led_table[led_id][8]);
	set_pwm_entry_color(blue_color , &led_table[led_id][16]);
}

void set_pwm_entry_color(uint8_t color, uint16_t *color_ptr)
{
	uint8_t bits;
	memset(color_ptr,0, 8);
	for(bits=0; bits<8;bits++){
		if (((color >>bits) & 0x01)){
			color_ptr[bits] = 1;
		}else{
			color_ptr[bits] = 0;
		}
	}

}


void led_set_color_ascii(char *ascii_color ,uint16_t led_id, uint32_t *color_out)
{
	uint32_t color_out_local;

	color_out_local = dbg_has_arguments((uint8_t *)ascii_color);
	set_color(color_out_local, led_id);
	if (color_out != 0){
		*color_out = color_out_local;
	}
}



static uint32_t dbg_has_arguments(uint8_t *msg)
{
	uint8_t *p_char = 0;
	//p_char = (uint8_t *)strchr((char *)msg,' ');
	//if (p_char){
		return  ascii_to_to_hex(msg, 'h');
	//}
	//return 0;
}


static uint8_t hexascii_to_hex( char  hex_char) {

	if ( hex_char <= 'F' && hex_char >= 'A') return (hex_char -'A')+10;

	if ( hex_char <= 'f' && hex_char >= 'a') return (hex_char -'a')+10;

	if ( hex_char <= '9' && hex_char >= '0') return (hex_char -'0');

	printf(" invalid arg\r\n");
	return INVALID_DIGIT_CONVERTION;
 }

 #define MAX_DIGITS                 6
static uint32_t ascii_to_to_hex(uint8_t *stream_pointer, uint8_t convertion_type)
{
	printf("converting \r\n");
	uint32_t hex_result        = 0;
	uint8_t stream_length      = 0;
	uint16_t converted_digit   = 1;
	uint16_t convertion_offset = 0x10;

	if (convertion_type != 'h')
		convertion_offset = 0xa;

	while((stream_length != MAX_DIGITS) &&  (*stream_pointer) ) {

		converted_digit = ( hexascii_to_hex(*stream_pointer));
		if (INVALID_DIGIT_CONVERTION == converted_digit) {
			printf(" invalid \r\n");
			break;
		}
		hex_result = (hex_result * convertion_offset) + converted_digit;
		printf("out [%x] \r\n",hex_result);
		stream_pointer++;
		stream_length++;
	}
	printf(" result is %04lx  ", hex_result);
	return hex_result;
}
void print_array(uint16_t *src, uint8_t size){
	uint8_t index;
	printf("[ ");
	for (index = 0;index< size;index++){
		if(index%8 == 0) printf("\r\n");
		printf(" %04x  ", src[index]);
	}
	printf(" ]\r\n");
}
int main(void){


	print_array(&led_table[0][0],24);

	led_set_color_ascii("bbccdd",0,0);
	printf("after convertion :\r\n");
	print_array(&led_table[0][0],24);
	return 0;

}
#endif
