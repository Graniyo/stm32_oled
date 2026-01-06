#include "DHT22.h"
#include "delay.h"
#include "stm32f042x6.h"

// Pin = PA8
// Port = GPIOA

static void configure_input(void);
static void configure_output(void);
static void output_low(void);
static void output_high(void);

void init_dht22(void) {
  // Enable clock to GPIOA
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // Setup MODER 8 to output mode (01)
  GPIOA->MODER &= ~(3U << 16);
  GPIOA->MODER |= (1U << 16);

  // Setup OTYPER = open drain (1)
  GPIOA->OTYPER |= (1U << 8);

  // Setup OSPEEDR = low speed (x0)
  GPIOA->OSPEEDR &= ~(3U << 16);

  // Setup PUPDR = no PUPDR (00), use external resistor
  GPIOA->PUPDR &= ~(3U << 16);

  // Start med linjen høy (idle)
  output_high();
}

static void configure_input(void) {
  // Setup MODER 8 to input mode (00)
  GPIOA->MODER &= ~(3U << 16);

  // Setup PUPDR = no PUPDR (00), use external resistor
  GPIOA->PUPDR &= ~(3U << 16);
}

static void configure_output(void) {
  // Setup MODER 8 to output mode (01)
  GPIOA->MODER &= ~(3U << 16);
  GPIOA->MODER |= (1U << 16);

  // Setup OTYPER = open drain (1)
  GPIOA->OTYPER |= (1U << 8);

  // Setup OSPEEDR = low speed (x0)
  GPIOA->OSPEEDR &= ~(3U << 16);

  // Setup PUPDR = no PUPDR (00), use external resistor
  GPIOA->PUPDR &= ~(3U << 16);
}

static void output_low(void) {
  GPIOA->BSRR = (1U << 24);
}

static void output_high(void) {
  GPIOA->BSRR = (1U << 8);
}

int get_dht22_data(int16_t *temperature, int16_t *humidity) {
  uint32_t t0;
  uint8_t data[5] = {0};
  uint32_t timeout;

  configure_output();
  output_low();
  bare_delay_ms(20);

  // Deaktiver interrupts for presis timing
  __disable_irq();

  output_high();
  bare_delay_us(30);

  configure_input();

  // Vent på DHT22 LAV respons (80us) med timeout
  timeout = 1000;
  while ((GPIOA->IDR & (1U << 8)) && --timeout);
  if (!timeout) {
    __enable_irq();
    return -1;
  }

  // Vent på DHT22 HØY (80us)
  timeout = 1000;
  while (!(GPIOA->IDR & (1U << 8)) && --timeout);
  if (!timeout) {
    __enable_irq();
    return -1;
  }

  // Les 40 bits
  for (int i = 0; i < 40; i++) {
    // Vent på LAV (50us forberedelse)
    timeout = 1000;
    while ((GPIOA->IDR & (1U << 8)) && --timeout);
    if (!timeout) {
      __enable_irq();
      return -1;
    }

    // Vent på HØY (start av databit)
    timeout = 1000;
    while (!(GPIOA->IDR & (1U << 8)) && --timeout);
    if (!timeout) {
      __enable_irq();
      return -1;
    }

    t0 = micros();

    // Vent på LAV (slutt av databit)
    timeout = 1000;
    while ((GPIOA->IDR & (1U << 8)) && --timeout);

    // 26-28us = 0, 70us = 1
    if (micros() - t0 > 50) {
      data[i / 8] |= (1 << (7 - (i % 8)));
    }
  }

  __enable_irq();

  // Sett tilbake til output høy (idle)
  configure_output();
  output_high();

  // Verifiser checksum
  if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
    return -2;
  }

  // Konverter til temperatur og fuktighet (i tiendeler)
  *humidity = (data[0] << 8) | data[1];
  *temperature = ((data[2] & 0x7F) << 8) | data[3];
  if (data[2] & 0x80) {
    *temperature = -*temperature;  // Negativ temperatur
  }

  return 0;
}


int get_dht22_data_struct(dht22 *data){
  uint32_t t0;
  uint8_t raw[5] = {0};
  uint32_t timeout;

  configure_output();
  output_low();
  bare_delay_ms(20);

  // Deaktiver interrupts for presis timing
  __disable_irq();

  output_high();
  bare_delay_us(30);

  configure_input();

  // Vent på DHT22 LAV respons (80us) med timeout
  timeout = 1000;
  while ((GPIOA->IDR & (1U << 8)) && --timeout);
  if (!timeout) {
    __enable_irq();
    return -1;
  }

  // Vent på DHT22 HØY (80us)
  timeout = 1000;
  while (!(GPIOA->IDR & (1U << 8)) && --timeout);
  if (!timeout) {
    __enable_irq();
    return -1;
  }

  // Les 40 bits
  for (int i = 0; i < 40; i++) {
    // Vent på LAV (50us forberedelse)
    timeout = 1000;
    while ((GPIOA->IDR & (1U << 8)) && --timeout);
    if (!timeout) {
      __enable_irq();
      return -1;
    }

    // Vent på HØY (start av databit)
    timeout = 1000;
    while (!(GPIOA->IDR & (1U << 8)) && --timeout);
    if (!timeout) {
      __enable_irq();
      return -1;
    }

    t0 = micros();

    // Vent på LAV (slutt av databit)
    timeout = 1000;
    while ((GPIOA->IDR & (1U << 8)) && --timeout);

    // 26-28us = 0, 70us = 1
    if (micros() - t0 > 50) {
      raw[i / 8] |= (1 << (7 - (i % 8)));
    }
  }

  __enable_irq();

  // Sett tilbake til output høy (idle)
  configure_output();
  output_high();

  // Verifiser checksum
  if (raw[4] != ((raw[0] + raw[1] + raw[2] + raw[3]) & 0xFF)) {
    return -2;
  }

  // Konverter til temperatur og fuktighet (i tiendeler)
  data->humidity = (raw[0] << 8) | raw[1];
  data->temperature = ((raw[2] & 0x7F) << 8) | raw[3];
  if (raw[2] & 0x80) {
    data->temperature = -data->temperature;  // Negativ temperatur
  }

  return 0;
}
