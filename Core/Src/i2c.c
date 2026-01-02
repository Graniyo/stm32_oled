#include "i2c.h"
#include "stm32f042x6.h"
#include <main.h>
#include <stdint.h>
// #include <sys/types.h>

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

void clear_flags() {
  // Clear command flags that may hinder transfer\reiceive
  I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; 
}

void i2c_send(uint8_t address, uint8_t data[], uint8_t length) {
  // Wait until bus is not busy
  while (I2C1->ISR & I2C_ISR_BUSY); 

  // Clear STOP and NACK flags.
  clear_flags(); 

  // Clear fields to be sure we write to empty fields 
  I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN |
                 I2C_CR2_RELOAD | I2C_CR2_AUTOEND);

  // Configure transfer address, NBYTES, write and enable AUTOEND 
  I2C1->CR2 |= ((uint32_t)(address << 1) << I2C_CR2_SADD_Pos);
  I2C1->CR2 |= ((uint32_t)length << I2C_CR2_NBYTES_Pos);
  I2C1->CR2 |= I2C_CR2_AUTOEND; 

  // Start
  I2C1->CR2 |= I2C_CR2_START;

  // End if NACK received from slave. Check if TXDR is empty (previous data is sent). Place data in TXDR register.
  for (uint8_t i = 0; i < length; i++) {

    while (!(I2C1->ISR & I2C_ISR_TXIS)) {
      if (I2C1->ISR & I2C_ISR_NACKF) {
        I2C1->ICR = I2C_ICR_NACKCF;
        return;
      }
    }

    I2C1->TXDR = data[i];
  }

  // Check if STOP is sent, clear STOP flag for next use of I2C1
  while (!(I2C1->ISR & I2C_ISR_STOPF)) {
  }
  I2C1->ICR = I2C_ICR_STOPCF;
}

// Streaming I2C functions for u8g2
// Uses RELOAD mode to allow byte-by-byte transmission within a single transaction

void i2c_start_write(uint8_t address) {
  // Wait until bus is not busy
  while (I2C1->ISR & I2C_ISR_BUSY);

  // Clear STOP and NACK flags
  I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF;

  // Configure: address, RELOAD=1, AUTOEND=0, NBYTES=1
  I2C1->CR2 = ((uint32_t)(address << 1) << I2C_CR2_SADD_Pos)
            | I2C_CR2_RELOAD
            | (1 << I2C_CR2_NBYTES_Pos);

  // Generate START
  I2C1->CR2 |= I2C_CR2_START;
}

void i2c_write(uint8_t *data, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    // Wait for TXIS, handling TCR (reload request) while waiting
    while (!(I2C1->ISR & I2C_ISR_TXIS)) {
      // If TCR is set, previous byte finished - reload NBYTES
      if (I2C1->ISR & I2C_ISR_TCR) {
        I2C1->CR2 = (I2C1->CR2 & ~I2C_CR2_NBYTES) | (1 << I2C_CR2_NBYTES_Pos);
      }
      if (I2C1->ISR & I2C_ISR_NACKF) {
        I2C1->ICR = I2C_ICR_NACKCF;
        return;
      }
    }

    // Write byte
    I2C1->TXDR = data[i];
  }
}

void i2c_stop(void) {
  // Wait for TCR (last byte transmitted, reload pending)
  while (!(I2C1->ISR & I2C_ISR_TCR)) {
    if (I2C1->ISR & I2C_ISR_NACKF) {
      I2C1->ICR = I2C_ICR_NACKCF;
      break;
    }
  }

  // Clear RELOAD and set NBYTES=0 to end transfer
  I2C1->CR2 &= ~(I2C_CR2_RELOAD | I2C_CR2_NBYTES);

  // Generate STOP
  I2C1->CR2 |= I2C_CR2_STOP;

  // Wait for STOP to complete
  while (!(I2C1->ISR & I2C_ISR_STOPF));
  I2C1->ICR = I2C_ICR_STOPCF;
}
