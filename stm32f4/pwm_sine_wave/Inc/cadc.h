#ifndef __CADC_H_
#define __CADC_H_
#include "stm32f4xx_hal.h"

void cadc_poll_reading(void);
uint32_t cadc_get_adc_value(uint8_t channel);

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);

uint8_t cadc_convertion_done(void);

void cadc_setup_adc(void);


#endif /*__CADC_H_*/
