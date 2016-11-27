#include "eeprom.h"
#include "I2C.h"

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

void write_eeprom(uint16_t addr, uint8_t data) {
  I2C_StartTransmission(I2C1, I2C_Direction_Transmitter, 0x57);
  I2C_WriteData(I2C1, (uint8_t)(addr >> 8)); // set next input to start at the seconds register
  I2C_WriteData(I2C1, (uint8_t)(addr & 0xFF)); // set next input to start at the seconds register
  I2C_WriteData(I2C1, data);
  I2C_GenerateSTOP(I2C1, ENABLE);
}
