#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_

#include "stm32f0xx_hal.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#define SD_CS_LOW()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
#define SD_CS_HIGH() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

#define SD_CARD_DETECTED()  HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)


/* Link functions for SD Card peripheral */
void SPIx_Init(void);
void SPIx_Write(uint8_t Value);
void SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLegnth);
void SPIx_FlushFifo(void);
uint32_t SPIx_Read(void);
void SPIx_Error (void);
void SPIx_MspInit(SPI_HandleTypeDef *hspi);


void SD_IO_Init(void);
void SD_IO_CSState(uint8_t state);
void SD_IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
uint8_t SD_IO_WriteByte(uint8_t Data);
void SD_IO_CSState(uint8_t val);






#endif
