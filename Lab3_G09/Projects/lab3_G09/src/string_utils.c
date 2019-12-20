/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Silvestre Kloss Teles
**/

#include "string_utils.h"

void sendString(char string[]) {
  for(uint8_t i = 0; i < strlen(string); i++) {
    UARTCharPut(UART0_BASE, string[i]);
  }
}// sendString