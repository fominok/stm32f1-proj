#include "logger.h"

uint8_t output_buffer[64];

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

  LCDI2C_setCursor(0, 0);
  sprintf(output_buffer, "%d: ", cursor);
  LCDI2C_write_String(output_buffer);
  if (input_status == 0) LCDI2C_write_String(     "Succeed  ");
  else if (input_status == 1) LCDI2C_write_String("Failed   ");
  else if (input_status == 2) LCDI2C_write_String("Cancelled");
  else {
    LCDI2C_write_String("No data");
    LCDI2C_setCursor(0, 1);
    LCDI2C_write_String("                   ");
    return;
  }
  LCDI2C_setCursor(0, 1);
  sprintf(output_buffer, "%02d:%02d:%02d %02d-%02d-%02d", hour, minute, second, dayOfMonth, month, year);

  LCDI2C_write_String(output_buffer);
}
