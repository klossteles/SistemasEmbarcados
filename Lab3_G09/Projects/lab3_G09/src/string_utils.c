/***
* S12_G09_Lab3
* Laborat�rio 3 - Sistemas Embarcados
* Andr� Luiz Rodrigues dos Santos
* Lu�s Henrique Beltr�o Santana
* Lucas Silvestre Kloss Teles
**/

#include "string_utils.h"

void sendString(char string[]) {
  for(uint8_t i = 0; i < strlen(string); i++) {
    UARTCharPut(UART0_BASE, string[i]);
  }
}// sendString