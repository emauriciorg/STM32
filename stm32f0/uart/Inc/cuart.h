#ifndef __cuart__h__
#define __cuart__h__

//#include "stm32f4xx_hal.h"
/*HAL WEAK functions */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) ;

/*Custom functions*/
void dbg_setup(void);
void cuart_parser(void);

void dbg_command_scan(void);

#endif /*__cuart__h__*/
