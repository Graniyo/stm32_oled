#include "delay.h"
#include "stm32f0xx_hal.h"
#include <stdint.h>

void delay_ms(uint32_t ms) {
  HAL_Delay(ms);
}

// Loop-based microsecond delay for STM32F0 at 8MHz HSI
// At 8MHz, 1us = 8 cycles. Loop overhead ~3-4 cycles per iteration.
void delay_us(uint32_t us) {
  // Approximate loop count - adjust if timing is off
  // Each iteration is roughly 4 cycles at -Og optimization
  volatile uint32_t count = us * 2;
  while (count--) {
    __NOP();
  }
}

void tim2_init_1mhz(void){
  // Init
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // Set prescaler
  TIM2->PSC = 8 - 1;

  // Reset CNT & force UEV (Update Event)
  TIM2->CNT = 0;
  TIM2->EGR = TIM_EGR_UG;

  // Start timer 
  TIM2->CR1 |= TIM_CR1_CEN;
}

void bare_delay_us(uint32_t us){
  uint32_t start = TIM2->CNT;
  while((uint32_t)(TIM2->CNT - start) < us);
}

void bare_delay_ms(uint32_t ms){
  for(int i = 0; i < ms; i++){
    bare_delay_us(1000);
  }
}

uint32_t micros(){
  return TIM2->CNT;
}
