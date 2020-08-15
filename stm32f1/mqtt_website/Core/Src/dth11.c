#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "dth11.h"
#include "stm32f1xx_hal.h"

uint8_t  rx_buffer[50];
uint8_t  data_available = 0;
uint16_t temperature;
uint16_t humidity;

extern TIM_HandleTypeDef htim2;

#define PAYLOAD_SIZE 40
#define DTH11_PORT GPIOB
#define DTH11_PIN  GPIO_PIN_0

void dth11_init(void)
{
	DTH11_PORT->ODR |=DTH11_PIN;
}



void dth11_listener(void){
	if (!data_available) return;

	printf("temperature  is %x, humidity is %x\r\n",temperature, humidity);
}

void change_pin_dir(uint32_t pin_mode);

void dth11_read(void)
{
/*
Data format: 8bit integral RH data || 8bit decimal RH data || 8bit integral T data || 8bit decimal T
data + 8bit check sum.
*/
	uint16_t local_temp = 0;
	uint16_t local_hum  = 0;
	uint16_t local_crc  = 0;

	uint8_t read_bits = 0;
	uint32_t micro_seconds = 0;
	uint32_t  timeout_temp = HAL_GetTick() + 20;
	uint8_t bit_value = 0;

	uint8_t temperature;
	uint8_t humidity;
	/*start signal*/
	//printf("reading dth11\r\n");

	DTH11_PORT->CRL = (DTH11_PORT->CRL & 0XFFFFFFF0)| 0X3; /*OUTMODE PP*/



	DTH11_PORT->ODR &=~DTH11_PIN;
	HAL_Delay(20);

	DTH11_PORT->CRL = (DTH11_PORT->CRL & 0XFFFFFFF0)| 0X4; /*INPUT PP*/


	/*port as in input mode & wait for sensor reply*/

	timeout_temp = HAL_GetTick() + 10;

	while (DTH11_PORT->IDR & DTH11_PIN){
		if (timeout_temp < HAL_GetTick() ) {
			printf("Sensor reply timeout_temp  \r\n");
			return;
		};
	};
	timeout_temp = HAL_GetTick() +10;

	while ((DTH11_PORT->IDR & DTH11_PIN) == 0){
		if (timeout_temp < HAL_GetTick() ) {
			printf("Sensor reply timeout_temp2  \r\n");
			return;
		};

	};
	timeout_temp = HAL_GetTick() +10;

	while ((DTH11_PORT->IDR & DTH11_PIN)){
		if (timeout_temp < HAL_GetTick() ) {
			printf("Sensor reply timeout_temp2  \r\n");
			return;
		};

	};

	timeout_temp = HAL_GetTick() + 20;

	/*read stage*/

	uint8_t check_sum_sw = 0;
	while(read_bits < 40)
	{
		while((DTH11_PORT->IDR &DTH11_PIN) == 0){}; /*50us or bit start */
		TIM2->CNT = 0;
		HAL_TIM_Base_Start(&htim2);
		while((DTH11_PORT->IDR &DTH11_PIN) == 1){} /*reading the actual bit*/
		micro_seconds = TIM2->CNT; /*gets bit time length*/
		__HAL_TIM_CLEAR_FLAG(&htim2, TIM_IT_UPDATE);
		HAL_TIM_Base_Stop(&htim2);

		if (micro_seconds > 40){
			bit_value= 1;
		}else{
			bit_value =0;
		}

		if (read_bits <  16) {
			local_hum = (local_hum << 1) |  bit_value;
		}else
		if (read_bits > 15  && read_bits < 32) {
			local_temp = (local_temp << 1) | bit_value;

		}else
		if (read_bits > 31  && read_bits < 40){
			local_crc = (local_crc << 1) |bit_value;
		}else{
			printf("out of range \r\n");
		}
		read_bits++;
		if (timeout_temp < HAL_GetTick()) break;
	}
	check_sum_sw = ((local_temp&0xff00) >>8) + (local_temp&0x00ff)+
	               ((local_hum &0xff00) >>8) + (local_hum&0x00ff);

#ifdef HUMAN_OUTPUT
	printf("temp %d . %d \r\n",  (local_temp&0xff00) >>8, local_temp&0x00ff);
	printf("hum %d . %d  \r\n",(local_hum & 0xff00)>>8 , local_hum&0x00ff);
	printf("crc %d  %d   \r\n",local_crc, check_sum_sw);
#else
	temperature = (local_temp&0xff00) >>8;
	humidity    = (local_hum & 0xff00)>>8;
	printf("#%d,%d*\r\n",temperature,humidity);
#endif


}



