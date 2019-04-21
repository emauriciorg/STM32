#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cuart.h"
#include "max7219.h"
extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart5;

#define UART_DBG
#define UART_RX_BUFFER_SIZE 256
#define INVALID_DIGIT_CONVERTION 255
#define MAX_DIGITS 5
#define MAX_PAYLOAD_LEN 32

#define UART_DBG_PORT &huart1

#define DEBUG_TIMEOUT 1000


#if 0
	#define CUART_DBG(...)  uart_tx.len= sprintf(uart_tx.data,__VA_ARGS__);\
			dbg_message(uart_tx.data,uart_tx.len);
#else
	#define CUART_DBG(...) printf(__VA_ARGS__)
#endif

#ifndef STR_CMP
	#define STR_CMP(buffer, string, len )  !memcmp(buffer, string, len)
#endif




static uint16_t cli_ascii_stream_to_hex(uint8_t *stream_pointer, uint8_t convertion_type);
static uint8_t cli_ascii_charhex_to_hex( char  hex_character);
static void dbg_uart_parser(uint8_t *msg);
static void dbg_clear_packet(void);


enum{
	false,
	true
};

typedef struct{
	int len;
	char data[UART_RX_BUFFER_SIZE];
} st_str;

volatile uint8_t Rx_data[1];
volatile uint8_t Rx_data5[1];
st_str  uart_tx;


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




void dbg_store_packet(char recieved_data)
{
	if (recieved_data == protocol.init_char){

		protocol.tail		=false;
		protocol.complete	=false;
		protocol.start		=true;
		return;
	}

	if ((recieved_data == protocol.end_char)  && protocol.start==true){

		protocol.complete	=true;
		protocol.start		=false;
	}

	protocol.data_in[ protocol.tail ]	= recieved_data;
	protocol.tail++;

	if ( protocol.tail >= (UART_RX_BUFFER_SIZE-1))
		protocol.tail=false;
}


uint32_t response_rx_it_sys_call;
uint32_t byte_counter_rx;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if (huart->Instance == USART1) {
		//LED_GREEN_TOGGLE();
		byte_counter_rx++;
		dbg_store_packet(Rx_data[0]);
		response_rx_it_sys_call = HAL_UART_Receive_IT(huart, (uint8_t *)Rx_data, 1);
		//SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);

	}



}

void dbg_command_scan(void)
{
	if (!protocol.complete) return;

	protocol.complete	= false;

	//printf("reception done [%d]\r\n",(int)byte_counter_rx);
	byte_counter_rx = 0;

	protocol.data_in[protocol.tail+1]='\0';
	protocol.command	=protocol.data_in[1];
	memcpy(protocol.parameters,  protocol.data_in,(protocol.tail-1));

	dbg_uart_parser( protocol.parameters) ;
	dbg_clear_packet();

}

static void dbg_clear_packet(void)
{
	protocol.tail		=false;
	protocol.start		=false;
	protocol.command	=false;
	protocol.end		=false;
	protocol.complete	=false;

	memset(protocol.parameters, false, UART_RX_BUFFER_SIZE);
	memset(protocol.data_in   , false, UART_RX_BUFFER_SIZE);
}




static uint8_t cli_ascii_charhex_to_hex( char  hex_character){

 	if ( hex_character <= 'F' && hex_character >= 'A') return (hex_character -'A')+10;

 	if ( hex_character <= 'f' && hex_character >= 'a') return (hex_character -'a')+10;

 	if ( hex_character <= '9' && hex_character >= '0') return (hex_character -'0');

 	return INVALID_DIGIT_CONVERTION;
 }

 static uint16_t cli_ascii_stream_to_hex(uint8_t *stream_pointer, uint8_t convertion_type)
 {
	uint16_t hex_result        = 0;
 	uint8_t stream_length      = 0;
 	uint16_t converted_digit   = 1;
 	uint16_t convertion_offset = 0x10;

 	if (convertion_type != 'h')
 		convertion_offset = 0xa;

 	while((stream_length != MAX_DIGITS) &&  (*stream_pointer) ){

 		converted_digit = ( cli_ascii_charhex_to_hex(*stream_pointer));
 		if (INVALID_DIGIT_CONVERTION == converted_digit){
			break;
		}

		hex_result= ( hex_result * convertion_offset	) + converted_digit;
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

}



void dbg_message(char *tx_data, uint16_t len)
{
	HAL_UART_Transmit(UART_DBG_PORT ,(uint8_t *)tx_data, len,HAL_MAX_DELAY);
}



void dbg_uart_parser(uint8_t *msg)
{

	if ( STR_CMP(msg,"test",4)){
		CUART_DBG("I2c test\r\n");
		max7219_test();

		return;
	}
	if (STR_CMP(msg,"init",4)){
		CUART_DBG("oled init \r\n");
		//oled_init();

	}
	if ( STR_CMP(msg,"read",4)){

		CUART_DBG("read\r\n");
		return;
	}

	if ( STR_CMP(msg,"scan",4)){
		CUART_DBG("write\r\n");
		return;
	}

	if (STR_CMP(msg,"write",4)){
			CUART_DBG("display\r\n");
			return;
	}

	if ( STR_CMP(msg, "CMD0",4)){
		CUART_DBG("CMD0\r\n");
		//CUART_DBG("extract [%d] [%s] \r\n",(int) cli_ascii_stream_to_hex ( &msg[4], 'd'),(char *)(&msg[4]) );
		//pwm_value = cli_ascii_stream_to_hex ( &msg[4], 'd');
		return;
	}

	if ( STR_CMP(msg,"CMD1",4)){
		CUART_DBG("CMD1\r\n");
		return;
	}

	if ( STR_CMP(msg,"CMD2",4)){
		CUART_DBG("CMD2\r\n");
		return;
	}

	if ( STR_CMP(msg,"CMD3",4)){
		CUART_DBG("CMD3\r\n");
		return;
	}


	if ( STR_CMP(msg,"echo",4)){
		CUART_DBG("[ECHO]\r\n");
		return;
	}
}
