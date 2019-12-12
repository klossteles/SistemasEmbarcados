/***
* S12_G09_Lab3
* Laborat�rio 3 - Sistemas Embarcados
* Andr� Luiz Rodrigues dos Santos
* Lu�s Henrique Beltr�o Santana
* Lucas Kloss Teles
**/

#include <string.h>
#include "queue.h"

#ifndef __ELEVATOR__
#define __ELEVATOR__

#define BUFFER 10

//queue_t controlList; //todo: trocar por queue

typedef struct elevator{
    char level;
    char nextLevel;
    char name;
    // ver state_machine.h
    int state;
    queue_t commands; //todo: trocar por queue
} Elevator;

void elevatorInit();
void rule(char* param, Elevator elev);
char* goUp(char elev[]);
char* goDown(char elev[]);
char* stop(char elev[]);
char* closeDoor(char elev[]);
char* openDoor(char elev[]);
char* turnLightOn(char param[]);
char levelMap(int level);
#endif
