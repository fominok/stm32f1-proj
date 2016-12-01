#include "rtc.h"
#include "eeprom.h"
#include "kbd.h"
#include "delay.h"
#include "LiquidCrystal_I2C.h"
#include <stdio.h>

// For __NOP()
#include "stm32f10x_gpio.h"

volatile uint8_t digits_entered;
volatile uint32_t ticks_passed;

typedef enum {
  IDLE,
  CODE_INPUT,
  LOG_CANCELLED,
  CODE_CHECK,
  LOG_SUCCEED,
  DOOR_OPEN,
  LOG_FAILED,
  SERVICE_MODE,
  SERVICE_CODE_INPUT,
  SERVICE_LOG_VIEW
} state_t;

state_t state = IDLE;

void automaton(void) {
  switch(state) {
  case IDLE:
    if (digits_entered > 0) {
      state = CODE_INPUT;
    }
    break;
  case CODE_INPUT:
    if (digits_entered > 3) {
      digits_entered = 0;
      state = CODE_CHECK;
    } else if (ticks_passed > 200) {
      state = LOG_CANCELLED;
    }
    break;
  case LOG_CANCELLED:
    LCDI2C_write_String("interval!");
    digits_entered = 0;
    state = IDLE;
    break;
  case CODE_CHECK:
    LCDI2C_write_String("check!");
    digits_entered = 0;
    state = IDLE;
    break;
  }
}

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
  Delay(500);
  //LCDI2C_write('N');
  init_gpio_keypad_clk();
  init_timer_keypad_clk();
  init_gpio_keypad_read();

  //write_eeprom(0x1488, 0xEF);
  //uint8_t test;
  //test = read_eeprom(0x1488);
  //sprintf(str, "%d", test);
  //LCDI2C_write_String(str);
  //read_rtc_time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  //LCDI2C_write_String("finished");

  state = IDLE;
  digits_entered = 0x0;
  while(1) {
    LCDI2C_setCursor(0,0);
    sprintf(str, "%d %d|", digits_entered, ticks_passed);
    LCDI2C_write_String(str);
    automaton();
    Delay(200);
  }
}
