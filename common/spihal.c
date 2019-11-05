/*!
   \file spihal.c
   \brief Header to allow migration of the code, replace low level funcions here
   \author Mauricio Rios
   \date 12/04/2019
*/

#include "spihal.h"
#include "stm32f0xx_hal.h"

/*extern variables  EV*/
extern HAL_HANDLE_T HAL_SD_SPI;


/*private macros  PM*/
#define HAL_TX_RX_SPI(...)  HAL_SPI_TransmitReceive(__VARGS__)
#define HAL_TX_RESULT_T     HAL_StatusTypeDef
#define HAL_SD_SPI          hspi1
#define HAL_HANDLE_T        SPI_HandleTypeDef
#define HAL_SUCCESS         HAL_OK

#define SD_CS_PORT GPIOB
#define SD_CS_PIN  GPIO_PIN_0

#define SD_DT_PORT GPIOB
#define SD_DT_PIN  GPIO_PIN_1

#define SD_CS_LOW()         HAL_GPIO_WritePin(SD_CS_PORT,SD_CS_PIN,GPIO_PIN_RESET);
#define SD_CS_HIGH()        HAL_GPIO_WritePin(SD_CS_PORT,SD_CS_PIN,GPIO_PIN_SET);
#define SD_CARD_DETECTED()  HAL_GPIO_ReadPin(SD_DT_PORT,SD_DT_PIN)

/*private variables PV */
static uint32_t timeout = 1000;


/*private function PF*/

static void SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLegnth);


/*!
   \brief low level spi write read function call
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
*/

static void SPIx_WriteReadData(const uint8_t *data_in, uint8_t *data_out, uint16_t len)
{
	HAL_TX_RESULT_T tx_result = HAL_SUCCESS;

	tx_result = HAL_TX_RX_SPI(&HAL_SD_SPI, data_in, data_out, len, timeout);

	if(tx_result != HAL_SUCCESS)
	{
		// SPIx_Error();
	}
}
/*!
   \brief "Description"
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
*/
void SD_IO_Init(void)
{
	hspi1.Instance               = SPI1;
	hspi1.Init.Mode              = SPI_MODE_MASTER;
	hspi1.Init.Direction         = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase          = SPI_PHASE_2EDGE;
	hspi1.Init.NSS               = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode            = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial     = 10;

	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		while(1);
		//_Error_Handler(__FILE__, __LINE__);
	}
}

/*!
   \brief "Description"
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
*/
void SD_IO_WriteReadData(const uint8_t *data_in, uint8_t *data_out, uint16_t len)
{
	SPIx_WriteReadData(data_in, data_out, len);
}



/*!
   \brief "Description"
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
*/
uint8_t SD_IO_WriteByte(uint8_t command_in)
{
	uint8_t data_out;

	SPIx_WriteReadData(&command_in, &data_out,1);
	return data_out;
}

/*!
   \brief "Description"
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
*/
void SD_IO_CSState(uint8_t cs_state)
{
	if(cs_state)
		SD_CS_HIGH();
	else
		SD_CS_LOW();
}
