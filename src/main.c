#include "rtc.h"
#include "eeprom.h"
#include "kbd.h"
#include "delay.h"
#include "logger.h"
#include "LiquidCrystal_I2C.h"
#include <stdio.h>

#include "stm32f10x_gpio.h"

#define CODE_BASE_ADDR 0x0000
#define STATUS_SUCCEED 0
#define STATUS_FAILED 1
#define STATUS_CANCELLED 2

volatile uint8_t digits_entered;
volatile uint32_t ticks_passed;
volatile uint8_t input_buffer[4];
volatile prevent_kb;
volatile no_display_kb;

typedef enum {
  IDLE,
  CODE_INPUT,
  LOG_CANCELLED,
  CODE_CHECK,
  LOG_SUCCEED,
  //DOOR_OPEN,
  LOG_FAILED,
  SERVICE_MODE,
  SERVICE_CODE_INPUT,
  SERVICE_LOG_VIEW
} state_t;

state_t state = IDLE;
uint8_t real_code_buf[4];
uint8_t log_view_cursor;
uint8_t redraw_log;

void write_code() {
  for (int i = 0; i < 4; i++) {
    write_eeprom(CODE_BASE_ADDR + i, input_buffer[i]);
    real_code_buf[i] = input_buffer[i];
    Delay(50);
  }
}

void read_code() {
  for (int i = 0; i < 4; i++) {
    real_code_buf[i] = read_eeprom(CODE_BASE_ADDR + i);
    Delay(50);
  }
}

void init_gpio_service_and_led() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef gpio_init_led_out;
  gpio_init_led_out.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init_led_out.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio_init_led_out.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOB, &gpio_init_led_out);

  GPIO_InitTypeDef gpio_init_service_in;
  gpio_init_service_in.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init_service_in.GPIO_Mode = GPIO_Mode_IPU;
  gpio_init_service_in.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOB, &gpio_init_service_in);
}

void lightning() {
  for (uint8_t i = 0; i < 30; i++) {
    GPIO_SetBits(GPIOB, GPIO_Pin_1);
    Delay(50);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
    Delay(50);
  }
}

void idle() {
  LCDI2C_clear();
  LCDI2C_write_String("> ") ;
  digits_entered = 0;
  no_display_kb = 0;
  prevent_kb = 0;
  state = IDLE;
}

void automaton(void) {
  switch(state) {
  case IDLE:
    if (digits_entered > 0) {
      state = CODE_INPUT;
    } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
      LCDI2C_clear();
      LCDI2C_setCursor(4, 1);
      LCDI2C_write_String("SERVICE MODE") ;
      LCDI2C_setCursor(5, 2);
      LCDI2C_write_String("A - new code") ;
      LCDI2C_setCursor(5, 3);
      LCDI2C_write_String("B - logs") ;
      state = SERVICE_MODE;
      no_display_kb = 1;
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
    LCDI2C_clear();
    write_log(STATUS_CANCELLED);
    idle();
    break;
  case CODE_CHECK:
    prevent_kb = 1;
    if (!memcmp(real_code_buf, input_buffer, 4)) {
      state = LOG_SUCCEED;
      } else {
      state = LOG_FAILED;
    }
    break;
  case LOG_SUCCEED:
    LCDI2C_clear();
    LCDI2C_setCursor(9, 1);
    LCDI2C_write_String("OK") ;
    write_log(STATUS_SUCCEED);
    lightning();
    prevent_kb = 0;
    idle();
    break;
  case LOG_FAILED:
    LCDI2C_clear();
    LCDI2C_setCursor(4, 1);
    LCDI2C_write_String("ACCESS DENIED") ;
    write_log(STATUS_FAILED);
    Delay(3000);
    prevent_kb = 0;
    idle();
    break;
  case SERVICE_MODE:
    if (digits_entered > 0) {
      if (input_buffer[0] == '0') {
        idle();
      } else if (input_buffer[0] == 'A') {
        LCDI2C_clear();
        LCDI2C_setCursor(0, 0);
        LCDI2C_write_String("Input new code:") ;
        LCDI2C_setCursor(0, 1);
        LCDI2C_write_String("> ") ;
        LCDI2C_setCursor(2, 1);
        state = SERVICE_CODE_INPUT;
      } else if (input_buffer[0] == 'B') {
        LCDI2C_clear();
        LCDI2C_setCursor(19, 0);
        LCDI2C_write_String("^") ;
        LCDI2C_setCursor(19, 1);
        LCDI2C_write_String("A") ;
        LCDI2C_setCursor(19, 2);
        LCDI2C_write_String("B") ;
        LCDI2C_setCursor(19, 3);
        LCDI2C_write_String("V") ;
        state = SERVICE_LOG_VIEW;
        log_view_cursor = read_eeprom(LOGS_CURSOR_ADDR);
        Delay(50);
        redraw_log = 1;
      }
      digits_entered = 0;
    }
    break;
  case SERVICE_CODE_INPUT:
    no_display_kb = 0;
    if (digits_entered > 3) {
      write_code();
      idle();
    }
    break;
  case SERVICE_LOG_VIEW:
    if (redraw_log) {
      read_log(log_view_cursor);
      redraw_log = 0;
    }
    if (digits_entered > 0) {
      if (input_buffer[0] == '0') {
        idle();
      } else if (input_buffer[0] == 'A') {
        log_view_cursor++;
        redraw_log = 1;
      } else if (input_buffer[0] == 'B') {
        log_view_cursor--;
        redraw_log = 1;
      }
      digits_entered = 0;
    }
    break;
  }
}

int main(void) {
  LCDI2C_init(0x27, 20, 4);
  LCDI2C_backlight();
  Delay(500);
  init_gpio_keypad_clk();
  init_timer_keypad_clk();
  init_gpio_keypad_read();
  init_gpio_service_and_led();

  read_code();
  state = IDLE;
  digits_entered = 0x0;
  idle();
  while(1) {
    automaton();
    Delay(50);
  }
}
