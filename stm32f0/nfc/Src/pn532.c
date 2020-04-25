#include "stm32f0xx_hal.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "pn532.h"

#define PN532_UART_HANDLER   huart2
#define PN532_UART_PTR       &huart2
#define PN532_UART_INSTANCE  USART2

extern UART_HandleTypeDef PN532_UART_HANDLER;

volatile uint8_t rx_data[2];
static volatile uint8_t tx_buffer[60];
static volatile uint8_t rx_buffer[60];

void pn532_store(uint8_t data);

void pn532_print_tx(void){

	uint8_t i=0;
	for ( i = 0; i < 30; i++){

  	printf(" %02x ", tx_buffer[i]);
	}
	printf("\r\n");
}
void pn532_send_command(uint8_t cmd, uint8_t *arg, uint8_t len)
{

	uint8_t lcs=0;
	uint8_t dcs1=0;
	uint8_t crc1=0;
	uint8_t payload_len = 0;
	uint8_t packet_len = 0;
	payload_len = 1 + 1 + len;
	lcs = (~payload_len) + 1;
	crc1  =  cmd + 0xd4;

	for (uint8_t i = 0; i < len ;  i++ ){
		crc1 += arg[i];
	}
	dcs1  = (~crc1) + 1;

	memset((char *)tx_buffer,0,sizeof(tx_buffer));
	tx_buffer[0] = 0x55;
	tx_buffer[1] = 0x00;
	tx_buffer[2] = 0x00;
	tx_buffer[3] = 0x00;
	tx_buffer[4] = 0x00;
	tx_buffer[5] = 0x00;
	tx_buffer[6] = 0x00;

	tx_buffer[0+7] = 0;
	tx_buffer[1+7] = 0;
	tx_buffer[2+7] = 0xff;
	tx_buffer[3+7] = payload_len;
	tx_buffer[4+7] = lcs;
	tx_buffer[5+7] = 0xD4;
	tx_buffer[6+7] = cmd;
	memcpy((char *) &tx_buffer[7+7],arg,len);
	tx_buffer[7+len+7] = dcs1;
	tx_buffer[7+len+1+7] = 0x00;
	packet_len  = 7 + len +2+7;

	HAL_UART_Transmit(PN532_UART_PTR,(uint8_t *)tx_buffer, packet_len ,HAL_MAX_DELAY);
	//pn532_print_tx();
}

void pn532_send_command_no_wake(uint8_t cmd, uint8_t *arg, uint8_t len)
{

	uint8_t lcs=0;
	uint8_t dcs1=0;
	uint8_t crc1=0;
	uint8_t payload_len = 0;
	uint8_t packet_len = 0;
	payload_len = 1 + 1 + len;
	lcs = (~payload_len) + 1;
	crc1  =  cmd + 0xd4;

	for (uint8_t i = 0; i < len ;  i++ ){
		crc1 += arg[i];
	}
	dcs1  = (~crc1) + 1;

	memset((char *)tx_buffer,0,sizeof(tx_buffer));

	tx_buffer[0] = 0;
	tx_buffer[1] = 0;
	tx_buffer[2] = 0xff;
	tx_buffer[3] = payload_len;
	tx_buffer[4] = lcs;
	tx_buffer[5] = 0xD4;
	tx_buffer[6] = cmd;
	memcpy((char *) &tx_buffer[7],arg,len);
	tx_buffer[7+len] = dcs1;
	tx_buffer[7+len+1] = 0x00;
	packet_len  = 7 + len +2;

	HAL_UART_Transmit(PN532_UART_PTR,(uint8_t *)tx_buffer, packet_len ,HAL_MAX_DELAY);
	//pn532_print_tx();
}

//void USART2_IRQHandler(void)
HAL_StatusTypeDef uart_status = 0;
uint8_t dummy_data;
void pn532_wake_up(void)
{
	uint8_t  wake_up_preamble[10]= {0x55, 0x00,0x00,
		0x00, 0x00,0x00,
		0x00};
	printf("waking up\r\n");
	HAL_UART_Transmit(PN532_UART_PTR,wake_up_preamble, 7 ,HAL_MAX_DELAY);


}


void pn532_isr(UART_HandleTypeDef *huart)
{
	/*clear ocr*/
	__HAL_UART_CLEAR_OREFLAG(huart);
	pn532_store(rx_data[0]);
	uart_status = HAL_UART_Receive_IT(huart, (uint8_t *)rx_data, 1);
	if(uart_status != HAL_OK){
			dummy_data = 1;
	}
}

uint8_t recieving_pay_load = 0;
uint8_t rx_index = 0;
uint8_t pn532_availble =0;
uint8_t payload_len = 0;
uint8_t is_ack = 0;
void pn532_store(uint8_t data){


	//rx_buffer[rx_index] = data;
	//rx_index++;
//	if (rx_index> 30) {
	//	pn532_availble =1;
	//	rx_index =0 ;
//
//	}
	//return;

	if (recieving_pay_load){
		if (rx_index ==3)payload_len = data;

		/*ack frame*/
		if (payload_len == 0xff  ||  payload_len == 0x00) {

			rx_buffer[rx_index] = data;
			if (rx_index == 5) pn532_availble = 1;
			is_ack = 1;
			rx_index++;
			return;
		}

		if (data == 0  && rx_index == 3){
			pn532_availble = 1;
			return;
		}

		if (data == 0x00 && (rx_index > payload_len)){
			rx_buffer[rx_index] = data;
			recieving_pay_load = 0;
			pn532_availble     = 1;
			rx_index++;
		}else{
			rx_buffer[rx_index] = data;
			rx_index++;
		}
		return;
	}

	if (data == 0x00  &&  rx_index < 2){
		rx_buffer[rx_index] = data;
		rx_index++;
		return;
	}

	if(rx_index == 2 && data == 0xff){
		rx_buffer[rx_index] = data;
		recieving_pay_load = 1;
		rx_index++;
	}
}

void pn532_init(void)
{
	__HAL_UART_CLEAR_OREFLAG(PN532_UART_PTR);
	HAL_UART_Receive_IT(PN532_UART_PTR, (uint8_t *)rx_data, 1);
}


void get_fw_version(void)
{
//	HAL_UART_Transmit(PN532_UART_PTR, (uint8_t) cmd_get_version,sizeof(cmd_get_version),HAL_MAX_DELAY)

	printf("getting version \r\n");

	pn532_send_command_no_wake(0x02,0,0);
}

void pn532_sam_config(void){
	static uint8_t cmd_arg[2]={0x01,0x01};
	pn532_send_command(0x14,cmd_arg,2);
}

void pn532_list_targets(void){
	static uint8_t cmd_arg[2]={0x01,0x00};
	pn532_send_command_no_wake(0x4A,cmd_arg,2);
}
void get_status(void)
{
//	HAL_UART_Transmit(PN532_UART_PTR, (uint8_t) cmd_get_version,sizeof(cmd_get_version),HAL_MAX_DELAY)

	printf("getting status \r\n");

	pn532_send_command_no_wake(0x04,0,0);
}

void get_fw_version_wk(void)
{
	printf("getting version wk\r\n");
	pn532_wake_up();
	pn532_send_command(0x02,0,0);
}

uint8_t safe_buffer[40] = {0};
uint8_t  safe_index = 0;
void pn532_print(void){

	uint8_t i=0;
	for ( i = 0; i < safe_index; i++){

  	printf(" %02x ", safe_buffer[i]);
	}
	printf("\r\n");

	if (safe_buffer[6]== 0x4b){
		printf("card Id is [ " );
		for (i = 0; i<4;i++){
			printf(" %02x ", safe_buffer[13+i]);
		}
		printf("] \r\n");

	}
}

void pn532_listener(void){
	if (!pn532_availble) return;
	uint8_t local_len;
	memset(safe_buffer,0,sizeof(safe_buffer));
	memcpy(safe_buffer,rx_buffer,rx_index);
	memset(rx_buffer,0,sizeof(rx_buffer));
	safe_index = rx_index;
	pn532_availble = 0;
	recieving_pay_load = 0;
	local_len = payload_len;
	payload_len = 0;
	rx_index = 0;
	if (is_ack) {
		is_ack =0;
		return;
	}

	pn532_print();
	printf(" len is %d \r\n", local_len);
	//	HAL_UART_Receive_IT(&huart2, (uint8_t *)rx_data, 1);
}

uint8_t pn532_scan(uint8_t *card_id){


	return false;
}
