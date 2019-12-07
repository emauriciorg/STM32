#include <stdio.h>


#include "stm32f1xx_hal.h"

/*MAX value for iwdg time is 26.21*/
//WIDNOW_VALUE_IN_MS

/*
	RESET CONDITIONS
	downcounter goes below 0x40
	downcounter is reloaded outsite the window

	ISR
	Early wake interrupt when the downcounter is equal to 0x40
*/

#define WATCHDOG_PERIOD_IN_SEC 25.6
#define WATCHDOG_PRESCALER     WWDG_PRESCALER_2
#define LOW_FREQ_OSC           40000
#define WATCDOG_AUTO_RELOAD_VALUE  ((WATCHDOG_PERIOD_IN_SEC/WATCHDOG_PRESCALER) *LOW_FREQ_OSC)

#define WDT_ENABLE_BIT     0x08
#define WIDNOW_VALUE_IN_MS 100


typedef struct {
	uint32_t window_ms;
	uint8_t  enabled;
} soft_watchdog_t;


volatile soft_watchdog_t soft_wdt;

WWDG_HandleTypeDef hwwdg;


static void setup_stm_wdt(void);


void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
	if (soft_wdt.window_ms) {
		GPIOB->ODR ^= GPIO_PIN_0;
		__HAL_WWDG_CLEAR_IT(hwwdg,WWDG_FLAG_EWIF);
		HAL_WWDG_Refresh(hwwdg);
		if (soft_wdt.enabled) {
			soft_wdt.window_ms--;
		}else{
			soft_wdt.window_ms = soft_wdt.window_ms;
		}
	}else{
		soft_wdt.window_ms = 0;
		/*
		HERE IT resets the mcu, before that, we  a
		a log can be saved
		*/
	}
}

/****************************************/
/* Initializing the watchdog			*/
/* Setting it to around 10 seconds		*/
/****************************************/
void feed_watchdog(void)
{
	soft_wdt.window_ms = WIDNOW_VALUE_IN_MS;
}


void init_wwdg(){
	watchdog_window_set(WATCDOG_AUTO_RELOAD_VALUE);

	setup_stm_wdt();

}


void watchdog_window_set(uint32_t wdt_time_ms)
{
	soft_wdt.window_ms = wdt_time_ms;
}

static void setup_stm_wdt(void) /*taken from stmcubemx*/
{
	hwwdg.Instance       = WWDG;
	hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
	hwwdg.Init.Window    = 127;
	hwwdg.Init.Counter   = 127;
	hwwdg.Init.EWIMode   = WWDG_EWI_ENABLE;
	HAL_WWDG_Init(&hwwdg);
}



void watchdog_enable(void)
{
	soft_wdt.enabled = 1;
}

void watchdog_disable(void)
{
	soft_wdt.enabled = 0;
}
