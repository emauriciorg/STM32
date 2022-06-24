/**
 * @file rtc_1307.c
 * @author Mauro R (emauriciorg@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "rtc_1307.h"
#include "rct_api.h"
#include "stm32f0xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

extern I2C_HandleTypeDef hi2c1;

#define DS1307_WRITE_REG(...)
#define DS1307_READ_REG(...)

RTC_driver_t ds1307_drv = {
	ds1307_init,
	ds1307_set_time,
	ds1307_get_time
};
#ifndef RTC_MOCK
RTC_driver_t  *rtc_drv = &ds1307_drv;
#endif
uint8_t readRegister(uint8_t addr){
	const uint8_t single_byte_read  = 1;
	uint8_t out_data = 0;
 	HAL_I2C_Mem_Read(&hi2c1, DS1307_ADDR | 0x01, addr, single_byte_read, &out_data, single_byte_read, 100);
	return out_data;
}

bool writeRegister(uint8_t addr, uint8_t val){
	const uint8_t single_byte_read  = 1;
	uint8_t out_data = 0;
 	HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDR, addr, single_byte_read, &val, single_byte_read, 100);
	return out_data;

}

bool readMultipleRegisters(uint8_t addr, uint8_t *dest, uint8_t len)
{
//	 	HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDR, addr, single_byte_read, &val, single_byte_read, 100);
	return true;
}

bool writeMultipleRegisters(uint8_t addr, uint8_t *values, uint8_t len)
{
// 	HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDR, addr, single_byte_read, &val, single_byte_read, 100);
	return true;
}


uint8_t decToBcd(uint8_t val) {
	return ((val / 10 * 16) + (val % 10));
}

uint8_t bcdToDec(uint8_t val) {
	return ((val / 16 * 10) + (val % 16));
}


void ds1307_init(void) {
	writeRegister(0x00, 0x00);
	bcdToDec(readRegister(0x00)&0xf7);
}

void ds1307_stop(void) {      
	writeRegister(0x00,0x00); 
}


void ds1307_get_time(date_time_t *date_time) {
	date_time->seconds     = bcdToDec(readRegister(REG_SECONDS) & 0x7f);
	date_time->minutes     = bcdToDec(readRegister(REG_MINUTES));
	date_time->hours       = bcdToDec(readRegister(REG_HOURS) & 0x3f);// Need to change this if 12 hour am/pm
	date_time->dayOfWeek  = bcdToDec(readRegister(REG_DAY));
	date_time->dayOfMonth = bcdToDec(readRegister(REG_DATE));
	date_time->month      = bcdToDec(readRegister(REG_MONTH));
	date_time->year       = bcdToDec(readRegister(REG_YEAR));
}

void ds1307_set_time(date_time_t date_time) {
	/*TODO : add 24H format*/
	writeRegister(REG_SECONDS, 0x00);
	writeRegister(REG_SECONDS, decToBcd(date_time.seconds));// 0 to bit 7 starts the clock
	writeRegister(REG_MINUTES, decToBcd(date_time.minutes));
	writeRegister(REG_HOURS  , decToBcd(date_time.hours));  // If you want 12 hour am/pm you need to set bit 6
	writeRegister(REG_DAY    , decToBcd(date_time.dayOfWeek));
	writeRegister(REG_DATE   , decToBcd(date_time.dayOfMonth));
	writeRegister(REG_MONTH  , decToBcd(date_time.month));
	writeRegister(REG_YEAR   , decToBcd(date_time.year));
}
