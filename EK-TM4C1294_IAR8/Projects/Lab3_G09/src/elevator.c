/***
* S12_G09_Lab3
* Laborat�rio 3 - Sistemas Embarcados
* Andre Luiz Rodrigues dos Santos
* Luis Henrique Beltrao Santana
* Lucas Kloss Teles
**/

#include "elevator.h"
#include "state_machine.h"

int pos = 0;

void elevatorInit(){
  elev_e.level = 'a';
  elev_e.nextLevel = 'a';
  elev_e.name = 'e';
  elev_e.state = STOPPED_OPEN_DOORS;
  
  elev_c.level = 'a';
  elev_c.nextLevel = 'a';
  elev_c.name = 'c';
  elev_c.state = STOPPED_OPEN_DOORS;
  
  
  elev_d.level = 'a';
  elev_d.nextLevel = 'a';
  elev_d.name = 'd';
  elev_d.state = STOPPED_OPEN_DOORS;
}

void rule(char param[], Elevator elev){
  char command[BUFFER];
  switch(param[1]){
     case 'I':
      if(elev.level != param[2]){
        //todo: proteger com mutex
        char* dest = turnLightOn(param);
        queue_append((queue_t **)&elev.commands, (queue_t *)dest);
        dest = closeDoor(param);
        queue_append((queue_t **)&elev.commands, (queue_t *)dest);
        //            controlList[pos++] = turnLightOn(param); // acende a luz do painel interno
        //            controlList[pos++] = closeDoor(param);   // fecha a porta
        if(elev.level != 'p' && (int) elev.level < (int) param[2]){
          dest = goUp(param);
          queue_append((queue_t **)&elev.commands, (queue_t *)dest);
          //              controlList[pos++] = goUp(param);    // subir
        } else if(elev.level != 'a' && (int) elev.level > (int) param[2]){
          dest = goDown(param);
          queue_append((queue_t **)&elev.commands, (queue_t *)dest);
          //              controlList[pos++] = goDown(param);    // descer
        }
      }
    break;
     case 'E':
    break;
     default: 
    break;
  }
}

char* goUp(char elev[]) {
  char command[BUFFER];
  command[0] = elev[0];  // qual elevador
  command[1] = 's';
  command[2] = '\r';
  command[3] = '\0';
  return command;
}

char* goDown(char elev[]) {
  char command[BUFFER];
  command[0] = elev[0];  // qual elevador
  command[1] = 'd';
  command[2] = '\r';
  command[3] = '\0';
  return command;
}

char* stop(char elev[]) {
  char command[BUFFER];
  command[0] = elev[0];  // qual elevador
  command[1] = 'p';
  command[2] = '\r';
  command[3] = '\0';
  return command;
}

char* closeDoor(char elev[]) {   
  char command[BUFFER];
  command[0] = elev[0];  // qual elevador
  command[1] = 'f';
  command[2] = '\r';
  command[3] = '\0';
  return command;
}

char* openDoor(char elev[]) {  
  char command[BUFFER];
  command[0] = elev[0];  // qual elevador
  command[1] = 'a';
  command[2] = '\r';
  command[3] = '\0';
  return command;
}

char* turnLightOn(char param[]) {
  char command[BUFFER];
  command[0] = param[0];  // qual elevador
  command[1] = 'L';       // acende a luz no simulador
  command[2] = param[2];  // qual andar
  command[3] = '\r';
  command[4] = '\0';
  return command;
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
