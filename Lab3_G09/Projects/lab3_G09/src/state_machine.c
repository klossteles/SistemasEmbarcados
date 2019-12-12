/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Silvestre Kloss Teles
**/

#include "state_machine.h"

// essa função reage aos comandos enviados pelo simulador
void changeState(Elevator *elev, char command[], char * str){
  printf("Command: %s\n", command);
  switch(command[1]){
     case 'I':
        char tmp[4] = "xx\r";
        tmp[0] = elev->name;
        if(elev->state == STOPPED_OPEN_DOORS && elev->level != command[2] && (int)command[2] >= (int)'a' && 
           (int)command[2] <= (int)'p' ){
         elev->nextLevel = command[2];
         tmp[1] = 'f';
         strcpy(str, tmp);
         // precisa acender a luz
       }
      break;
     case 'E':
        char tmpi[3] = "xx";
        tmpi[1] = command[2];
        tmpi[2] = command[3];
        char requestLevel= levelMap(atoi(&tmpi));
        if((command[4] == 's' || command[4] == 'd') && elev->state == STOPPED_OPEN_DOORS){
          elev->nextLevel = levelMap(atoi(&tmp));
          char tmp[4] = "xx\r";
          tmp[0] = elev->name;
          tmp[1] = 'f';
          strcpy(str, tmp);
        }
    break;
    default: break;
  }
  switch(elev->state){
    case STOPPED_OPEN_DOORS:
        // sinal de porta fechada recebido recebido
        if(strlen(command) == 3 && command[0] == elev->name && command[1] == 'F'){
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
          strcpy(str, tmp);
        }
    break;
    case STOPPED_CLOSE_DOORS:
        if(command[0] == elev->name && command[1] == 'A'){ // aguarda a porta abrir
          elev->state = STOPPED_OPEN_DOORS;
        }
        break;
         case GOING_UP:
         case GOING_DOWN:
        if(strlen(command) == 3 && command[0] == elev->name && command[1] == elev->nextLevel){
          char tmp[4] = "xp\r";
          tmp[0] = elev->name;
          elev->state = STOPPED_CLOSE_DOORS;
          strcpy(str, tmp);
          // logo em seguida precisa enviar um open door e um apagar a luz se aplicável
        }
    break;
    default: break;
  }
  char tmp[5] = "erro";
  strcpy(str, tmp);
}