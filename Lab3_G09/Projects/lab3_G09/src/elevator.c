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

char strMap(char *str){
  if(str[2] == '\0'){
    switch(str[1]){
       case '0': return 'a';
       case '1': return 'b';
       case '2': return 'c';
       case '3': return 'd';
       case '4': return 'e';
       case '5': return 'f';
       case '6': return 'g';
       case '7': return 'h';
       case '8': return 'i';
       case '9': return 'j';
       default: return '\0';
    }
  } else {
    switch(str[2]){
       case '0': return 'k';
       case '1': return 'l';
       case '2': return 'm';
       case '3': return 'n';
       case '4': return 'o';
       case '5': return 'p';
       default: return '\0';
    }
  }
}

void addElementToQueue(Elevator *elev, int level) {
  elev->nextLevel[level] = 1;
}

void removeElementFromQueue(Elevator *elev, int level) {
  elev->nextLevel[level] = 0;
}

int levelCharToInt(char level){
  switch(level) {
     case 'a': return 0;
     case 'b': return 1;
     case 'c': return 2;
     case 'd': return 3;
     case 'e': return 4;
     case 'f': return 5;
     case 'g': return 6;
     case 'h': return 7;
     case 'i': return 8;
     case 'j': return 9;
     case 'k': return 10;
     case 'l': return 11;
     case 'm': return 12;
     case 'n': return 13;
     case 'o': return 14;
     case 'p': return 15;
     default: return -1;     
  }
}

char levelIntToChar(int level) {
  switch(level) {
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
     case 15: return 'p';
     default: return '\0';     
  }
}

void getElevatorNextMovement(Elevator *elev, char * str){
  int currentLevel = levelCharToInt(elev->level);
  //mantém orientação do elevador
  if (elev->prevMovState == GOING_UP) {
    for (uint8_t i = currentLevel ; i < 15 ; i++ ) {
      if (elev->nextLevel[i] == 1) {
        str[1] = 's';
        elev->state = GOING_UP;
        return;
      }
    }
  } else if (elev->prevMovState == GOING_DOWN) {
    for (uint8_t i = 0 ; i < currentLevel  ; i++ ) {
      if (elev->nextLevel[i] == 1) {
        str[1] = 'd';
        elev->state = GOING_DOWN;
        return;
      }
    }
  }
  for (uint8_t i = 0 ; i < 15 ; i++ ) {
    if (elev->nextLevel[i] == 1) {
      if (i < currentLevel) {
        str[1] = 'd';
        elev->state = GOING_DOWN;  
      } else {
        str[1] = 's';
        elev->state = GOING_UP;  
      }
      return;
    }
  }  
}