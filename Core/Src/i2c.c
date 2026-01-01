#include "i2c.h"
#include "stm32f042x6.h"
#include <main.h>
#include <stdint.h>
#include <sys/types.h>

// Init PB7 & PB6 for I2C
void i2c_init() {
  // Enable clock GPIOB
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  // Write 10 for AF on GPIOB PIN 7 & PIN 6 into MODER
  GPIOB->MODER |= (1U << 13);
  GPIOB->MODER &= ~(1U << 12);
  GPIOB->MODER |= (1U << 15);
  GPIOB->MODER &= ~(1U << 14);

  // Set open drain
  GPIOB->OTYPER |= (1U << 6);
  GPIOB->OTYPER |= (1U << 7);

  // Set output speed to low
  GPIOB->OSPEEDR &= ~(1U << 12);
  GPIOB->OSPEEDR &= ~(1U << 14);

  // Set no pull up, pupp down (12K resistor on breadboard)
  GPIOB->PUPDR &= ~(1U << 12);
  GPIOB->PUPDR &= ~(1U << 13);
  GPIOB->PUPDR &= ~(1U << 14);
  GPIOB->PUPDR &= ~(1U << 15);

  // Hard set alternative function I2C1 for pin 6 & pin 7
  GPIOB->AFR[0] = 0b00010001000000000000000000000000;

  // Enable clock to I2C1
  RCC->APB1ENR |= (1U << 21);

  // Setup timing
  I2C1->TIMINGR = (uint32_t)0x10301115;

  // Enable I2C1 peripheral
  I2C1->CR1 |= I2C_CR1_PE;
}

void clear_flags() { I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; }

void i2c_send(uint8_t address, uint8_t data[], uint8_t length)
{
    while (I2C1->ISR & I2C_ISR_BUSY) { }

    clear_flags(); // bør skrive til ICR, ikke ISR

    // Clear felter vi setter (felt, ikke *_Pos)
    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN |
                  I2C_CR2_RELOAD | I2C_CR2_AUTOEND);

    // Sett adresse + nbytes + write + autoend
    // NB: vurder (address << 1) hvis du ikke får ACK (STM32F0-adresseplassering)
    I2C1->CR2 |= ((uint32_t)(address << 1) << I2C_CR2_SADD_Pos);
    I2C1->CR2 |= ((uint32_t)length << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 |= I2C_CR2_AUTOEND;     // stop automatisk etter NBYTES

    // Start
    I2C1->CR2 |= I2C_CR2_START;

    for (uint8_t i = 0; i < length; i++) {

        while (!(I2C1->ISR & I2C_ISR_TXIS)) {
            if (I2C1->ISR & I2C_ISR_NACKF) {
                I2C1->ICR = I2C_ICR_NACKCF;
                return;
            }
        }

        I2C1->TXDR = data[i];
    }

    while (!(I2C1->ISR & I2C_ISR_STOPF)) { }
    I2C1->ICR = I2C_ICR_STOPCF;
}
