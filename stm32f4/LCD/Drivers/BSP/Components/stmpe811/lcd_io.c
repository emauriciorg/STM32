#include "main.h"
#include "stm32f4xx_hal.h"

#include "ili9341.h"

extern SPI_HandleTypeDef hspi5;

/**
 * @brief  Configures the LCD_SPI interface.
 */
void LCD_IO_Init(void)
{
	/* Initialize the SPI5 here if it has not been initialized yet */
}

/**
 * @brief  Writes register address.
 */
void LCD_IO_WriteReg(uint8_t Reg)
{
	HAL_GPIO_WritePin(ILI9341_WRX_DCX_GPIO_Port, ILI9341_WRX_DCX_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ILI9341_CSX_GPIO_Port, ILI9341_CSX_Pin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi5, &Reg, 1, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(ILI9341_CSX_GPIO_Port, ILI9341_CSX_Pin, GPIO_PIN_SET);
}
/**
 * @brief  Writes register value.
 */

void LCD_IO_WriteData(uint16_t RegValue)
{
	HAL_GPIO_WritePin(ILI9341_WRX_DCX_GPIO_Port, ILI9341_WRX_DCX_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ILI9341_CSX_GPIO_Port, ILI9341_CSX_Pin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi5, (uint8_t *)&RegValue, 1, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(ILI9341_CSX_GPIO_Port, ILI9341_CSX_Pin, GPIO_PIN_SET);
}

/**
 * @brief  Reads register value.
 * @param  RegValue Address of the register to read
 * @param  ReadSize Number of bytes to read
 * @retval Content of the register value
 */

uint32_t LCD_IO_ReadData(uint16_t RegValue, uint8_t ReadSize)
{
	uint32_t content = 0;

	HAL_GPIO_WritePin(ILI9341_WRX_DCX_GPIO_Port, ILI9341_WRX_DCX_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ILI9341_CSX_GPIO_Port, ILI9341_CSX_Pin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi5, (uint8_t *)&RegValue, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi5, (uint8_t *)&content, ReadSize, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(ILI9341_CSX_GPIO_Port, ILI9341_CSX_Pin, GPIO_PIN_SET);

	return content;
}
