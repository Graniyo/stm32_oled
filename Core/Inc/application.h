#include <stdint.h>
#include <u8g2.h>
#include <DHT22.h>
#include <hdc1080.h>


typedef struct{
  uint8_t hi_pos_x;
  uint8_t hi_pos_y;

  uint8_t low_pos_x;
  uint8_t low_pos_y;

  uint8_t main_pos_x;
  uint8_t main_pos_y;
} positions_t;


void temperature_draw( u8g2_t *u8g2, positions_t *box, dht22_t *dht22);
void convert_string(dht22_t *dht);
void check_hi_low(dht22_t *dht22);

void hdc1080_temperature_draw(u8g2_t *u8g2, positions_t *box, hdc1080_t *hdc);
void hdc1080_convert_string(hdc1080_t *hdc);
void hdc1080_check_hi_low(hdc1080_t *hdc);
