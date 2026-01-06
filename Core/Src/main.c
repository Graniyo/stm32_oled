#include <stdio.h>
#include "main.h"
#include "i2c.h"
#include "stm32f0xx_hal.h"
#include "tests.h"
#include "u8g2_port.h"
#include "u8g2.h"
#include "delay.h"
#include "DHT22.h"

void SystemClock_Config(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  onboard_led_init();
  i2c_init();
  tim2_init_1mhz();
  init_dht22();
  u8g2_t *u8g2 = u8g2_port_init();

  // Get text width for smooth wrapping
  u8g2_SetFont(u8g2, u8g2_font_helvB12_tr);
  const char *text = "Martin Grani";
  int16_t text_width = u8g2_GetStrWidth(u8g2, text);
  uint16_t middle_screen_pos = (128/2) - (text_width/2);

  char buf[16];

  u8g2_ClearBuffer(u8g2);
  u8g2_DrawStr(u8g2, middle_screen_pos, 38, text);
  u8g2_SendBuffer(u8g2);




  int16_t temperature = 0;
  int16_t humidity = 0;
  int dht_status = 0;
  int dht_status_struct = 0;

  dht22 sensor;

  while (1) {
    onboard_led_toggle();

    // Les DHT22-data
    //dht_status = get_dht22_data(&temperature, &humidity);
    dht_status_struct = get_dht22_data_struct(&sensor);

    u8g2_ClearBuffer(u8g2);

    if (dht_status_struct == 0) {
      // Vis temperatur (tiendeler -> grader)
      snprintf(buf, sizeof(buf), "T: %d.%d C", sensor.temperature / 10, sensor.temperature % 10);
      u8g2_DrawStr(u8g2, 0, 20, buf);

      // Vis fuktighet
      snprintf(buf, sizeof(buf), "H: %d.%d %%", sensor.humidity / 10, sensor.humidity % 10);
      u8g2_DrawStr(u8g2, 0, 40, buf);
    } else if (dht_status_struct == -1) {
      u8g2_DrawStr(u8g2, 0, 30, "DHT22: Timeout");
    } else {
      u8g2_DrawStr(u8g2, 0, 30, "DHT22: CRC feil");
    }

    u8g2_SendBuffer(u8g2);

    // DHT22 krever minst 2 sekunder mellom avlesninger
    bare_delay_ms(2000);
  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */
