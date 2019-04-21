#ifndef _FAT32_H_
#define _FAT32_H_

/*
 *       @author:  mauricio.rios@titoma.com
 *
 *
 *
 */
#include  "stm32f0xx_hal.h"
#include  "sdio.h"
#include  <string.h>
#define MAX_NUMBER_BIN_FILES   16
#define LOG_FILE_NAME "FLASHINGLOG.TXT"//CSV

//#define FAT32_DEBUG
#define LONG_ENTRY_MARK_1 0X42
#define LONG_ENTRY_MARK_2 0X01
/*
*  @TO DO:  -check for errors in the reading/writing proccess, if any error occurs
*           then use the return as the error flag
*
*           -use union and block pointer  to avoid memory consumption eg data_block[512]
*           -use data structure to make a direct asignation from the block reading
*/

#define WRITE_READ_ADDR     ((uint32_t)0x0800)

#define SDRAM_BANK_ADDR                 ((uint32_t)0xD0000000)




#define WRITE_ALLOWED
//#define NO_WRITE_ALLOWED

#define BOOT_SECTOR_ADDRESS  0X00 //if not other partition is present
#define BLOCK_SIZE_IN_1_BYTE  512   //usual block size in bytes of fat32
#define BLOCK_SIZE_IN_4_Bytes   128
#define BITS_PER_PHY_ADDR  31


#define SHORT_FILE_NAME_SIZE    12
#define EMPTY_ROOT_ENTRY       0xE5000000
#define END_OF_FILE_AT_CLUSTER 0x0FFFFFFF
#define FAT32_EMPTY_BYTE          0X00
#define MAXIMUM_BYTES_PER_NAME          7  //not including the extention .txt

#define SINGLE_BLOCK                 1

#define LONG_NAME_MAX_SIZE 42
/*
*       FAT32 STRUCTURES
*/
typedef struct{

 uint8_t FileName[12];
 uint16_t u16_file_size;
}st_file_DirEntry;



typedef struct {
                 uint16_t  BPB_BytesPerSec;
                 uint8_t   BPB_SecPerClus;
                 uint16_t  BPB_RsvdSecCnt;
                 uint8_t   BPB_FSInfo;
                 uint8_t   BPB_NumFATS;
                 uint32_t  BPB_FATSz32;
                 uint32_t  BPB_RootClus;
                 uint16_t  BPB_signature;
}BPB_boot_sector;

enum{
    false,
    true
};

/*short file entry*/
typedef struct {

      char sfn[12];
    union {
          uint8_t  ATTR_LONG_NAME;

          struct {
                  uint8_t
                         ATTR_READ_ONLY:1,
                         ATTR_HIDDEN:1,
                         ATTR_SYSTEM:1,
                         ATTR_VOLUME_ID:1,
                         ATTR_DIRECTORY:1,
                         ATTR_ARCHIVE:1;

          }st_DIR_Attr;


    }un_DIR_Attr;

    uint8_t  NTRes;
    uint8_t  CrtTimeTenth;

    uint16_t CrtTime;
    uint16_t CrtDate;
    uint16_t LstAccDate;
    uint16_t FstClusHI;
    uint16_t WrtTime;
    uint16_t WrtDate;
    uint16_t FstClusLO;

    uint32_t FileSize;
    uint32_t cluster_addr;
    char  lfn[LONG_NAME_MAX_SIZE];

    uint8_t  lfn_flag;

}st_file_entry;

/*long file entry*/

typedef struct {
	char name[11];
	char dummy_fixed; //0x0f
	char dummy_zero; //0x00
	char check_sum; //
	char name_last_char[2];
} st_long_directory_entry_name;

typedef struct {
	uint16_t name[8];
} st_long_directory_entry_meta_data;

typedef union {
	unsigned char long_directory_entry[100];
	struct {
  			st_long_directory_entry_name        last_entry;
			st_long_directory_entry_meta_data   last_meta;
			st_long_directory_entry_name        midlde_name;
			st_long_directory_entry_meta_data   middle_meta;
			st_long_directory_entry_name        first_name;
			st_long_directory_entry_meta_data   first_meta;
			uint32_t cluster_addr;
	} long_directy   ;
}st_long_directory_entry;



/*typedef struct {

    uint8_t file_name[12];//"filename.txt "
    uint8_t  DIR_Attr ;

    uint16_t FstClusHI;
    uint16_t FstClusLO;
    uint32_t FileSize;
    uint32_t cluster_addr;
}simple_file_entry;
*/

typedef struct {
                 union {
                          uint64_t u64_vol_name;
                          uint8_t  u8_vol_name[11];
                 }un_volume_label;

                 uint32_t lba_address;
                 uint32_t Fat1Addr;
                 uint32_t fat_two_address;
                 uint32_t RootDirAddr;
                 uint32_t RootDirStartSector;
                 uint32_t FileCntr;
                 uint32_t ClstrSize;
                 //st_file_entry DirEntry;
                 BPB_boot_sector BPB_boot_partition_block;//boot partition block

				 //ADRESS OF THE BUFFER TO SAVE
				 char  *ptr_data_to_save;
				 uint8_t Ready;

}st_fat32;

/*
 *
 *   NOT FAT32 STRUCTURES  ,
 *
 */


/*typedef struct{
            uint8_t   FileName[12];
            uint8_t   lfn[LONG_NAME_MAX_SIZE];
            uint8_t   lfn_flag;
            uint32_t  FileSize;
            uint32_t  cluster_addr;
            //MISSING NUMBER OF FILES
}st_file_entry;
*/

typedef struct{
            uint32_t  esp_address;
            uint32_t  FileSize;
            uint32_t  SdramAddr;
}fileEntry;


/*
    FAT32 FUNCTIONS

*/


uint8_t fat32_init(st_fat32 *sd_fat32);

uint8_t fat32_get_file_system_type(st_fat32 *sd_fat32); //not implemented


/*
 * Fat32 functions  to get main  fat32  addresses
 *
 *
 *
 */

uint8_t fat32_get_boot_sector ( st_fat32     *sd_fat32);
void fat32_get_first_sector( st_fat32     *sd_fat32);
void fat32_get_fat1_address( st_fat32     *sd_fat32);
void fat32_get_fat2_address( st_fat32     *sd_fat32);
void fat32_get_RootDirAddr(st_fat32    *sd_fat32);

/*
 * Fat32 functions  to get first block of the main sectors
 *
 * Mostly used on debug
 *
 */

uint16_t GetLastPosition(uint32_t *block,uint32_t  *LastPosition);

void fat_32_get_fat1_entry(st_fat32 *sd_fat32);
uint8_t fat32_get_door_entry(st_fat32 *sd_fat32);
char  fat32_directory_entry_assignation(uint32_t *block_data ,  st_file_entry *DirEntry , uint32_t RowCounter);

void fat32_append_data(st_fat32 *file_system ,uint32_t cluster_number, char  *output_buffer  );
/*
*
*   Functions used for the the read/write file
*   In the proccess , data sector, fat_one and root_directory are
*   touched
*/


void fat32_get_cluster_list_for_file(uint32_t file_entry, uint32_t *cluster_list, uint32_t file_number, st_fat32  *sd_fat32);
uint32_t fat32_get_cluster_address_from_file(st_fat32 *file_system, char *file_entry_name,uint32_t *fileSize);
uint8_t fat32_search_last_entry(uint32_t *temp_buffer, st_fat32 *sd_fat32, uint32_t *last_file_cluster_address, uint32_t *file_size);




uint8_t fat32_read_file_directory_entry(st_fat32 *file_system , st_file_entry *file_entry, char  *output_buffer);

void fat32_read_file_directory_entryFromFat(st_fat32 *file_system ,uint32_t cluster_number , char  *output_buffer );
void fat32_read_cluster(st_fat32 *sd_fat32, uint32_t u32_cluster ,char  *output_buffer  );

uint8_t fat32_append_to_file(st_fat32 *file_system,char    *FileName,  const uint8_t *InData  );

uint8_t fat32_list_files_by_extention(st_fat32 *file_system, st_file_entry   *pfile_entries, const char *file_extention);



uint8_t fat32_convert_file_name(char *file_name_with_dot_extention_input, char *file_name_formatted_output);
uint8_t fat32_format_file_name(char *file_name_dot, char *file_name_space);

/*      FAT32 modifiers
*           Erase entry : prototype is just deleting the root directory entry
*
*/

uint8_t EraseEntry(	st_fat32 *sd_fat32, char 	*file_name);


uint8_t fat32_write_to_file(st_fat32 *sd_fat32,char   *file_name,char *buffer_input );
uint8_t fat32_write_fat1(uint32_t *block_to_read, st_fat32 *file_system, uint32_t *Fat1Pos, uint32_t *fat32_address, uint32_t *FileSize);
uint8_t fat32_write_directory_entry( st_fat32 *sd_fat32, char   *file_name, char *buffer_input, uint32_t *file_size, uint32_t *cluster_location);
uint8_t fat32_write_data( st_fat32 *file_system, const  char *InputData, uint32_t *ClusterNumber);
uint8_t fat32_read_data( st_fat32 *file_system, uint32_t *OutData, uint32_t *ClusterNumber);

//long file names

char  fat32_check_directory_entry_type(uint32_t *block_data );
void fat32_get_lfn(uint32_t *pblock_data,  char *long_name );

char fat32_check_file_entry(st_fat32 *file_system, st_file_entry   *pfile_entry, char *requested_file, uint32_t *sector, uint32_t *block_pos);
uint8_t fat32_update_directory_entry( st_fat32 *file_system, st_file_entry  *new_entry,  uint32_t FileSize, uint32_t block_addr, uint32_t block_pos);


char fat32_assign_sfn(uint32_t *block_data ,  st_file_entry *DirEntry);
#ifdef FAT32_DEBUG

	uint8_t print_block	( uint32_t *block_to_print, uint8_t format_to_print );

 #endif

#endif
