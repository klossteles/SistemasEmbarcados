/***
 * S12_G09_Lab3
 * Laboratório 3 - Sistemas Embarcados
 * André Luiz Rodrigues dos Santos
 * Luís Henrique Beltrão Santana
 * Lucas Silvestre Kloss Teles
**/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system_TM4C1294.h"
#include "inc/tm4c1294ncpdt.h"                                                  // CMSIS-Core
// includes da biblioteca driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"

#include "system_TM4C1294.h" 

#include "driverleds.h" // device drivers
#include "driverlib/interrupt.h"
#include "cmsis_os2.h" // CMSIS-RTOS

#include "elevator.h"
#include "string_utils.h"

#define DDEBUG false

extern void UARTStdioIntHandler(void);

//osSemaphoreId_t controlSemId;
osThreadId_t elevE_id, elevC_id, elevD_id, control_id, messageControl_id;

osMessageQueueId_t osControlMessageQueue_id, osLeftElevatorMessageQueue_id;
osMessageQueueId_t osRightElevatorMessageQueue_id, osCentralElevatorMessageQueue_id;

osMutexId_t osMutexId;

Elevator elev_e;
Elevator elev_c;
Elevator elev_d;

void leftElevatorTask(void *arg0);
void centralElevatorTask(void *arg0);
void rightElevatorTask(void *arg0);
void controlTask(void *arg0);
void controlMessageThread(void *arg0);
void UARTInit(void);

void leftElevatorTask(void *arg0){
  elevatorInit(elev_e);                                                         // inicializa elevador
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(osLeftElevatorMessageQueue_id, msg, NULL, 0U);   // wait for message
    if (status == osOK) {
      printf("Message E: %s\n", msg);
//      changeState(elev_e, msg, str);
      osMutexAcquire(osMutexId, osWaitForever);                                 // adquire mutex para inserção na fila de mensagem da thread control
      osMessageQueuePut(osControlMessageQueue_id, str, 1, 0);
      osMutexRelease(osMutexId);                                                // libera o mutex
      osThreadYield();
    }
  }
}

void centralElevatorTask(void *arg0){
  elevatorInit(elev_c);                                                         // inicializa elevador
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(osCentralElevatorMessageQueue_id, msg, NULL, 0U);// wait for message
    if (status == osOK) {
      printf("Message C: %s\n", msg);
      //      changeState(elev_e, msg, str);
      osMutexAcquire(osMutexId, osWaitForever);                                 // adquire mutex para inserção na fila de mensagem da thread control
      sprintf(str, "cf\r\0");
      osMessageQueuePut(osControlMessageQueue_id, str, 0U, 0U);
      osMutexRelease(osMutexId);                                                // libera o mutex
      osThreadYield();
    }
  }
}

void rightElevatorTask(void *arg0){
  elevatorInit(elev_d);                                                         // inicializa elevador
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(osRightElevatorMessageQueue_id, msg, NULL, 0U);   // wait for message
    if (status == osOK) {
      printf("Message D: %s\n", msg);
      //      changeState(elev_e, msg, str);
      osMutexAcquire(osMutexId, osWaitForever);                                 // adquire mutex para inserção na fila de mensagem da thread control
      sprintf(str, "df\r\0");
      osMessageQueuePut(osControlMessageQueue_id, str, 0U, 0U);
      osMutexRelease(osMutexId);                                                // libera o mutex
      osThreadYield();
    }
  }
}

void controlMessageThread(void *arg0){
  osStatus_t status;
  char str[BUFFER];
  while (1) {
    status = osMessageQueueGet(osControlMessageQueue_id, str, NULL, 0U);   // wait for message
    if (status == osOK) {
      sendString(str);
    }
  }
}

void controlTask(void *arg0){
  UARTInit();
  char uartEntry[5];
//  char firstStr[] = "er\rcr\rdr\r";
  char firstStr[] = "er\r";
  sendString(firstStr);
  
  while(1){
    UARTgets(uartEntry, 5);
    switch(uartEntry[0]){
      case 'e': 
        osMessageQueuePut(osLeftElevatorMessageQueue_id, uartEntry, 1, 0);
      break;
//     case 'c': 
//        osMessageQueuePut(osCentralElevatorMessageQueue_id, uartEntry, 1, 0);
//      break;
//     case 'd': 
//        osMessageQueuePut(osRightElevatorMessageQueue_id, uartEntry, 1, 0);
//      break;
     default:
        printf("ERROR %s\n", uartEntry);
        break;
    }
    osThreadYield();
  };  
}

void UARTInit(void){
  // Enable the GPIO Peripheral used by the UART.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

  // Enable UART0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

  // Configure GPIO Pins for UART mode.
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  // Initialize the UART for console I/O.
  UARTStdioConfig(0, 115200, SystemCoreClock);
} // UARTInit

void UART0_Handler(void){
  UARTStdioIntHandler();
} // UART0_Handler

int main (void){
  SystemInit();
//  LEDInit(LED4 | LED3 | LED2 | LED1);
  
  osKernelInitialize();
  osMutexId = osMutexNew(NULL);         // cria mutex para controle de inserção na fila
  
  char msg[BUFFER];
  // cria fila de mensagens
  osControlMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  
  //cria filas de mensagens dos elevadores
  osLeftElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
//  osRightElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
//  osCentralElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  
  //cria thread responsável pela fila de mensagens e envio para uart.  
  messageControl_id = osThreadNew(controlMessageThread, (void*) NULL, NULL);
  control_id = osThreadNew(controlTask, (void*) LED4, NULL);    //recebimento de dados da uart
  elevE_id = osThreadNew(leftElevatorTask, (void*) LED1, NULL); //thread elevador esquerdo
//  elevC_id = osThreadNew(centralElevatorTask, (void*) LED2, NULL);    //thread elevador central
//  elevD_id = osThreadNew(rightElevatorTask, (void*) LED3, NULL);      //thread elevador direito
  
  if(osKernelGetState() == osKernelReady)
    osKernelStart();
  
  while(1);
}
