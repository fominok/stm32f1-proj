#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"


GPIO_InitTypeDef gpio_init_kp_out;
TIM_TimeBaseInitTypeDef tim_init;

void init_rcc(void) {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void init_timer_keypad_clk(void) {
  tim_init.TIM_Prescaler = 48000 - 1;
  tim_init.TIM_Period = 10;
  TIM_TimeBaseInit(TIM2, &tim_init);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  NVIC_EnableIRQ(TIM2_IRQn);
}

void init_gpio_keypad_clk(void) {
  gpio_init_kp_out.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init_kp_out.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio_init_kp_out.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_Init(GPIOA, &gpio_init_kp_out);
}

void init_gpio_keypad_read() {
  GPIO_InitTypeDef gpio_init_kp_in;

  EXTI_InitTypeDef exti_init_kp_in_5;
  EXTI_InitTypeDef exti_init_kp_in_6;
  EXTI_InitTypeDef exti_init_kp_in_7;
  EXTI_InitTypeDef exti_init_kp_in_8;

  NVIC_InitTypeDef nvic_init_kp;

  gpio_init_kp_in.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init_kp_in.GPIO_Mode = GPIO_Mode_IPD;
  gpio_init_kp_in.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
  GPIO_Init(GPIOA, &gpio_init_kp_in);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);

  exti_init_kp_in_5.EXTI_LineCmd = ENABLE;
  exti_init_kp_in_5.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init_kp_in_5.EXTI_Trigger = EXTI_Trigger_Rising;
  exti_init_kp_in_5.EXTI_Line = EXTI_Line5;
  EXTI_Init(&exti_init_kp_in_5);

  exti_init_kp_in_6.EXTI_LineCmd = ENABLE;
  exti_init_kp_in_6.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init_kp_in_6.EXTI_Trigger = EXTI_Trigger_Rising;
  exti_init_kp_in_6.EXTI_Line = EXTI_Line6;
  EXTI_Init(&exti_init_kp_in_6);

  exti_init_kp_in_7.EXTI_LineCmd = ENABLE;
  exti_init_kp_in_7.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init_kp_in_7.EXTI_Trigger = EXTI_Trigger_Rising;
  exti_init_kp_in_7.EXTI_Line = EXTI_Line7;
  EXTI_Init(&exti_init_kp_in_7);

  exti_init_kp_in_8.EXTI_LineCmd = ENABLE;
  exti_init_kp_in_8.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init_kp_in_8.EXTI_Trigger = EXTI_Trigger_Rising;
  exti_init_kp_in_8.EXTI_Line = EXTI_Line8;
  EXTI_Init(&exti_init_kp_in_8);

  nvic_init_kp.NVIC_IRQChannelCmd = ENABLE;
  nvic_init_kp.NVIC_IRQChannel = EXTI9_5_IRQn;
  nvic_init_kp.NVIC_IRQChannelPreemptionPriority = 0x03;
  NVIC_Init(&nvic_init_kp);
}

uint16_t state = 0;
uint16_t row = 0;

void TIM2_IRQHandler() {
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) ||
       GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) ||
       GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) ||
       GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)
       ) return;

    row = state;
    switch (state) {
    case 0:
      GPIO_SetBits(GPIOA, GPIO_Pin_0);
      GPIO_ResetBits(GPIOA, GPIO_Pin_3);
      state = 1;
      break;
    case 1:
      GPIO_SetBits(GPIOA, GPIO_Pin_1);
      GPIO_ResetBits(GPIOA, GPIO_Pin_0);
      state = 2;
      break;
    case 2:
      GPIO_SetBits(GPIOA, GPIO_Pin_2);
      GPIO_ResetBits(GPIOA, GPIO_Pin_1);
      state = 3;
      break;
    case 3:
      GPIO_SetBits(GPIOA, GPIO_Pin_3);
      GPIO_ResetBits(GPIOA, GPIO_Pin_2);
      state = 0;
      break;

    }
  }
}

char keypad[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

void EXTI9_5_IRQHandler() {
  if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
    EXTI_ClearITPendingBit(EXTI_Line5);
    if (row == 3) return; // broken asterisk on my keypad
    if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)) return;
    LCDI2C_write(keypad[row][0]);
  } else if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
    EXTI_ClearITPendingBit(EXTI_Line6);
    if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)) return;
    LCDI2C_write(keypad[row][1]);
  } else if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
    EXTI_ClearITPendingBit(EXTI_Line7);
    if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) return;
    if (row == 3) return; // broken hash on my keypad
    LCDI2C_write(keypad[row][2]);
  } else if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
    EXTI_ClearITPendingBit(EXTI_Line8);
    if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)) return;
    LCDI2C_write(keypad[row][3]);
  }
}

int main(void) {
  LCDI2C_init(0x27, 20, 4);
  LCDI2C_backlight();
  //LCDI2C_write('N');
  init_rcc();
  init_gpio_keypad_clk();
  init_timer_keypad_clk();
  init_gpio_keypad_read();

  while(1) {
    __NOP();
  }
}
