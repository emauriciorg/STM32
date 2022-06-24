/**
 * @file rtc_1307.h
 * @author your name (you@domain.com)
 * @brief rtc low level driver
 * @version 0.1
 * @date 2022-06-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __DS1307_H__
#define __DS1307_H__

#include <stdbool.h>
#include <stdint.h>
#include "rct_api.h"

#define DS1307_ADDR 0xD0
#define REG_SECONDS   0X00
#define REG_MINUTES   0X01
#define REG_HOURS     0X02
#define REG_DAY       0X03
#define REG_DATE      0X04
#define REG_MONTH     0X05
#define REG_YEAR      0X06
#define REG_CONTROL   0X07
#define REG_RAM_START 0X08
#define REG_RAM_END   0X3F

typedef enum days_of_the_week_enum {
 	DAY_MON= 1,
 	DAY_TUE= 2,
 	DAY_WED= 3,
 	DAY_THU= 4,
 	DAY_FRI= 5,
 	DAY_SAT= 6,
 	DAY_SUN= 7
}days_of_the_week_t;



uint8_t readRegister(uint8_t addr);
bool writeRegister(uint8_t addr, uint8_t val);
bool readMultipleRegisters(uint8_t addr, uint8_t *dest, uint8_t len);
bool writeMultipleRegisters(uint8_t addr, uint8_t *values, uint8_t len);

bool writeConfigEEPROM_RAMmirror(uint8_t eepromaddr, uint8_t val);
uint8_t readConfigEEPROM_RAMmirror(uint8_t eepromaddr);
bool waitforEEPROM();

void setBit(uint8_t reg_addr, uint8_t bit_num);
void clearBit(uint8_t reg_addr, uint8_t bit_num);
bool readBit(uint8_t reg_addr, uint8_t bit_num);

void startClock(void);
void stopClock(void);

uint8_t decToBcd(uint8_t val);
uint8_t bcdToDec(uint8_t val);

void ds1307_get_time(date_time_t *date_time);
void ds1307_set_time(date_time_t date_time);
void ds1307_init(void);

#endif /*__DS1307_H__*/
