#ifndef rtc_header
#define rtc_header
#include <stdint.h>

uint8_t dec_to_bcd(uint8_t val);
uint8_t bcd_to_dec(uint8_t val);
void set_rtc_time(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t
                  dayOfMonth, uint8_t month, uint8_t year);
void read_rtc_time(uint8_t *second, uint8_t *minute, uint8_t *hour,
                   uint8_t *dayOfWeek, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year);

#endif
