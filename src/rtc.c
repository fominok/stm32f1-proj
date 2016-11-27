#include "rtc.h"
#include "I2C.h"

uint8_t dec_to_bcd(uint8_t val) {
  return( (val/10*16) + (val%10) );
}

uint8_t bcd_to_dec(uint8_t val) {
  return( (val/16*10) + (val%16) );
}

void set_rtc_time(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t
                   dayOfMonth, uint8_t month, uint8_t year) {
  I2C_StartTransmission(I2C1, I2C_Direction_Transmitter, 0x68);
  I2C_WriteData(I2C1, 0);
  I2C_WriteData(I2C1, dec_to_bcd(second));
  I2C_WriteData(I2C1, dec_to_bcd(minute));
  I2C_WriteData(I2C1, dec_to_bcd(hour));
  I2C_WriteData(I2C1, dec_to_bcd(dayOfWeek));
  I2C_WriteData(I2C1, dec_to_bcd(dayOfMonth));
  I2C_WriteData(I2C1, dec_to_bcd(month));
  I2C_WriteData(I2C1, dec_to_bcd(year));
  I2C_GenerateSTOP(I2C1, ENABLE);
}

void read_rtc_time(uint8_t *second, uint8_t *minute, uint8_t *hour,
                    uint8_t *dayOfWeek, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year) {
  I2C_StartTransmission(I2C1, I2C_Direction_Transmitter, 0x68);
  I2C_WriteData(I2C1, 0);
  I2C_GenerateSTOP(I2C1, ENABLE);
  I2C_StartTransmission(I2C1, I2C_Direction_Receiver, 0x68);
  *second = bcd_to_dec(I2C_ReadData(I2C1) & 0x7f);
  *minute = bcd_to_dec(I2C_ReadData(I2C1));
  *hour = bcd_to_dec(I2C_ReadData(I2C1) & 0x3f);
  *dayOfWeek = bcd_to_dec(I2C_ReadData(I2C1));
  *dayOfMonth = bcd_to_dec(I2C_ReadData(I2C1));
  *month = bcd_to_dec(I2C_ReadData(I2C1));
  *year = bcd_to_dec(I2C_ReadData(I2C1));
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_GenerateSTOP(I2C1, ENABLE);
}

