// Program to handle the interaction with SD card
// Based on the defined protocol, set values are written to register
// Base functions to read and write a block has been coded which is referred from fat.c


#include "stm32f0xx.h"

#include <string.h>
#include <stdio.h>

#include "sd.h"
#include "spi.h"
#include <stdio.h>
//#include "Foundation.h"

extern  SPI_HandleTypeDef hspi1;

/*private macros PM*/

#define SD_CMD_LENGTH 6


#define SD_CMD_GO_IDLE_STATE          0   /* CMD0 = 0x40  */
#define SD_CMD_SEND_OP_COND           1   /* CMD1 = 0x41  */
#define SD_CMD_SEND_IF_COND           8   /* CMD8 = 0x48  */
#define SD_CMD_SEND_CSD               9   /* CMD9 = 0x49  */
#define SD_CMD_SEND_CID               10  /* CMD10 = 0x4A */
#define SD_CMD_STOP_TRANSMISSION      12  /* CMD12 = 0x4C */
#define SD_CMD_SEND_STATUS            13  /* CMD13 = 0x4D */
#define SD_CMD_SET_BLOCKLEN           16  /* CMD16 = 0x50 */
#define SD_CMD_READ_SINGLE_BLOCK      17  /* CMD17 = 0x51 */
#define SD_CMD_READ_MULT_BLOCK        18  /* CMD18 = 0x52 */
#define SD_CMD_SET_BLOCK_COUNT        23  /* CMD23 = 0x57 */
#define SD_CMD_WRITE_SINGLE_BLOCK     24  /* CMD24 = 0x58 */
#define SD_CMD_WRITE_MULT_BLOCK       25  /* CMD25 = 0x59 */
#define SD_CMD_PROG_CSD               27  /* CMD27 = 0x5B */
#define SD_CMD_SET_WRITE_PROT         28  /* CMD28 = 0x5C */
#define SD_CMD_CLR_WRITE_PROT         29  /* CMD29 = 0x5D */
#define SD_CMD_SEND_WRITE_PROT        30  /* CMD30 = 0x5E */
#define SD_CMD_SD_ERASE_GRP_START     32  /* CMD32 = 0x60 */
#define SD_CMD_SD_ERASE_GRP_END       33  /* CMD33 = 0x61 */
#define SD_CMD_UNTAG_SECTOR           34  /* CMD34 = 0x62 */
#define SD_CMD_ERASE_GRP_START        35  /* CMD35 = 0x63 */
#define SD_CMD_ERASE_GRP_END          36  /* CMD36 = 0x64 */
#define SD_CMD_UNTAG_ERASE_GROUP      37  /* CMD37 = 0x65 */
#define SD_CMD_ERASE                  38  /* CMD38 = 0x66 */
#define SD_CMD_SD_APP_OP_COND         41  /* CMD41 = 0x69 77*/
#define SD_CMD_APP_CMD                55  /* CMD55 = 0x77 */
#define SD_CMD_READ_OCR               58  /* CMD58 = 0x79 */



#define SD_TOKEN_START_DATA_SINGLE_BLOCK_READ    0xFE  /* Data token start byte, Start Single Block Read */
#define SD_TOKEN_START_DATA_MULTIPLE_BLOCK_READ  0xFE  /* Data token start byte, Start Multiple Block Read */
#define SD_TOKEN_START_DATA_SINGLE_BLOCK_WRITE   0xFE  /* Data token start byte, Start Single Block Write */
#define SD_TOKEN_START_DATA_MULTIPLE_BLOCK_WRITE 0xFD  /* Data token start byte, Start Multiple Block Write */
#define SD_TOKEN_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xFD  /* Data toke stop byte, Stop Multiple Block Write */

/**
  * @brief  SD reponses and error flags
  */
typedef enum
{
/* R1 answer value */
  SD_R1_NO_ERROR            = (0x00),
  SD_R1_IN_IDLE_STATE       = (0x01),
  SD_R1_ERASE_RESET         = (0x02),
  SD_R1_ILLEGAL_COMMAND     = (0x04),
  SD_R1_COM_CRC_ERROR       = (0x08),
  SD_R1_ERASE_SEQUENCE_ERROR= (0x10),
  SD_R1_ADDRESS_ERROR       = (0x20),
  SD_R1_PARAMETER_ERROR     = (0x40),

/* R2 answer value */
  SD_R2_NO_ERROR            = 0x00,
  SD_R2_CARD_LOCKED         = 0x01,
  SD_R2_LOCKUNLOCK_ERROR    = 0x02,
  SD_R2_ERROR               = 0x04,
  SD_R2_CC_ERROR            = 0x08,
  SD_R2_CARD_ECC_FAILED     = 0x10,
  SD_R2_WP_VIOLATION        = 0x20,
  SD_R2_ERASE_PARAM         = 0x40,
  SD_R2_OUTOFRANGE          = 0x80,

/**
  * @brief  Data response error
  */
  SD_DATA_OK                = (0x05),
  SD_DATA_CRC_ERROR         = (0x0B),
  SD_DATA_WRITE_ERROR       = (0x0D),
  SD_DATA_OTHER_ERROR       = (0xFF)
} SD_Error;




#define CMD_DUMMY_BYTE 0XFF
#define MAX_TRIES_PER_COMMAND 20
#define MAX_TRIES_PER_RESPONSE 20

#define DEBUG_SD_HEADER_ENABLE
#ifdef DEBUG_SD_HEADER_ENABLE
	#define SD_DEBUG(...) printf(__VA_ARGS__)
#else
	#define SD_DEBUG(...)
#endif

char log_message_sd[50];
char replyBytes[5];
char bitShifter = 9;


void sdio_response_parse(uint8_t response, uint8_t sent_command){


	SD_DEBUG("\r\n ****************CMD: %x to ****************\r\n", sent_command);


	/*Command parser*/

	if ( SD_CMD_GO_IDLE_STATE == sent_command){
		SD_DEBUG("\t\tSD_CMD_GO_IDLE_STATE \r\n");
	}
	if ( SD_CMD_SEND_OP_COND == sent_command){
		SD_DEBUG("\t\tSD_CMD_SEND_OP_COND \r\n");
	}
	if ( SD_CMD_SEND_IF_COND == sent_command){
		SD_DEBUG("\t\tSD_CMD_SEND_IF_COND \r\n");
	}
	if ( SD_CMD_SEND_CSD == sent_command){
		SD_DEBUG("\t\tSD_CMD_SEND_CSD \r\n");
	}
	if ( SD_CMD_SEND_CID == sent_command){
		SD_DEBUG("\t\tSD_CMD_SEND_CID \r\n");
	}
	if ( SD_CMD_STOP_TRANSMISSION == sent_command){
		SD_DEBUG("\t\tSD_CMD_STOP_TRANSMISSION \r\n");
	}
	if ( SD_CMD_SEND_STATUS == sent_command){
		SD_DEBUG("\t\tSD_CMD_SEND_STATUS \r\n");
	}
	if ( SD_CMD_SET_BLOCKLEN == sent_command){
		SD_DEBUG("\t\tSD_CMD_SET_BLOCKLEN \r\n");
	}
	if ( SD_CMD_READ_SINGLE_BLOCK == sent_command){
		SD_DEBUG("\t\tSD_CMD_READ_SINGLE_BLOCK \r\n");
	}
	if ( SD_CMD_READ_MULT_BLOCK == sent_command){
		SD_DEBUG("\t\tSD_CMD_READ_MULT_BLOCK \r\n");
	}
	if ( SD_CMD_SET_BLOCK_COUNT == sent_command){
		SD_DEBUG("\t\tSD_CMD_SET_BLOCK_COUNT \r\n");
	}
	if ( SD_CMD_WRITE_SINGLE_BLOCK == sent_command){
		SD_DEBUG("\t\tSD_CMD_WRITE_SINGLE_BLOCK \r\n");
	}
	if ( SD_CMD_WRITE_MULT_BLOCK == sent_command){
		SD_DEBUG("\t\tSD_CMD_WRITE_MULT_BLOCK \r\n");
	}
	if ( SD_CMD_PROG_CSD == sent_command){
		SD_DEBUG("\t\tSD_CMD_PROG_CSD \r\n");
	}
	if ( SD_CMD_SET_WRITE_PROT == sent_command){
		SD_DEBUG("\t\tSD_CMD_SET_WRITE_PROT \r\n");
	}
	if ( SD_CMD_CLR_WRITE_PROT == sent_command){
		SD_DEBUG("\t\tSD_CMD_CLR_WRITE_PROT \r\n");
	}
	if ( SD_CMD_SEND_WRITE_PROT == sent_command){
		SD_DEBUG("\t\tSD_CMD_SEND_WRITE_PROT \r\n");
	}
	if ( SD_CMD_SD_ERASE_GRP_START == sent_command){
		SD_DEBUG("\t\tSD_CMD_SD_ERASE_GRP_START \r\n");
	}
	if ( SD_CMD_SD_ERASE_GRP_END == sent_command){
		SD_DEBUG("\t\tSD_CMD_SD_ERASE_GRP_END \r\n");
	}
	if ( SD_CMD_UNTAG_SECTOR == sent_command){
		SD_DEBUG("\t\tSD_CMD_UNTAG_SECTOR \r\n");
	}
	if ( SD_CMD_ERASE_GRP_START == sent_command){
		SD_DEBUG("\t\tSD_CMD_ERASE_GRP_START \r\n");
	}
	if ( SD_CMD_ERASE_GRP_END == sent_command){
		SD_DEBUG("\t\tSD_CMD_ERASE_GRP_END \r\n");
	}
	if ( SD_CMD_UNTAG_ERASE_GROUP == sent_command){
		SD_DEBUG("\t\tSD_CMD_UNTAG_ERASE_GROUP \r\n");
	}
	if ( SD_CMD_ERASE == sent_command){
		SD_DEBUG("\t\tSD_CMD_ERASE \r\n");
	}
	if ( SD_CMD_SD_APP_OP_COND == sent_command){
		SD_DEBUG("\t\tSD_CMD_SD_APP_OP_COND \r\n");
	}
	if ( SD_CMD_APP_CMD == sent_command){
		SD_DEBUG("\t\tSD_CMD_APP_CMD \r\n");
	}
	if ( SD_CMD_READ_OCR == sent_command){
		SD_DEBUG("\t\tSD_CMD_READ_OCR \r\n");
	}

	/*Response parser*/
	// /SD_DEBUG(" res: %x to\r\n", response);

	// if ((SD_R1_NO_ERROR) == (response & SD_R1_NO_ERROR) ) {
		// SD_DEBUG("\t\tSD_R1_NO_ERROR \r\n");
	// }

	if ((SD_R1_IN_IDLE_STATE) == (response & SD_R1_IN_IDLE_STATE) ) {
		SD_DEBUG("\t\t\tSD_R1_IN_IDLE_STATE \r\n");
	}

	if ((SD_R1_ERASE_RESET) == (response & SD_R1_ERASE_RESET) ) {
		SD_DEBUG("\t\t\tSD_R1_ERASE_RESET \r\n");
	}

	if ((SD_R1_ILLEGAL_COMMAND) == (response & SD_R1_ILLEGAL_COMMAND) ) {
		SD_DEBUG("\t\t\tSD_R1_ILLEGAL_COMMAND \r\n");
	}

	if ((SD_R1_COM_CRC_ERROR) == (response & SD_R1_COM_CRC_ERROR) ) {
		SD_DEBUG("\t\t\tSD_R1_COM_CRC_ERROR \r\n");
	}

	if ((SD_R1_ERASE_SEQUENCE_ERROR) == (response & SD_R1_ERASE_SEQUENCE_ERROR) ) {
		SD_DEBUG("\t\t\tSD_R1_ERASE_SEQUENCE_ERROR \r\n");
	}

	if ((SD_R1_ADDRESS_ERROR) == (response & SD_R1_ADDRESS_ERROR) ) {
		SD_DEBUG("\t\t\tSD_R1_ADDRESS_ERROR \r\n");
	}

	if ((SD_R1_PARAMETER_ERROR) == (response & SD_R1_PARAMETER_ERROR) ) {
		SD_DEBUG("\t\t\tSD_R1_PARAMETER_ERROR \r\n");
	}
}


void sd_print_responde(uint8_t *sd_card_reply){
	SD_DEBUG("\tR %x ", sd_card_reply[0]);
	SD_DEBUG("%x ", sd_card_reply[1]);
	SD_DEBUG("%x ", sd_card_reply[2]);
	SD_DEBUG("%x ", sd_card_reply[3]);
	SD_DEBUG("%x \r\n\r\n", sd_card_reply[4]);
}

unsigned char SD_init(void)
{
	unsigned char response  = 0;
	uint8_t spi_clk_counter = 0;
	unsigned int retry      = 0 ;

	uint8_t spi_response[6];
	uint8_t frame[10];

	memset(spi_response,0, sizeof(spi_response));
	memset(frame,0, sizeof(frame));

	/*! \todo SSP_Initialize is full of registers */
	SSP_Initialize();



	for(spi_clk_counter = 0; spi_clk_counter < 11; spi_clk_counter++){
		SSP_Write(CMD_DUMMY_BYTE);
		CS_ASSERT;
	}



	SD_DEBUG("SD_CMD_GO_IDLE_STATE CMD0\r\n");
	do
	{

		response = SD_SendCmd_2(SD_CMD_GO_IDLE_STATE, 0,0x95, SD_ANSWER_R1_EXPECTED);

		CS_ASSERT;
		SSP_Write(CMD_DUMMY_BYTE);
		sd_print_responde(replyBytes);

		if (retry++ > 20){
			return (1);
		}
	} while (response != SD_R1_IN_IDLE_STATE);

	retry = 0;

	SD_DEBUG("SD_CMD_SEND_IF_COND CMD8 \r\n");
	response = SD_SendCmd_2(SD_CMD_SEND_IF_COND, 0x1AA ,0x87, SD_ANSWER_R7_EXPECTED);
	CS_ASSERT;
	SSP_Write(CMD_DUMMY_BYTE);

	sd_print_responde(replyBytes);


	if (response  == SD_R1_IN_IDLE_STATE)
	{
		/*checks SD_CMD_SEND_IF_COND  response*/
		if (! (replyBytes[3] == 0x01 && replyBytes[4] == 0xAA))
		{
			return (3);
		}

		SD_DEBUG("SD_CMD_APP_CMD(sequence) CMD45  \r\n");
		do
		{
			memset(replyBytes,0 ,sizeof(replyBytes));

/*trasmi the SD_CMD_SD_APP_OP_COND*/

			response = SD_SendCmd_2(SD_CMD_APP_CMD, 0, 0xFF, SD_ANSWER_R1_EXPECTED);
			CS_ASSERT;
			SSP_Write(CMD_DUMMY_BYTE);
			//sd_print_responde(replyBytes);

			memset(replyBytes,0 ,sizeof(replyBytes));
			response = SD_SendCmd_2(SD_CMD_SD_APP_OP_COND, 0x40000000,0xFF,SD_ANSWER_R1_EXPECTED);
			CS_ASSERT;
			SSP_Write(CMD_DUMMY_BYTE);
			//sd_print_responde(replyBytes);

			if (retry++ > 0x20){
				return 1;
			}
		} while (response == SD_R1_IN_IDLE_STATE);


		//SD_DEBUG("SD_CMD_READ_OCR CMD58 \r\n");


		memset(replyBytes,0 ,sizeof(replyBytes));

		response = SD_SendCmd_2(SD_CMD_READ_OCR, 0x00000000, 0xFF, SD_ANSWER_R3_EXPECTED);
		CS_ASSERT;
		SSP_Write(CMD_DUMMY_BYTE);

		//response = SD_sendCommand(SD_CMD_READ_OCR, 0X00,SD_ANSWER_R3_EXPECTED);
		sd_print_responde(replyBytes);



		if ((replyBytes[1] & 0x40) != 0)
			bitShifter = 0;
		else
			bitShifter = 9;



	}
	else
	{
		SD_DEBUG(" CMD55 \r\n");
		do
		{
			/*
			CMD55, must be sent before any ACMD command
			*/
			response = SD_sendCommand(APP_CMD,0,SD_ANSWER_R1_EXPECTED);
			/*
			 CMD41
			*/
			response = SD_sendCommand(SD_CMD_SD_APP_OP_COND,0x0,SD_ANSWER_R3_EXPECTED);
			if (retry++ > MAX_TRIES_PER_COMMAND)
				return 2;
		} while(response != 0x00);
	}
	CS_DEASSERT;
	return 0;
}

unsigned char SD_sendCommand(unsigned char cmd, unsigned long arg, uint8_t answer)
{
	unsigned char response = 0xFF;
	unsigned char retry = 0;

	if (( cmd == READ_SINGLE_BLOCK) ||
	    ( cmd == READ_MULTIPLE_BLOCKS) ||
	    ( cmd == WRITE_SINGLE_BLOCK) ||
	    ( cmd == WRITE_MULTIPLE_BLOCKS) ||
	    ( cmd == ERASE_BLOCK_START_ADDR) ||
	    ( cmd == ERASE_BLOCK_END_ADDR) )
		arg = arg << bitShifter;

	CS_DEASSERT;

	SSP_Write(cmd | 0x40);
	SSP_Write(arg >>24);
	SSP_Write(arg >>16);
	SSP_Write(arg >>8);
	SSP_Write(arg);

	switch (cmd  ) {
	case SD_CMD_APP_CMD :
			SSP_Write(0xFF);
			break;
	case SD_CMD_SEND_IF_COND:
			SSP_Write(0x87);
		break;

	case SD_CMD_SD_APP_OP_COND:
			SSP_Write(0X01 );
		break;

	case SD_CMD_GO_IDLE_STATE:
			SSP_Write(0x95 );
	 	break;
	default :
			SSP_Write(0x01);
		break;

	}



        memset(replyBytes, 0, sizeof(replyBytes));



	while((response == 0xFF))// || (response == 0x3F))
	{
		 response = SSP_Read();

		 if (retry++ > 0xfe){
			 SD_DEBUG("max retry reached for send command %x\r\n", response);
			 break;
		}
	}

	replyBytes[0] = response;
	replyBytes[1] = SSP_Write(CMD_DUMMY_BYTE);
	replyBytes[2] = SSP_Write(CMD_DUMMY_BYTE);
	replyBytes[3] = SSP_Write(CMD_DUMMY_BYTE);
	replyBytes[4] = SSP_Write(CMD_DUMMY_BYTE);



	return response;
}

uint8_t SD_WaitData(uint8_t data)
{
  uint16_t timeout = 0xFFFF;
  uint8_t readvalue;

  /* Check if response is got or a timeout is happen */

  do {
    readvalue = SSP_Write(CMD_DUMMY_BYTE);
    timeout--;
  }while ((readvalue != data) && timeout);

  if (timeout == 0)
  {
    /* After time out */
    return 1;
  }

  /* Right response got */
  return 0;
}

uint8_t SD_SendCmd_2(uint8_t Cmd, uint32_t Arg, uint8_t Crc, uint8_t Answer)
{
	uint8_t frame[SD_CMD_LENGTH];
	uint8_t frameout[SD_CMD_LENGTH];
	uint8_t retr;


	frame[0] = (Cmd | 0x40);         /* Construct byte 1 */
	frame[1] = (uint8_t)(Arg >> 24); /* Construct byte 2 */
	frame[2] = (uint8_t)(Arg >> 16); /* Construct byte 3 */
	frame[3] = (uint8_t)(Arg >> 8);  /* Construct byte 4 */
	frame[4] = (uint8_t)(Arg);       /* Construct byte 5 */
	frame[5] = (Crc | 0x01);         /* Construct byte 6 */

	/* Send the command */
	CS_DEASSERT;
	HAL_SPI_TransmitReceive(&hspi1,frame, frameout, SD_CMD_LENGTH, 1000);
	/* Send the Cmd bytes */

	switch(Answer)
	{
	case SD_ANSWER_R1_EXPECTED :
		replyBytes[0] = SSP_Read();
		break;
	case SD_ANSWER_R1B_EXPECTED :
		replyBytes[0] = SSP_Read();
		replyBytes[1] = SSP_Write(CMD_DUMMY_BYTE);
		/* Set CS High */
		CS_ASSERT;
		HAL_Delay(1);
		/* Set CS Low */
		CS_DEASSERT;

		/* Wait IO line return 0xFF */
		while (SSP_Write(CMD_DUMMY_BYTE) != 0xFF);
		break;
	case SD_ANSWER_R2_EXPECTED :
		replyBytes[0] = SSP_Read();
		replyBytes[1] = SSP_Write(CMD_DUMMY_BYTE);
		break;
	case SD_ANSWER_R3_EXPECTED :

	case SD_ANSWER_R7_EXPECTED :
		replyBytes[0] = SSP_Read();
		replyBytes[1] = SSP_Write(CMD_DUMMY_BYTE);
		replyBytes[2] = SSP_Write(CMD_DUMMY_BYTE);
		replyBytes[3] = SSP_Write(CMD_DUMMY_BYTE);
		replyBytes[4] = SSP_Write(CMD_DUMMY_BYTE);
		break;
	default :
		break;
	}
	retr = replyBytes[0];
	return retr;
}

unsigned char SD_readSingleBlock(unsigned long startBlock, char *data)
{
	unsigned char response;

	uint8_t dummy_write[512+1];
	memset(dummy_write, CMD_DUMMY_BYTE, sizeof(dummy_write));
	CS_ASSERT;
	response = SD_SendCmd_2(READ_SINGLE_BLOCK, startBlock,0xFF,SD_ANSWER_R1_EXPECTED);

	// CS_ASSERT;
	// SSP_Write(CMD_DUMMY_BYTE);

	if (response != 0x00)
	{
		CS_DEASSERT;
		return response;
	}
#if 1

	if (SD_WaitData(SD_TOKEN_START_DATA_SINGLE_BLOCK_READ) == 0)
	{
		/* Read the SD block data : read NumByteToRead data */

		HAL_SPI_TransmitReceive(&hspi1,dummy_write, data, 512, 1000);

		/* Set next read address*/

		/* get CRC bytes (not really needed by us, but required by SD) */
		SSP_Write(CMD_DUMMY_BYTE);
		SSP_Write(CMD_DUMMY_BYTE);
	}
	else
	{	SD_DEBUG("TOKEN got response != SD_R1_NO_ERROR %x  \r\n",response);
		return 4;
	}

	CS_ASSERT;
	SSP_Write(CMD_DUMMY_BYTE);
#else
	retry = 0;
	do
	{
		response = SSP_Read();
		if (retry++ > 0xFFFE)
		{
			CS_DEASSERT;
			return 1;
		}
	} while(response != 0xfe);
	SD_DEBUG("reading bytes  \r\n");
	for(i=0; i<512; i++)
		data[i] = SSP_Read();

	SSP_Read();
	SSP_Read();
	SSP_Read();
	CS_DEASSERT;
        //SD_DEBUG("Read ok");
#endif
	return 0;
}

unsigned char SD_writeSingleBlock(unsigned long startBlock, char *Data)
{
	unsigned char response;
	unsigned int i, retry=0;
	CS_ASSERT;
	response = SD_sendCommand(WRITE_SINGLE_BLOCK, startBlock,SD_ANSWER_R1_EXPECTED);
	if (response != 0x00)
	{
		CS_DEASSERT;
		return response;
	}
	SSP_Write(0xfe);
	for(i=0; i<512; i++)
		SSP_Write(Data[i]);
	SSP_Write(CMD_DUMMY_BYTE);
	SSP_Write(CMD_DUMMY_BYTE);
	do
	{
		response = SSP_Read();
		if (retry++ > 0xfff)
		{
			CS_DEASSERT;
			return 2;
		}
	}
	while( (response & 0x1f) != 0x05);
	retry = 0;
	while(!SSP_Read())
	{
		if (retry++ > 0xfffe)
		{
			CS_DEASSERT;
			return 3;
		}
                HAL_Delay(5);
	}
	CS_DEASSERT;
	return 0;
}
void change_sd_speed(unsigned long freq)
{
	change_ssp_freq(freq);
}
