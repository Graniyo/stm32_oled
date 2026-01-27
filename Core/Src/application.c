#include "application.h"
#include "DHT22.h"
#include "u8g2.h"
#include <stdint.h>
#include <stdio.h>

#define WIDTH 128
#define HEIGHT 64 


void temperature_draw( u8g2_t *u8g2, positions_t *box, dht22_t *dht22){
  convert_string(dht22);
  u8g2_SetFont(u8g2,u8g2_font_helvB12_tr );
  u8g2_ClearBuffer(u8g2);
  u8g2_DrawStr(u8g2, box->hi_pos_x, box->hi_pos_y, dht22->hi_temp_text);
  u8g2_DrawStr(u8g2, box->low_pos_x, box->low_pos_y,dht22->low_temp_text);
  u8g2_DrawStr(u8g2, box->main_pos_x, box->main_pos_y, dht22->temperature_text);
  u8g2_SendBuffer(u8g2);
}

void convert_string(dht22_t *dht) {
  int16_t temp = dht->temperature;
  int16_t hum = dht->humidity;
  int16_t high_temp = dht->hi_temp;
  int16_t low_temp = dht->low_temp;

  if (temp < 0) {
    snprintf(dht->temperature_text, sizeof(dht->temperature_text), "-%d.%d", (-temp) / 10, (-temp) % 10);
  } else {
    snprintf(dht->temperature_text, sizeof(dht->temperature_text), "%d.%d", temp / 10, temp % 10);
  }

  snprintf(dht->humidity_text, sizeof(dht->humidity_text), "%d.%d", hum / 10, hum % 10);

  snprintf(dht->hi_temp_text, sizeof(dht->hi_temp_text), "%d.%d", high_temp / 10, high_temp % 10);
  snprintf(dht->low_temp_text, sizeof(dht->low_temp_text), "%d.%d", low_temp / 10, low_temp % 10);
}

void check_hi_low(dht22_t *dht22){

  if(!dht22->initalized){
    dht22->hi_temp = dht22->temperature;
    dht22->low_temp = dht22->temperature;
    dht22->initalized = 1;
  }

  if (dht22->temperature > dht22->hi_temp) {
    dht22->hi_temp = dht22->temperature;
  }

  if (dht22->temperature < dht22->low_temp) {
    dht22->low_temp = dht22->temperature;
  }
}










