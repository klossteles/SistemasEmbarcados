#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"

#include "system_TM4C1294.h" 

uint8_t LED_D1 = 0;

extern void UARTStdioIntHandler(void);

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
  UARTStdioConfig(0, 9600, SystemCoreClock);
} // UARTInit

void UART0_Handler(void){
  UARTStdioIntHandler();
} // UART0_Handler

void SysTick_Handler(void){
  LED_D1 ^= GPIO_PIN_1; // Troca estado do LED D1
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, LED_D1); // Acende ou apaga LED D1
} // SysTick_Handler

void main(void){
  UARTInit();
  UARTprintf("Sistemas Embarcados - 2019/2\n");
  
  SysTickPeriodSet(12000000); // f = 1Hz para clock = 24MHz
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK); // Habilita GPIO K
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)); // Aguarda final da habilitação
  GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
  GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_0);

  SysTickIntEnable();
  SysTickEnable();
  
  int high = 0, low = 0, samples = 0;
  int veth[10], vetl[10];

  while(1){
    for(samples = 0; samples<10 ; samples++){
      if(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0) == GPIO_PIN_0){
        while(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0) == GPIO_PIN_0);
        do{
          low++;
        }while(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0) == 0);
        do{
          high++;
        }while(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0) == 1);
        vetl[samples] = low;
        veth[samples] = high;
        low = 0;
        high = 0;
      }
    }
    int sumh = 0, suml = 0;
    for(samples = 0; samples<10 ; samples++){
      sumh += veth[samples];
      suml += vetl[samples];
    }
    UARTprintf("%d %d\n", sumh, suml);
  } // while
} // main