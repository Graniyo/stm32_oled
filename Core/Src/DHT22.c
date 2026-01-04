#include "delay.h"
#include "stm32f042x6.h"
#include <stdint.h>

// Pin = 8
// Port = GPIOA 

void init_dht22(void){
  // Enable clock to GPIOA
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // Setup MODER 8 to output mode (01)
  GPIOA->MODER &= ~(3U << 16);
  GPIOA->MODER |= (1U << 16);
  
  // Setup OTYPER = open drain (1)
  GPIOA->OTYPER |= (1U << 8);

  // Setup OSPEEDR = low speed (x0)
  GPIOA->OSPEEDR &= ~(3U << 16);

  // Setup PUPDR = no PUPDR (00), use external resistor.
  GPIOA->PUPDR &= ~(3U << 16);
}

void configure_input(){
  // Setup MODER 8 to input mode (00)
  GPIOA->MODER &= ~(3U << 16);
 
  // Setup PUPDR = no PUPDR (00), use external resistor.
  GPIOA->PUPDR &= ~(3U << 16);
}

void output_low(void){
  GPIOA->BSRR = (1U << 24);
}

void output_high(void){
  GPIOA->BSRR = (1U << 8);
}

void get_dht22_data(void){
  uint32_t t0, ts;
  uint32_t data[40];

  // Set output low 20ms
  output_low();
  bare_delay_ms(20);
  
  //Configure pin to INPUT
  configure_input();

  // Wait until low
  while(GPIOA->IDR & (1U << 8));

  // Wait until HIGH
  while(!(GPIOA->IDR & (1U << 8)));

  //loop through bits, solve 1's and 0's
  for(int i = 0; i < 40; i++){
    // Wait until low
    while(GPIOA->IDR & (1U << 8));

    // Wait until high, get current time 
    while(!(GPIOA->IDR & (1U << 8)));
    t0 = micros();

    // Wait until low, calculate time difference
    while(GPIOA->IDR & (1U << 8));
    ts = micros() - t0;

    if(ts > 50){
      data[i] = 1;
    } else {
      data[i] = 0;
    }
  }
}
