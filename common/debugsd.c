
/***DEBUG SECTION**/

#include "debugsd.h"
#include <stdint.h>
#include <string.h>


extern UART_HandleTypeDef huart3; //channel 7 UART3
uint8_t Rx_data[3];  
st_str  uart_tx;


enum {
	CMD_READ_FILE='0',
	CMD_OVER_WRITE_FILE,
	CMD_APPEND_FILE,
	CMD_LIST_FILE,
	CMD_GET_STATUS,
};

enum{
	false,
	true
};


static struct {
	const char init_char;
	const char end_char;
	char
	complete:1,
	start:1;
	char rx_byte;


	char tail;
	char command;
	char data_out[UART_RX_BUFFER_SIZE];
	char parameters[UART_RX_BUFFER_SIZE];
	char data_in[UART_RX_BUFFER_SIZE];
	char end;
} protocol={'#','*',0,0,{0},{0},0,0};




void dbg_store_packet(char recieved_data)
{
	if (recieved_data == protocol.init_char){

		protocol.tail		=false; 
		protocol.complete	=false;
		protocol.start		=true;
	}

	if ((recieved_data == protocol.end_char)  &&         protocol.start==true){
		
		protocol.complete	=true; 
		//protocol.rx_byte	=false;
		protocol.start		=false;
	}  

	protocol.data_in[ protocol.tail ]	= recieved_data;
	protocol.tail++;

	if ( protocol.tail >= (UART_RX_BUFFER_SIZE-1)) 
		protocol.tail=false;
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance != USART3) return;  

	dbg_store_packet(Rx_data[0]);
	HAL_UART_Receive_IT(&huart3,Rx_data, 1);	
}


void dbg_command_scan(void)
{
	if (!protocol.complete) return;

	protocol.command	=protocol.data_in[1];
	memcpy(protocol.parameters,  protocol.data_in, (protocol.tail-1));       
	SDBG("[RECIEVED %s]", protocol.parameters);
	dbg_uart_parser( protocol.command) ;  
	dbg_clear_packet();
}

void dbg_clear_packet(void)
{
	protocol.tail		=false;
	protocol.start		=false;
	protocol.command	=false;
	protocol.end		=false;
	protocol.complete	=false;
	
	memset(protocol.parameters, false, UART_RX_BUFFER_SIZE);
	memset(protocol.data_in   , false, UART_RX_BUFFER_SIZE);
}

void dbg_set_port(void)
{	
	HAL_UART_Receive_IT(&huart3,Rx_data,1);
}

char dbg_get_command(char *command)
{
	if(!protocol.complete) return  0;
	
	*command          = protocol.command;
	protocol.complete = 0;
	return 1;
}



void dbg_message(char *tx_data, int len)
{	
	HAL_Delay(10);\
	HAL_UART_Transmit(&huart3 ,(uint8_t *)tx_data, len, 100);
}




void dbg_uart_parser(char uart_command)
{


	switch(uart_command){
	case CMD_READ_FILE:	
				SDBG("CMD_READ_FILE \n");
					//fsSaveMACs((uint8_t *)"ONE.bin", (uint8_t *)"first line saved\n");
				break;
	case CMD_OVER_WRITE_FILE:
				SDBG((char *)"CMD_OVER_WRITE_FILE \n");
				break;
		
	case CMD_APPEND_FILE:
				SDBG("CMD_APPEND_FILE \n");
				break;
	case CMD_LIST_FILE:
				SDBG("CMD_LIST_FILE \n");
				break;
	case CMD_GET_STATUS:
				SDBG("CMD_GET_STATUS \n");
				break;			

	default:		SDBG("Unknow Command\n");
				break;
	}
}

