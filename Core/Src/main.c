#include "main.h"
#include "i2c.h"
#include "stm32f0xx_hal.h"
#include "tests.h"
#include "u8g2_port.h"
#include "u8g2.h"
#include "delay.h"
#include "DHT22.h"
#include "application.h"

void SystemClock_Config(void);

volatile uint8_t dht22_due = 0;

int main(void) {
  HAL_Init();
  SystemClock_Config();
  onboard_led_init();
  i2c_init();
  tim2_init_1mhz();
  tim3_init_1min();
  init_dht22();

  u8g2_t *u8g2 = u8g2_port_init();
  dht22_t sensor;
  positions_t positions;


  positions.hi_pos_x = 10;
  positions.hi_pos_y = 22;

  positions.low_pos_x = 10;
  positions.low_pos_y = 54;

  positions.main_pos_x = 75;
  positions.main_pos_y = 38;

  sensor.hi_temp = 500;
  sensor.low_temp = 150;

  while (1) {
    if(dht22_due == 1){
      dht22_due = 0;
      get_dht22_data_struct(&sensor);
      onboard_led_toggle();
    }
    temperature_draw(u8g2, &positions, &sensor);

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

void TIM3_IRQHandler(void){
  if(TIM3->SR & TIM_SR_UIF){
    TIM3->SR &= ~TIM_SR_UIF;
    dht22_due = 1;
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
