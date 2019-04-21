#include "max7219.h"
#include "stm32f0xx.h"

/*
D15    D14    D13    D12
  X      X      X      x

D11    D10    D09    D08
[      ADDRESS         ]

D07    D06    D05    D04
[      MSB    DATA     ]
D03    D02    D01    D00
[      LSB    DATA     ]
*/

#define SPI_HANDLE hspi1
#define PTR_SPI &SPI_HANDLE
extern SPI_HandleTypeDef SPI_HANDLE;

#define ADDR_NO_OP 0x00
#define ADDR_DIGIT0 0x01
#define ADDR_DIGIT1 0x02
#define ADDR_DIGIT4 0x05
#define ADDR_INTENSITY 0x0A
#define ADDR_DECODE_MODE 0x09
#define ADDR_SHUTDOWN 0x0C

#define ADDR_SCAN_LIMIT 0x0B /*0-7 display to scan , digits to show*/
#define CS_ASSERT   GPIOC->BSRR = GPIO_PIN_4
#define CS_DEASSERT GPIOC->BRR   = GPIO_PIN_4


void max7219_init(void)
{



}

void max7219_test(void)
{
	CS_ASSERT;
	HAL_Delay(1000);

	// CS_DEASSERT;
	// max7219_send(ADDR_SHUTDOWN, 0x00);
	// CS_ASSERT;
	// HAL_Delay(500);

	// CS_DEASSERT;
	// max7219_send(ADDR_SHUTDOWN, 0x01);
	// CS_ASSERT;
	// HAL_Delay(500);


	CS_DEASSERT;
	HAL_Delay(50);
	max7219_send(ADDR_INTENSITY,0x0F);
	CS_ASSERT;
	HAL_Delay(500);

	CS_DEASSERT;
	HAL_Delay(50);
	max7219_send(ADDR_SCAN_LIMIT,0x03);
	CS_ASSERT;
	HAL_Delay(500);

	CS_DEASSERT;
	HAL_Delay(50);
	max7219_send(ADDR_DIGIT0,0x07);
	CS_ASSERT;
	HAL_Delay(500);

	CS_DEASSERT;
	max7219_send(ADDR_DIGIT1,0x80);
	CS_ASSERT;

}
/* HAL_SPI_Transmit(
SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
*/
void max7219_send(uint8_t address, uint8_t data)
{
	uint8_t frameout[2];
	frameout[1] = address;
	frameout[0] = data;
	HAL_SPI_Transmit(PTR_SPI,frameout,2,HAL_MAX_DELAY );

}
