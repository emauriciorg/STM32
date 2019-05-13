#include <stdint.h>
#include <stdlib.h>

#include "max7219.h"
#include "max7219_cmd.h"


#include "stm32f0xx_hal.h"

#define P_SPI &hspi1

extern SPI_HandleTypeDef hspi1;
#define max7219_CS_LOW  GPIOA->ODR &=~GPIO_PIN_6
#define max7219_CS_HIGH GPIOA->ODR |= GPIO_PIN_6
void max7219_send_cmd(uint8_t cmd, uint8_t data)
{
	uint8_t frame_out [2];
	frame_out[0] = cmd;
	frame_out[1] = data;

	max7219_CS_LOW;
	HAL_SPI_Transmit(P_SPI, frame_out, 2, 100);
	max7219_CS_HIGH;
}
void max7219_init(void)
{
	/*normal mode*/
	max7219_CS_HIGH;
	HAL_Delay(500);

	max7219_send_cmd(CMD_SHUTDOWN, 0X01);


	/*decode mode applied to all 7 digits: only d0-d3 are needed*/
#ifdef MAX7218_SEVEN_DISPLAY_MODE
	max7219_send_cmd(CMD_DECODE_MODE, 0XFF);
#else /*8x8 dot matrix*/
	max7219_send_cmd(CMD_DECODE_MODE, 0X00);
#endif
	/*intensity  duty cycle 5/32 */
	max7219_send_cmd (CMD_INTENSITY, 0);
	/*scan limit: only digits  0-2 */
	max7219_send_cmd (CMD_SCAN_LIMIT, 0X07);

	/*test off*/
	max7219_send_cmd (CMD_DISPLAY_TEST , 0X00);

	max7219_send_cmd (CMD_DIGIT_0 , 0X00);
	max7219_send_cmd (CMD_DIGIT_1 , 0X00);
	max7219_send_cmd (CMD_DIGIT_2 , 0X00);
	max7219_send_cmd (CMD_DIGIT_3 , 0X00);
	max7219_send_cmd (CMD_DIGIT_4 , 0X00);
	max7219_send_cmd (CMD_DIGIT_5 , 0X00);
	max7219_send_cmd (CMD_DIGIT_6 , 0X00);
	max7219_send_cmd (CMD_DIGIT_7 , 0X00);



	/*shows 3 in digit 0*/
	max7219_send_cmd (CMD_DIGIT_0 , 0X03);

	/*shows 1 in digit 1*/
	max7219_send_cmd (CMD_DIGIT_1 , 0X01);

	/*shows 4 in digit 2*/
	max7219_send_cmd (CMD_DIGIT_2 , 0X02);


	max7219_send_cmd (CMD_DIGIT_3 , 0X00);

	max7219_send_cmd (CMD_DIGIT_4 , 0X00);
	max7219_send_cmd (CMD_DIGIT_5 , 0X00);
	max7219_send_cmd (CMD_DIGIT_6 , 0X00);
	max7219_send_cmd (CMD_DIGIT_7 , 0X00);
	srand(50);

}

void max7219_display_number(uint16_t number,uint8_t display)
{
	uint8_t units = 0;
	uint8_t dec   = 0;
	uint8_t cen   = 0;
	uint8_t mil   = 0;

	units = (number  %10);
	dec   = ((number %100))/10;
	cen   = ((number %1000)/100);
	mil   = ((number %10000)/1000);
	/*shows 3 in digit 0*/
	max7219_send_cmd (CMD_DIGIT_0 , units);
	max7219_send_cmd (CMD_DIGIT_1 , dec);
	max7219_send_cmd (CMD_DIGIT_2 , cen);
	max7219_send_cmd (CMD_DIGIT_3 , mil);
}


void show_seconds(void)
{
	static uint32_t seconds = 0;
	static uint16_t display_seconds = 0;
	if ( HAL_GetTick() >seconds){
		seconds = HAL_GetTick() +1000;
		max7219_display_number(display_seconds++,0);
	}

}
void show_animation(void)
{
	static uint32_t seconds = 0;
	uint8_t dot_animation    = 0;


	if ( HAL_GetTick() >seconds){
		seconds = HAL_GetTick() + 800;

		dot_animation = rand()%0xff;
		max7219_send_cmd (CMD_DIGIT_0 , dot_animation );
		dot_animation = rand()%0xff;
		max7219_send_cmd (CMD_DIGIT_1 , dot_animation );
		dot_animation = rand()%0xff;
		max7219_send_cmd (CMD_DIGIT_2 , dot_animation );
		dot_animation = rand()%0xff;
		max7219_send_cmd (CMD_DIGIT_3 , dot_animation );
		dot_animation = rand()%0xff;
		max7219_send_cmd (CMD_DIGIT_4 , dot_animation );
		dot_animation = rand()%0xff;
		max7219_send_cmd (CMD_DIGIT_5 , dot_animation );
		dot_animation = rand()%0xff;
		max7219_send_cmd (CMD_DIGIT_6 , dot_animation );
		dot_animation = rand()%0xff;
		max7219_send_cmd (CMD_DIGIT_7 , dot_animation );

	}

}
