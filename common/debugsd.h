#ifndef DEBUG_SD_H
#define DEBUG_SD_H

#include "stm32f4xx_hal.h"

#define UART_RX_BUFFER_SIZE 1024


typedef struct{
	int len;
	char data[UART_RX_BUFFER_SIZE];
} st_str;


#define SDBG(...)  uart_tx.len= sprintf(uart_tx.data,__VA_ARGS__);\
		   dbg_message(uart_tx.data,uart_tx.len);\

#define SDBG1(...)  uart_tx.len= sprintf(uart_tx.data,__VA_ARGS__);\
		   dbg_message(uart_tx.data,uart_tx.len);\

#define SDBG2(...)  uart_tx.len= sprintf(uart_tx.data,__VA_ARGS__);\
		   dbg_message(uart_tx.data,uart_tx.len);\

char dbg_get_command(char *command);

void dbg_set_port(void);
void dbg_get_string(void);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void print(uint8_t *message);
void dbg_message(char  *tx_data, int len);


void dbg_command_scan(void);

void dbg_uart_parser(char uart_command);
void dbg_clear_packet(void);


#endif
