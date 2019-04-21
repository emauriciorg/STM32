#include "i2c.h"

#include <stdio.h>
#include <string.h>
#include "stm32f0xx.h"


#define I2C_HANDLE hi2c1
#define PTR_HANDLE &I2C_HANDLE

#define MAX_ADDR 0X7F

#define I2C_DEBUG_ENABLED
#ifdef I2C_DEBUG_ENABLED
	#define I2C_DBG(...) printf(__VA_ARGS__)
#else
	#define I2C_DBG(...)
#endif

extern I2C_HandleTypeDef I2C_HANDLE;

#define CMD_DISPLAY_ON 0XAF
#define CMD_DISPLAY_OFF 0XAE
#define OLED_ADDR 0X79


typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;
#if 0
typedef union{
	struct{
		uint8_t Continuation:1, /* next byte is 0:data 1:command */
			data_command_sel:1, /*0:command, 1:data stored GDDRAM */
			control_byte:6;
	}bits;
	uint8_t all;
};

#endif

#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

#define CMD_SET_PAGE 0xB0 //0XB0 TO 0xB7 PAGE[0-8]
#define CMD_SET_COLUMN_LOW 0x00
#define CMD_SET_COLUMN_HIGH 0X10

#define COLUMN_MAX_SIZE 128
#define ROWS_MAX_SIZE  8

// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];


void ssd1306_WriteData(uint8_t* buffer, size_t buff_size)
{

	HAL_I2C_Mem_Write(PTR_HANDLE, OLED_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}
void ssd1306_WriteCommand(uint8_t byte)
{

#ifdef MASTER_TRANSMIT_I2C_FUNCTION_ENABLED
	uint8_t frame_out[2];
	frame_out[0] = 0x00;
	frame_out[1] = byte;
	HAL_I2C_Master_Transmit(PTR_HANDLE, OLED_ADDR , frame_out, 2, HAL_MAX_DELAY);
#else
	HAL_I2C_Mem_Write(PTR_HANDLE, OLED_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
#endif
}


// Write the screenbuffer with changed to the screen
inline void ssd1306_set_page(uint8_t page)
{
	ssd1306_WriteCommand(CMD_SET_PAGE+ page); //set page addres
}

inline void ssd1306_set_column(uint8_t column)
{
	ssd1306_WriteCommand(0x00 | (column&0x0F));
	ssd1306_WriteCommand(0x10 |((column&0xF0) >> 4));
}

void set_pixels(uint8_t *image, uint16_t image_size)
{
	ssd1306_WriteData(image,image_size);
}



void ssd1306_UpdateScreen(void) {
    uint8_t i;
    for(i = 0; i < 8; i++) {

//	ssd1306_WriteCommand(0xB0 + i); //set page addres
	/*Set column lower part and upper part*/
	// ssd1306_WriteCommand(0x00);
	// ssd1306_WriteCommand(0x10);

	ssd1306_set_page(i);
	ssd1306_set_column(0x00);

	ssd1306_WriteData(&SSD1306_Buffer[SSD1306_WIDTH*i],SSD1306_WIDTH);
    }
}

void ssd1306_Fill(SSD1306_COLOR color) {
    /* Set memory */
    uint32_t i;

    for(i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}
void oled_init(void)
{
	// Init OLED
        ssd1306_WriteCommand(0xAE); //display off

        ssd1306_WriteCommand(0x20); //Set Memory Addressing Mode
        ssd1306_WriteCommand(0x10); // 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode;
                                    // 10,Page Addressing Mode (RESET); 11,Invalid

        ssd1306_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

        ssd1306_WriteCommand(0xC8); //Set COM Output Scan Direction

        ssd1306_WriteCommand(0x00); //---set low column address
        ssd1306_WriteCommand(0x10); //---set high column address

        ssd1306_WriteCommand(0x40); //--set start line address - CHECK

        ssd1306_WriteCommand(0x81); //--set contrast control register - CHECK
        ssd1306_WriteCommand(0x0f); //ff

        ssd1306_WriteCommand(0xA1); //--set segment re-map 0 to 127 - CHECK

        ssd1306_WriteCommand(0xA6); //--set normal color

        ssd1306_WriteCommand(0xA8); //--set multiplex ratio(1 to 64) - CHECK
        ssd1306_WriteCommand(0x3F); //

        ssd1306_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

        ssd1306_WriteCommand(0xD3); //-set display offset - CHECK
        ssd1306_WriteCommand(0x00); //-not offset

        ssd1306_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
        ssd1306_WriteCommand(0xF0); //--set divide ratio

        ssd1306_WriteCommand(0xD9); //--set pre-charge period
        ssd1306_WriteCommand(0x22); //

        ssd1306_WriteCommand(0xDA); //--set com pins hardware configuration - CHECK
        ssd1306_WriteCommand(0x12);

        ssd1306_WriteCommand(0xDB); //--set vcomh
        ssd1306_WriteCommand(0x20); //0x20,0.77xVcc

        ssd1306_WriteCommand(0x8D); //--set DC-DC enable
        ssd1306_WriteCommand(0x14); //
        ssd1306_WriteCommand(0xAF); //--turn on SSD1306 panel

        // Clear screen
        ssd1306_Fill(Black);
	ssd1306_UpdateScreen();

}
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {

    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }

    // Check if pixel should be inverted


    // Draw in the right color
    if(color == White) {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}


void write_register(uint16_t device_addr, uint8_t *frame_in, uint16_t len){

	HAL_I2C_Mem_Write(PTR_HANDLE, OLED_ADDR, 0x00, 1, &frame_in, 1, HAL_MAX_DELAY);

//	HAL_I2C_Master_Transmit(PTR_HANDLE, device_addr , &frame_in, len , 100);

}

void oled_draw_chest(void)
{
	uint8_t  half_column_chest[(COLUMN_MAX_SIZE/2)];
	uint8_t  zeroes[(COLUMN_MAX_SIZE/2)];
	memset(zeroes,0,sizeof(zeroes));
	uint16_t chestboard_size = sizeof(half_column_chest);
	uint16_t pixels;
	HAL_Delay(2000);

	for(pixels = 0 ; pixels < chestboard_size; pixels++) {
		half_column_chest[pixels] = (0x0AA >> (pixels %2));
	}

	uint8_t pages = 0;
	for (pages = 0 ; pages < 8; pages++ )
	{
		ssd1306_set_page(pages);
		if (pages %2){
			ssd1306_set_column(0);
			set_pixels(half_column_chest, chestboard_size);
			ssd1306_set_column(64);
			set_pixels(zeroes, chestboard_size);

		}else{
			ssd1306_set_column(0);
			set_pixels(zeroes, chestboard_size);
			ssd1306_set_column(64);
			set_pixels(half_column_chest, chestboard_size);
		}
		HAL_Delay(100);
	}

}

void oled_animated_square(void)
{
	uint8_t  open_mouth[8];
	uint8_t  closed_mouth[8];
	uint8_t  blank_space[8];
	uint8_t  points_to_eat[8];
	uint8_t  column = 0;
	uint8_t  path_points_to_eat[128];

	uint16_t animation_size = sizeof(open_mouth);
/*
	fruits/points

	00000000   0 0 0 0 1 1 0 0  0 0 0 1 1 0 0 0   00011100   00011100
	00000000   0 0 1 1 1 1 1 0  0 1 1 1 1 1 1 0   00111110   01111110
	00011000   0 1 1 0 0 1 0 0  0 1 1 0 0 1 1 0   01111110   01111110
	00111100   1 1 1 1 1 0 0 0  1 1 1 1 1 1 1 1   01011111   11011111
	00111100   1 1 1 1 0 0 0 0  1 1 1 1 1 1 1 1   11010111   11011111
	00011000   1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1   11100111   01111111
	00000000   0 1 1 1 1 1 1 0  0 1 1 1 1 1 1 0   01000110   01111110
	00000000   0 0 0 1 1 1 0 0  0 0 0 1 1 1 0 0   00000100   00011100
*/

	points_to_eat[0] = 0b00000000;
	points_to_eat[1] = 0b00000000;
	points_to_eat[2] = 0b00011000;
	points_to_eat[3] = 0b00111100;
	points_to_eat[4] = 0b00111100;
	points_to_eat[5] = 0b00011000;
	points_to_eat[6] = 0b00000000;
	points_to_eat[7] = 0b00000000;


	for(column = 0 ; column < (sizeof(path_points_to_eat)); column +=8) {
		memcpy(&path_points_to_eat[column],&points_to_eat[0],8);
	}
	open_mouth[0]   = 0b00011100;
	open_mouth[1]   = 0b00111110;
	open_mouth[2]   = 0b01111110;
	open_mouth[3]   = 0b01011111;
	open_mouth[4]   = 0b11010111;
	open_mouth[5]   = 0b11100111;
	open_mouth[6]   = 0b01000110;
	open_mouth[7]   = 0b00000100;

	closed_mouth[0] = 0b00011100;
	closed_mouth[1] = 0b01111110;
	closed_mouth[2] = 0b01111110;
	closed_mouth[3] = 0b11011111;
	closed_mouth[4] = 0b11011111;
	closed_mouth[5] = 0b01111111;
	closed_mouth[6] = 0b01111110;
	closed_mouth[7] = 0b00011100;


	memset(blank_space, 0 , sizeof(blank_space));

	uint8_t column_move=0;

	/*sets points to eat */
	ssd1306_set_page(0x5);
	ssd1306_set_column(0);
	set_pixels(path_points_to_eat,sizeof(path_points_to_eat));

	ssd1306_set_page(0x5);
	ssd1306_set_column(0);
	set_pixels(open_mouth, animation_size);

	HAL_Delay(100);

	for(column_move = 1; column_move< (128-8) ; column_move++){

		ssd1306_set_column(column_move -1  );
		set_pixels(blank_space, 8);

		ssd1306_set_column(column_move);
		set_pixels(closed_mouth, animation_size);
		HAL_Delay(50);

		ssd1306_set_column(column_move);
		set_pixels(open_mouth, animation_size);
		HAL_Delay(100);

	}
	ssd1306_set_column(column_move-1);
	set_pixels(blank_space, 8);


}


void i2c_test(void)
{

	uint8_t column = 0;
	uint8_t row  = 0;

	for (column = 0; column < SSD1306_WIDTH; column = column+8 ){
		for (row = 0; row < SSD1306_HEIGHT; row ++)
			ssd1306_DrawPixel(column,row,White);
	}
	ssd1306_UpdateScreen();

	oled_draw_chest();
	oled_animated_square();
//	i2c_scan();
}
void read_register (uint8_t register_addr, uint16_t device_addr)
{
	uint8_t frame_out;
/*
1. trasmit address1
2. trasmist control registers/byte with the c0, dR
3. trasmit/recieve register content /
4. read

*/
	HAL_I2C_Master_Receive(PTR_HANDLE, device_addr , &frame_out, 1 , 100);

}

void turn_on_oled(void){

}
void i2c_scan(void)
{
	uint16_t device_addr = OLED_ADDR;
	uint8_t frame_out = 0;
	//uint8_t device_found = 0;
	//OLED found at the 0X79

#if 1
	for (device_addr = 1; device_addr < MAX_ADDR ; device_addr+=0x02){
		HAL_I2C_Master_Receive(PTR_HANDLE, device_addr , &frame_out, 1 , 100);
	//	HAL_I2C_Master_Receive(PTR_HANDLE, device_addr | 0x01, &data_out, 1 , 100);
		if (frame_out != 0){
			I2C_DBG("found device at addr 0x%x data 0x%x\r\n",device_addr, frame_out);
			frame_out = 0;
		}
	}
#endif

	device_addr = OLED_ADDR;
	HAL_I2C_Master_Receive(PTR_HANDLE, device_addr , &frame_out, 1 , 100);
	HAL_I2C_Master_Receive(PTR_HANDLE, device_addr , &frame_out, 1 , 100);
	I2C_DBG("Idle  addr 0x%x data 0x%x\r\n",device_addr, frame_out);


	I2C_DBG ("Scanning finish \r\n");

	uint8_t command ;
	uint8_t data ;

	data    = CMD_DISPLAY_ON;
	command = 0x00;





	// HAL_I2C_Master_Transmit(PTR_HANDLE, device_addr , &frame_in[1], 1 , 100);
	// HAL_I2C_Master_Transmit(PTR_HANDLE, device_addr , &frame_in[2], 1 , 100);
	HAL_I2C_Master_Receive(PTR_HANDLE, device_addr , &frame_out, 1 , 100);

//	write_register (OLED_ADDR, frame_in,3);

	I2C_DBG("ON  0x%x data 0x%x\r\n",device_addr, frame_out);

}



uint8_t i2c_read(void)
{

//	HAL_I2C_Master_Transmit(PTR_HANDLE,);
	return 0;
}


uint8_t i2c_write(void)
{
	return 0;
}
