#ifndef __DISPLAY_DRIVER_H__
#define __DISPLAY_DRIVER_H__

#include <stdint.h>
#include "STMConfig.h"

#define TOTAL_LCD_PINS 13


#define LCD_DATA_PORT_SET(lcd_data) {\
	if(lcd_data & 0X01) { LCD_DB10_PORT->BSRR = LCD_DB10_PIN;}\
	if(lcd_data & 0X02) { LCD_DB11_PORT->BSRR = LCD_DB11_PIN;}\
	if(lcd_data & 0X04) { LCD_DB12_PORT->BSRR = LCD_DB12_PIN;}\
	if(lcd_data & 0X08) { LCD_DB13_PORT->BSRR = LCD_DB13_PIN;}\
	if(lcd_data & 0X10) { LCD_DB14_PORT->BSRR = LCD_DB14_PIN;}\
	if(lcd_data & 0X20) { LCD_DB15_PORT->BSRR = LCD_DB15_PIN;}\
	if(lcd_data & 0X40) { LCD_DB16_PORT->BSRR = LCD_DB16_PIN;}\
	if(lcd_data & 0X80) { LCD_DB17_PORT->BSRR = LCD_DB17_PIN;}\
}
#define LCD_CLEAR_DATA_PORT {\
	LCD_DB10_PORT->BRR = LCD_DB10_PIN;\
	LCD_DB11_PORT->BRR = LCD_DB11_PIN;\
	LCD_DB12_PORT->BRR = LCD_DB12_PIN;\
	LCD_DB13_PORT->BRR = LCD_DB13_PIN;\
	LCD_DB14_PORT->BRR = LCD_DB14_PIN;\
	LCD_DB15_PORT->BRR = LCD_DB15_PIN;\
	LCD_DB16_PORT->BRR = LCD_DB16_PIN;\
	LCD_DB17_PORT->BRR = LCD_DB17_PIN;\
}

// SSD2119 controller entry modes

#define WRTDWN	0x6878	// rows downward, cols rightward, 0�
#define WRTRGT	0x6870	// rows rightward, cols upward, 90�
#define WRTUP	0x6848	// rows upward, cols leftward, 180�
#define WRTLFT	0x6860	// rows leftward, cols downward, 270�



#define ROTN	0xC0	// bit mask
#define ROT270	0xC0	// 270� CCW, top to bottom
#define ROT180	0x80	// 180� CCW, right to left
#define ROT90	0x40	// 90� CCW, bottom to top

#define DD_DEFAULT_DISPLAY_DIRECTION     WRTRGT

/*END OF PINOUT MANIPULATION*/
#define NR_COLS		320
#define LASTCOL		(NR_COLS - 1)
#define NR_ROWS		240
#define LASTROW		(NR_ROWS - 1)

#define OUT_OF_BOUND    -2
#define CURSOR_SET      0


//#define SSD2119_ENTRY_MODE_DEFAULT 0x6830

#define SSD2119_ENTRY_MODE_SET(x) ((DD_DEFAULT_DISPLAY_DIRECTION & 0xFF00) | (x))


int gotoxy(int y, int x);
void init_display_panel(unsigned char alignment);
void Display_Home(void);
void LCDSetTCur(unsigned short row, unsigned short col, unsigned short CJ_code);
void write_data(uint16_t data);
void write_command(uint16_t command);


#endif /*__DISPLAY_DRIVER_H__*/
