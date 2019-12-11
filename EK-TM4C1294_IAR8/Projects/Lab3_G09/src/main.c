/***
 * S12_G09_Lab3
 * Laboratório 3 - Sistemas Embarcados
 * André Luiz Rodrigues dos Santos
 * Luís Henrique Beltrão Santana
 * Lucas Silvestre Kloss Teles
**/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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

extern void UARTStdioIntHandler(void);

osSemaphoreId_t controlSemId;
osThreadId_t elevE_id, elevC_id, elevD_id, control_id;

void leftElevatorTask(void *arg0);
void centralElevatorTask(void *arg0);
void rightElevatorTask(void *arg0);
void controlTask(void *arg0);
void UARTInit(void);

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
  UARTInit();
  char str[] = "er\rcr\rdr\r";
  sendString(str);
  char uartEntry[5];
  while(1){
    UARTgets(uartEntry, 5);
    printf("Entry: %s \n", uartEntry);
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
  LEDInit(LED4 | LED3 | LED2 | LED1);
  
  osKernelInitialize();
  control_id = osThreadNew(controlTask, (void*) LED4, NULL);
  //  elevE_id = osThreadNew(leftElevatorTask, (void*) LED1, NULL);
  //  elevC_id = osThreadNew(centralElevatorTask, (void*) LED2, NULL);
  //  elevD_id = osThreadNew(rightElevatorTask, (void*) LED3, NULL);
  
  //  controlSemId = osSemaphoreNew(BUFFER, BUFFER, NULL); // espa�os dispon�veis = BUFFER_SIZE
  
  if(osKernelGetState() == osKernelReady)
    osKernelStart();
  
  while(1);
}
