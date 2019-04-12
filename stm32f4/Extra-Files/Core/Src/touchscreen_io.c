#include "main.h"
#include "stm32f4xx_hal.h"

#include "stmpe811.h"

extern I2C_HandleTypeDef hi2c3;

/**
 * @brief  IOE Low Level Initialization.
 */
void IOE_Init(void)
{
	/* Initialize the I2C3 here if it has not been initialized yet */
}

/**
 * @brief  IOE Low Level Interrupt configuration.
 */
void IOE_ITConfig(void)
{
	/* TODO: configure external interrupt */
}

/**
 * @brief  IOE Writes single data operation.
 * @param  Addr: I2C Address
 * @param  Reg: Reg Address
 * @param  Value: Data to be written
 */
void IOE_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
	HAL_I2C_Mem_Write(&hi2c3, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, HAL_MAX_DELAY);
}

/**
 * @brief  IOE Reads single data.
 * @param  Addr: I2C Address
 * @param  Reg: Reg Address
 * @retval The read data
 */
uint8_t IOE_Read(uint8_t Addr, uint8_t Reg)
{
	uint8_t value = 0;
	HAL_I2C_Mem_Read(&hi2c3, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);

	return value;
}

/**
 * @brief  IOE Writes multiple data.
 * @param  Addr: I2C Address
 * @param  Reg: Reg Address
 * @param  pBuffer: pointer to data buffer
 * @param  Length: length of the data
 */
void IOE_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length)
{
	HAL_I2C_Mem_Write(&hi2c3, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Length, HAL_MAX_DELAY);
}

/**
 * @brief  IOE Reads multiple data.
 * @param  Addr: I2C Address
 * @param  Reg: Reg Address
 * @param  pBuffer: pointer to data buffer
 * @param  Length: length of the data
 * @retval 0 if no problems to read multiple data
 */
uint16_t IOE_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length)
{
	uint8_t value = 0;
	HAL_I2C_Mem_Read(&hi2c3, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Length, HAL_MAX_DELAY);

	return value;
}

/**
 * @brief  IOE Delay.
 * @param  Delay in ms
 */
void IOE_Delay(uint32_t Delay)
{
	HAL_Delay(Delay);
}
