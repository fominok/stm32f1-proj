#ifndef logger_header
#define logger_header

#include <stdint.h>

#define LOGS_CURSOR_ADDR 0x0004
#define LOGS_BASE_ADDR 0x0005

void write_log(uint8_t input_status);
void read_log(uint8_t cursor);

#endif
