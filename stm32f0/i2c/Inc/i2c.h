#ifndef __I2C__H__
#define __I2C__H__
#include <stdint.h>


void i2c_init(void);
uint8_t i2c_read(void);
uint8_t i2c_write(void);

void ssd1306_set_page(uint8_t page);
void ssd1306_set_column(uint8_t column);

void i2c_test(void);
void i2c_scan(void);

void oled_init(void);

#endif /*__I2C__H__*/
