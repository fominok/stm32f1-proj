#include "rtc.h"
#include "eeprom.h"
#include "kbd.h"
#include "delay.h"
#include "LiquidCrystal_I2C.h"
#include <stdio.h>

// For __NOP()
#include "stm32f10x_gpio.h"

#define CODE_BASE_ADDR 0x0000
#define LOGS_CURSOR_ADDR 0x0004
#define LOGS_BASE_ADDR 0x0005
#define STATUS_SUCCEED 0
#define STATUS_FAILED 1
#define STATUS_CANCELLED 2

volatile uint8_t digits_entered;
volatile uint32_t ticks_passed;
volatile uint8_t input_buffer[4];

uint8_t output_buffer[64];

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
uint8_t real_code_buf[4];

void write_log(uint8_t input_status) {
  uint8_t cursor = read_eeprom(LOGS_CURSOR_ADDR);
  cursor++;
  uint16_t record_base_addr = cursor * 8 + LOGS_BASE_ADDR;
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t dayOfWeek;
  uint8_t dayOfMonth;
  uint8_t month;
  uint8_t year;
  read_rtc_time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  Delay(50);
  write_eeprom(LOGS_CURSOR_ADDR, cursor);
  Delay(50);
  write_eeprom(record_base_addr, input_status);
  Delay(50);
  write_eeprom(record_base_addr + 1, second);
  Delay(50);
  write_eeprom(record_base_addr + 2, minute);
  Delay(50);
  write_eeprom(record_base_addr + 3, hour);
  Delay(50);
  write_eeprom(record_base_addr + 4, dayOfWeek);
  Delay(50);
  write_eeprom(record_base_addr + 5, dayOfMonth);
  Delay(50);
  write_eeprom(record_base_addr + 6, month);
  Delay(50);
  write_eeprom(record_base_addr + 7, year);
  Delay(50);
}

void read_log(uint8_t cursor) {
  uint16_t record_base_addr = cursor * 8 + LOGS_BASE_ADDR;
  uint8_t input_status = read_eeprom(record_base_addr);
  Delay(50);
  uint8_t second = read_eeprom(record_base_addr + 1);
  Delay(50);
  uint8_t minute = read_eeprom(record_base_addr + 2);
  Delay(50);
  uint8_t hour = read_eeprom(record_base_addr + 3);
  Delay(50);
  uint8_t dayOfWeek = read_eeprom(record_base_addr + 4);
  Delay(50);
  uint8_t dayOfMonth = read_eeprom(record_base_addr + 5);
  Delay(50);
  uint8_t month = read_eeprom(record_base_addr + 6);
  Delay(50);
  uint8_t year = read_eeprom(record_base_addr + 7);
  Delay(50);

  sprintf(output_buffer,
          "%d %d %d %d %d %d %d %d",
          input_status,
          second,
          minute,
          hour,
          dayOfWeek,
          dayOfMonth,
          month,
          year
  );
  LCDI2C_write_String(output_buffer);
  //read_rtc_time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
}

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
    write_log(STATUS_CANCELLED);
    //LCDI2C_setCursor(0,0);
    //read_log(0);
    digits_entered = 0;
    state = IDLE;
    break;
  case CODE_CHECK:
    if (!strncmp(real_code_buf, input_buffer, 4)) {
      LCDI2C_write_String("correct!") ;
      write_log(STATUS_SUCCEED);
    } else {
      LCDI2C_write_String("incorrect!");
      write_log(STATUS_FAILED);
    }
    digits_entered = 0;
    state = IDLE;
    break;
  }
}

//uint8_t str[15];

int main(void) {
  LCDI2C_init(0x27, 20, 4);
  LCDI2C_backlight();
  Delay(500);
  //LCDI2C_write('N');
  init_gpio_keypad_clk();
  init_timer_keypad_clk();
  init_gpio_keypad_read();

  real_code_buf[0] = read_eeprom(0x0000);
  Delay(50);
  real_code_buf[1] = read_eeprom(0x0001);
  Delay(50);
  real_code_buf[2] = read_eeprom(0x0002);
  Delay(50);
  real_code_buf[3] = read_eeprom(0x0003);
  Delay(50);
  //uint8_t test = 255;
  //test += 10;
  //sprintf(str, "%d", test);
  //LCDI2C_write_String(str);
  //read_rtc_time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  //LCDI2C_write_String("finished");

  state = IDLE;
  digits_entered = 0x0;
  //read_log(0);
  //Delay(5000);
  //LCDI2C_setCursor(0,0);
  //read_log(1);
  //Delay(5000);
  //LCDI2C_setCursor(0,0);
  //read_log(2);
  while(1) {
    //LCDI2C_setCursor(0,0);
    //sprintf(str, "%d %d|", digits_entered, ticks_passed);
    //LCDI2C_write_String(str);
    automaton();
    Delay(200);
  }
}
