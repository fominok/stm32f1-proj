#include "rtc.h"
#include "eeprom.h"
#include "kbd.h"
#include "delay.h"
#include "logger.h"
#include "LiquidCrystal_I2C.h"
#include <stdio.h>

// For __NOP()
#include "stm32f10x_gpio.h"

#define CODE_BASE_ADDR 0x0000
#define STATUS_SUCCEED 0
#define STATUS_FAILED 1
#define STATUS_CANCELLED 2

volatile uint8_t digits_entered;
volatile uint32_t ticks_passed;
volatile uint8_t input_buffer[4];
volatile prevent_kb;

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

void automaton(void) {
  switch(state) {
  case IDLE:
    if (digits_entered > 0) {
      state = CODE_INPUT;
    } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
      state = SERVICE_MODE;
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
    LCDI2C_write_String("> ") ;
    write_log(STATUS_CANCELLED);
    digits_entered = 0;
    state = IDLE;
    break;
  case CODE_CHECK:
    if (!strncmp(real_code_buf, input_buffer, 4)) {
      LCDI2C_clear();
      LCDI2C_setCursor(9, 1);
      LCDI2C_write_String("OK") ;
      write_log(STATUS_SUCCEED);
      prevent_kb = 1;
      lightning();
      prevent_kb = 0;
    } else {
      LCDI2C_clear();
      LCDI2C_setCursor(4, 1);
      LCDI2C_write_String("ACCESS DENIED") ;
      write_log(STATUS_FAILED);
      prevent_kb = 1;
      Delay(3000);
      prevent_kb = 0;
    }
    LCDI2C_clear();
    LCDI2C_write_String("> ") ;
    digits_entered = 0;
    state = IDLE;
    break;
  case SERVICE_MODE:
    LCDI2C_clear();
    LCDI2C_setCursor(4, 1);
    LCDI2C_write_String("SERVICE MODE") ;
    LCDI2C_setCursor(5, 2);
    LCDI2C_write_String("A - new code") ;
    LCDI2C_setCursor(5, 3);
    LCDI2C_write_String("B - logs") ;
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

  real_code_buf[0] = read_eeprom(CODE_BASE_ADDR);
  Delay(50);
  real_code_buf[1] = read_eeprom(CODE_BASE_ADDR + 1);
  Delay(50);
  real_code_buf[2] = read_eeprom(CODE_BASE_ADDR + 2);
  Delay(50);
  real_code_buf[3] = read_eeprom(CODE_BASE_ADDR + 3);
  Delay(50);
  state = IDLE;
  digits_entered = 0x0;
  //for (int i=0; i<20; i++) {
  //  LCDI2C_setCursor(0,0);
  //  read_log(i);
  //  Delay(2000);
  //}
  prevent_kb = 0;
  LCDI2C_write_String("> ") ;
  while(1) {
    automaton();
    Delay(200);
  }
}
