#ifndef eeprom_header
#define eeprom_header

#include <stdint.h>

uint8_t read_eeprom(uint16_t addr);
void write_eeprom(uint16_t addr, uint8_t data);

#endif
