/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Silvestre Kloss Teles
**/

#include <string.h>
#include "queue.h"
#include "cmsis_os2.h" // CMSIS-RTOS

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
    osMessageQueueId_t osMessageQueue_id;
    osMessageQueueId_t osMsgControl_id;
} Elevator;

void elevatorTask(void *arg0);
void elevatorInit(Elevator *elev, char name, osMessageQueueId_t osMessageQueue, osMessageQueueId_t osControlMsgQueueId);
void rule(char* param, Elevator elev);
void goUp(char elev[], char * str);
void goDown(char elev[], char * str);
void stop(char elev[], char * str);
void closeDoor(char elev[], char * str);
void openDoor(char elev[], char * str);
void turnLightOn(char param[], char * str);
void turnLightOff(char param[], char * str);
char levelMap(int level);
#endif
