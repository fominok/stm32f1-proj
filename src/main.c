#include "rtc.h"
#include "eeprom.h"
#include "kbd.h"
#include "delay.h"
#include "LiquidCrystal_I2C.h"
#include <stdio.h>

// For __NOP()
#include "stm32f10x_gpio.h"

uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t dayOfWeek;
uint8_t dayOfMonth;
uint8_t month;
uint8_t year;

char str[15];

int main(void) {
  LCDI2C_init(0x27, 20, 4);
  LCDI2C_backlight();
  Delay(2000);
  //LCDI2C_write('N');
  init_gpio_keypad_clk();
  init_timer_keypad_clk();
  init_gpio_keypad_read();

  LCDI2C_write_String("before");
  Delay(500);
  //write_eeprom(0x1488, 0xEF);
  Delay(500);
  uint8_t test;
  test = read_eeprom(0x1488);
  sprintf(str, "%d", test);
  LCDI2C_write_String(str);
//set_rtc_time(40, 44, 23, 7, 26, 11, 16);
  //read_rtc_time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  ////bitch();
  //sprintf(str, "%d", minute);
  //LCDI2C_write_String(str);
  //LCDI2C_write_String("finished");
  while(1) {
    __NOP();
  }
}
