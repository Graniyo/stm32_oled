#ifndef DHT22_H
#define DHT22_H

#include <stdint.h>

typedef struct {
  int16_t temperature;
  uint16_t humidity;
  uint16_t low_temp;
  uint16_t hi_temp;
  char temperature_text[8];
  char humidity_text[8];
  char hi_temp_text[8];
  char low_temp_text[8];
} dht22_t;

// Returverdier:
//  0 = OK
// -1 = Timeout (ingen respons fra sensor)
// -2 = Checksum feil
int get_dht22_data(int16_t *temperature, int16_t *humidity);
int get_dht22_data_struct(dht22_t *data);

void init_dht22(void);

#endif
