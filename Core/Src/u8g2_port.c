#include "u8g2_port.h"
#include "u8g2.h"
#include "u8x8.h"
#include "i2c.h"
#include "delay.h"
#include "stm32f0xx.h"
#include <stdint.h>

static u8g2_t u8g2;


uint8_t U8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg){
    case U8X8_MSG_BYTE_START_TRANSFER:
      i2c_start_write(u8x8->i2c_address >> 1);
      return 1;

    case U8X8_MSG_BYTE_SEND:
      i2c_write((uint8_t *)arg_ptr, arg_int);
      return 1;

    case U8X8_MSG_BYTE_END_TRANSFER:
      i2c_stop();
      return 1;

    default:
      return 1;
  }
  return 1;
}

uint8_t U8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  (void)u8x8;
  (void)arg_ptr;

  switch(msg){
    case U8X8_MSG_DELAY_MILLI:
      delay_ms(arg_int);
      return 1;

    case U8X8_MSG_DELAY_10MICRO:
      delay_us(arg_int * 10);
      return 1;

    case U8X8_MSG_DELAY_100NANO:
      // At 8MHz, 100ns < 1 cycle, so NOP is sufficient
      __NOP();
      return 1;

    default:
      return 1;
  }
  return 1;
}

u8g2_t* u8g2_port_init(void){
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, U8x8_byte_hw_i2c, U8x8_gpio_and_delay_stm32);

  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);

  return &u8g2;
}

void u8g2_port_test_pattern(void){
  u8g2_ClearBuffer(&u8g2);
  u8g2_DrawBox(&u8g2, 0, 0, 128, 64);
  u8g2_SendBuffer(&u8g2);
}




