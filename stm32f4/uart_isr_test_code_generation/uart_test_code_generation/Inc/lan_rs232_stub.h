#ifndef __LAN_RS232_STUB_H__
#define __LAN_RS232_STUB_H__

void lan_rs232_stub_init(void);
void lan_rs232_stub_scan(void);

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) ;

/*Custom functions*/
void cuart_parser(void);


uint8_t dbg_register_task(void (*task_routine)(void), uint8_t *task_command, uint8_t args);

#endif /*__LAN_RS232_STUB_H__*/
