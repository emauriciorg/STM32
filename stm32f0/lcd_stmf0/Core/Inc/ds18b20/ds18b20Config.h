#ifndef	_DS18B20CONFIG_H
#define	_DS18B20CONFIG_H
#include "stm32f0xx_hal.h"

//	Init timer on cube    1us per tick				example 72 MHz cpu >>> Prescaler=(72-1)      counter period=Max 
//###################################################################################
#define _DS18B20_MAX_SENSORS		    				1
#define	_DS18B20_GPIO												GPIOA
#define	_DS18B20_PIN												GPIO_PIN_6

#define	_DS18B20_CONVERT_TIMEOUT_MS					1000		



#define	_DS18B20_TIMER											htim3						
//###################################################################################

#endif


