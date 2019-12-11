#include "string_utils.h"

void sendString(char string[]) {
  for(uint8_t i = 0; i < strlen(string); i++) {
    UARTCharPut(UART0_BASE, string[i]);
  }
  UARTCharPut(UART0_BASE, '\r');
}
