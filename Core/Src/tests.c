#include "tests.h"
#include <main.h>

void onboard_led_init() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER &= (3U << (3 * 2));
  GPIOB->MODER |= (1U << (3 * 2));
}

void onboard_led_toggle() { GPIOB->ODR ^= (1U << 3); }
