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

extern void UARTStdioIntHandler(void);

osThreadId_t control_id, messageControl_id;
osMessageQueueId_t osControlMessageQueue_id;

void controlMessageTask(void *arg0);
void UARTInit(void);
void app_main (void *argument);

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
  
  osKernelInitialize();  
  
  UARTInit();
  // cria fila de mensagens
  char msg[BUFFER];
  osControlMessageQueue_id = osMessageQueueNew(BUFFER, sizeof(msg), NULL);
  // cria thread responsável pela fila de mensagens e envio para uart.  
  messageControl_id = osThreadNew(controlMessageTask, NULL, NULL);
  control_id = osThreadNew(controlTask, (void*) osControlMessageQueue_id, NULL);                            //recebimento de dados da uart
  
  if(osKernelGetState() == osKernelReady)
    osKernelStart();
  
  while(1);
}// main
