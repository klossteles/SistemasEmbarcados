#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h" 
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/uartstdio.h"
#include "driverlib/pin_map.h"

//#define BASE_FREQUENCY 24000000
#define BASE_FREQUENCY 120000000
#define SAMPLE_SIZE 10
#define HIGH_BASE 668.46
#define LOW_BASE 623.11


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

void setClockFrequency(int clockFrequency){

  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                              SYSCTL_OSC_MAIN |
                                              SYSCTL_USE_PLL |
                                              SYSCTL_CFG_VCO_480),
                                              clockFrequency);
}

void onEdgeDown(void);
void onEdgeUp(void);

//Inicaliza contagem dos tempos em baixa
void onEdgeDown(void) {
   // if (GPIOIntStatus(GPIO_PORTD_BASE, false) & GPIO_PIN_0) {
      // PF4 was interrupt cause
       // GPIOIntRegister(GPIO_PORTK_BASE, onEdgeUp);   // Register our handler function for port F
       // GPIOIntTypeSet(GPIO_PORTK_BASE, GPIO_PIN_7,
       //     GPIO_RISING_EDGE);          // Configure PF4 for rising edge trigger
        //GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_0); // Clear interrupt flag
   // }
    
    
                          
       TimerIntClear(TIMER0_BASE, TIMER_CAPB_EVENT);     // Clear interrupt flag
 
}
int a = 0;
int b = 0; //depois isso evolui pros arrays que usaremos pra capturar amostras 
void onEdgeUp(void) {
    //if (GPIOIntStatus(GPIO_PORTD_BASE, false) & GPIO_PIN_0) {
        // PF4 was interrupt cause
        //GPIOIntRegister(GPIO_PORTK_BASE, onEdgeDown); // Register our handler function for port F
        //GPIOIntTypeSet(GPIO_PORTK_BASE, GPIO_PIN_7,
            //GPIO_FALLING_EDGE);                         // Configure PF4 for falling edge trigger
        //GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_0);      // Clear interrupt flag
    //}
        
    TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT);
    a = TimerValueGet(TIMER0_BASE, TIMER_A);
    b = TimerValueGet(TIMER0_BASE, TIMER_B);
  
}

void initGPIO(){
    //todo: alterar portk e testar em outro port
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK));
  
    GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_7);
    GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
    // Interrupt setup
    GPIOIntDisable(GPIO_PORTK_BASE, GPIO_PIN_7);        // Disable interrupt for PF4 (in case it was enabled)
    GPIOIntClear(GPIO_PORTK_BASE, GPIO_PIN_7);          // Clear pending interrupts for PF4
    GPIOIntRegister(GPIO_PORTK_BASE, onEdgeDown);       // Register our handler function for port F
    GPIOIntTypeSet(GPIO_PORTK_BASE, GPIO_PIN_7,
        GPIO_FALLING_EDGE);                             // Configure PK4 for falling edge trigger
    GPIOIntRegister(GPIO_PORTK_BASE, onEdgeUp);         // Register our handler function for port F
    GPIOIntTypeSet(GPIO_PORTK_BASE, GPIO_PIN_7,
        GPIO_RISING_EDGE);                              // Configure PF4 for falling edge trigger
    GPIOIntEnable(GPIO_PORTK_BASE, GPIO_PIN_7);         // Enable interrupt for PF4
  }


void initTimers(){
  
  /////Configurações do Timer0 -------------------------------------------------
  // Enable and configure Timer0 peripheral.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  // Wait for the Timer0 module to be ready.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)){}

  // Configure TimerA and B 
  TimerConfigure(TIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP | TIMER_CFG_B_CAP_TIME_UP));
  // Set both timers to start at zero
  TimerLoadSet(TIMER0_BASE, TIMER_BOTH, 0);
  // TimerA triggers on positive edge, TimerB triggers on negative edge
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  TimerControlEvent(TIMER0_BASE, TIMER_B, TIMER_EVENT_NEG_EDGE);
  
  // Enable the timers.
  TimerEnable(TIMER0_BASE, TIMER_BOTH);
  
  /////Configurações do Pino D0 e D1 -------------------------------------------
  //Enable para os periféricos
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  
  GPIOPinConfigure(GPIO_PD0_T0CCP0);
  GPIOPinConfigure(GPIO_PD1_T0CCP1);
  GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_0);
  GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_1);
  
  // Interrupt setup GPIO -> Funciona, sinal PWM apenas no D0
  //GPIOIntDisable(GPIO_PORTD_BASE, GPIO_PIN_0);        // Disable interrupt for PD0
  //GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_0);          // Clear pending interrupts for PD
  //GPIOIntRegister(GPIO_PORTD_BASE, onEdgeDown);       // Register our handler function for port D0
  //GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_0,
  //GPIO_FALLING_EDGE);                             // Configure PK4 for falling edge trigger
  //GPIOIntRegister(GPIO_PORTD_BASE, onEdgeUp);         // Register our handler function for port D
  //GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_0,
  //GPIO_RISING_EDGE);                              // Configure PF4 for falling edge trigger
  //GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_0);         // Enable interrupt for PF4

  
  //Interrup setup Timer -> Timer A no D0 e TimerB no D1
  //Registers a interrupt function to be called when timer b hits a neg edge event
    TimerIntRegister(TIMER0_BASE, TIMER_A, onEdgeUp); 
    TimerIntRegister(TIMER0_BASE, TIMER_B, onEdgeDown); 
    // Makes sure the interrupt is cleared
    TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT);
    TimerIntClear(TIMER0_BASE, TIMER_CAPB_EVENT);
    // Enable the indicated timer interrupt source.
    TimerIntEnable(TIMER0_BASE, TIMER_CAPA_EVENT);
    TimerIntEnable(TIMER0_BASE, TIMER_CAPB_EVENT);
    // The specified interrupt is enabled in the interrupt controller.
    IntEnable(INT_TIMER0A);
   IntEnable(INT_GPIOD);
 
}

void main(void){
  
  setClockFrequency(BASE_FREQUENCY);
  
  initTimers();

  while(1){ };
 
} 
