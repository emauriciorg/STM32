#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "mifare.h"

/*!
   \todo update the max mifare to be 2??
*/
#define MAX_MIFARE_BOARD      1


#define PAYLOAD_MAX_LEN      80
#define CARD_NUMBER_POSITION 10
#define CARD_BYTES_TO_READ    4
#define MIN_SAMPLE_PERIOD    500
#define MIFARE_STATUS         8 /*taken from Kiran code*/
#define MIFARE_REPLY_TIME_MS  100//70

uint32_t dinamyc_sample_period = MIFARE_REPLY_TIME_MS;

#define MULTIPLEXOR_PORT  GPIOE
#define MUX_INIT_CHANNEL  0
enum mifare_request_type {
	MIFAER_AUTO_SCAN,
	MIFAER_MANUAL_SCAN
};


#define GENERIC_TIMER_HAL_HANDLER      htim7
extern TIM_HandleTypeDef               GENERIC_TIMER_HAL_HANDLER;
#define PTR_GENERIC_TIMER_HAL_HANDLER &GENERIC_TIMER_HAL_HANDLER


extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart8;
extern UART_HandleTypeDef huart6;


volatile uint32_t read_time[3] = {0};
volatile uint8_t board_to_read_mux = MUX_INIT_CHANNEL;
/*------------------------------mifare structures-----------------------------*/
struct payload_st{
	uint8_t buffer[PAYLOAD_MAX_LEN];
	uint8_t len;
	uint8_t max_len;
};

struct mifare_st{
	struct payload_st payload;
	USART_TypeDef *instance;
	UART_HandleTypeDef *handler;
	uint8_t id;
	uint8_t card_id[10];
	uint8_t card_present;
	uint8_t reader_present;
	uint8_t enabled;
	uint8_t ready_to_sample;
	uint32_t sample_period_ms; /*period to sample the card/send scan request*/
	uint32_t sample_time_start; /*time in which the card is sample*/
};


struct mifare_service_st{
	struct mifare_st reader[MAX_MIFARE_BOARD];
	uint8_t available;
	uint8_t request;
	uint32_t systick;
}mifare_service;


struct mifare_mux_pin{
	GPIO_TypeDef* port;
	uint16_t pin;
}board_selector[4];

/*------------------------mifare commands from SAS src code-------------------*/
static const char mifare_sequence_scan[17] = {
	0x43, 0x4d, 0x44,
	0x5f, 0x05, 0x5f,
	0x05, 0x5f, 0x00,
	0x5f, 0x00, 0x5f,
	0x00, 0x5f, 0x45,
	0x4e, 0x44
};

static const char mifare_RF_field_on[17] = {
	0x43, 0x4d , 0x44,
	0x5f, 0x13 , 0x5f,
	0x05, 0x5f , 0x00,
	0x5f, 0x00 , 0x5f,
	0x16, 0x5f , 0x45,
	0x4e, 0x44
};

static const char mifare_RF_field_off[17] = {
	0x43, 0x4d, 0x44,
	0x5f, 0x14, 0x5f,
	0x05, 0x5f, 0x00,
	0x5f, 0x00, 0x5f,
	0x11, 0x5f, 0x45,
	0x4e, 0x44
};
/*------------------------------private routines-----------------------------*/

void mifare_extract_single_reader_card_id(struct mifare_st  *reader);
void mifare_store_packet(struct mifare_st  *reader, uint8_t rx_byte);
void mifare_board_selector_init(void);
void mifare_mux_choose_board(void);



void timer_update_irq_callback(void)
{
	if(__HAL_TIM_GET_FLAG(PTR_GENERIC_TIMER_HAL_HANDLER, TIM_FLAG_UPDATE) != RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(PTR_GENERIC_TIMER_HAL_HANDLER, TIM_IT_UPDATE) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(PTR_GENERIC_TIMER_HAL_HANDLER, TIM_IT_UPDATE);
			//timer_interrupt();

			__HAL_TIM_CLEAR_FLAG(PTR_GENERIC_TIMER_HAL_HANDLER,TIM_FLAG_UPDATE);
			HAL_TIM_Base_Stop_IT(PTR_GENERIC_TIMER_HAL_HANDLER);
			GENERIC_TIMER_HAL_HANDLER.Instance->CNT = 0;
			HAL_TIM_Base_Start_IT(PTR_GENERIC_TIMER_HAL_HANDLER);
		}
	}
}


void mifare_timer_irq(void)
{
	volatile uint8_t index = 0;

	do {
		if (mifare_service.reader[index].instance == NULL){
			continue;
		}
		if (!(mifare_service.reader[index].sample_time_start--)){
			mifare_service.reader[index].sample_time_start = mifare_service.reader[index].sample_period_ms;
			mifare_service.reader[index].ready_to_sample = true;
		}
	} while( (index++) < MAX_MIFARE_BOARD);
}



void mifare_register_uart(UART_HandleTypeDef *huart, uint8_t id){

	mifare_service.reader[id].instance        = huart->Instance;
	mifare_service.reader[id].handler         = huart;
	mifare_service.reader[id].payload.max_len = PAYLOAD_MAX_LEN;
	huart->RxCpltCallback = mifare_irq_handler;

}


void mifare_deregister_uart(UART_HandleTypeDef *huart, uint8_t id)
{
	mifare_service.reader[id].instance        = 0;
	mifare_service.reader[id].handler         = 0;
}


void mifare_enable_reader_irq(uint8_t id)
{
	HAL_UART_Receive_IT(mifare_service.reader[id].handler, (uint8_t *)&(mifare_service.reader[id].payload.buffer[0]), 1);
}


void mifare_init(void)
{
	//htim7->PeriodElapsedCallback = timer_update_irq_callback;

	mifare_register_uart(&huart3,0);
	mifare_enable_reader_irq(0);

	mifare_board_selector_init();
	//mifare_register_uart(&huart2,0);
	// mifare_register_uart(&huart3,1);
	// mifare_register_uart(&huart8,2);
	// mifare_register_uart(&huart5,3);
	// mifare_register_uart(&huart6,4);

	// mifare_enable_reader_irq(0);
	// mifare_enable_reader_irq(1);
	// mifare_enable_reader_irq(2);
	// mifare_enable_reader_irq(3);
	// mifare_enable_reader_irq(4);

//	HAL_TIM_Base_Start_IT(PTR_GENERIC_TIMER_HAL_HANDLER);
}


void mifare_irq_handler(UART_HandleTypeDef *huart)
{
	uint8_t index = 0;
	/*over run error*/
	if (huart->Instance->SR & USART_SR_ORE) {
		huart->Instance->SR &= ~USART_SR_ORE;
		volatile uint32_t SR_uart = huart->Instance->SR;
		volatile uint32_t DR_uart = huart->Instance->DR;
		UNUSED(DR_uart);
		UNUSED(SR_uart);
		/*here we shall exit and enabled the isr!*/
	}

	do {
		if (huart->Instance != mifare_service.reader[index].instance) {
			continue;
		}

		mifare_service.reader[index].reader_present = true;
		mifare_store_packet(&(mifare_service.reader[index]),
		      mifare_service.reader[index].payload.buffer[0]);

		mifare_enable_reader_irq(index);
		//read_time[1] = HAL_GetTick();
	} while((index++) < MAX_MIFARE_BOARD);
}


/*------------------------------generic mifare routines-----------------------*/

void mifare_send_scan_command(struct  mifare_st *mifare_reader)
{
	mifare_reader->reader_present = false;
	mifare_reader->payload.len = 0;
	memset((char * )mifare_reader->payload.buffer, 0x00,
	    sizeof(mifare_reader->payload.buffer));
	HAL_UART_Transmit( mifare_reader->handler,
	         (uint8_t *) mifare_sequence_scan,
	         sizeof(mifare_sequence_scan),
	         HAL_MAX_DELAY);
}

void mifare_send_rf_field_command(struct  mifare_st *mifare_reader, uint8_t field_enabled)
{
	mifare_reader->reader_present = false;
	mifare_reader->payload.len = 0;
	memset((char * )mifare_reader->payload.buffer, 0x00,
	    sizeof(mifare_reader->payload.buffer));

	if (field_enabled == true) {
		HAL_UART_Transmit( mifare_reader->handler,
		(uint8_t *) mifare_RF_field_on,
		sizeof(mifare_RF_field_on),
		HAL_MAX_DELAY);
	}
	else{
		HAL_UART_Transmit( mifare_reader->handler,
		(uint8_t *) mifare_RF_field_off,
		sizeof(mifare_RF_field_off),
		HAL_MAX_DELAY);
	}

}


void mifare_store_packet(struct mifare_st  *reader, uint8_t rx_byte)
{
	if (reader->payload.len <reader->payload.max_len) {
		reader->payload.buffer[reader->payload.len++] = rx_byte;
	}
	else {
		reader->payload.len = 0;
	}
}

/*--------------------interface to other libraries-----------------------------*/



uint8_t mifare_check_reader_id(uint32_t reader_id)
{
	if (reader_id < MAX_MIFARE_BOARD) {
		printf("OK\r\n" );
		return true;
	}else {
		printf("reader id shall be < %x\r\n", MAX_MIFARE_BOARD);
	}
	return false;
}

void mifare_enable_reader(uint32_t reader_id)
{
	if (mifare_check_reader_id(reader_id)) {
		mifare_service.reader[reader_id].enabled = true;
	}
}

void mifare_disable_reader(uint32_t reader_id)
{
	if (mifare_check_reader_id(reader_id)) {
		mifare_service.reader[reader_id].enabled = false;
	}
}

#if 0
/*using timer*/
void mifare_sample_period_change(uint32_t reader_id, uint32_t period_in_ms )
{

	if (!mifare_check_reader_id(reader_id)) {
		return;
	}

	if (period_in_ms > MIN_SAMPLE_PERIOD) {
		mifare_service.reader[reader_id].sample_period_ms = period_in_ms;
	}
}
#endif

void mifare_sample_period_change( uint32_t period_in_ms )
{
	printf("sample period changed to [%d] ms \r\n", period_in_ms);
	dinamyc_sample_period = period_in_ms;
}

void mifare_send_scan_broadcast(void)
{
	uint8_t index = 0;

	mifare_service.available = false;


	do {
		if (mifare_service.reader[index].instance == NULL) {
			continue;
		}
		mifare_send_scan_command(&(mifare_service.reader[index]));
	}while( (index++) < MAX_MIFARE_BOARD);

	//read_time[0] = HAL_GetTick();
	/*leave time to the isr to execute*/
	uint32_t timeout = HAL_GetTick() + dinamyc_sample_period;
	while (timeout > HAL_GetTick());

}


void mifare_rf_field(uint8_t field){
	uint8_t index = 0;
	if (field == 0){
		printf("RF field OFF\r\n");
	}
	else {
		printf("RF field ON\r\n");
	}
	do {
		if (mifare_service.reader[index].instance == NULL) {
			continue;
		}
		mifare_send_rf_field_command(&(mifare_service.reader[index]), field);
	}while( (index++) < MAX_MIFARE_BOARD);
}

void mifare_extract_readers_card_id(void)
{
	uint8_t index = 0;

	do {
		if (mifare_service.reader[index].instance == NULL) {
			continue;
		}
		mifare_extract_single_reader_card_id(&(mifare_service.reader[index]));
	}while( (index++) < MAX_MIFARE_BOARD);
}

void mifare_read_single_board(void){
	mifare_auto_scanner();
}

void mifare_print_readers_card_id (void)
{
	uint8_t index = 0;
	do {
		/*none card got detected*/
		if (mifare_service.available == false){
			break;
		}

		if ( (mifare_service.reader[index].instance     == NULL)
		||   (mifare_service.reader[index].card_present == false)) {
			//printf("%0x. nothing read\r\n", index);
			continue;
		}

		printf("%0x.%x card detected : %s\r\n"
		,index
		,board_to_read_mux
		,mifare_service.reader[index].card_id ) ;
//		printf("Reply time %d ms \r\n", read_time[1]-read_time[0] );

	}while( (index++) < MAX_MIFARE_BOARD);
}


/*burst scan*/
void mifare_auto_scanner(void)
{
	mifare_mux_choose_board();
	mifare_send_scan_broadcast();
	mifare_extract_readers_card_id();
	mifare_print_readers_card_id();
}



void mifare_extract_single_reader_card_id(struct mifare_st  *reader)
{
	uint8_t cardNumberInBytes[21] = {0};
	uint8_t top_byte, bottom_byte;
	uint8_t i ;

	reader->card_present = false;
	if (memcmp((char *)reader->payload.buffer, "RES", 3) != 0){

		if (reader->payload.buffer[MIFARE_STATUS] != 0x43) return;

		reader->card_present = true;
		mifare_service.available = true;
		memcpy(cardNumberInBytes, (char *)&reader->payload.buffer[CARD_NUMBER_POSITION], 4);


		cardNumberInBytes[CARD_BYTES_TO_READ] = '\0';

		for (i = 0; i < CARD_BYTES_TO_READ; i++)
		{
			top_byte = (cardNumberInBytes[i]&0xF0)>>4;
			bottom_byte = (cardNumberInBytes[i]&0x0F);
			reader->card_id[i*2]   = (top_byte    <= 9)?(top_byte   + '0'):(top_byte   +'A'-10);
			reader->card_id[i*2+1] = (bottom_byte <= 9)?(bottom_byte+'0') :(bottom_byte+'A'-10);
		}
	}

}

#define MUX_PIN_OFFSET 0X02



struct table_entry_st{
	uint8_t *table_ptr;
	size_t size;
	uint8_t current_pos;
}table_entry[3];

struct table_entry_st *table_entry_mode_ptr;

uint8_t multiplexing_mode = 0;
uint8_t ctl_table_option_0[4] ={
	0x00,
	0x02,
	0x01,
	0x03
};

uint8_t ctl_table_option_1[5]={
	0x02,
	0x00,
	0x05,
	0x01,
	0x09
};

uint8_t ctl_table_option_2[4] ={
	0x01,
	0x02,
	0x04,
	0x08
};


void mifare_board_selector_init(void)
{
/*
	#option 1*
	#option 2*
#sample 100*
OK/error
*/
	table_entry[0].table_ptr = ctl_table_option_0;
	table_entry[0].size = sizeof(ctl_table_option_0);

	table_entry[1].table_ptr = ctl_table_option_1;
	table_entry[1].size = sizeof(ctl_table_option_1);

	table_entry[2].table_ptr = ctl_table_option_2;
	table_entry[2].size = sizeof(ctl_table_option_2);


	table_entry_mode_ptr = &table_entry[multiplexing_mode];

	board_selector[0].port = MULTIPLEXOR_PORT;
	board_selector[0].pin  = GPIO_PIN_2;

	board_selector[1].port = MULTIPLEXOR_PORT;
	board_selector[1].pin  = GPIO_PIN_3;

	board_selector[2].port = MULTIPLEXOR_PORT;
	board_selector[2].pin  = GPIO_PIN_4;

	board_selector[3].port = MULTIPLEXOR_PORT;
	board_selector[3].pin  = GPIO_PIN_5;
}




void mifare_mux_choose_board(void)
{
	uint8_t pos = table_entry_mode_ptr->current_pos;

	//printf("multiplexing pin %x [%x]\r\n", table_entry_mode_ptr->table_ptr[pos], (table_entry_mode_ptr->table_ptr[pos]) << MUX_PIN_OFFSET);
	MULTIPLEXOR_PORT->ODR  = 0;
	MULTIPLEXOR_PORT->ODR = ( table_entry_mode_ptr->table_ptr[pos]) << MUX_PIN_OFFSET;

	// HAL_Delay(500);
	// printf("reading antenna [%d %x] in mode [%d]\r\n",pos,table_entry_mode_ptr->table_ptr[pos],multiplexing_mode);

	if ((table_entry_mode_ptr->current_pos+1) >= table_entry_mode_ptr->size  ) {
		(table_entry_mode_ptr->current_pos) = 0;
	//	printf("\r\n");

	}else{
		table_entry_mode_ptr->current_pos++;
	}
	//HAL_Delay(500);
//multiplexing_mode = mode-1;
//table_entry_mode_ptr = &table_entry[multiplexing_mode];
}


void mifare_choose_mode(uint16_t mode){
	if ( (mode > 3)  ||  (mode==0)) {
		printf("ERROR: WRONG MODE , available [1-3]\r\n");
	}
	else{
		printf("OK\r\n");
	}
	printf("Mode set to %d \r\n", mode);

	multiplexing_mode = mode-1;
	table_entry_mode_ptr = &table_entry[multiplexing_mode];
}
void mifare_handler(void)
{
	//if(!mifare_service.request) return;
	mifare_auto_scanner();
	/*here we can add a delay to hold the sample */

}
