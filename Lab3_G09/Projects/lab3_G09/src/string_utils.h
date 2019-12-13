/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Silvestre Kloss Teles
**/

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"

void sendString(char string[]);
#endif
