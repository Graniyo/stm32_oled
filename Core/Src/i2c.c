#include "i2c.h"
#include "stm32f042x6.h"
#include <main.h>
#include <stdint.h>

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

void i2c_send() {
  // Slave address (address of slave chip etc. oled ssd1306)
  I2C1->CR2 |= (0x3C << 1);

  // Choose send or recieve (Send in this case)
  I2C1->CR2 &= ~(I2C_CR2_RD_WRN);

  // Nbytes to send
  I2C1->CR2 |= (1 << 16);

  // Send start condition
  I2C1->CR2 |= I2C_CR2_START;

  // Make sure I2C is not busy by reading the start bit
  while (I2C1->CR2 & I2C_CR2_START)
    ;

  // Data to send
  I2C1->TXDR = COMMAND;

  // Wait until data is sent
  while (!(I2C1->ISR & I2C_ISR_TXE))
    ;

  I2C1->TXDR = INVERT;
  while (!(I2C1->ISR & I2C_ISR_TXE))

    // Send stop condition
    I2C1->CR2 |= I2C_CR2_STOP;

  // Wait until stop condition is sent
  while (I2C1->CR2 & I2C_CR2_STOP)
    ;
}
