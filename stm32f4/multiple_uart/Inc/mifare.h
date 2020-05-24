#ifndef __MIFARE_H__
#define __MIFARE_H__

#include "stm32f4xx_hal.h"
void mifare_irq_handler(UART_HandleTypeDef *huart);

void mifare_handler(void);
void mifare_init(void);
void mifare_choose_mode(uint16_t mode);
void mifare_sample_period_change( uint32_t period_in_ms );


void mifare_rf_field(uint8_t field);
void mifare_read_single_board(void);

#endif /*__MIFARE_H__*/
