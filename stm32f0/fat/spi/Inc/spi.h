#ifndef __HAL_SPI__
#define __HAL_SPI__

#include <stdint.h>
#include "stm32f0xx.h"
#define REFACTOR_SPI
#define CS_SD_PIN GPIO_PIN_4
#ifdef NO_HAL_GPIO
	#define CS_ASSERT    GPIOA->BSRR   = CS_SD_PIN
	#define CS_DEASSERT  GPIOA->BRR    = CS_SD_PIN
	#warning "low register used"
#else
	#define CS_ASSERT   HAL_GPIO_WritePin(GPIOA, CS_SD_PIN, GPIO_PIN_SET)
	#define CS_DEASSERT HAL_GPIO_WritePin(GPIOA, CS_SD_PIN, GPIO_PIN_RESET)


#endif



void SPI_Initialize(void);
unsigned char SPI_Write(unsigned char Data);
unsigned char SPI_Read(void);

void SSP_Initialize(void);
void change_ssp_freq(unsigned long);
uint8_t SSP_Write(unsigned char Data);
unsigned char SSP_Read(void);
unsigned char SPI_Read_WithTimeout(void);
void SPI_Initialize_ledsystem(void);

#endif /*__HAL_SPI__*/
