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

#include "elevator.h"
#include "queue.h"

osSemaphoreId_t controlSemId;
osThreadId_t elevE_id, elevC_id, elevD_id, control_id;

queue_t ce, cc, cd;
Elevator elev_e = {'a', 'p', &ce};
Elevator elev_c = {'a', 'p', &cc};
Elevator elev_d = {'a', 'p', &cd};

void leftElevatorTask(void *arg0);
void centralElevatorTask(void *arg0);
void rightElevatorTask(void *arg0);
void controlTask(void *arg0);
void initUART(void);

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
//  char input[BUFFER];
//  UART_Handle uart;
//  UART_Params uartParams;
//  /* Create a UART with data processing off. */
//  UART_Params_init(&uartParams);
//  uartParams.writeDataMode = UART_DATA_TEXT;
//  uartParams.readDataMode = UART_DATA_TEXT;
//  uartParams.readReturnMode = UART_RETURN_NEWLINE;
//  uartParams.readEcho = UART_ECHO_OFF;
//  uartParams.baudRate = 115200;
//  uart = UART_open(Board_UART0, &uartParams);
//  
//  if (uart == NULL) {
//    System_abort("Error opening the UART");
//  }
//  
//  UART_write(uart, echoPrompt, sizeof(echoPrompt));
  
//  UARTprintf("er\r");
//  UARTprintf("cr\r");
//  UARTprintf("dr\r");
//  int size = 1;
  uint8_t led = (uint32_t)arg0;
  uint8_t state = 0;  
  
  /* Loop forever echoing */
  while (1) {
//    size = UART_read(uart, &input, 80);
//    input[size] = '\0';
//    rule(input, elev_e);
    state ^= led;
    LEDWrite(led, state);
    osDelay(200);
  }
}

void initUART(void) { 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioConfig(0, 115200, 16000000);
  UARTprintf("UART started\n");
}
