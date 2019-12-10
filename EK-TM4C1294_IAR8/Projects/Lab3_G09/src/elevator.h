/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Kloss Teles
**/

#ifndef __ELEVATOR__
#define __ELEVATOR__
#endif

#include <string.h>
#include "queue.h"
//#include "state_machine.h"

#define BUFFER 10

//queue_t controlList; //todo: trocar por queue

typedef struct elevator{
    char level;
    // ver state_machine.h
    int state;
    queue_t commands; //todo: trocar por queue
} Elevator;

void rule(char* param, Elevator elev);
char* goUp(char elev[]);
char* goDown(char elev[]);
char* stop(char elev[]);
char* closeDoor(char elev[]);
char* openDoor(char elev[]);
char* turnLightOn(char param[]);
char levelMap(int level);