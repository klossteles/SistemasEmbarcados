/* 
    Grupo 09
    Integrantes: 
      - André Luiz
      - Lucas Silvestre Kloss Teles
      - Luis Henrique
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "inc/tm4c1294ncpdt.h"                                                  // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"                                                   // driverlib
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/uartstdio.h"
#include "driverlib/pin_map.h"

//#define BASE_FREQUENCY 24000000
#define BASE_FREQUENCY 120000000
#define SAMPLE_SIZE 10

int sample_high[SAMPLE_SIZE],sample_low[SAMPLE_SIZE];   
int sample_index = 0; 
int high_counter = 0; 
int low_counter = 0;
int systickStartCount = 0;                                                      // Armazenar tempo que iniciou a mudança
int systickEndCount = 0;                                                        // Armazenar tempo que trocou sudida->descida ou descida->subida
int enableCount = 0;                                                            // varável para ignorar primeira contagem, 
                                                                                // usada para iniciar a contagem no primeira subida
void onEdgeUp(void);
void onEdgeDown(void);
void initUART(void);
void enableInterrupts(void);
void disableInterrupts(void);
void initGPIO(void);
void sendValueToUART(void);

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
  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), clockFrequency);
}

void onEdgeDown(void) {                  
       TimerIntClear(TIMER0_BASE, TIMER_CAPB_EVENT);     // Clear interrupt flag
 
}

int a = 0;
int b = 0; //depois isso evolui pros arrays que usaremos pra capturar amostras 
void onEdgeUp(void) {
    TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT);
    a = TimerValueGet(TIMER0_BASE, TIMER_A);
    b = TimerValueGet(TIMER0_BASE, TIMER_B);
}

void enableInterrupts(void) {
    GPIOIntRegister(GPIO_PORTK_BASE, onEdgeDown);                               // Registra função para o port K
    GPIOIntTypeSet(GPIO_PORTK_BASE, GPIO_PIN_7, GPIO_FALLING_EDGE);             // Configura PK7 para a borda de descida
    GPIOIntRegister(GPIO_PORTK_BASE, onEdgeUp);                                 // Registra função para o port K
    GPIOIntTypeSet(GPIO_PORTK_BASE, GPIO_PIN_7, GPIO_RISING_EDGE);              // Configura PK7 para a borda de descida
    GPIOIntEnable(GPIO_PORTK_BASE, GPIO_PIN_7);                                 // Habilita interrupção para o port PK7
}

void disableInterrupts(void) {
    GPIOIntDisable(GPIO_PORTK_BASE, GPIO_PIN_7);                                // Desabilita as interrupções do GPIOK7 (caso estivessem habilitadas)
    GPIOIntClear(GPIO_PORTK_BASE, GPIO_PIN_7);                                  // Clear pending interrupts for PK7
}

void initGPIO(void){
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

void sendValueToUART(void) {
  int sumh = 0;
  int suml = 0;
  for (int sample_index = 0 ; sample_index< SAMPLE_SIZE; sample_index++){
    sumh += sample_high[sample_index];
    suml += sample_low[sample_index]; 
  }
  int h = (int) 669*sumh/SAMPLE_SIZE;
  int l = (int) 623*suml/SAMPLE_SIZE;
  int tot = h + l;
  int freq = (int) 1000000000/tot;
  int duty = (int) 100*h/tot;
  
  UARTprintf("DutyCycle: %d\nFrequencia: %dHz\n", duty, freq);
  
  /* Após imprimir resetar variáveis globais para valores iniciais */
  //sample_high[SAMPLE_SIZE];
  //sample_low[SAMPLE_SIZE];   
  sample_index = high_counter = low_counter = 0;
}

void main(void){
  setClockFrequency(BASE_FREQUENCY);

  initUART();
  initGPIO(); 
  initTimers();
  while(1) {
    if (sample_index <= SAMPLE_SIZE) {
      disableInterrupts();                                                      // Desabilita as interrupções temporariamente, para que possa ser feita a escrita na uart
      sendValueToUART();                                                        // Imprime valores na uart  
      enableInterrupts();                                                       // Habilita as interrupções novamente
    }
  }
} 
