/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Silvestre Kloss Teles
**/

#include "elevator.h"
#include "state_machine.h"

int pos = 0;

void elevatorInit(Elevator *elev, char name, osMessageQueueId_t osMessageQueue, osMessageQueueId_t osControlMsgQueueId){  
  elev->level = 'a';
  elev->nextLevel = 'a';
  elev->name = name;
  elev->state = STOPPED_OPEN_DOORS;
  elev->osMessageQueue_id = osMessageQueue;
  elev->osMsgControl_id = osControlMsgQueueId;
}

void elevatorTask(void *arg0){
  char name = ((Elevator *)arg0)->name;
  osMessageQueueId_t osMessageQueueId = ((Elevator *)arg0)->osMessageQueue_id;
  osMessageQueueId_t osControlMsgQueueId = ((Elevator *)arg0)->osMsgControl_id;
  Elevator elev;
  elevatorInit(&elev, name, osMessageQueueId, osControlMsgQueueId );            // inicializa elevador
  
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(elev.osMessageQueue_id, msg, NULL, 0U);   // wait for message
    if (status == osOK) {
      changeState(&elev, msg, str);
      osThreadYield();
    }
  }
}// leftElevatorTask

void goUp(char elev[], char * str) {
  char command[BUFFER] = "xs\r\0";
  command[0] = elev[0];  // qual elevador
  strcpy(str, command);
}

void goDown(char elev[], char * str) {
  char command[BUFFER] = "xd\r\0";
  command[0] = elev[0];  // qual elevador
  strcpy(str, command);
}

void stop(char elev[], char * str) {
  char command[BUFFER] = "xp\r\0";
  command[0] = elev[0];  // qual elevador
  strcpy(str, command);
}

void closeDoor(char elev[], char * str) {   
  char command[BUFFER] = "xf\r\0";
  command[0] = elev[0];  // qual elevador
  strcpy(str, command);
}

void openDoor(char elev[], char * str) {  
  char command[BUFFER] = "xa\r\0";
  command[0] = elev[0];  // qual elevador
  strcpy(str, command);
}

void turnLightOn(char param[], char * str) {
  char command[BUFFER] = "xLx\r\0";
  command[0] = param[0];  // qual elevador
  command[2] = param[2];  // qual andar
  strcpy(str, command);
}

void turnLightOff(char param[], char * str) {
  char command[BUFFER] = "xDx\r\0";
  command[0] = param[0];  // qual elevador
  command[2] = param[2];  // qual andar
  strcpy(str, command);
}

char levelMap(int level){
  switch(level){
    case 0: return 'a';
    case 1: return 'b';
    case 2: return 'c';
    case 3: return 'd';
    case 4: return 'e';
    case 5: return 'f';
    case 6: return 'g';
    case 7: return 'h';
    case 8: return 'i';
    case 9: return 'j';
    case 10: return 'k';
    case 11: return 'l';
    case 12: return 'm';
    case 13: return 'n';
    case 14: return 'o';
    default: return 'p';
  }
}
