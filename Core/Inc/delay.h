#ifndef DELAY_H
#define DELAY_H
#include <stdint.h>

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

void tim2_init_1mhz(void);
void bare_delay_us(uint32_t us);
uint32_t micros(void);

#endif
