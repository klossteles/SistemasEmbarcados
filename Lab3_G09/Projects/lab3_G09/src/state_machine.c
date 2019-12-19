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

uint32_t tickCount;

osThreadId_t elevE_id, elevC_id, elevD_id;
osMessageQueueId_t osRightElevatorMessageQueue_id, osLeftElevatorMessageQueue_id, osCentralElevatorMessageQueue_id;
osMutexId_t osMutexId;
const osMutexAttr_t Thread_Mutex_attr = {
  "messageControlMutex",                    // human readable mutex name
  osMutexRecursive | osMutexPrioInherit,    // attr_bits
  NULL,                                     // memory for control block   
  0U                                        // size for control block
};

Elevator elev_e, elev_c, elev_d;

void centralElevatorTask(void *arg0){  
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(elev_c.osMessageQueue_id, msg, NULL, 0U);   // wait for message
    if (status == osOK) {
      changeState(&elev_c, msg, str);
      osThreadYield();
    }
  }
}// centralElevatorTask

void leftElevatorTask(void *arg0){
  
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(elev_e.osMessageQueue_id, msg, NULL, 0U);   // wait for message
    if (status == osOK) {
      changeState(&elev_e, msg, str);
      osThreadYield();
    }
  }
}// leftElevatorTask

void rightElevatorTask(void *arg0){
  
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(elev_d.osMessageQueue_id, msg, NULL, 0U);   // wait for message
    if (status == osOK) {
      changeState(&elev_d, msg, str);
      osThreadYield();
    }
  }
}// rightElevatorTask

void controlTask(void *arg0){
  char msg[BUFFER];
  tickCount = osKernelGetTickCount();
  osMessageQueueId_t controlMessageQueue = ((osMessageQueueId_t *)arg0);
  osMutexId = osMutexNew(&Thread_Mutex_attr);                                   // cria mutex para controle de inserção na fila
  
  osLeftElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  osRightElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  osCentralElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  
  elev_e.level = 'a';
  for (uint8_t i = 0; i < 15 ; i++) {
    elev_e.nextLevel[i] = 0;
  }
  elev_e.name = 'e';
  elev_e.osMessageQueue_id = osLeftElevatorMessageQueue_id;
  elev_e.osMsgControl_id = controlMessageQueue;
  elev_e.state = STOPPED_OPEN_DOORS;
  elev_e.prevMovState = GOING_UP;
  
  elev_c.level = 'a';
  for (uint8_t i = 0; i < 15 ; i++) {
    elev_c.nextLevel[i] = 0;
  }
  elev_c.name = 'c';
  elev_c.osMessageQueue_id = osCentralElevatorMessageQueue_id;
  elev_c.osMsgControl_id = controlMessageQueue;
  elev_c.state = STOPPED_OPEN_DOORS;
  elev_c.prevMovState = GOING_UP;
  
  elev_d.level = 'a';
  for (uint8_t i = 0; i < 15 ; i++) {
    elev_d.nextLevel[i] = 0;
  }
  elev_d.name = 'd';
  elev_d.osMessageQueue_id = osRightElevatorMessageQueue_id;
  elev_d.osMsgControl_id = controlMessageQueue;
  elev_d.state = STOPPED_OPEN_DOORS;
  elev_d.prevMovState = GOING_UP;
  
  elevE_id = osThreadNew(leftElevatorTask, NULL, NULL);                         //thread elevador esquerdo
  elevC_id = osThreadNew(centralElevatorTask, NULL, NULL);                      //thread elevador central
  elevD_id = osThreadNew(rightElevatorTask, NULL, NULL);                        //thread elevador direito
  
  char uartEntry[5];
  char firstStr[] = "er\rcr\rdr\r";
  sendString(firstStr);
  
  while(1){
    uartEntry[0]  = '\0';
    uartEntry[1]  = '\0';
    uartEntry[2]  = '\0';
    uartEntry[3]  = '\0';
    uartEntry[4]  = '\0';
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
  for(int i = 0; i < BUFFER; i++){
    str[i] = '\0';
  }
  switch(command[1]){
     case 'I':
    if(elev->state == STOPPED_OPEN_DOORS && elev->level != command[2] && (int)command[2] >= (int)'a' && 
       (int)command[2] <= (int)'p' ){
         addElementToQueue(elev, levelCharToInt(command[2]));
         closeDoor(command, str);                                                // close door
         osMutexAcquire(osMutexId, osWaitForever);
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         turnLightOn(command, str);                                              // turn light on
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         osMutexRelease(osMutexId);
       } else if ((elev->state == GOING_UP || elev->state == GOING_DOWN) && elev->level != command[2] && (int)command[2] >= (int)'a' && 
                  (int)command[2] <= (int)'p' ){
                    addElementToQueue(elev, levelCharToInt(command[2]));
                    osMutexAcquire(osMutexId, osWaitForever);
                    osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
                    turnLightOn(command, str);                                              // turn light on
                    osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
                    osMutexRelease(osMutexId);
                  }
    return;
    break;
     case 'E':{
       char tmpi[3] = "xx";
       if (command[2] == '0' && command[1] != '1') {
         tmpi[1] = command[3];
       } else {
         tmpi[1] = command[2];
         tmpi[2] = command[3]; 
       }
       char requestedLevel = strMap(tmpi);
       if(elev->state == STOPPED_OPEN_DOORS){
         addElementToQueue(elev, levelCharToInt(requestedLevel));
         closeDoor(command, str);
         osMutexAcquire(osMutexId, osWaitForever);
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         osMutexRelease(osMutexId);
       } else if ((elev->state == GOING_UP || elev->state == GOING_DOWN)) {
         addElementToQueue(elev, levelCharToInt(requestedLevel));
         closeDoor(command, str);
         osMutexAcquire(osMutexId, osWaitForever);
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         osMutexRelease(osMutexId);
       }
       return;
       break; 
     }
     default: break;
  }
  switch(elev->state){
     case STOPPED_OPEN_DOORS:
    // sinal de porta fechada recebido recebido
    if(command[0] == elev->name && command[1] == 'F'){
      char tmp[4] = "xx\r";
      tmp[0] = elev->name;
      getElevatorNextMovement(elev, tmp);
      osMutexAcquire(osMutexId, osWaitForever);
      osMessageQueuePut(elev->osMsgControl_id, tmp, 1, 0);
      osMutexRelease(osMutexId);
    }
    return;
    break;
     case STOPPED_CLOSE_DOORS:{
       if(command[0] == elev->name && command[1] == 'A'){ // aguarda a porta abrir
         elev->state = STOPPED_OPEN_DOORS;
         removeElementFromQueue(elev, levelCharToInt(elev->level));
         uint8_t cont = 0;
         for (uint8_t i = 0 ; i < 15 ; i++) {
           if (elev->nextLevel[i] == 1) {
             cont = 1;
             break;
           }
         }
         if (cont == 1) {
           tickCount = osKernelGetTickCount();
           closeDoor(command, str);
           osDelayUntil(2000 + tickCount);
           osMutexAcquire(osMutexId, osWaitForever);
           osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
           osMutexRelease(osMutexId);
         }
       }
       break;
     }
     case GOING_UP:{
       char tmpi[2] = "xx";
       if (command[2] != '\0') {
         tmpi[0] = command[1];
         tmpi[1] = command[2];
       } else {
         tmpi[0] = '0';
         tmpi[1] = command[1];
       }
       int currentLevel = atoi(tmpi);
       char level = levelIntToChar(currentLevel);
       if(command[0] == elev->name && elev->nextLevel[currentLevel] == 1){
         char tmp[4] = "xp\r";
         tmp[0] = elev->name;
         elev->prevMovState = elev->state;
         elev->state = STOPPED_CLOSE_DOORS;
         elev->level = level;
         osMutexAcquire(osMutexId, osWaitForever);
         osMessageQueuePut(elev->osMsgControl_id, tmp, 1, 0);
         openDoor(command, str);
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         command[2] = elev->level;
         turnLightOff(command, str);
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         osMutexRelease(osMutexId);
       }
       return;
       break;
     }
     case GOING_DOWN: {
       char tmpi[2] = "xx";
       if (command[2] != '\0') {
         tmpi[0] = command[1];
         tmpi[1] = command[2];
       } else {
         tmpi[0] = '0';
         tmpi[1] = command[1];
       }
       int currentLevel = atoi(tmpi);
       char level = levelIntToChar(currentLevel);
       if(command[0] == elev->name && elev->nextLevel[currentLevel] == 1){
         char tmp[4] = "xp\r";
         tmp[0] = elev->name;
         elev->prevMovState = elev->state;
         elev->state = STOPPED_CLOSE_DOORS;
         elev->level = level;
         osMutexAcquire(osMutexId, osWaitForever);
         osMessageQueuePut(elev->osMsgControl_id, tmp, 1, 0);
         openDoor(command, str);
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         command[2] = elev->level;
         turnLightOff(command, str);
         osMessageQueuePut(elev->osMsgControl_id, str, 1, 0);
         osMutexRelease(osMutexId);
       }
       return;
       break;
     }
     default: break;
  }
}