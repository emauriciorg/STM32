//#ifndef

#define GO_IDLE_STATE            0
#define SEND_OP_COND             1
#define SEND_IF_COND		 8
#define SEND_CSD                 9
#define STOP_TRANSMISSION        12
#define SEND_STATUS              13
#define SET_BLOCK_LEN            16
#define READ_SINGLE_BLOCK        17
#define READ_MULTIPLE_BLOCKS     18
#define WRITE_SINGLE_BLOCK       24
#define WRITE_MULTIPLE_BLOCKS    25
#define ERASE_BLOCK_START_ADDR   32
#define ERASE_BLOCK_END_ADDR     33
#define ERASE_SELECTED_BLOCKS    38
#define SD_SEND_OP_COND		 41
#define APP_CMD			 55
#define READ_OCR		 58
#define CRC_ON_OFF               59


typedef enum {
SD_ANSWER_R1_EXPECTED,
SD_ANSWER_R1B_EXPECTED,
SD_ANSWER_R2_EXPECTED,
SD_ANSWER_R3_EXPECTED,
SD_ANSWER_R4R5_EXPECTED,
SD_ANSWER_R7_EXPECTED,
}SD_Answer_type;


unsigned char SD_init(void);

unsigned char SD_readSingleBlock(unsigned long startBlock, char *Data);
unsigned char SD_writeSingleBlock(unsigned long startBlock, char *Data);
void change_sd_speed(unsigned long);

unsigned char SD_sendCommand(unsigned char cmd, unsigned long arg, uint8_t answer);


uint8_t SD_SendCmd_2(uint8_t Cmd, uint32_t Arg, uint8_t Crc, uint8_t Answer);
