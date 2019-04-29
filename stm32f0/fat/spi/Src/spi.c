
#include "SPI.h"
#include "stm32f0xx.h"

#define SD_SPI_INSTANCE SPI1

#define DEBUG_SPI_ENABLED
#ifdef DEBUG_SPI_ENABLED
	#define SPI_DEBUG(...) printf(__VA_ARGS__)
#else
	#define SPI_DEBUG(...)
#endif

extern SPI_HandleTypeDef hspi1;

void SSP_Initialize(void)
{

	CS_DEASSERT;
	//MX_SPI1_Init();

}

void change_ssp_freq(unsigned long freq)
{
	/*
	At init, use
		BaudRatePrescaler
	baudrate !=  freq

		SPI_BAUDRATEPRESCALER_2
		SPI_BAUDRATEPRESCALER_4
		SPI_BAUDRATEPRESCALER_8
		SPI_BAUDRATEPRESCALER_16
		SPI_BAUDRATEPRESCALER_32
		SPI_BAUDRATEPRESCALER_64
		SPI_BAUDRATEPRESCALER_128
		SPI_BAUDRATEPRESCALER_256
	*/
	//		BaudRatePrescaler
	// SD_SPI_INSTANCE->CR1 &= SPI_BAUDRATEPRESCALER_256;
	// SD_SPI_INSTANCE->CR1 |= freq;

}


#ifdef LPC_FUNCTION
unsigned char SSP_Read(void)
{
	unsigned char data      = 0xFF;
	unsigned char data_out  = 0;
	uint8_t transmit_result = HAL_OK;

	transmit_result = HAL_SPI_TransmitReceive(&hspi1, &data, &data_out, 1, 1000);

	if (HAL_OK != transmit_result )
			SPI_DEBUG("RX ERROR %x\r\n", transmit_result);

	return (data_out);
}
#else


void SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLegnth)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) DataIn, DataOut, DataLegnth, 1000);

}

uint8_t SD_IO_WriteByte(uint8_t Data)
{
	uint8_t tmp = 0;
	/* Send the byte */
	SPIx_WriteReadData(&Data,&tmp,1);
	return tmp;
}

uint8_t SSP_Read(void)
{
  uint8_t timeout = 0x08;
  uint8_t readvalue;

  /* Check if response is got or a timeout is happen */
  do {
    readvalue = SD_IO_WriteByte(0xFF);
    timeout--;

}while ((readvalue == 0XFF) && timeout);

  /* Right response got */
  return readvalue;
}
#ifdef LPC_FUCNT
uint8_t  SSP_Write(unsigned char data)
{

	uint8_t transmit_result = HAL_OK;
	uint8_t data_out;


	transmit_result = HAL_SPI_TransmitReceive(&hspi1, &data, &data_out, 1, 1000);

	if (HAL_OK != transmit_result )
			SPI_DEBUG("TX ERROR \r\n");

	return data_out;

}
#else

uint8_t  SSP_Write(unsigned char data)
{

	uint8_t tmp = 0;
	/* Send the byte */
	SPIx_WriteReadData(&data,&tmp,1);
	return tmp;
}

#endif

#endif
