#include <string.h>
#include <stdio.h>
#include "fat.h"
#include "SD.h"
//#include "foundation.h"

#define DEBUG_SD_HEADER_ENABLE
#ifdef DEBUG_SD_HEADER_ENABLE
	#define SD_DEBUG(...) printf(__VA_ARGS__)
#else
	#define SD_DEBUG(...)
#endif


#define SD_INIT_TRIES	2

struct FileDetails fileDet[20];

char logMessage[20];

unsigned long blockWritten = 0;

unsigned char init_FAT(void)
{
	unsigned char sd_init_result = 0;
	int sd_init_counter          = 0;

	memset(fileDet,0x00,sizeof(fileDet));

	sd_init_result = SD_init();

	while ((sd_init_result != 0) && (sd_init_counter < SD_INIT_TRIES))
	{
		sd_init_result = SD_init();
		sd_init_counter++;
	}

	if (sd_init_result !=0 )
	{
		SD_DEBUG ("[sd]: Initialization Error %d \r\n", sd_init_result);
	}else{
		SD_DEBUG ("[sd: Initialization SUCCESS]\r\n");
	}
	/*
		disable until freq is found!!!
		change_sd_speed(0x0002);
	*/
	return sd_init_result;
}

int formatDrive(void)
{
	unsigned long noOfFile = 0x00000000, nextFreeSector = DATA_SECTOR_START;
        unsigned char status;
        int blockSaveRetries;
	char sectorData[SECTOR_SIZE], i;
	strncpy(sectorData, "SEC0_", 5);
	memcpy(&sectorData[5], (char *)&noOfFile, sizeof(unsigned long));
	sectorData[9] = '_';
	memcpy(&sectorData[10], (char *)&nextFreeSector, sizeof(unsigned long));
	strncpy(&sectorData[14], "_END", 4);

        status = 1;
        blockSaveRetries = 0;
        while ((status != 0) && (blockSaveRetries < 3))
        {
                status = SD_writeSingleBlock(HEADER_SECTOR_START, sectorData);
                blockSaveRetries++;
        }
        if (status != 0)
        {
                sprintf(logMessage, "Sec 0 %d", status);
                SD_DEBUG(logMessage);
                return WRITE_SEC0_FAILURE;
        }
	memset(sectorData, 0x00, sizeof(sectorData));
	for(i=0; i < INDEX_SECTORS; i++)
        {
		status = SD_writeSingleBlock(i + HEADER_SECTOR, sectorData);
                if (status != 0)
                        return WRITE_IDX_FAILURE;
        }
	memset(sectorData, 0x00, sizeof(sectorData));
	SD_readSingleBlock(HEADER_SECTOR_START, sectorData);
	if(!strncmp(sectorData, "SEC0_", 5))
		return COMPLETE;
	else
		return SD_CARD_NOT_FORMATTED;
}

int saveFileHeader(char *fileName, unsigned long fileSize)
{
	char sectorData[SECTOR_SIZE], fileActive, fileStatus;
	unsigned long noOfFile, nextFreeSector;
	unsigned long indexSectorPosition, indexSectorBlock;
	unsigned long startSector = 0, blockWrittern = 0;
	SD_readSingleBlock(HEADER_SECTOR_START, sectorData);
	if(!strncmp(sectorData, "SEC0", 4))
	{
		memcpy(&noOfFile, &sectorData[5], sizeof(unsigned long));
		memcpy(&nextFreeSector, &sectorData[10], sizeof(unsigned long));
		startSector = nextFreeSector;
		indexSectorBlock = (noOfFile / (SECTOR_SIZE/FILE_INDEX_SIZE));
		indexSectorPosition = (noOfFile * FILE_INDEX_SIZE) % SECTOR_SIZE;
		if(++noOfFile < MAX_FILES)
		{
			nextFreeSector += (fileSize/SECTOR_SIZE);
			if((fileSize%SECTOR_SIZE) != 0)
				nextFreeSector++;
			if(nextFreeSector <= DATA_SECTOR)
			{
				fileActive = 0;
				fileStatus = 0;
				memcpy(&sectorData[5], (char *)&noOfFile, sizeof(unsigned long));
				memcpy(&sectorData[10], (char *)&nextFreeSector, sizeof(unsigned long));
				SD_writeSingleBlock(HEADER_SECTOR_START, sectorData);
				SD_readSingleBlock(indexSectorBlock + HEADER_SECTOR, sectorData);
				strncpy(&sectorData[indexSectorPosition], fileName, 12);
				sectorData[indexSectorPosition + 12] = '_';
				memcpy(&sectorData[indexSectorPosition + FILE_SIZE_START], (char *)&fileSize, sizeof(unsigned long));
				sectorData[indexSectorPosition + 17] = '_';
				memcpy(&sectorData[indexSectorPosition + FILE_SECTOR_START], (char *)&startSector, sizeof(unsigned long));
				sectorData[indexSectorPosition + 22] = '_';
				memcpy(&sectorData[indexSectorPosition + FILE_BLOCKWRITTEN_START], (char *)&blockWrittern, sizeof(unsigned long));
				sectorData[indexSectorPosition + 27] = '_';
				fileActive = 1;
				fileStatus = 0;
				sectorData[indexSectorPosition + FILE_ACTIVE_START] = fileActive;
				sectorData[indexSectorPosition + 29] = '_';
				sectorData[indexSectorPosition + FILE_STATUS_START] = fileStatus;
				SD_writeSingleBlock(indexSectorBlock + HEADER_SECTOR, sectorData);
				return COMPLETE;
			}
			else
				return NO_FREE_SECTORS;
		}
		else
			return NO_INDEX_SPACE;
	}
	else
		return SD_CARD_NOT_FORMATTED;
}
int saveFileBlock(char *fileName, char *fileData, unsigned char fileComplete, long blockNumber)
{
	char fileStatus;
	char fileHeaderBuffer[SECTOR_SIZE];
	unsigned long fileSize, fileIndexPostion;
	unsigned long startSector = 0;
	int returnValue;
	int fileDetIndex;
        int fileWriteStatus;

	returnValue = retrieveHeader(fileName);
	if (returnValue == FILE_NOT_FOUND)
		return FILE_NOT_FOUND;
	else
	{
                SD_readSingleBlock((returnValue>>9)+HEADER_SECTOR, fileHeaderBuffer);
		fileIndexPostion = returnValue%SECTOR_SIZE;
		memcpy(&fileSize,&fileHeaderBuffer[fileIndexPostion + FILE_SIZE_START], sizeof(unsigned long));
		memcpy(&startSector,&fileHeaderBuffer[fileIndexPostion + FILE_SECTOR_START], sizeof(unsigned long));
		memcpy(&blockWritten,&fileHeaderBuffer[fileIndexPostion + FILE_BLOCKWRITTEN_START], sizeof(unsigned long));

		if (blockNumber == -1)
                        blockNumber = blockWritten;

                if ((blockNumber*512) >= fileSize)
			return INCORRECT_BLOCK;
		fileWriteStatus = SD_writeSingleBlock(startSector + blockNumber, fileData);
                if (fileWriteStatus == 0)
                {
                        blockNumber++;
                        if (blockNumber > blockWritten)
                                blockWritten = blockNumber;
                        memcpy(&fileHeaderBuffer[fileIndexPostion + FILE_BLOCKWRITTEN_START], (char *)&blockWritten, sizeof(unsigned long));

                        if ((blockWritten*512) >= fileSize)
                                fileStatus = 0x01;
                        else
                                fileStatus = 0x00;
                        fileHeaderBuffer[fileIndexPostion + FILE_STATUS_START] = fileStatus;
                        fileWriteStatus = SD_writeSingleBlock((returnValue>>9)+HEADER_SECTOR, fileHeaderBuffer);
                        fileDetIndex = retrieveFile(fileName);
                        fileDet[fileDetIndex].fileStatus = fileStatus;
                        fileDet[fileDetIndex].blockWrittern =  blockWritten;
                        if (fileWriteStatus == 0)
                        {
                                return COMPLETE;
                        }
                        else
                                return WRITE_IDX_FAILURE;
                }
                else
                {
                        sprintf(logMessage, "Sec Wr %d", fileWriteStatus);
                        SD_DEBUG(logMessage);

                        return BLOCK_WRITE_FAILURE;
                }
	}
}

int retrieveHeader(char *fileName)
{
	int j = 0, i;
        int sdReadFlag;
	unsigned long noOfFile, noOfFileRead = 0;
	char fileHeaderBuffer[SECTOR_SIZE];

	sdReadFlag = SD_readSingleBlock(HEADER_SECTOR_START, fileHeaderBuffer);
	memcpy(&noOfFile, &fileHeaderBuffer[5], sizeof(unsigned long));
	for(i=0; i<=(noOfFile / (SECTOR_SIZE/FILE_INDEX_SIZE)); i++)
	{
		sdReadFlag = SD_readSingleBlock(i + HEADER_SECTOR, fileHeaderBuffer);

		for(j=0; (j<SECTOR_SIZE) && (noOfFileRead < noOfFile);)
		{
			noOfFileRead++;
			if(!strncmp(&fileHeaderBuffer[j], fileName,12))
				return (i*SECTOR_SIZE+j);
			else
				j+=FILE_INDEX_SIZE;
		}
	}
	return FILE_NOT_FOUND;
}

int retrieveFile(char *fileName)
{
	unsigned char fileFoundFlag = 0, leastAccessedIndex = 0, i = 0, SDFlag = 0;
	char sectorData	[SECTOR_SIZE];
	int returnValueRFH, blockPos, localIndexHeader = 0;

	SDFlag = SD_readSingleBlock(HEADER_SECTOR_START, sectorData);
	if (SDFlag != 0)
	{
//		sprintf(logMessage,"SD Check %c", (SDFlag+0x30));
//		SD_DEBUG(logMessage);
		return CANNOT_READ_SD_CARD;
	}
	if(!strncmp(sectorData, "SEC0", 4))
	{
		#ifdef DEBUGMODE
		sprintf(logMessage,"Header read");
		SD_DEBUG(logMessage);
		#endif
		do
		{
			if (!strncmp(fileDet[i].fileName, fileName, 12))
			{
				fileFoundFlag = 1;
				localIndexHeader = i;
				fileDet[i].timesAccessed ++;

			}
			else
				leastAccessedIndex = (fileDet[i].timesAccessed < fileDet[leastAccessedIndex].timesAccessed) ? i : leastAccessedIndex;
			i++;
		}
		while (i < 20 && !fileFoundFlag);

		if(!fileFoundFlag)
		{
			returnValueRFH = retrieveHeader(fileName);
			#ifdef DEBUGMODE
			sprintf(logMessage,"Searching file %d",returnValueRFH);
			SD_DEBUG(logMessage);
			#endif

			SDFlag = SD_readSingleBlock((returnValueRFH>>9)+HEADER_SECTOR, sectorData);
			if (SDFlag != 0)
			{
				//sprintf(logMessage,"SD Check %c", (SDFlag+0x30));
				//SD_DEBUG(logMessage);
				return CANNOT_READ_SD_CARD;
			}
			if (returnValueRFH != FILE_NOT_FOUND)
			{
				blockPos = returnValueRFH % SECTOR_SIZE;
				strncpy(fileDet[leastAccessedIndex].fileName, &sectorData[blockPos], 12);
				memcpy(&fileDet[leastAccessedIndex].fileSize, &sectorData[blockPos+FILE_SIZE_START], sizeof(unsigned long));
				memcpy(&fileDet[leastAccessedIndex].startSector, &sectorData[blockPos+FILE_SECTOR_START], sizeof(unsigned long));
				memcpy(&fileDet[leastAccessedIndex].blockWrittern, &sectorData[blockPos+FILE_BLOCKWRITTEN_START], sizeof(unsigned long));
				memcpy(&fileDet[leastAccessedIndex].fileActive, &sectorData[blockPos+FILE_ACTIVE_START], sizeof(unsigned char));
				memcpy(&fileDet[leastAccessedIndex].fileStatus, &sectorData[blockPos+FILE_STATUS_START], sizeof(unsigned char));
				fileDet[leastAccessedIndex].timesAccessed = 1;
				localIndexHeader = leastAccessedIndex;
			}
			else
				return FILE_NOT_FOUND;
		}
		if (fileDet[leastAccessedIndex].fileActive)
			return localIndexHeader;
		else
		{
			#ifdef DEBUGMODE
			sprintf(logMessage,"FD %d %d %d %d",returnValueRFH, blockPos, fileDet[leastAccessedIndex].fileStatus, leastAccessedIndex);
			SD_DEBUG(logMessage);
			#endif
			return FILE_DELETED;
		}
	}
	else
		return SD_CARD_NOT_FORMATTED;
}

int retrieveFileBlock(int localIndexHeader, char *fileName, int blockNumToRead, char *fileData)
{
	unsigned char flag;
	if (strcmp(fileDet[localIndexHeader].fileName, fileName) != 0)
		localIndexHeader = retrieveFile(fileName);

	if (blockNumToRead < fileDet[localIndexHeader].blockWrittern)
	{
		flag = SD_readSingleBlock(fileDet[localIndexHeader].startSector + blockNumToRead, fileData);
		if (!flag)
			return localIndexHeader;
		else
			return INCOMPLETE;
	}
	else
		return FILE_DONE;
}

long get_file_size(int localIndexHeader,char *fileName)
{
	if (strcmp(fileDet[localIndexHeader].fileName, fileName) != 0)
		localIndexHeader = retrieveFile(fileName);
	if (localIndexHeader != - 1)
		return fileDet[localIndexHeader].fileSize;
	else
		return -1;
}

long get_blocks_written(int localIndexHeader,char *fileName)
{
	if (strcmp(fileDet[localIndexHeader].fileName, fileName) != 0)
		localIndexHeader = retrieveFile(fileName);
	if (localIndexHeader != - 1)
		return fileDet[localIndexHeader].blockWrittern;
	else
		return -1;
}

unsigned char get_file_complete_status(int localIndexHeader,char *fileName)
{
	if (strcmp(fileDet[localIndexHeader].fileName, fileName) != 0)
		localIndexHeader = retrieveFile(fileName);
	if (localIndexHeader != - 1)
		return fileDet[localIndexHeader].fileStatus;
	else
		return 0x00;
}

void log_file_details(int localIndexHeader)
{
        SD_DEBUG(fileDet[localIndexHeader].fileName);
        sprintf(logMessage, "FDet %d %d", fileDet[localIndexHeader].fileSize, fileDet[localIndexHeader].startSector);
        SD_DEBUG(logMessage);
}
char *errorFormat(int errorCode)
{
	static char errorMessage[20];
	switch(errorCode)
	{
		case NO_FREE_SECTORS :
			strcpy(errorMessage,"MEMORY FULL \n");
		break;

		case NO_INDEX_SPACE :
			strcpy(errorMessage,"INDEX FULL \n");
		break;

		case SD_CARD_NOT_FORMATTED :
			strcpy(errorMessage,"SD CARD NOT FORMATTED \n");
		break;

		case FILE_NOT_FOUND :
			strcpy(errorMessage,"FILE NOT FOUND \n");
		break;

		case FILE_DELETED :
			strcpy(errorMessage,"FILE DELETED \n");
		break;

		case SD_INTIALIZATION_FAILED:
			strcpy(errorMessage,"SD_INTIALIZATION_FAILED \n");
		break;
                case INVALID_COMMAND:
                        strcpy(errorMessage,"Invalid command \n");
		break;
                case WRITE_SEC0_FAILURE:
                        strcpy(errorMessage,"Sec 0 Failure \n");
		break;
                case WRITE_IDX_FAILURE:
                        strcpy(errorMessage,"Index sec Failure \n");
		break;
                default:
                        strcpy(errorMessage, "General failure \n");
                break;
	}
	return(errorMessage);
}
