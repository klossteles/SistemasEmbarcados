/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Kloss Teles
**/

#include "state_machine.h"

char* changeState(Elevator *elev, char command[]){
  switch(elev->state){
    case STOPPED_OPEN_DOORS:
      // sinal de porta fechada recebido recebido
      if(strlem(command) == 3 && command[0] == elev->name && command[1] == 'F'){
        char tmp[4] = "xx\r";
        tmp[0] = elev->name;
        if((int)elev->level > (int)elev->nextLevel){
          tmp[1] = 'd'; // descer
          elev->state = GOING_DOWN;
        }
        else{
          tmp[1] = 's'; // subir
          elev->state = GOING_UP;
        }
        return tmp;
      }
      break;
    case STOPPED_CLOSE_DOORS:
      break;
    case GOING_UP:
    case GOING_DOWN:
      if(strlem(command) == 3 && command[0] == elev->name && command[1] == elev->nextLevel){
        char tmp[4] = "xp\r";
        tmp[0] = elev->name;
        return tmp;
      }
      break;
    default: break;
  }
  char tmp[5] = "erro";
  return tmp;
}