#ifndef __MAX7219__H_
#define __MAX7219__H_

#include <stdint.h>
void max7219_init(void);
void max7219_test(void);
void max7219_send(uint8_t address, uint8_t data);
#endif /*__MAX7219__H_*/
