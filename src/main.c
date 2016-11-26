#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "I2C.h"
#include <stdio.h>

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

uint8_t decToBcd(uint8_t val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
uint8_t bcdToDec(uint8_t val)
{
  return( (val/16*10) + (val%16) );
}

void setDS3231time(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t
                   dayOfMonth, uint8_t month, uint8_t year)
{
  // sets time and date data to DS3231
  I2C_StartTransmission(I2C1, I2C_Direction_Transmitter, 0x68);
  I2C_WriteData(I2C1, 0); // set next input to start at the seconds register
  I2C_WriteData(I2C1, decToBcd(second)); // set seconds
  I2C_WriteData(I2C1, decToBcd(minute)); // set minutes
  I2C_WriteData(I2C1, decToBcd(hour)); // set hours
  I2C_WriteData(I2C1, decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  I2C_WriteData(I2C1, decToBcd(dayOfMonth)); // set date (1 to 31)
  I2C_WriteData(I2C1, decToBcd(month)); // set month
  I2C_WriteData(I2C1, decToBcd(year)); // set year (0 to 99)
  I2C_GenerateSTOP(I2C1, ENABLE);
}

void readDS3231time(uint8_t *second, uint8_t *minute, uint8_t *hour,
                    uint8_t *dayOfWeek, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year) {
  I2C_StartTransmission(I2C1, I2C_Direction_Transmitter, 0x68);
  I2C_WriteData(I2C1, 0); // set next input to start at the seconds register
  I2C_GenerateSTOP(I2C1, ENABLE);
  I2C_StartTransmission(I2C1, I2C_Direction_Receiver, 0x68);
  // request seven uint8_ts of data from DS3231 starting from register 00h
  *second = bcdToDec(I2C_ReadData(I2C1) & 0x7f);
  *minute = bcdToDec(I2C_ReadData(I2C1));
  *hour = bcdToDec(I2C_ReadData(I2C1) & 0x3f);
  *dayOfWeek = bcdToDec(I2C_ReadData(I2C1));
  *dayOfMonth = bcdToDec(I2C_ReadData(I2C1));
  *month = bcdToDec(I2C_ReadData(I2C1));
  *year = bcdToDec(I2C_ReadData(I2C1));
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_GenerateSTOP(I2C1, ENABLE);
}

uint8_t read_eeprom(uint16_t addr) {
  uint8_t data;
  I2C_StartTransmission(I2C1, I2C_Direction_Transmitter, 0x57);
  I2C_WriteData(I2C1, (uint8_t)(addr >> 8)); // set next input to start at the seconds register
  I2C_WriteData(I2C1, (uint8_t)(addr & 0xFF)); // set next input to start at the seconds register
  I2C_GenerateSTOP(I2C1, ENABLE);
  I2C_StartTransmission(I2C1, I2C_Direction_Receiver, 0x57);
  data = I2C_ReadData(I2C1);
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_GenerateSTOP(I2C1, ENABLE);

  return data;
}

void save_eeprom(uint16_t addr, uint8_t data) {
  I2C_StartTransmission(I2C1, I2C_Direction_Transmitter, 0x57);
  I2C_WriteData(I2C1, (uint8_t)(addr >> 8)); // set next input to start at the seconds register
  I2C_WriteData(I2C1, (uint8_t)(addr & 0xFF)); // set next input to start at the seconds register
  I2C_WriteData(I2C1, data);
  I2C_GenerateSTOP(I2C1, ENABLE);
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
  Delay(2000);
  //LCDI2C_write('N');
  init_rcc();
  init_gpio_keypad_clk();
  init_timer_keypad_clk();
  init_gpio_keypad_read();

  LCDI2C_write_String("before");
  Delay(500);
  save_eeprom(0x1488, 0xEF);
  Delay(500);
  uint8_t test;
  test = read_eeprom(0x1488);
  sprintf(str, "%d", test);
  LCDI2C_write_String(str);
//setDS3231time(40, 44, 23, 7, 26, 11, 16);
  //readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  ////bitch();
  //sprintf(str, "%d", minute);
  //LCDI2C_write_String(str);
  //LCDI2C_write_String("finished");
  while(1) {
    __NOP();
  }
}
