#include "main.h"
#include "i2c.h"
#include "stm32f0xx_hal.h"
#include "tests.h"
#include "u8g2_port.h"
#include "u8g2.h"
#include "delay.h"
#include "hdc1080.h"
#include "application.h"

#define ANCHOR_POSITION_X 15
#define ANCHOR_POSITION_Y 25

void SystemClock_Config(void);

volatile uint8_t sensor_due = 0;

int main(void) {
  HAL_Init();
  SystemClock_Config();
  onboard_led_init();
  i2c_init();
  tim2_init_1mhz();
  tim3_init_1min();
  hdc1080_init();

  u8g2_t *u8g2 = u8g2_port_init();
  hdc1080_t sensor = {0};
  positions_t positions;

  init_positions(&positions, ANCHOR_POSITION_X, ANCHOR_POSITION_Y);

  while (1) {
    if (sensor_due == 1) {
      sensor_due = 0;
      hdc1080_read(&sensor);
      hdc1080_check_hi_low(&sensor);
      onboard_led_toggle();
    }
    hdc1080_temperature_draw(u8g2, &positions, &sensor);

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

void TIM3_IRQHandler(void){
  if(TIM3->SR & TIM_SR_UIF){
    TIM3->SR &= ~TIM_SR_UIF;
    sensor_due = 1;
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
