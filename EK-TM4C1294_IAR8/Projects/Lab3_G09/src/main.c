/***
* S12_G09_Lab3
* Laboratório 3 - Sistemas Embarcados
* André Luiz Rodrigues dos Santos
* Luís Henrique Beltrão Santana
* Lucas Kloss Teles
**/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "system_TM4C1294.h"
#include "inc/tm4c1294ncpdt.h"                                                  // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"                                                   // driverlib
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/uartstdio.h"
#include "driverlib/pin_map.h"
#include "cmsis_os2.h" // CMSIS-RTOS
#include "driverleds.h" // device drivers
#include "driverlib/interrupt.h"

#include "elevator.h"
#include "queue.h"

osSemaphoreId_t controlSemId;
osThreadId_t elevE_id, elevC_id, elevD_id, control_id;

void leftElevatorTask(void *arg0);
void centralElevatorTask(void *arg0);
void rightElevatorTask(void *arg0);
void controlTask(void *arg0);

/*
*  ======== main ========
*/
void main(void) {
  SystemInit();
  LEDInit(LED4 | LED3 | LED2 | LED1);
  
  osKernelInitialize();
  
  control_id = osThreadNew(controlTask, (void*) LED4, NULL);
  elevE_id = osThreadNew(leftElevatorTask, (void*) LED1, NULL);
  elevC_id = osThreadNew(centralElevatorTask, (void*) LED2, NULL);
  elevD_id = osThreadNew(rightElevatorTask, (void*) LED3, NULL);
  
  controlSemId = osSemaphoreNew(BUFFER, BUFFER, NULL); // espaços disponíveis = BUFFER_SIZE
  
  if(osKernelGetState() == osKernelReady)
    osKernelStart();
  
  while(1);
}

void leftElevatorTask(void *arg0){
  uint8_t led = (uint32_t)arg0;
  uint8_t state = 0;  
  while(1){
    state ^= led;
    LEDWrite(led, state);
    osDelay(200);
  }
}

void centralElevatorTask(void *arg0){
  uint8_t led = (uint32_t)arg0;
  uint8_t state = 0;  
  while(1){
    state ^= led;
    LEDWrite(led, state);
    osDelay(200);
  }
}

void rightElevatorTask(void *arg0){
  uint8_t led = (uint32_t)arg0;
  uint8_t state = 0;  
  while(1){
    state ^= led;
    LEDWrite(led, state);
    osDelay(200);
  }
}

void controlTask(void *arg0){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
  UARTConfigSetExpClk(UART0_BASE, SystemCoreClock, 115200, (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8));
  UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
  UARTIntDisable(UART0_BASE, 0xFFFFFFFF);
  UARTIntEnable(UART0_BASE, UART_INT_RX);
  IntEnable(INT_UART0);
  UARTEnable(UART0_BASE);
  
  char str[] = "er\rcr\rdr\r";
  for(uint8_t i = 0; i < strlen(str); i++){
    UARTCharPut(UART0_BASE, str[i]);
  }
  
  osDelay(osWaitForever);
  while(1);
}
