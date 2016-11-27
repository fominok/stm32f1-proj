#ifndef kbd_header
#define kbd_header

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "LiquidCrystal_I2C.h"

void init_timer_keypad_clk();
void init_gpio_keypad_clk();
void init_gpio_keypad_read();

#endif
