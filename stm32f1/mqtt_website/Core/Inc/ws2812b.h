#ifndef __ADDRESSABLE_LEDS__
#define __ADDRESSABLE_LEDS__

#include <stdint.h>
#define TIMER_PERIOD ((SystemCoreClock / 800000) - 1)
#define TIMER_PRESCALER 0
void led_set_color_ascii(char *ascii_color ,uint16_t led_id, uint32_t *color_out);
void set_color(uint32_t color_rgb, uint16_t led_id);
void start_led_sequence(void);
void stop_led_sequence(void);
void parse_led_color_input(char *ascii_color, uint32_t *color_out);

#endif /*__ADDRESSABLE_LEDS__*/
