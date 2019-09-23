#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h" // driverlib
#include "driverlib/gpio.h"

#include "driverlib/uart.h"
#include "driverlib/uartstdio.h"

#include "driverlib/pin_map.h"

#define BASE_FREQUENCY 24000000
#define SAMPLE_SIZE 10

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

void initGPIO(){
 //todo: alterar portk e testar em outro port
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK));
  
  GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_7);
  GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // Habilita GPIO J (push-button SW1 = PJ0, push-button SW2 = PJ1)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)); // Aguarda final da habilitação
    
  GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0); // push-buttons SW1 e SW2 como entrada
  GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

void processSamples(int sample_high[SAMPLE_SIZE], int sample_low[SAMPLE_SIZE]){
  
  
  for (int sample_index = 0 ; sample_index< SAMPLE_SIZE; sample_index++){
     UARTprintf("Processamento Amostra%d --> : High%d; Low: %d \n", sample_index,  sample_high[sample_index], sample_low[sample_index]);
  }

}
void getSamples2(){

  int sample_high[SAMPLE_SIZE],sample_low[SAMPLE_SIZE];   
  int sample_index=0; 
  int high_counter=0; 
  int low_counter = 0;
  
  while(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_7) == GPIO_PIN_7);
  while(sample_index <= SAMPLE_SIZE){ 
    while(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_7) != GPIO_PIN_7);
    //TODO: FAZER UM CONTROLE PRO ESTADO DA WAVE PRA NÃO PASSAR POR AQUI DIRETO
    while(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_7) == GPIO_PIN_7)
    {
      ++high_counter;
    } 
    while(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_7) != GPIO_PIN_7)
    {
      ++low_counter;
    }
      
    
    if (sample_index!= 0){ //rejeita a primeira amostra
      int aux = sample_index -1;
      sample_low[aux] = low_counter;
      sample_high[aux] = high_counter;
      //UARTprintf("Amostra%d --> Ticks High: %d; Ticks Low: %d \n", sample_index-1, high_counter, low_counter);
    }
    
    sample_index++;
    high_counter = 0;
    low_counter = 0;   
   } //termina de pegar as samples 
   
   processSamples(sample_high, sample_low);
   
}

void main(void){
  
  setClockFrequency(BASE_FREQUENCY);
  
  initUART();
  
  initGPIO();
  
  getSamples2();
  

 
} 
