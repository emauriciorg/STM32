#include "cuart.h"
#include "cgpio.h"
#include "cadc.h"
#include <stdint.h>
#include <string.h>
#include "ctimer.h"
#include <stdio.h>


#define UART_DBG
#ifdef UART_DBG
	#define DBG(...) printf(__VA_ARGS__ )
#else
	#define DBG(...)
#endif

#ifndef STR_CMP
	#define STR_CMP(buffer, string, len )  !memcmp(buffer, string, len)
#endif


uint8_t UART1_rx_buffer[32];
uint8_t u8_UART1_index = 0;
uint8_t UART1_message[32];

UART_HandleTypeDef huart1;
#define MAX_PAYLOAD_LEN 32
typedef struct{
	const uint8_t packet_start;
	uint8_t payload[MAX_PAYLOAD_LEN];
	uint8_t checksum;
	const uint8_t packer_end;
	uint8_t index;
	uint8_t rx_buffer[5];
	uint8_t message_complete;
}uart_protocol_t;

uart_protocol_t debug_port ;//= {'*',{0},'#'};


static void cuart_save_data(UART_HandleTypeDef *huart);
static uint16_t cli_ascii_stream_to_hex(char *stream_pointer, uint8_t convertion_type);
static uint8_t cli_ascii_charhex_to_hex( char  hex_character);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	 if (huart->Instance == USART1){
		TOGGLE_GREEN_LED();
		cuart_save_data(huart);
		HAL_UART_Receive_IT(huart, (uint8_t *)(debug_port.rx_buffer), 1);
	 }
 }

static void cuart_save_data(UART_HandleTypeDef *huart){
	if (debug_port.index>= MAX_PAYLOAD_LEN)
		debug_port.index = 0;


	if(debug_port.rx_buffer[0] !='#'){
		debug_port.payload[debug_port.index++] = debug_port.rx_buffer[0];
	}else {
		debug_port.payload[debug_port.index] = '\0';
		debug_port.message_complete = 1;

		debug_port.rx_buffer[0] = 0;
		debug_port.index = 0;
	}

}

#define INVALID_DIGIT_CONVERTION 255
#define MAX_DIGITS 5

static uint8_t cli_ascii_charhex_to_hex( char  hex_character){

 	if ( hex_character <= 'F' && hex_character >= 'A') return (hex_character -'A')+10;

 	if ( hex_character <= 'f' && hex_character >= 'a') return (hex_character -'a')+10;

 	if ( hex_character <= '9' && hex_character >= '0') return (hex_character -'0');

 	return INVALID_DIGIT_CONVERTION;
 }

 static uint16_t cli_ascii_stream_to_hex(char *stream_pointer, uint8_t convertion_type)
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



void uart_setup(void){
	HAL_UART_Receive_IT(&huart1, (uint8_t *)(debug_port.rx_buffer), 1);

		memset(debug_port.payload,0,sizeof(debug_port.payload));
		memset(debug_port.rx_buffer,0,sizeof(debug_port.rx_buffer));
 }



void cuart_parser(void){
	uint16_t pwm_value = 0 ;
	uint8_t *msg = 0;
	msg =  &debug_port.payload[0];
	if (!debug_port.message_complete)
		return;
	debug_port.message_complete = 0;
//	HAL_UART_Receive_IT(&huart1, (uint8_t *)(debug_port.rx_buffer), 1);

	#if 1
	if ( STR_CMP(msg, "pwm",3)){
		 pwm_value = cli_ascii_stream_to_hex ( &debug_port.payload[4], 'd');
		 ctimer_update_pwm(0,pwm_value,0);
		 DBG("extract [%d] [%s] \r\n", cli_ascii_stream_to_hex ( &debug_port.payload[4], 'h'),debug_port.payload );
	}
	if ( STR_CMP(msg,"adc",3)){
		DBG("adc channel is [%x]\n", cadc_get_adc_value (5));
	}
	if ( STR_CMP(msg,"echo",3)){
		DBG("ECHO\r\n");
	}
	#endif
//	HAL_UART_Receive_IT(&huart1, (uint8_t *)(debug_port.rx_buffer), 1);


	memset(debug_port.payload,0,sizeof(debug_port.payload));
}
