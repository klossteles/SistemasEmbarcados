/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Silvestre Kloss Teles
**/

#include <string.h>
#include "cmsis_os2.h" // CMSIS-RTOS

#ifndef __ELEVATOR__
#define __ELEVATOR__

#define BUFFER 10

typedef struct elevator{
  char level;
  uint8_t nextLevel[15];
  char name;
  // ver state_machine.h
  int state;
  int prevMovState;
  osMessageQueueId_t osMessageQueue_id;
  osMessageQueueId_t osMsgControl_id;
} Elevator;

void rule(char* param, Elevator elev);
void goUp(char elev[], char * str);
void goDown(char elev[], char * str);
void stop(char elev[], char * str);
void closeDoor(char elev[], char * str);
void openDoor(char elev[], char * str);
void turnLightOn(char param[], char * str);
void turnLightOff(char param[], char * str);
char strMap(char *str);
void addElementToQueue(Elevator *elev, int level);
void removeElementFromQueue(Elevator *elev, int level);
int levelCharToInt(char level);
char levelIntToChar(int level);
void getElevatorNextMovement(Elevator *elev, char * str);
#endif
