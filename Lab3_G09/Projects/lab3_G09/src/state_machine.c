/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Silvestre Kloss Teles
**/

#include "state_machine.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os2.h" // CMSIS-RTOS
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "elevator.h"
#include "string_utils.h"

osThreadId_t elevE_id, elevC_id, elevD_id;
osMessageQueueId_t osRightElevatorMessageQueue_id, osLeftElevatorMessageQueue_id, osCentralElevatorMessageQueue_id;
osMutexId_t osMutexId;
const osMutexAttr_t Thread_Mutex_attr = {
  "messageControlMutex",                    // human readable mutex name
  osMutexRecursive | osMutexPrioInherit,    // attr_bits
  NULL,                                     // memory for control block   
  0U                                        // size for control block
};

void controlTask(void *arg0){
  char msg[BUFFER];
  osMessageQueueId_t controlMessageQueue = ((osMessageQueueId_t *)arg0);
  osMutexId = osMutexNew(&Thread_Mutex_attr);                                   // cria mutex para controle de inserção na fila
  
  osLeftElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
//  osRightElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
//  osCentralElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  
  Elevator elev;
  elev.osMsgControl_id = controlMessageQueue;
  elev.name = 'e';
  elev.osMessageQueue_id = osLeftElevatorMessageQueue_id;
  elevE_id = osThreadNew(elevatorTask, (void *)(&elev), NULL);                  //thread elevador esquerdo
  
//  elev.name = 'c';
//  elev.osMessageQueue_id = osCentralElevatorMessageQueue_id;
//  elevC_id = osThreadNew(elevatorTask, (void *)(&elev), NULL);                  //thread elevador central
//  
//  elev.name = 'd';
//  elev.osMessageQueue_id = osRightElevatorMessageQueue_id;
//  elevD_id = osThreadNew(elevatorTask, (void *)(&elev), NULL);                  //thread elevador direito
  
  char uartEntry[5];
  char firstStr[] = "er\rcr\rdr\r";
  sendString(firstStr);
    
  while(1){
    UARTgets(uartEntry, 5);
    switch(uartEntry[0]){
      case 'e': 
        osMessageQueuePut(osLeftElevatorMessageQueue_id, uartEntry, 1, 0);
      break;
     case 'c': 
        osMessageQueuePut(osCentralElevatorMessageQueue_id, uartEntry, 1, 0);
      break;
     case 'd': 
        osMessageQueuePut(osRightElevatorMessageQueue_id, uartEntry, 1, 0);
      break;
     default:
        printf("ERROR %s\n", uartEntry);
        break;
    }
    osThreadYield();
  };  
}// controlTask

// essa função reage aos comandos enviados pelo simulador
void changeState(Elevator *elev, char command[], char * str){
  switch(command[1]){
     case 'I':
        if(elev->state == STOPPED_OPEN_DOORS && elev->level != command[2] && (int)command[2] >= (int)'a' && 
           (int)command[2] <= (int)'p' ){
         elev->nextLevel = command[2];
         closeDoor(command, str);
         osMutexAcquire(osMutexId, osWaitForever);
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         turnLightOn(command, str);
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         osMutexRelease(osMutexId);
         // precisa acender a luz
       }
       return;
      break;
     case 'E':{
      char tmpi[3] = "xx";
        tmpi[0] = command[2];
        tmpi[1] = command[3];
        char requestLevel = levelMap(atoi(tmpi));
        if(elev->state == STOPPED_OPEN_DOORS){
          elev->nextLevel = levelMap(atoi(tmpi));
          closeDoor(command, str);
          osMutexAcquire(osMutexId, osWaitForever);
          osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
          osMutexRelease(osMutexId);
        }
        return;
        break; 
     }
    case 'A':
    break;
    case 'F':
    break;
     default:{
     char tmp[2] = "xx";
      if (strlen(command) == 2) {
        tmp[0] = '0';
        tmp[1] = command[1];
      } else if(strlen(command) == 3) {
        tmp[0] = command[1];
        tmp[1] = command[2]; 
      }
      char currentLevel = levelMap(atoi(tmp));
      if (currentLevel == elev->nextLevel) {
          stop(command, str);
          osMutexAcquire(osMutexId, osWaitForever);
          osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
          openDoor(command, str);
          osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
          command[2] = currentLevel;
          turnLightOff(command, str);
          osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
          osMutexRelease(osMutexId);
      }  
      break;
     }
  }
  switch(elev->state){
    case STOPPED_OPEN_DOORS:
        // sinal de porta fechada recebido recebido
        if(strlen(command) == 2 && command[0] == elev->name && command[1] == 'F'){
          char tmp[4] = "xx\r";
          tmp[0] = elev->name;
          if((int)elev->level > (int)elev->nextLevel){
            tmp[1] = 'd'; // descer
            elev->state = GOING_DOWN;
          } else {
            tmp[1] = 's'; // subir
            elev->state = GOING_UP;
          }
          osMutexAcquire(osMutexId, osWaitForever);
          osMessageQueuePut(elev->osMsgControl_id, tmp, 1, 0);
          osMutexRelease(osMutexId);
        }
        return;
    break;
    case STOPPED_CLOSE_DOORS:
        if(command[0] == elev->name && command[1] == 'A'){ // aguarda a porta abrir
          elev->state = STOPPED_OPEN_DOORS;
        }
        break;
         case GOING_UP:
         case GOING_DOWN:
        if(strlen(command) == 2 && command[0] == elev->name && command[1] == elev->nextLevel){
          char tmp[4] = "xp\r";
          tmp[0] = elev->name;
          elev->state = STOPPED_CLOSE_DOORS;
          osMutexAcquire(osMutexId, osWaitForever);
          osMessageQueuePut(elev->osMsgControl_id, tmp, 1, 0);
          openDoor(command, str);
          osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
          turnLightOff(command, str);
          osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
          osMutexRelease(osMutexId);
        }
        return;
    break;
    default: break;
  }
}