#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "dt11.h"

uint8_t  rx_buffer[50];
uint8_t  data_available = 0;
uint16_t temperature;
uint16_t humidity;
#define PAYLOAD_SIZE 40
void dth11_init(void)
{
	GPIOC->ODR &=~GPIO_PIN_0;
}


void dth11_listener(void){
	if (!data_available) return;

	print("temperature  is %x, humidity is %x\r\n",temperature, humidity);
}


void dth11_read(void)
{

	uint8_t i;
	for (i = 0; i < PAYLOAD_SIZE; i++){


	}

}
