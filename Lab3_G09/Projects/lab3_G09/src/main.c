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
#include "state_machine.h"
#include "string_utils.h"
#include "state_machine.h"

extern void UARTStdioIntHandler(void);

osThreadId_t elevE_id, elevC_id, elevD_id, control_id, messageControl_id;

osMessageQueueId_t osControlMessageQueue_id, osLeftElevatorMessageQueue_id;
osMessageQueueId_t osRightElevatorMessageQueue_id, osCentralElevatorMessageQueue_id;

osMutexId_t osMutexId;
const osMutexAttr_t Thread_Mutex_attr = {
  "messageControlMutex",                    // human readable mutex name
  osMutexRecursive | osMutexPrioInherit,    // attr_bits
  NULL,                                     // memory for control block   
  0U                                        // size for control block
};

Elevator elev_e;
Elevator elev_c;
Elevator elev_d;

void leftElevatorTask(void *arg0);
void centralElevatorTask(void *arg0);
void rightElevatorTask(void *arg0);
void controlTask(void *arg0);
void controlMessageTask(void *arg0);
void UARTInit(void);
void app_main (void *argument);

void leftElevatorTask(void *arg0){
  elevatorInit(&elev_e, 'e');                                                         // inicializa elevador
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(osLeftElevatorMessageQueue_id, msg, NULL, 0U);   // wait for message
    if (status == osOK) {
      changeState(&elev_e, msg, str);
      osMutexAcquire(osMutexId, osWaitForever);                                 // adquire mutex para inserção na fila de mensagem da thread control
      osMessageQueuePut(osControlMessageQueue_id, str, 1, 0);
      osMutexRelease(osMutexId);                                                // libera o mutex
      osThreadYield();
    }
  }
}// leftElevatorTask

void centralElevatorTask(void *arg0){
  elevatorInit(&elev_c, 'c');                                                         // inicializa elevador
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(osCentralElevatorMessageQueue_id, msg, NULL, 0U);// wait for message
    if (status == osOK) {
      changeState(&elev_c, msg, str);
      osMutexAcquire(osMutexId, osWaitForever);                                 // adquire mutex para inserção na fila de mensagem da thread control
      osMessageQueuePut(osControlMessageQueue_id, str, 0U, 0U);
      osMutexRelease(osMutexId);                                                // libera o mutex
      osThreadYield();
    }
  }
}// centralElevatorTask

void rightElevatorTask(void *arg0){
  elevatorInit(&elev_d, 'd');                                                         // inicializa elevador
  char str[BUFFER], msg[5];
  osStatus_t status;
  while(1){
    status = osMessageQueueGet(osRightElevatorMessageQueue_id, msg, NULL, 0U);   // wait for message
    if (status == osOK) {
      changeState(&elev_d, msg, str);
      osMutexAcquire(osMutexId, osWaitForever);                                 // adquire mutex para inserção na fila de mensagem da thread control
      osMessageQueuePut(osControlMessageQueue_id, str, 0U, 0U);
      osMutexRelease(osMutexId);                                                // libera o mutex
    }
    osThreadYield();
  }
}// rightElevatorTask

void controlMessageTask(void *arg0){
  osStatus_t status;
  char str[BUFFER];
  while (1) {
    status = osMessageQueueGet(osControlMessageQueue_id, str, NULL, 0U);        // wait for message
    if (status == osOK) {
      sendString(str);
    }
    osThreadYield();
  }
}// controlMessageTask

void controlTask(void *arg0){
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


void app_main (void *argument) {
  UARTInit();
  // cria thread responsável pela fila de mensagens e envio para uart.  
  messageControl_id = osThreadNew(controlMessageTask, (void*) NULL, NULL);
  control_id = osThreadNew(controlTask, NULL, NULL);                            //recebimento de dados da uart
  
  elevE_id = osThreadNew(leftElevatorTask, NULL, NULL);                         //thread elevador esquerdo
  elevC_id = osThreadNew(centralElevatorTask, NULL, NULL);                      //thread elevador central
  elevD_id = osThreadNew(rightElevatorTask, NULL, NULL);                        //thread elevador direito
  
  osMutexId = osMutexNew(&Thread_Mutex_attr);                                   // cria mutex para controle de inserção na fila
  
  // cria fila de mensagens
  char msg[BUFFER];
  osControlMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  osLeftElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  osRightElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  osCentralElevatorMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  while(1) {
    osDelay(osWaitForever);
  }
}// app_main

int main (void){
  SystemInit();
  
  osKernelInitialize();  
  
  osThreadNew(app_main, NULL, NULL);
  
  if(osKernelGetState() == osKernelReady)
    osKernelStart();
  
  while(1);
}// main
