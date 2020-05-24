#ifndef __cuart__h__
#define __cuart__h__

#include "stm32f4xx_hal.h"
/*HAL WEAK functions */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) ;

/*Custom functions*/
void dbg_init(void);
void cuart_parser(void);

void dbg_command_scan(void);

uint8_t dbg_register_task(void (*task_routine)(void), uint8_t *task_command, uint8_t args);

#endif /*__cuart__h__*/
