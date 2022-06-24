#ifndef __RTC_API_H__
#define __RTC_API_H__

#include <stdint.h>
#include <stdbool.h>

#define RTC_MOCK

typedef struct date_time_st{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds;
    uint16_t microseconds;
    uint16_t dayOfWeek;
    uint16_t dayOfMonth;
    bool hour_24h_format;
}date_time_t;

typedef struct
{
  void (*init)(void);
  void (*set_time)(date_time_t date_time);
  void (*get_time)(date_time_t *date_time);
}RTC_driver_t;

void rtc_init(uint8_t address);
void rtc_set_time(date_time_t date_time);
void rtc_get_time(date_time_t *date_time);


#endif /*__RTC_API_H__*/
