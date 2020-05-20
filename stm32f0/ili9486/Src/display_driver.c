#include <string.h>
#include "display_driver.h"
#include "display_registers.h"

/*----------------------------lcd pin macros----------------------------------*/
#define DB10_PIN   LCD_DB10_PIN
#define DB10_PORT  LCD_DB10_PORT
#define DB11_PIN   LCD_DB11_PIN
#define DB11_PORT  LCD_DB11_PORT
#define DB12_PIN   LCD_DB12_PIN
#define DB12_PORT  LCD_DB12_PORT
#define DB13_PIN   LCD_DB13_PIN
#define DB13_PORT  LCD_DB13_PORT
#define DB14_PIN   LCD_DB14_PIN
#define DB14_PORT  LCD_DB14_PORT
#define DB15_PIN   LCD_DB15_PIN
#define DB15_PORT  LCD_DB15_PORT
#define DB16_PIN   LCD_DB16_PIN
#define DB16_PORT  LCD_DB16_PORT
#define DB17_PIN   LCD_DB17_PIN
#define DB17_PORT  LCD_DB17_PORT

#define RD_PIN      LCD_RD_PIN
#define RD_PORT     LCD_RD_PORT
#define DC_PIN      LCD_DC_PIN
#define DC_PORT     LCD_DC_PORT
#define WR_PIN      LCD_WR_PIN
#define WR_PORT     LCD_WR_PORT
#define CS_PIN      LCD_CS_PIN
#define CS_PORT     LCD_CS_PORT

#define RST_PIN     LCD_RST_PIN
#define RST_POR     LCD_RST_PORT

#define CLR_RD      LCD_CLR_RD
#define SET_RD      LCD_SET_RD
#define CLR_RESET   LCD_CLR_RESET
#define SET_RESET   LCD_SET_RESET
#define CLR_CS      LCD_CLR_CS
#define SET_CS      LCD_SET_CS
#define CLR_CD      LCD_CLR_DC
#define SET_CD      LCD_SET_DC
#define CLR_WR      LCD_CLR_WR
#define SET_WR      LCD_SET_WR

#define DATA_PORT_SET(bits) LCD_DATA_PORT_SET(bits)
#define CLEAR_DATA_PORT  LCD_CLEAR_DATA_PORT
/*--------------------------------------local routines------------------------*/

static void dd_write_reg(uint16_t lcd_register, uint16_t data);
static void LCD_init_gpios(void);
static void dd_write_reg(uint16_t index,uint16_t dat);


/*--------------------------------------routines------------------------------*/

void write_command(uint16_t command)
{
	CLEAR_DATA_PORT;
	CLR_CD;
	CLR_CS;
	DATA_PORT_SET (0x00FF&(command>>8));
	CLR_WR;
	SET_WR;
	CLEAR_DATA_PORT;
	DATA_PORT_SET(0x00FF&(command));
	CLR_WR;
	SET_WR;
	SET_CS;
}


void write_data(uint16_t data)
{
	CLEAR_DATA_PORT;
	CLR_CS;
	SET_CD;
	DATA_PORT_SET(0x00FF&(data>>8));
	CLR_WR;
	SET_WR;
	CLEAR_DATA_PORT;
	DATA_PORT_SET(0x00FF&(data));
	CLR_WR;
	SET_WR;
	SET_CS;
}


static void dd_write_reg(uint16_t lcd_register, uint16_t data)
{
	write_command(lcd_register);
	write_data(data);
}


void init_display_panel(unsigned char alignment)
{

	uint16_t display_reg_aligment;
	LCD_init_gpios();

	if(alignment == REVERSEALIGN)
		display_reg_aligment = DD_ALIGMENT_REVERSE;
	else
		display_reg_aligment = DD_ALIGMENT_NORMAL;

	SET_RD;
	SET_WR;
	SET_CS;
	SET_CD;
	CLEAR_DATA_PORT;

	delay_us(500);
	CLR_RESET;
	delay_us(500);
	SET_RESET;
	delay_us(500);

	dd_write_reg(DD_VCOM_OPT_1           , 0X0006);
	dd_write_reg(DD_OSCILATION_START     , DD_START_OSCILLATOR);
	dd_write_reg(DD_SLEEP_MODE_1         , DD_OUT_OF_SLEEP);
	dd_write_reg(DD_DRIVER_OUTPUT_CONTROL, display_reg_aligment);
	dd_write_reg(DD_ENTRY_MODE_1         , DD_DEFAULT_DISPLAY_DIRECTION);
	dd_write_reg(DD_GENERIC_INTERFACE    , 0x0000);    // IhVCDOCLK#,HSYNC#,VSYNC#,
	dd_write_reg(DD_DRIVER_AC_CONTROL    , 0x0600);    // LCD Driving Waveform Control


	dd_write_reg(DD_POWER_CONTROL_1     , 0x6A38);   // Power Control 1
	dd_write_reg(DD_GATE_SCAN_START_POS , 0x0000);   // Gate Scan Position
	dd_write_reg(DD_FRAME_CYCLE_CONTROL , 0x5308);   // Frame Cycle Control
	dd_write_reg(DD_POWER_CONTROL_2     , 0x0003);   // Power Control 2
	dd_write_reg(DD_POWER_CONTROL_3     , 0x000A);   // Power Control 3
	dd_write_reg(DD_POWER_CONTROL_4     , 0x2B00);   // Power Control 4
	dd_write_reg(DD_POWER_CONTROL_5     , 0x00B7);   // Power Control 5
	dd_write_reg(DD_FRAME_FREQUENCY     , 0x8000);   // Frame Frequency Control
	dd_write_reg(DD_ANALOGUE_SETTING    , 0X3800);   // Analog setting

	dd_write_reg(DD_SLEEP_MODE_2              ,  0x0D99);  // Sleep mode
	dd_write_reg(DD_VERTICAL_RAM_ADDRESS     ,  0xEF00);   // Per settings from COSMICVOID
	dd_write_reg(DD_HORIZONTAL_RAM_ADDR_START,  0X0000);   // Per settings from COSMICVOID
	dd_write_reg(DD_HORIZONTAL_RAM_ADDR_END  ,  0X013F);   // Per settings from COSMICVOID
	dd_write_reg(DD_SET_GGDRAM_X             ,  0X0000);   // Ram Address Set
	dd_write_reg(DD_SET_GGDRAM_Y             ,  0x0000);   // Ram Address Set
	dd_write_reg(DD_SLEEP_MODE_2             ,  0x08D9);   // Sleep mode
	dd_write_reg(DD_GAMA_CONTROL_1           ,  0x0000);	// Gamma Control (R30h to R3Bh) -- Page 56 of SSD2119 datasheet
	dd_write_reg(DD_GAMA_CONTROL_2           ,  0x0104);
	dd_write_reg(DD_GAMA_CONTROL_3           ,  0x0100);
	dd_write_reg(DD_GAMA_CONTROL_4           ,  0x0305);
	dd_write_reg(DD_GAMA_CONTROL_5           ,  0x0505);
	dd_write_reg(DD_GAMA_CONTROL_6           ,  0x0305);
	dd_write_reg(DD_GAMA_CONTROL_7           ,  0x0707);
	dd_write_reg(DD_GAMA_CONTROL_8           ,  0x0300);
	dd_write_reg(DD_GAMA_CONTROL_9           ,  0x1200);
	dd_write_reg(DD_GAMA_CONTROL_9           ,  0x0800);
	dd_write_reg(DD_DISPLAY_CONTROL          ,  0x0033);    // Display Control
	delay_us(1000);
	write_command(DD_RAM_DATA_WRITE_READ);                  // RAM data write/read


}


static void LCD_init_gpios(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	struct lcd_lines_st{
		GPIO_TypeDef *port;
		uint16_t     pin;
	};

	struct lcd_lines_st lcd_lines[TOTAL_LCD_PINS] = {
		{ LCD_DB10_PORT , LCD_DB10_PIN },
		{ LCD_DB11_PORT , LCD_DB11_PIN },
		{ LCD_DB12_PORT , LCD_DB12_PIN },
		{ LCD_DB13_PORT , LCD_DB13_PIN },
		{ LCD_DB14_PORT , LCD_DB14_PIN },
		{ LCD_DB15_PORT , LCD_DB15_PIN },
		{ LCD_DB16_PORT , LCD_DB16_PIN },
		{ LCD_DB17_PORT , LCD_DB17_PIN },
		{ LCD_RD_PORT   , LCD_RD_PIN   },
		{ LCD_DC_PORT   , LCD_DC_PIN   },
		{ LCD_WR_PORT   , LCD_WR_PIN   },
		{ LCD_CS_PORT   , LCD_CS_PIN   },
		{ LCD_RST_PORT  , LCD_RST_PIN  },
	};


	for (uint16_t led_id  = 0; led_id < TOTAL_LCD_PINS ; led_id++)
	{
		HAL_GPIO_WritePin(lcd_lines[led_id].port,lcd_lines[led_id].pin, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin   = lcd_lines[led_id].pin;
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull  = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(lcd_lines[led_id].port, &GPIO_InitStruct);
		memset(&GPIO_InitStruct, 0x00, sizeof(GPIO_InitStruct));
	}
}


void Display_Home(void)
{
	dd_write_reg(DD_SET_GGDRAM_X, 0);
	dd_write_reg(DD_SET_GGDRAM_Y,0);
	write_command(DD_RAM_DATA_WRITE_READ);
}



int gotoxy(int y, int x)
{
	if (x > LASTCOL || y > LASTROW)
		return OUT_OF_BOUND;
	dd_write_reg(DD_SET_GGDRAM_X, x);
	dd_write_reg(DD_SET_GGDRAM_Y, y);
	write_command(DD_RAM_DATA_WRITE_READ);
	return CURSOR_SET;
}



void LCDSetTCur(unsigned short row, unsigned short col, unsigned short CJ_code)
{
	unsigned char a;
	unsigned short x, y;

	a = CJ_code & ROTN;
	if (a == 0)
	{
		x = col;
		y = row;
	}
	else if (a == ROT90)
	{
		x = row;
		y = LASTROW - col;
	}
	else if (a == ROT180)
	{
		x = LASTCOL - col;
		y = LASTROW - row;
	}
	else	// ROT270
	{
		x = LASTCOL - row;
		y = col;
	}
	/* Ver 1.1 - Commented to allow wraping of text
	if (x > LASTCOL || y > LASTROW) return; */

	{
		x = x%LASTCOL;
		y = y%LASTROW;
	}

	dd_write_reg(DD_SET_GGDRAM_X , x);
	dd_write_reg(DD_SET_GGDRAM_Y , y);
	write_command(DD_RAM_DATA_WRITE_READ);
}
