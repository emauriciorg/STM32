#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_


#include "stdlib.h"
#include "string.h"
#include "stdio.h"

/* Link functions for SD Card peripheral */
#ifdef CROSS_PLATFORM_IMPLEMENTATION_AVAILABLE

void              SPIx_Init(void);
void              SPIx_FlushFifo(void);
uint32_t          SPIx_Read(void);
void              SPIx_Error (void);
void              SPIx_MspInit(SPI_HandleTypeDef *hspi);

#endif /*CROSS_PLATFORM_IMPLEMENTATION_AVAILABLE*/

void SD_IO_Init(void);

void SD_IO_WriteReadData(const uint8_t *data_in, uint8_t *data_out, uint16_t len);
uint8_t SD_IO_WriteByte(uint8_t command_in);
void SD_IO_CSState(uint8_t cs_state);



#endif /*_SPI_DRIVER_H_ */
