#include "delay.h"
#include "stm32f0xx_hal.h"

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
