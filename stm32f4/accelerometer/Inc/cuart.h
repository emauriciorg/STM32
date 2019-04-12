#ifndef __cuart__h__
#define __cuart__h__

#include "stm32f4xx_hal.h"
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
 void uart_setup(void);
void cuart_parser(void);
#endif /*__cuart__h__*/
