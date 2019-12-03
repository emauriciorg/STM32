#include <stdio.h>


#include "stm32f1xx_hal.h"

/*MAX value for iwdg time is 26.21*/
#define WATCHDOG_PERIOD_IN_SEC 25.6
#define WATCHDOG_PRESCALER     IWDG_PRESCALER_256
#define LOW_FREQ_OSC           40000
#define WATCDOG_AUTO_RELOAD_VALUE  ((WATCHDOG_PERIOD_IN_SEC/WATCHDOG_PRESCALER) *LOW_FREQ_OSC)

typedef struct {
	uint32_t window_ms;
	uint8_t  enabled;
} soft_watchdog_t;

volatile soft_watchdog_t soft_wdt;

WWDG_HandleTypeDef hwwdg;


static void setup_stm_wdt(void);
static void init_reset_pin(void);


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


static void init_reset_pin(void)
{
	GPIO_InitTypeDef   GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin   = WATCHDOG_RESET_PIN;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	DISABLE_SYSTEM_RESET;

	HAL_GPIO_DeInit(WATCHDOG_RESET_PORT, WATCHDOG_RESET_PIN);
	HAL_GPIO_Init(WATCHDOG_RESET_PORT, &GPIO_InitStruct);
}

void watchdog_enable(void)
{
	soft_wdt.enabled = 1;
}

void watchdog_disable(void)
{
	soft_wdt.enabled = 0;
}
