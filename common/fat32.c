/*
* 	author: mauro r.
*      GET THE ADDRESSES FOR THE FAT TABLES, DIRECTORY ENTRY AND BOOT SECTOR
*       well know bugs:
*        1. Only short file names
*        2. Issues when writing outsite of the first file directory cluster
*        3. detect fat32 or fat16 etc NOT IMPLEMNTED
*        4.ONLY READS IF THE UNIT IS DEFRAGMENTEC ,(files follow a continuos addres on fat32)
*
*/


#include "fat32.h"


#ifdef FAT32_DEBUG
#warning "DEBUG MODE ON FAT32.H IS SET"
	#include "debugsd.h"
extern st_str                  uart_tx;
#else
	#define SDBG(...)
	#define SDBG1(...)
#endif

/****************************************************************************************************************
* Function name :
* Porpuse       :
* Returns       :
****************************************************************************************************************/

uint8_t fat32_get_boot_sector(st_fat32 *file_system)
{
uint32_t block_data[BLOCK_SIZE_IN_4_Bytes];

if ( !SD_read_block (  block_data , 0  )) return false;

file_system->lba_address  = BLOCK_SIZE_IN_1_BYTE* ((  block_data[113]>>16 ) &  0xffff );


if ( !SD_read_block( block_data,  file_system->lba_address)) return false;


file_system->BPB_boot_partition_block.BPB_BytesPerSec  = ((block_data[ 3]        &0xf)<<8)  | (block_data[2] &  0xff000000)>>24;
file_system->BPB_boot_partition_block.BPB_SecPerClus   =  (block_data[ 3] >>8)   &0xff;
file_system->BPB_boot_partition_block.BPB_RsvdSecCnt   =  (block_data[ 3] >>16)  &0xffff;
file_system->BPB_boot_partition_block.BPB_NumFATS      =   block_data[ 4];
file_system->BPB_boot_partition_block.BPB_FATSz32      =   block_data[ 9]        &0xffff;
file_system->BPB_boot_partition_block.BPB_RootClus     =   block_data[11];


return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------


void fat32_get_fat1_address( st_fat32 *file_system )
{

file_system->Fat1Addr  =  file_system->lba_address+( (file_system->BPB_boot_partition_block.BPB_RsvdSecCnt)*BLOCK_SIZE_IN_1_BYTE);
}


void fat32_get_fat2_address(st_fat32 *file_system)
{
file_system->fat_two_address   =  file_system->lba_address+ (BLOCK_SIZE_IN_1_BYTE*(file_system->BPB_boot_partition_block.BPB_RsvdSecCnt + file_system->BPB_boot_partition_block.BPB_FATSz32 * (file_system->BPB_boot_partition_block.BPB_NumFATS) / 2)+ file_system->lba_address);
}


void fat32_get_RootDirAddr(st_fat32 *file_system)
{
file_system->RootDirAddr     = file_system->lba_address+((BLOCK_SIZE_IN_1_BYTE*( file_system->BPB_boot_partition_block.BPB_RsvdSecCnt + file_system->BPB_boot_partition_block.BPB_FATSz32 * (file_system->BPB_boot_partition_block.BPB_NumFATS))));
file_system->RootDirStartSector      = (file_system->BPB_boot_partition_block.BPB_RsvdSecCnt +  (file_system->BPB_boot_partition_block.BPB_NumFATS* file_system->BPB_boot_partition_block.BPB_FATSz32)) ;
}


uint8_t fat32_init (st_fat32 *file_system)
{

	if ( !fat32_get_boot_sector(file_system) )  return false;

	fat32_get_fat1_address (file_system);
	fat32_get_fat2_address(file_system);
	fat32_get_RootDirAddr(file_system);
	file_system->ClstrSize =   (file_system->BPB_boot_partition_block.BPB_BytesPerSec)*(file_system->BPB_boot_partition_block.BPB_SecPerClus);
	fat32_get_door_entry (file_system);

	return true;
}

#define SIZE_OF_LONG_ENTRY 4
static  uint32_t temp_block_data[ SIZE_OF_LONG_ENTRY ]; //memset(temp_block_data, 0,sizeof(temp_block_data));
/*read the given row from the directory entry checks for the two fixed long name value (0x42,0x01) if found long name then assigns the
short name to the short name entry and the long entry name to the long name */
char  fat32_directory_entry_assignation(uint32_t *block_data ,  st_file_entry *DirEntry , uint32_t entry_index)
{

	static char flag_pending_entry=0;
	static uint8_t cpy_index=0;


	char temp_name[32];
	uint8_t find_long_name_file=8;	//VERY REDUNDANT RETURN
	DirEntry->lfn_flag=false;

#ifdef DIR_OUTSIDE_BLOCK

	if( (entry_index+16) < BLOCK_SIZE_IN_4_Bytes){

		if (fat32_check_directory_entry_type(block_data+ entry_index)){
			//SDBG1("found long name!!!\n");
			fat32_get_lfn ((block_data+ entry_index), DirEntry->lfn );
			DirEntry->lfn_flag=true; //VERY REDUNDANT RETURN
			find_long_name_file     =24;
			(entry_index)+=16;
		}
	}else{
		SDBG1 ("entry_index in bits   [%d] \n\
			entry_index in bytes  [%d] \n\
			BLOCK_SIZE_IN_32_Bits [%d] \n\
			BLOCK_SIZE_IN_Bytes   [%d] \n\
			Pending data in bits  [%d] \n\
			Pending data in bytes [%d] \n",
			entry_index,
			entry_index*4,
			BLOCK_SIZE_IN_4_Bytes ,
			BLOCK_SIZE_IN_4_Bytes*4,
			BLOCK_SIZE_IN_4_Bytes-entry_index,
			((BLOCK_SIZE_IN_4_Bytes-entry_index)*4));
	}

#else

	if(flag_pending_entry){
		//read and join pending entry (reading entry between blocks, each entry is max 96 bytes)
		//SDBG1("\n\n BEFORE JOIN \n");
		//print_block(temp_block_data,'x'); //PRINT THE BLOCK HERE (BLCOK THAT GOT SAVED)
		//
		//SDBG1("\n\n BLOCK TO JOIN \n");
		//print_block(block_data,'x'); //PRINT THE BLOCK HERE (BLCOK THAT GOT SAVED)

		memcpy(&temp_block_data[cpy_index/4],(block_data), 96-cpy_index);

	//	SDBG1("\n\n AFTER JOIN \n");
		//SDBG1("join of two blocks to get name %s\n",temp_block_data);
	 	//print_block(temp_block_data,'x'); //PRINT THE BLOCK HERE (BLCOK THAT GOT SAVED)

		if (fat32_check_directory_entry_type(temp_block_data)){

			fat32_get_lfn (temp_block_data, DirEntry->lfn );
		//	SDBG1("\npending lfn file is %s\n",DirEntry->lfn);
			DirEntry->lfn_flag=true;
			find_long_name_file= (96/4) - (cpy_index/4);
		}

		fat32_assign_sfn(&(temp_block_data[16]), DirEntry);
		//SDBG1("\npending sfn file is %s\n",DirEntry->sfn);
		memset(temp_block_data, 0,sizeof(temp_block_data));
		flag_pending_entry=0;

		return find_long_name_file;

	}
	//lfn entry is 8 posisitions of  32bit

	if( (entry_index+16) < BLOCK_SIZE_IN_4_Bytes){
		if (fat32_check_directory_entry_type(block_data+ entry_index)){

			fat32_get_lfn ((block_data+ entry_index), DirEntry->lfn );
			DirEntry->lfn_flag=true; //VERY REDUNDANT RETURN
			find_long_name_file     =24;
			(entry_index)+=16;
		}

	}
	else{
		//
		//SDBG1 ("\n extract long name pending\n");
		//SDBG1("\n\n BEFORE COPY \n");
		//print_block(block_data+entry_index,'x'); //PRINT THE BLOCK HERE (BLCOK THAT GOT SAVED)
		flag_pending_entry=1;
		cpy_index=(BLOCK_SIZE_IN_4_Bytes-entry_index)*4;/*get the current size of the pending entry*/
		memcpy(temp_block_data, (block_data+entry_index),  cpy_index);/*copy the incomplete part of the pending file*/
		memset(DirEntry->sfn, 0, sizeof(DirEntry->sfn));/*set file name to zero, so the upper function can detect file*/

		//SDBG1("\n\n AFTER COPY \n");
		//print_block(temp_block_data,'x'); //PRINT THE BLOCK HERE (BLCOK THAT GOT SAVED)
/*
		SDBG1 ("entry_index in bits  [%d] \
			       entry_index in bytes  [%d] \n\
			       BLOCK_SIZE_IN_32_Bits [%d] \n\
			       BLOCK_SIZE_IN_Bytes   [%d] \
			       Pending data in bits  [%d] \n\
			       Pending data in bytes [%d] \n",
			entry_index,
			entry_index*4,
			BLOCK_SIZE_IN_4_Bytes ,
			BLOCK_SIZE_IN_4_Bytes*4,
			BLOCK_SIZE_IN_4_Bytes-entry_index,
			((BLOCK_SIZE_IN_4_Bytes-entry_index)*4));
*/
		return find_long_name_file;

	}

#endif
	//get shortfile_name
	fat32_assign_sfn(&block_data[ entry_index ],DirEntry);
	/*

	memcpy(DirEntry->sfn,      &block_data[ entry_index   ],  7);
	memcpy(&DirEntry->sfn[8],  &block_data[ (entry_index)+2 ],  3);


	DirEntry->sfn[7]='.';
	DirEntry->sfn[11]=0;

	DirEntry->un_DIR_Attr.ATTR_LONG_NAME =  (block_data[(entry_index) + 2] >>24)   &0xff  ;
	DirEntry->NTRes                  =   block_data[(entry_index) + 3]        &0xff  ;
	DirEntry->CrtTimeTenth           =  (block_data[(entry_index) + 3] >>8)   &0xff  ;
	DirEntry->CrtTime                =  (block_data[(entry_index) + 3] >>16)  &0xffff;
	DirEntry->CrtDate                =  (block_data[(entry_index) + 4]     )  &0xffff;
	DirEntry->LstAccDate             =  (block_data[(entry_index) + 4] >>16)  &0xffff;
	DirEntry->FstClusHI              =   block_data[(entry_index) + 5]        &0xffff;
	DirEntry->WrtTime                =  (block_data[(entry_index) + 5] )>>16  &0xffff;
	DirEntry->WrtDate                =  (block_data[(entry_index) + 6] )      &0xffff;
	DirEntry->FstClusLO              =  (block_data[(entry_index) + 6] >>16)  &0xffff;
	DirEntry->FileSize               =  (block_data[(entry_index) + 7] )         ;
	DirEntry->cluster_addr           =  ((((uint32_t)(DirEntry->FstClusHI))<<16) | DirEntry->FstClusLO);
	*/
	return find_long_name_file;
}
char fat32_assign_sfn(uint32_t *block_data ,  st_file_entry *DirEntry){
	memcpy(DirEntry->sfn,      &block_data,  7);
	memcpy(&DirEntry->sfn[8],  &block_data[ +2 ],  3);


	DirEntry->sfn[7]='.';
	DirEntry->sfn[11]=0;

	DirEntry->un_DIR_Attr.ATTR_LONG_NAME =  (block_data[ 2] >>24)   &0xff  ;
	DirEntry->NTRes                  =   block_data[ 3]        &0xff  ;
	DirEntry->CrtTimeTenth           =  (block_data[ 3] >>8)   &0xff  ;
	DirEntry->CrtTime                =  (block_data[ 3] >>16)  &0xffff;
	DirEntry->CrtDate                =  (block_data[ 4]     )  &0xffff;
	DirEntry->LstAccDate             =  (block_data[ 4] >>16)  &0xffff;
	DirEntry->FstClusHI              =   block_data[ 5]        &0xffff;
	DirEntry->WrtTime                =  (block_data[ 5] )>>16  &0xffff;
	DirEntry->WrtDate                =  (block_data[ 6] )      &0xffff;
	DirEntry->FstClusLO              =  (block_data[ 6] >>16)  &0xffff;
	DirEntry->FileSize               =  (block_data[ 7] )         ;
	DirEntry->cluster_addr           =  ((((uint32_t)(DirEntry->FstClusHI))<<16) | DirEntry->FstClusLO);
	return 0;
}

uint8_t fat32_list_files_by_extention(st_fat32 *file_system, st_file_entry   *pfile_entries, const char *file_extention)
{

	uint32_t SectorCntr =0;
	uint32_t rowCntr =0;
	//uint32_t OrowCntr =0;

	uint32_t block_data[BLOCK_SIZE_IN_4_Bytes];
	st_file_entry single_file_entry;
	uint8_t max_tries=30;

	file_system->FileCntr=0;
//	SDBG1("[# ]   [ size ]     [File name]******************\n");

	for(SectorCntr=0;   SectorCntr  <=  file_system->BPB_boot_partition_block.BPB_SecPerClus;   SectorCntr++ ){

		SD_read_block(  block_data, (file_system->RootDirAddr)+ (BLOCK_SIZE_IN_1_BYTE*SectorCntr)  );
		for( rowCntr =0; (rowCntr< BLOCK_SIZE_IN_4_Bytes)&(max_tries!=0);max_tries-- ){

			memset(&pfile_entries[file_system->FileCntr] , 0, sizeof (st_file_entry));
			memset(&single_file_entry,0,sizeof(st_file_entry));

			if (!( block_data [rowCntr] ||( EMPTY_ROOT_ENTRY     == (block_data [rowCntr]&&EMPTY_ROOT_ENTRY) ) )){

				rowCntr      =   BLOCK_SIZE_IN_4_Bytes;
				SectorCntr   =   file_system->BPB_boot_partition_block.BPB_SecPerClus;
			 	continue;
			}
			rowCntr+= (fat32_directory_entry_assignation(block_data , &single_file_entry, rowCntr));

			if ((memcmp(&single_file_entry.sfn[8],file_extention,3 )) || (!single_file_entry.cluster_addr) ) continue;

			memcpy(&pfile_entries[  file_system->FileCntr  ], &single_file_entry,sizeof (st_file_entry));

			if (!pfile_entries[file_system->FileCntr].lfn_flag)
				memcpy(pfile_entries[file_system->FileCntr].lfn,single_file_entry.sfn, sizeof(single_file_entry.sfn));
#ifdef SHOW_LISTED_FILES
			SDBG1("%2d....%5d..... %s \n",
				file_system-> FileCntr,
				pfile_entries[  file_system->FileCntr  ].FileSize,
				pfile_entries[  file_system->FileCntr  ].lfn

				);
#endif
			file_system->FileCntr++;//only bin files

			if (file_system->FileCntr>MAX_NUMBER_BIN_FILES){
//				SDBG1 ("[ERROR] MAX NUMBER OF BIN FILES REACHED\n");
				return false;
			}
		}

	}



return false;
}

/**************************************************************************************************************************************/


uint8_t fat32_read_file_directory_entry(st_fat32 *file_system , st_file_entry *file_entry, char  *output_buffer)
{
	if (!file_entry->cluster_addr)return false;
	fat32_read_file_directory_entryFromFat( file_system,  file_entry->cluster_addr, output_buffer);

return true;
}


/**************************************************************************************************************************************/

uint32_t fat32_get_cluster_address_from_file(st_fat32 *file_system, char *file_entry_name,uint32_t *fileSize)
{

	uint32_t rowCntr            = 0;
	uint32_t block_data[BLOCK_SIZE_IN_4_Bytes];
	uint32_t LastFileEntryClusterAddr=0;
	char FileNameIn[12]     ;
	char FileNameOut[12]     ;
	st_file_entry DirEntry     ;

	fat32_convert_file_name(file_entry_name, FileNameIn);
	SD_read_block(block_data, file_system->RootDirAddr );
	for( rowCntr =0; rowCntr<= BLOCK_SIZE_IN_4_Bytes;)
	{

		if ( block_data [rowCntr] ||
			( EMPTY_ROOT_ENTRY     == (block_data [rowCntr]&&EMPTY_ROOT_ENTRY) ) )
		{
			rowCntr+= (fat32_directory_entry_assignation(block_data , &DirEntry, rowCntr));

			fat32_convert_file_name(DirEntry.sfn , FileNameOut);

			if (!( memcmp(FileNameOut,  FileNameIn, strlen((const char*)FileNameIn)) ))
			{
				rowCntr                    = BLOCK_SIZE_IN_4_Bytes;
				LastFileEntryClusterAddr   = DirEntry.cluster_addr   ;
				if ( fileSize!= 0)
					*fileSize=DirEntry.FileSize;

				break;
			}
		}
}
//	SDBG1 ("LastFileEntryClusterAddr %d ,%s\n",LastFileEntryClusterAddr,__FUNCTION__);

return LastFileEntryClusterAddr;
}
/**************************************************************************************************************************************/

void fat32_read_file_directory_entryFromFat(st_fat32 *file_system ,uint32_t cluster_number, char  *output_buffer  )
{

	uint32_t cnt     ;

	uint32_t PhyAdress ;
	uint32_t fat_one_data [ BLOCK_SIZE_IN_4_Bytes];
	fat32_read_cluster(file_system, cluster_number , &output_buffer[0]);

	PhyAdress= (file_system->Fat1Addr)+ (BLOCK_SIZE_IN_1_BYTE*(cluster_number/128));

	SD_read_block(fat_one_data, PhyAdress );

	for (cnt = cluster_number; cnt < BLOCK_SIZE_IN_4_Bytes; cnt++){

		if ( (( fat_one_data[cnt]  &   END_OF_FILE_AT_CLUSTER) == END_OF_FILE_AT_CLUSTER)
				|| (( fat_one_data[cnt]) == FAT32_EMPTY_BYTE))  {

				cnt= BLOCK_SIZE_IN_4_Bytes;
		}
		else{
			SDBG1 ("[reading cluster %d acerted]\n",cnt);
			fat32_read_cluster(file_system, fat_one_data[cnt],&output_buffer[(file_system->ClstrSize)*(cnt-cluster_number)+file_system->ClstrSize]);

		}
	}
}


/**************************************************************************************************************************************/

uint8_t fat32_get_door_entry(st_fat32 *file_system)
{

uint32_t block_data[BLOCK_SIZE_IN_4_Bytes];
SD_read_block(block_data, (file_system->RootDirAddr));
return false;
}
/**************************************************************************************************************************************/

void fat32_read_cluster(st_fat32 *file_system, uint32_t u32_cluster ,char  *output_buffer )
{

	uint32_t SectorCntr              =0;
	uint32_t PhyAdress              ;
	union{
		uint32_t u32_block_data[BLOCK_SIZE_IN_4_Bytes];
		uint8_t  u8_block_data[BLOCK_SIZE_IN_1_BYTE];
	}block_data;

	memset(block_data.u32_block_data,0,sizeof(block_data.u32_block_data));
	//PhyAdress=(RootDirStartSector)+ ((u32_cluster-2)*(BPB_SecPerClus)))*BLOCK_SIZE_IN_1_BYTE)+(lba_address);
	PhyAdress=(((file_system->RootDirStartSector)+ ((u32_cluster-2)*( file_system->BPB_boot_partition_block.BPB_SecPerClus)))*BLOCK_SIZE_IN_1_BYTE)+(file_system->lba_address);

	for(  SectorCntr=0 ; SectorCntr < file_system->BPB_boot_partition_block.BPB_SecPerClus ; SectorCntr++)
	{
		SD_read_block(block_data.u32_block_data, PhyAdress+(BLOCK_SIZE_IN_1_BYTE* SectorCntr));
		//SDBG1("[SDBG1 %s ]%s",__FUNCTION__, block_data.u8_block_data);
//#define FAT32_SHOW_FILE_CONTENT

		#ifdef FAT32_SHOW_FILE_CONTENT
			SDBG1("%s", block_data.u8_block_data);
		#endif
		memcpy(&output_buffer [BLOCK_SIZE_IN_1_BYTE*SectorCntr], block_data.u8_block_data,BLOCK_SIZE_IN_1_BYTE);
	}
}
/**************************************************************************************************************************************/



uint8_t EraseDirEntry( st_fat32 *file_system, char   *FileName)
{
	uint32_t            rowCntr   =8          ;
	uint32_t            block_data[BLOCK_SIZE_IN_4_Bytes];
	st_file_entry  DirEntry               ;

	//@ Delete entry
	#ifdef FAT32_DEBUG
		SDBG1("cluster %d and bytes %d   \n", file_system->BPB_boot_partition_block.BPB_BytesPerSec,
		file_system->BPB_boot_partition_block.BPB_SecPerClus);
	#endif

	if (  fat32_convert_file_name(FileName, DirEntry.sfn)){

		SD_read_block(block_data, (file_system->RootDirAddr) );

		while ((   (EMPTY_ROOT_ENTRY    !=   block_data[rowCntr] )
			&&(BLOCK_SIZE_IN_4_Bytes   >   rowCntr          ))){

			if (!memcmp( &block_data[rowCntr], DirEntry.sfn , MAXIMUM_BYTES_PER_NAME)  ){

				block_data[rowCntr]     =   EMPTY_ROOT_ENTRY;
				block_data[7+rowCntr]   =   0;
				fat32_directory_entry_assignation(block_data , &DirEntry , rowCntr);
			//	SD_write_block(block_data, (file_system->RootDirAddr));
				rowCntr= BLOCK_SIZE_IN_4_Bytes;

			}
			rowCntr+=8;
		}
	}
	/* @TO DO : DELETE addresses at FAT ENTRY*/
	return false;
}

/**************************************************************************************************************************************/

//uint8_t	 fat32_update_fat1()
uint8_t fat32_write_fat1(uint32_t *block_to_read, st_fat32 *file_system, uint32_t *Fat1Pos, uint32_t *fat32_address, uint32_t *FileSize)
{
	// @ cluster_location of the last file if the disk is defragmented;
	//
	SD_read_block(block_to_read,*fat32_address);
	SDBG1("[SDBG1]\nbyte_location");
	SDBG1("[SDBG1][FAT1POS %d][BLOCK(0) TO READ %d] [BLOCK(1) TO READ %d]\n", (*Fat1Pos),block_to_read[(*Fat1Pos)-2],block_to_read[(*Fat1Pos)-1]);
	block_to_read[*Fat1Pos]= 0xfffffff;

//	SD_write_block(block_to_read, *fat32_address );
	return false;
}

/**************************************************************************************************************************************/


uint8_t fat32_search_last_entry(uint32_t *temp_buffer,st_fat32 *file_system, uint32_t *last_file_cluster_address, uint32_t *FileSize)
{
	uint32_t rowCntrx =16;
	uint8_t FileEntryState=0;
	st_file_entry DirEntry;
	uint32_t block_data[BLOCK_SIZE_IN_4_Bytes];

	SD_read_block(block_data, (file_system->RootDirAddr));
	DirEntry.cluster_addr=0;

	for ( rowCntrx =32 ; rowCntrx<= (BLOCK_SIZE_IN_4_Bytes-8); rowCntrx+=8){

		if ( !((0x00             ==   block_data[rowCntrx]                   )
			|| (EMPTY_ROOT_ENTRY == (EMPTY_ROOT_ENTRY & block_data[rowCntrx]))  )){

			fat32_directory_entry_assignation(block_data , &DirEntry , rowCntrx);

			if ((*last_file_cluster_address) <  DirEntry.cluster_addr    ){

				*last_file_cluster_address     = DirEntry.cluster_addr ;
				*FileSize                     = DirEntry.FileSize  ;
				FileEntryState=1;
			}
		}
	}
	return FileEntryState;
}

/**************************************************************************************************************************************/

uint8_t Fat1EndOfFileCluster(uint32_t LastCluster, uint32_t *EndOfCluster, st_fat32 *file_system){

	union{
		uint32_t u32Data1[BLOCK_SIZE_IN_4_Bytes];
		uint8_t  u8Data1[BLOCK_SIZE_IN_1_BYTE];
	}Block1;

	SD_read_block(Block1.u32Data1, file_system->Fat1Addr );
	for(uint16_t cntr=LastCluster; cntr<512;){
	  //      printInt(cntr);printInt(Block1.u32Data1[cntr]);
		if ((Block1.u32Data1[cntr]& 0xFFFFFFF) == 0xFFFFFFF){
			SDBG1("[SDBG1]found  end \n");
			//printInt(cntr);
			//printInt(Block1.u32Data1[cntr]);
			*EndOfCluster=cntr;
			return 1;

		}else{
			cntr=Block1.u32Data1[cntr];
		}
	}
	return false;
}

/**************************************************************************************************************************************/

void FindNextAvailableCluster(st_fat32 *file_system ,uint32_t cluster_number, char  *output_buffer)
{



}
/**************************************************************************************************************************************/
uint8_t fat32_append_to_file(st_fat32 *file_system,char   *FileName, const uint8_t *InData )
{


	uint32_t fileSize;
	char Fat32FileName[12];
	union{
		uint32_t u32Data1[BLOCK_SIZE_IN_4_Bytes];
		char  u8Data1[BLOCK_SIZE_IN_4_Bytes];
	}Block1;

	uint32_t  ClusterAddress= fat32_get_cluster_address_from_file( file_system,  FileName,&fileSize);

	memset(Block1.u8Data1,0,sizeof(Block1));

	if (ClusterAddress==0)return false ;

	uint32_t EndOfCluster=0;
	uint32_t    LastClusterLenght = (strlen((const char *)Block1.u8Data1));;

	Fat1EndOfFileCluster(ClusterAddress , &EndOfCluster,file_system);

	fat32_read_cluster(file_system,EndOfCluster, Block1.u8Data1);//read cluster


	//printInt(LastClusterLenght);
	//printInt((strlen((const char *)&(Block1.u8Data1[LastClusterLenght]))));
	SDBG1("[SDBG1]\n[last bytes ");

	for (int x=30;x>=0;x--)
		SDBG1("[SDBG1]%d ",&(Block1.u8Data1[LastClusterLenght-x]));

	if (LastClusterLenght<sizeof(Block1)){

		if ((sizeof(Block1) -LastClusterLenght)> strlen((const char*)InData)){

			memcpy(&(Block1.u8Data1[LastClusterLenght]), InData,strlen((const char *)InData));
		//	printu(&(Block1.u8Data1[LastClusterLenght]));
			SDBG1("[SDBG1] block data%d ",&(Block1.u8Data1[LastClusterLenght]));

			if (!fat32_format_file_name(FileName, Fat32FileName) )
				return false;
			fat32_write_data( file_system, Block1.u8Data1 , &EndOfCluster);
					//LastClusterLenght=strlen((const char *)Block1.u8Data1) +fileSize;
					//fat32_write_directory_entry(file_system, Fat32FileName,  Block1.u8Data1, &LastClusterLenght, &EndOfCluster);

		}
	}

	return false;


	//    		 NewCluster = LastCluster+ ((FileSize/(file_system->ClstrSize)))+1;
	//check that current cluster has space
	//1.Find Clustert of the file, find last cluster of the file
	//2. get size of the input, write more than 1?
	//4check if data has been trasnfer all
	//5.final cluster reached? save position and search for a new space,
	//6.set all position with the new positio
	//check if ther is space on the file
	//        fat32_write_fat1(block_data.u32_block_data, file_system , &NewCluster, &Fat1AddrLastFile, &InFileSize);
	//			fat32_write_fat1(block_data.u32_block_data, file_system , &NewCluster, &Fat2AddrLastFile, &InFileSize);
	//	 fat32_write_data( file_system, InData , &NewCluster);
	// fat32_write_directory_entry(file_system, Fat32FileName,  InData, &InFileSize, &NewCluster);

}

/**************************************************************************************************************************************/

void fat32_append_data(st_fat32 *file_system ,uint32_t cluster_number, char  *output_buffer  )
{
	uint16_t cnt     ;
	uint32_t Index  =0;
	uint32_t PhyAdress ;
	uint32_t fat_one_datax [ BLOCK_SIZE_IN_4_Bytes];


	fat32_read_cluster(file_system, cluster_number , &output_buffer[0]);
	PhyAdress= (file_system->Fat1Addr)+ (BLOCK_SIZE_IN_1_BYTE*(cluster_number/128));
	Index= cluster_number- (BLOCK_SIZE_IN_4_Bytes * (cluster_number/ BLOCK_SIZE_IN_4_Bytes) );

	SD_read_block(fat_one_datax, PhyAdress );

	for (cnt = Index; cnt < BLOCK_SIZE_IN_4_Bytes; cnt++){

		if (    ((fat_one_datax[cnt]& END_OF_FILE_AT_CLUSTER) == END_OF_FILE_AT_CLUSTER)
			|| ((fat_one_datax[cnt]) == FAT32_EMPTY_BYTE))  {

			cnt= BLOCK_SIZE_IN_4_Bytes;

		}
		else{
			fat32_read_cluster(file_system, fat_one_datax[cnt],&output_buffer[(file_system->ClstrSize)*(cnt-Index)+file_system->ClstrSize]);
		}
	}
}

/**************************************************************************************************************************************/

//@write raw data in the data sector
uint8_t fat32_write_data( st_fat32 *file_system,  const char *InputData, uint32_t *ClusterNumber){

	uint32_t u32_cluster= *ClusterNumber;
	uint32_t SectorCntr              =0;
	uint32_t PhyAdress              ;

	uint16_t len_input;
	uint16_t len_block;
	union{
		uint32_t u32_block_data[BLOCK_SIZE_IN_4_Bytes];
		uint8_t  u8_block_data[BLOCK_SIZE_IN_1_BYTE];
	}block_data;

	memset(block_data.u8_block_data,0,sizeof(block_data.u8_block_data));


	PhyAdress=(((file_system->RootDirStartSector)+ ((u32_cluster-2)*( file_system->BPB_boot_partition_block.BPB_SecPerClus)))*BLOCK_SIZE_IN_1_BYTE)+(file_system->lba_address);

	for(  SectorCntr=0 ; SectorCntr < file_system->BPB_boot_partition_block.BPB_SecPerClus ; SectorCntr++){
		SD_read_block(block_data.u32_block_data, PhyAdress+(BLOCK_SIZE_IN_1_BYTE* SectorCntr));

		//SDBG1("%s", block_data.u8_block_data);
		len_block=strlen(block_data.u8_block_data);
		len_input=strlen(InputData);
		SDBG1 ("\nlen_block [%d]. lend_input[%d]\n",len_block, len_input);

	        if ( BLOCK_SIZE_IN_1_BYTE > ( len_input+len_block)){
	        	SDBG1 ("\nYOU CAN WRITE IN THIS BLOCK!!\n");
	        	memcpy(&block_data.u8_block_data[len_block],InputData ,len_input);
	        //	SDBG1 ("\n%s\n", block_data.u8_block_data);
        		SD_write_block(block_data.u32_block_data, PhyAdress+(BLOCK_SIZE_IN_1_BYTE* SectorCntr));
        		break;
	        }else{
	        	if(len_block!=BLOCK_SIZE_IN_1_BYTE){

				memcpy(&block_data.u8_block_data[len_block],
					InputData,
					BLOCK_SIZE_IN_1_BYTE-len_block);

				SD_write_block(block_data.u32_block_data, PhyAdress+(BLOCK_SIZE_IN_1_BYTE* SectorCntr));

				SDBG1("\nJoining sectors[%d][%s]\n", len_block,
				 	&InputData[BLOCK_SIZE_IN_1_BYTE-len_block]);

	        		memset(block_data.u8_block_data,0,sizeof(block_data.u8_block_data));
				memcpy(block_data.u8_block_data,
					&InputData[BLOCK_SIZE_IN_1_BYTE-len_block],
					(len_input-(BLOCK_SIZE_IN_1_BYTE-len_block)));
				SD_write_block(block_data.u32_block_data, PhyAdress+(BLOCK_SIZE_IN_1_BYTE* (SectorCntr+1)));

				break;
	        	}
	        }

	        if(!(len_block)) {
	        	SDBG1("\nEND OF FILE REACHED\n");
	        	 break;
	        }
	}

	return false;
}



/**************************************************************************************************************************************/

uint8_t fat32_read_data( st_fat32 *file_system, uint32_t *OutData, uint32_t *ClusterNumber){
	uint32_t PhyAddress;


	PhyAddress=   (( ( file_system->RootDirStartSector)
		+ (( (*ClusterNumber)  -2         ) * ( file_system->BPB_boot_partition_block.BPB_SecPerClus) ))*BLOCK_SIZE_IN_1_BYTE)
	+     (file_system->lba_address);
	SDBG1("[SDBG1]/n Data Address %x",PhyAddress);

	SD_read_block(OutData, PhyAddress);


	return false;
}
/**************************************************************************************************************************************/


uint8_t print_block( uint32_t *block_to_print, uint8_t format_to_print )
{
	#ifdef FAT32_DEBUG
	uint16_t rowCntr;
	uint16_t byteCntr;


	for (rowCntr = 0; rowCntr < BLOCK_SIZE_IN_4_Bytes; rowCntr++){

		if (format_to_print == 'x'){
			SDBG1(" [(%x) %8x] ",rowCntr,  block_to_print [rowCntr]);
		}

		if (format_to_print == 'c'){
			for (byteCntr=0; byteCntr < BITS_PER_PHY_ADDR ; byteCntr+=8){//scan bytes of the row
				SDBG1("%c", (uint8_t)( ( block_to_print [rowCntr] >>    byteCntr) & 0xff));
			}
		}
	}
	#endif

	return false;
}
/**************************************************************************************************************************************/

//@DOT REMOVED  FROM THE EXTENTION
uint8_t fat32_update_directory_entry( st_fat32 *file_system, st_file_entry  *new_entry,  uint32_t FileSize, uint32_t block_addr, uint32_t block_pos)
{

	uint32_t rowCntr =  8;

	uint32_t u32_block[BLOCK_SIZE_IN_4_Bytes];

	SD_read_block(u32_block, block_addr);
	//SDBG1("BEFORE UPDATE\n");
	//print_block(&u32_block[block_pos],'x');
	//SDBG1("\n\n");
	//print_block(&u32_block[block_pos],'c');

	u32_block[5+block_pos+16]=   (((new_entry->cluster_addr))>>16 & 0x0000FFFF)|0x5a730000;    //hihg cluster address | dummy value
	u32_block[6+block_pos+16]=   (((new_entry->cluster_addr))<<16 & 0xFFFF0000)|0x00004b67;    //low cluster addresss  | dummy value
	u32_block[7+block_pos+16]=   FileSize;

	//SDBG1("\n\nAFTER UPDATE\n");
	//print_block(&u32_block[block_pos],'x');

	SD_write_block(u32_block, block_addr);


	return false;
}


uint8_t fat32_write_directory_entry( st_fat32 *file_system, char   *FileName, char *InData, uint32_t *FileSize, uint32_t *cluster_location)
{

	uint32_t rowCntr =  8;

	uint32_t u32_block[BLOCK_SIZE_IN_4_Bytes];

	//@REMOVING THE DOT FROM THE EXTENTION

	SD_read_block(u32_block, (file_system->RootDirAddr));

	while(    (    BLOCK_SIZE_IN_4_Bytes   >     rowCntr                  )
		&& (    0X00                !=     u32_block [ rowCntr   ]  )
		&& (    EMPTY_ROOT_ENTRY    !=    (u32_block [ rowCntr ] & EMPTY_ROOT_ENTRY)  ))
	{
		rowCntr+=8;
	}


	if (       (    0X00             ==   u32_block[rowCntr]        )
		|| (    EMPTY_ROOT_ENTRY == ( u32_block[rowCntr] & EMPTY_ROOT_ENTRY   ) ))
	{
		memcpy(&u32_block[0+rowCntr],FileName,strlen((const char *)FileName));//SHORT_FileName_SIZE);

		u32_block[2+rowCntr]|=  0x20000000; //0x20 indicates that the entry is a file
		u32_block[3+rowCntr]=   0x16ae0718; //dummy value     (creation date)
		u32_block[4+rowCntr]=   0x4b6d4b6d; //dummy value      (modification date)
		u32_block[5+rowCntr]=   ((*cluster_location)>>16 & 0x0000FFFF)|0x5a730000;    //hihg cluster address | dummy value
		u32_block[6+rowCntr]=   ((*cluster_location)<<16 & 0xFFFF0000)|0x00004b67;    //low cluster addresss  | dummy value
		u32_block[7+rowCntr]=   *FileSize;
	#ifdef WRITE_ALLOWED

		SD_write_block(u32_block, (file_system->RootDirAddr));
	#endif
	}

	return false;
}

/**************************************************************************************************************************************/


uint8_t fat32_convert_file_name(char *FileNameDot, char *FileNameSpace){

uint8_t cntr =0;
uint8_t IndexCntr=0;
uint8_t FileCntr     =0;

memset(  &FileNameSpace[0] , 0 , SHORT_FILE_NAME_SIZE  );

if (  strlen( (const char *) FileNameDot)  <   SHORT_FILE_NAME_SIZE)
{

	while(   0  !=   (FileNameDot[IndexCntr])   )
	{

		if ( FileNameDot[IndexCntr] != ' '  && FileNameDot[IndexCntr]  !=  '.' )
		{
			FileNameSpace[cntr]  = FileNameDot[ IndexCntr ];
			cntr++;
			FileCntr=1;
		}
		IndexCntr++;
	}
}
return FileCntr;
}

/**************************************************************************************************************************************/
uint8_t fat32_format_file_name( char  *FileNameDot,     char *FileNameSpace)
{


uint8_t IndexCntr      =0;
uint8_t FileCntr           =0;
uint8_t DotIndex     =0;
uint8_t StopIfg    =1;


if ( strlen(  (const char *)FileNameDot  )    <   SHORT_FILE_NAME_SIZE){

	while (   (0 !=   (FileNameDot  [IndexCntr] )) && ( StopIfg== 1)){

		if ( (FileNameDot[ IndexCntr ]== ' ') || FileNameDot[IndexCntr] == '.'){

			StopIfg=0;
			DotIndex= IndexCntr;
			memset(FileNameSpace,' ', 8);
			memcpy( FileNameSpace   ,  FileNameDot                , DotIndex);
			memcpy(&FileNameSpace[8], &FileNameDot[DotIndex+1],            3);
		}
		IndexCntr++;
	}
	FileCntr  =1;
}

return FileCntr;
}
/**************************************************************************************************************************************/

void CalculateNewAddresses()
{
/*
  byte_location 	 =  FileSize% (file_system->BPB_boot_partition_block.BPB_BytesPerSec) / 4;
  NewCluster = LastCluster+ ((FileSize/(file_system->ClstrSize))+1);
  Fat1AddrLastFile=  (file_system->Fat1Addr)+ (BLOCK_SIZE_IN_1_BYTE*(NewCluster/BLOCK_SIZE_IN_4_Bytes));
*/
}
/**************************************************************************************************************************************/

uint16_t GetLastPosition(uint32_t *block,uint32_t *LastPosition)
{
	uint32_t cntr=0;
	while((block[cntr]!=0 )&& cntr<512){
		(*LastPosition)++;
		cntr++;
	}
	return false;
}

/**************************************************************************************************************************************/

char fat32_check_file_entry(st_fat32 *file_system, st_file_entry   *pfile_entry, char *requested_file, uint32_t *sector, uint32_t *block_pos){
//1.search on dirEntry
//2.get index on the dirEntru
//3.read back paratmeters
//
	uint32_t SectorCntr =0;
	uint32_t rowCntr =0;
	uint32_t block_data[BLOCK_SIZE_IN_4_Bytes];
	st_file_entry DirEntry;
	uint8_t max_tries=30;
	char name_formated[12];
	file_system->FileCntr=0;


	for(SectorCntr=0;   SectorCntr  <=  file_system->BPB_boot_partition_block.BPB_SecPerClus;   SectorCntr++ ){

		SD_read_block(  block_data, (file_system->RootDirAddr)+ (BLOCK_SIZE_IN_1_BYTE*SectorCntr)  );
		*sector=(file_system->RootDirAddr)+ (BLOCK_SIZE_IN_1_BYTE*SectorCntr) ;
		for( rowCntr =0,max_tries=30; (rowCntr< BLOCK_SIZE_IN_4_Bytes)&(max_tries!=0);max_tries-- ){


			if (!( block_data [rowCntr] ||( EMPTY_ROOT_ENTRY     == (block_data [rowCntr]&&EMPTY_ROOT_ENTRY) ) )){
				rowCntr      =   BLOCK_SIZE_IN_4_Bytes;
				SectorCntr   =   file_system->BPB_boot_partition_block.BPB_SecPerClus;
			 	continue;
			}

			*block_pos=rowCntr;
			rowCntr+=fat32_directory_entry_assignation(block_data , &DirEntry, rowCntr);

			if ((memcmp(DirEntry.lfn, requested_file,strlen(requested_file) )) || (!DirEntry.cluster_addr) ) continue;

			memcpy(pfile_entry, &DirEntry,sizeof (st_file_entry));

			return 1;
		}
	}
	return 0;
}

uint8_t fat32_write_to_file(st_fat32 *file_system,char   *FileName, char  *InData )
{

	char  Fat32FileName[12]={0};

	uint32_t FileSize          = 1;
	uint32_t InFileSize;
	uint32_t LastCluster=0;
	uint32_t NewCluster;
	uint32_t Fat1AddrLastFile;//,Fat2AddrLastFile;
	uint32_t Fat1Addres =file_system->Fat1Addr;

	union{
		uint32_t u32_block_data[BLOCK_SIZE_IN_4_Bytes];
		uint8_t  u8_block_data[BLOCK_SIZE_IN_1_BYTE];
	}block_data;



	if (false == fat32_format_file_name(FileName, Fat32FileName) )
		return false;

	//check if file already exist
	//
	InFileSize= strlen((const char *)InData);

	fat32_search_last_entry(&Fat1AddrLastFile, file_system, &LastCluster ,&FileSize);

	SDBG1("[SDBG1]\n Last Cluster address ");//printInt(LastCluster);   	printInt( FileSize); 	printInt(InFileSize);


	SD_read_block(block_data.u32_block_data,file_system->Fat1Addr);
	uint32_t FATTable_availablePost=0;
//	SDBG1(block_data.octet);
	GetLastPosition(block_data.u32_block_data,&FATTable_availablePost );

	SDBG1("[SDBG1]\nLast fat pos");//printInt(LastCluster);
	SDBG1("[SDBG1]Fat1 last File");//printInt(Fat1AddrLastFile);

		//	 NumberOfClusters 	 =  FileSize% (BytesPerSector) / (BLOCK_SIZE_IN_1_BYTE/BLOCK_SIZE_IN_4_Bytes);

	NewCluster = LastCluster+ ((FileSize/(file_system->ClstrSize)))+1;
	Fat1AddrLastFile=  Fat1Addres+ (BLOCK_SIZE_IN_1_BYTE*(NewCluster/BLOCK_SIZE_IN_4_Bytes));
			// Fat2AddrLastFile=  Fat2Addres+ (BLOCK_SIZE_IN_1_BYTE*(NewCluster/BLOCK_SIZE_IN_4_Bytes));

//			SDBG1("[SDBG1]fat1Addr ");printInt(Fat1AddrLastFile);
//			SDBG1("[SDBG1]newClus ");printInt(NewCluster);

	fat32_write_fat1(block_data.u32_block_data, file_system , &NewCluster, &Fat1AddrLastFile, &InFileSize);
		//	fat32_write_fat1(block_data.u32_block_data, file_system , &NewCluster, &Fat2AddrLastFile, &InFileSize);

	fat32_write_data( file_system, InData , &NewCluster);
	fat32_write_directory_entry(file_system, Fat32FileName,  InData, &InFileSize, &NewCluster);

	return false;
}

//32/8 = 4
char  fat32_check_directory_entry_type(uint32_t *block_data )
{
//	SDBG1 ("\n[%s] Block0 [%x]   Block8 [%x]  %x\n\n",__FUNCTION__,(block_data[0]& LONG_ENTRY_MARK_1),block_data[8]& LONG_ENTRY_MARK_2,block_data);
	if ((block_data[0]&LONG_ENTRY_MARK_1)== LONG_ENTRY_MARK_1 )
		if((block_data[8]&LONG_ENTRY_MARK_2)==LONG_ENTRY_MARK_2)
			return 1;
		return 0;
	}

void fat32_get_lfn(uint32_t *pblock_data,  char *long_name ){

	union{
		uint32_t u32_block_data[BLOCK_SIZE_IN_4_Bytes];
		uint8_t  u8_block_data[BLOCK_SIZE_IN_1_BYTE];
	}block_data;

	uint8_t name1[16];	memset(name1,0,16);
	uint8_t name2[16];	memset(name2,0,16);
	uint8_t name3[16]; 	memset(name3,0,16);
	memset(long_name, 0, LONG_NAME_MAX_SIZE);


	for (int i=0;i<32;i++)
		block_data.u32_block_data[i] = pblock_data[i];

	for (int i=0 ;i<5;i++){
		name1[i]  =  block_data.u8_block_data[ (i*2)+1 ];
		name2[i]  =  block_data.u8_block_data[ (i*2)+33];
		name3[i]  =  block_data.u8_block_data[ (i*2)+48];
	}

	name1[5]   = block_data.u8_block_data[14];
	name2[5]   = block_data.u8_block_data[46];
	name3[5]   = block_data.u8_block_data[60];
	name3[6]   = block_data.u8_block_data[62];

	sprintf(long_name,"%s%s%s",name2,name3,name1);

	return;
}

#ifdef LONG_NAME_CHECKSUM
/*
const unsigned char name[12]={'T', 'H', 'E', 'Q', 'U', 'I',0X7E , '1' ,'F', 'O','X'};

the checksum  is applied to the SFN, and for the above name is 0x7
*/
	unsigned char long_name_checksum(const unsigned char *pName){

		unsigned char  sum=0;
		int i=11;

		for (i = 11; i; i--)
			sum = ((sum & 1) << 7) + (sum >> 1) + pname++;

		return sum;
//   SDBG1("\nchecksum is %d %0x ", sum,sum);
	}
#endif
