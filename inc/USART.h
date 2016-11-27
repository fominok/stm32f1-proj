#include <stdint.h>

void USART1_Init(void); //Объявление функции инициализации периферии
void Usart1_Send_symbol(uint8_t); //Объявление функции передачи символа
void Usart1_Send_String(char* str); //Объявление функции передачи строки
