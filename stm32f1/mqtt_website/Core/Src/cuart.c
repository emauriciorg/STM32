#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "cuart.h"

extern UART_HandleTypeDef huart1;

#define TRUE 1
#define FALSE 0
#define UART_DBG
#define UART_RX_BUFFER_SIZE      256
#define INVALID_DIGIT_CONVERTION 255
#define MAX_DIGITS                 5
#define MAX_PAYLOAD_LEN           32
#define UART_DBG_PORT        (&huart1)
#define DEBUG_TIMEOUT           1000
#define DEBUG_USART_INSTANCE USART1

#define UART_DEBUG(...) printf(__VA_ARGS__)

#ifndef STR_CMP
#define STR_CMP(buffer, string, len )  !memcmp(buffer, string, len)
#endif

static uint16_t ascii_to_to_hex(uint8_t *stream_pointer, uint8_t convertion_type);
static uint8_t hexascii_to_hex( char  hex_character);
static void dbg_uart_parser(uint8_t *msg);
static void dbg_clear_packet(void);

extern volatile uint32_t soft_watchdog ;


#define MAX_TASK 10

union function_t{
	void (*ptr_arg)(uint8_t *);
	void (*uint_args)(uint16_t);
	void (*void_args)(void);
	void (*basic)(void);
};


struct task_entry_st{
	union function_t handlers;
	uint8_t command[10];
	uint8_t command_len;
	uint8_t args;

};

struct task_pool_st{
	struct task_entry_st entry[MAX_TASK];
	uint8_t taken_task;
	/*const*/ uint8_t  limit;
}task_pool;

enum{
	false,
	true
};

volatile uint8_t Rx_data[1];

static struct {
	const uint8_t init_char;
	const uint8_t end_char;

	uint8_t
		complete:1,
		start:1;
	uint8_t rx_byte;

	uint8_t tail;
	uint8_t command;
	uint8_t data_out[UART_RX_BUFFER_SIZE];
	uint8_t parameters[UART_RX_BUFFER_SIZE];
	uint8_t data_in[UART_RX_BUFFER_SIZE];
	uint8_t end;
} protocol={'#','*', 0,0,0,0,0, {0},{0},{0},0};

#define MAX_COMMAND_LEN  10


uint8_t dbg_register_task(void (*task_routine)(void), char *task_command, uint8_t args)
{
	uint8_t	 task_command_len  = strlen((char *)task_command);

	 if (task_routine == NULL ) return FALSE;

	if (task_command == NULL) return FALSE;

	if (task_command_len > MAX_COMMAND_LEN) return FALSE;

	if ((task_pool.taken_task+1) >= task_pool.limit ) return FALSE;

	task_pool.entry[task_pool.taken_task].args            = args;
	task_pool.entry[task_pool.taken_task].handlers.basic  = task_routine;
	task_pool.entry[task_pool.taken_task].command_len     = task_command_len;
	memcpy (task_pool.entry[task_pool.taken_task].command  , task_command,task_command_len);

	task_pool.taken_task++;

	printf("Task %lx with cmd [%s] OK\r\n", (uint32_t)task_routine, task_command);
	return TRUE;

}

void dbg_store_packet(char recieved_data)
{
	if((recieved_data =='\r') || recieved_data =='\n')
		return;
	if (recieved_data == protocol.init_char) {

		protocol.tail     = false;
		protocol.complete = false;
		protocol.start    = true;
		return;
	}

	if ((recieved_data == protocol.end_char)  && protocol.start==true) {

		protocol.complete = true;
		protocol.start    = false;
	}

	protocol.data_in[ protocol.tail ] = recieved_data;
	protocol.tail++;

	if ( protocol.tail >= (UART_RX_BUFFER_SIZE-1))
		protocol.tail=false;
}

void dbg_command_scan(void)
{



	if (!protocol.complete) { return;}
	//printf("something recieved\r\n");
	protocol.complete	= false;
	protocol.data_in[protocol.tail+1] = '\0';
	protocol.command	= protocol.data_in[1];
	memcpy(protocol.parameters,  protocol.data_in,(protocol.tail-1));

	dbg_uart_parser( protocol.parameters) ;
	dbg_clear_packet();

}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == DEBUG_USART_INSTANCE) {
		dbg_store_packet(Rx_data[0]);

		__HAL_UART_CLEAR_OREFLAG(UART_DBG_PORT);

		HAL_UART_Receive_IT(huart, (uint8_t *)Rx_data, 1);
	}
}


static void dbg_clear_packet(void)
{
	protocol.tail		= false;
	protocol.start		= false;
	protocol.command	= false;
	protocol.end		= false;
	protocol.complete	= false;

	memset(protocol.parameters, false, UART_RX_BUFFER_SIZE);
	memset(protocol.data_in   , false, UART_RX_BUFFER_SIZE);
}


static uint16_t dbg_has_arguments(uint8_t *msg)
{
	uint8_t *p_char = 0;
	p_char = (uint8_t *) strchr((char *)msg,' ');
	if (p_char)
		return  ascii_to_to_hex(p_char+1, 'd');
	return INVALID_DIGIT_CONVERTION;
}


static uint8_t *get_arg_ptr(uint8_t *msg)
{
	uint8_t *p_char = 0;
	p_char = (uint8_t *) strchr((char *)msg,' ');
	if (p_char)
		return  (p_char+1);
	return 0;
}

static uint8_t hexascii_to_hex( char  hex_char) {

 	if ( hex_char <= 'F' && hex_char >= 'A') return (hex_char -'A')+10;

 	if ( hex_char <= 'f' && hex_char >= 'a') return (hex_char -'a')+10;

 	if ( hex_char <= '9' && hex_char >= '0') return (hex_char -'0');

 	return INVALID_DIGIT_CONVERTION;
 }


static uint16_t ascii_to_to_hex(uint8_t *stream_pointer, uint8_t convertion_type)
{
	uint16_t hex_result        = 0;
 	uint8_t stream_length      = 0;
 	uint16_t converted_digit   = 1;
 	uint16_t convertion_offset = 0x10;

 	if (convertion_type != 'h')
 		convertion_offset = 0xa;

 	while((stream_length != MAX_DIGITS) &&  (*stream_pointer) ) {

 		converted_digit = ( hexascii_to_hex(*stream_pointer));
 		if (INVALID_DIGIT_CONVERTION == converted_digit) {
			return INVALID_DIGIT_CONVERTION;
		}
		hex_result = (hex_result * convertion_offset) + converted_digit;
		stream_pointer++;
		stream_length++;
	}
	return hex_result;
  }



void dbg_setup(void)
{
	__HAL_UART_CLEAR_OREFLAG(UART_DBG_PORT);
	__HAL_UART_CLEAR_NEFLAG(UART_DBG_PORT);
	__HAL_UART_CLEAR_FEFLAG(UART_DBG_PORT);
	__HAL_UART_DISABLE_IT(UART_DBG_PORT, UART_IT_ERR);

	HAL_UART_Receive_IT(UART_DBG_PORT,(uint8_t *)Rx_data,1);

	task_pool.limit = MAX_TASK;
}



void dbg_message(char *tx_data, uint16_t len)
{
	HAL_UART_Transmit(UART_DBG_PORT ,(uint8_t *)tx_data, len,HAL_MAX_DELAY);
}



void dbg_uart_parser(uint8_t *msg)
{
	uint8_t task_id = 0;
	uint16_t args   = dbg_has_arguments(msg);
	uint8_t cmd_len;
	uint8_t *cmd_str;

	for (task_id = 0; task_id < task_pool.taken_task; task_id++) {

		cmd_str = task_pool.entry[task_id].command;
		cmd_len = task_pool.entry[task_id].command_len;

		if ( !STR_CMP(msg ,cmd_str,cmd_len))
			continue;

		if ((task_pool.entry[task_id].args=='p')){
			// printf("executing task arg\r\n");
			task_pool.entry[task_id].handlers.ptr_arg(get_arg_ptr(msg));
			return;
		}

		if ((args!=INVALID_DIGIT_CONVERTION) && task_pool.entry[task_id].args){
			task_pool.entry[task_id].handlers.uint_args(args);
			return;
		}


		if (! (task_pool.entry[task_id].args)) {
			// printf("executing task NO arg\r\n");

			task_pool.entry[task_id].handlers.void_args();
			return;
		}
		break;
	}
	//printf("task executed\r\n");
}
