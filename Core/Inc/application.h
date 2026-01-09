#include <stdint.h>
#include <u8g2.h>
#include <DHT22.h>


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
