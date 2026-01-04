#include "delay.h"
#include "stm32f042x6.h"

// Pin = ?
// Port = ? 

void init_dht22(void){
  // Enable clock to GPIOA
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // Setup MODER
  
  // Setup OTYPER

  // Setup OSPEEDR

  // Setup PUPDR
}

void get_dht22_data(void){

  

}
