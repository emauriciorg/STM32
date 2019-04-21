
#define TRUE 1
#define FALSE 0
#define COMPLETE 0
#define INCOMPLETE -6
#define INVALID_COMMAND -8

#define FILE_FOUND 		0
#define FILE_SAVE_COMPLETE	0x01
#define FILE_NOT_FOUND 		-200
#define FILE_DONE 		-201
#define FILE_DELETED 		-202
#define NO_FREE_SECTORS 	-203
#define NO_INDEX_SPACE 		-204
#define SD_CARD_NOT_FORMATTED 	-205
#define CANNOT_READ_SD_CARD	-206
#define FAILURE_TO_READ		-207
#define INCORRECT_BLOCK         -208
#define WRITE_SEC0_FAILURE       -209
#define WRITE_IDX_FAILURE       -210
#define BLOCK_WRITE_FAILURE     -211


#define SD_INTIALIZATION_FAILED -7

#define FILE_NAME_START 0
#define FILE_SIZE_START 13
#define FILE_SECTOR_START 18
#define FILE_BLOCKWRITTEN_START 23
#define FILE_ACTIVE_START 28
#define FILE_STATUS_START 30

#define MEMORY_SIZE 0X80000000
#define HEADER_SECTOR 1
#define HEADER_SECTOR_START 0
#define INDEX_SECTORS 32
#define INDEX_SECTOR_START 1
#define FILE_INDEX_SIZE 64
#define DATA_SECTOR (((MEMORY_SIZE/SECTOR_SIZE)) - INDEX_SECTORS - HEADER_SECTOR)
#define DATA_SECTOR_START INDEX_SECTORS + HEADER_SECTOR
#define SECTOR_SIZE 512
#define MAX_FILES (INDEX_SECTORS*SECTOR_SIZE/FILE_INDEX_SIZE)

/*structure added by Mauro*/
typedef struct {
                 uint16_t  BPB_BytesPerSec;
                 uint8_t   BPB_SecPerClus;
                 uint16_t  BPB_RsvdSecCnt;
                 uint8_t   BPB_FSInfo;
                 uint8_t   BPB_NumFATS;
                 uint32_t  BPB_FATSz32;
                 uint32_t  BPB_RootClus;
                 uint16_t  BPB_signature;
}BPB_boot_sector_t;


struct FileDetails
{
	char fileName[12];
	unsigned long fileSize;
	unsigned long startSector;
	unsigned long blockWrittern;
	unsigned char fileActive;
	unsigned char fileStatus;
	unsigned short timesAccessed;
};
unsigned char init_FAT(void);
int retrieveFile(char *);
int retrieveFileBlock(int , char *, int , char *);
long get_file_size(int ,char *);
int retrieveHeader(char *);
int saveFileBlock(char *, char *, unsigned char, long );
int saveFileHeader(char *, unsigned long );
long get_blocks_written(int ,char *);
unsigned char get_file_complete_status(int ,char *);
int formatDrive(void);
char *errorFormat(int );
