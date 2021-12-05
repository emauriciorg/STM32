#include "ssd1306.h"

#include <stdint.h>
typedef struct{
	uint8_t fontsize;
	uint8_t font_id;
	uint8_t raw_font

}font_type;

void put_char (uint8_t *raw_char,uint16_t char_width, uint16_t char_heigth)
{

	for (uint8_t page= 0 ;page < char_heigth/8; page++){
		set_page(page);
		set_colum(colum);
		set_pixel(raw_char+(page*char_width), char_width);
	}


}
const char raw_font_8x8[10][8*withd]{

	{ /*digit 0*/
	 	0 1 1 0 0 1 1 0
		0 1 1 0 1 0 1 0
		0 1 1 1 0 0 1 0
		0 1 1 0 0 0 1 0
		0 1 1 0 0 0 1 0
		0 1 1 1 1 1 1 0

	},
	{

		0 0 0 0 0 0 0 0
		0 0 1 1 1 0 0 0
		0 1 1 1 1 0 0 0
		0 0 0 1 1 0 0 0
		0 0 0 1 1 0 0 0
		0 0 0 1 1 0 0 0
		0 0 0 1 1 0 0 0
		0 0 1 1 1 0 0 0

	},
	{
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0

	},
	{
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0

	},
	{
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0

	},
	{
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0

	},
	{
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
	}
};
