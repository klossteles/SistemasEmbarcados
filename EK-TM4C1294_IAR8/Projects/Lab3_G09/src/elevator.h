
#ifndef __ELEVATOR__
#define __ELEVATOR__
#endif

#include <string.h>

#define BUFFER 10

char** controlList; //todo: trocar por queue

typedef struct elevator{
    char level;
    // s: subindo
    // p: parado
    // d: descendo
    char state;
    char** commands; //todo: trocar por queue
} Elevator;

void rule(char* param, Elevator elev);
char* goUp(char elev);
char* goDown(char elev);
char* stop(char elev);
char* closeDoor(char elev);
char* openDoor(char elev);
char* turnOnLight(char* param);
