#include "spihal.h"


 extern SPI_HandleTypeDef hspi1;
#define  SPI_TIMEOUT  1000


void SD_IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
{
	/* Send the byte */
	SPIx_WriteReadData(DataIn, DataOut, DataLength);
}


void SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLegnth)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) DataIn, DataOut, DataLegnth, SPI_TIMEOUT);

}

uint8_t SD_IO_WriteByte(uint8_t Data)
{
	uint8_t tmp = 0;
	/* Send the byte */
	SPIx_WriteReadData(&Data,&tmp,1);
	return tmp;
}

void SD_IO_CSState(uint8_t val)
{
	if(val){
		SD_CS_HIGH();
	}else{
		SD_CS_LOW();
	}
}
