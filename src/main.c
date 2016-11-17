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
  gpio_init_kp_out.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &gpio_init_kp_out);
}

void init_gpio_keypad_read() {
  GPIO_InitTypeDef gpio_init_kp_in;

  EXTI_InitTypeDef exti_init_kp_in_0;
  EXTI_InitTypeDef exti_init_kp_in_1;
  EXTI_InitTypeDef exti_init_kp_in_2;
  EXTI_InitTypeDef exti_init_kp_in_3;

  NVIC_InitTypeDef nvic_init_kp_in_0;
  NVIC_InitTypeDef nvic_init_kp_in_1;
  NVIC_InitTypeDef nvic_init_kp_in_2;
  NVIC_InitTypeDef nvic_init_kp_in_3;

  gpio_init_kp_in.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init_kp_in.GPIO_Mode = GPIO_Mode_IPD;
  gpio_init_kp_in.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_Init(GPIOA, &gpio_init_kp_in);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
  //GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
  //GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
  //GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);

  exti_init_kp_in_0.EXTI_LineCmd = ENABLE;
  exti_init_kp_in_0.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init_kp_in_0.EXTI_Trigger = EXTI_Trigger_Rising;
  exti_init_kp_in_0.EXTI_Line = EXTI_Line0;
  EXTI_Init(&exti_init_kp_in_0);

  //exti_init_kp_in_1.EXTI_LineCmd = ENABLE;
  //exti_init_kp_in_1.EXTI_Mode = EXTI_Mode_Interrupt;
  //exti_init_kp_in_1.EXTI_Trigger = EXTI_Trigger_Rising;
  //exti_init_kp_in_1.EXTI_Line = EXTI_Line1;
  //EXTI_Init(&exti_init_kp_in_1);

  //exti_init_kp_in_2.EXTI_LineCmd = ENABLE;
  //exti_init_kp_in_2.EXTI_Mode = EXTI_Mode_Interrupt;
  //exti_init_kp_in_2.EXTI_Trigger = EXTI_Trigger_Rising;
  //exti_init_kp_in_2.EXTI_Line = EXTI_Line2;
  //EXTI_Init(&exti_init_kp_in_2);

  //exti_init_kp_in_3.EXTI_LineCmd = ENABLE;
  //exti_init_kp_in_3.EXTI_Mode = EXTI_Mode_Interrupt;
  //exti_init_kp_in_3.EXTI_Trigger = EXTI_Trigger_Rising;
  //exti_init_kp_in_3.EXTI_Line = EXTI_Line3;
  //EXTI_Init(&exti_init_kp_in_3);

  nvic_init_kp_in_0.NVIC_IRQChannelCmd = ENABLE;
  nvic_init_kp_in_0.NVIC_IRQChannel = EXTI0_IRQn;
  nvic_init_kp_in_0.NVIC_IRQChannelPreemptionPriority = 0x03;
  NVIC_Init(&nvic_init_kp_in_0);

  //nvic_init_kp_in_1.NVIC_IRQChannelCmd = ENABLE;
  //nvic_init_kp_in_1.NVIC_IRQChannel = EXTI1_IRQn;
  //nvic_init_kp_in_1.NVIC_IRQChannelPreemptionPriority = 0x04;
  //NVIC_Init(&nvic_init_kp_in_1);

  //nvic_init_kp_in_2.NVIC_IRQChannelCmd = ENABLE;
  //nvic_init_kp_in_2.NVIC_IRQChannel = EXTI2_IRQn;
  //nvic_init_kp_in_2.NVIC_IRQChannelPreemptionPriority = 0x05;
  //NVIC_Init(&nvic_init_kp_in_2);

  //nvic_init_kp_in_3.NVIC_IRQChannelCmd = ENABLE;
  //nvic_init_kp_in_3.NVIC_IRQChannel = EXTI3_IRQn;
  //nvic_init_kp_in_3.NVIC_IRQChannelPreemptionPriority = 0x06;
  //NVIC_Init(&nvic_init_kp_in_3);
}

uint16_t state = 0;
uint16_t row = 0;

void TIM2_IRQHandler() {
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    row = state;
    switch (state) {
    case 0:
      GPIO_SetBits(GPIOA, GPIO_Pin_4);
      GPIO_ResetBits(GPIOA, GPIO_Pin_7);
      state = 1;
      break;
    case 1:
      GPIO_SetBits(GPIOA, GPIO_Pin_5);
      GPIO_ResetBits(GPIOA, GPIO_Pin_4);
      state = 2;
      break;
    case 2:
      GPIO_SetBits(GPIOA, GPIO_Pin_6);
      GPIO_ResetBits(GPIOA, GPIO_Pin_5);
      state = 3;
      break;
    case 3:
      GPIO_SetBits(GPIOA, GPIO_Pin_7);
      GPIO_ResetBits(GPIOA, GPIO_Pin_6);
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

void EXTI0_IRQHandler() {
  if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
    EXTI_ClearITPendingBit(EXTI_Line0);
    LCDI2C_write(keypad[row][0]);
  }
}

//void EXTI1_IRQHandler() {
//  if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
//    EXTI_ClearITPendingBit(EXTI_Line1);
//    LCDI2C_write(keypad[state][1]);
//  }
//}
//
//void EXTI2_IRQHandler() {
//  if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
//    EXTI_ClearITPendingBit(EXTI_Line2);
//    LCDI2C_write(keypad[state][2]);
//  }
//}
//
//void EXTI3_IRQHandler() {
//  if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
//    EXTI_ClearITPendingBit(EXTI_Line3);
//    LCDI2C_write(keypad[state][3]);
//  }
//}

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
