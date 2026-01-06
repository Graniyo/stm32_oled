#ifndef DHT22_H
#define DHT22_H

#include <stdint.h>

typedef struct {
 int16_t temperature;
 uint16_t humidity;
}dht22;

// Returverdier:
//  0 = OK
// -1 = Timeout (ingen respons fra sensor)
// -2 = Checksum feil
int get_dht22_data(int16_t *temperature, int16_t *humidity);
int get_dht22_data_struct(dht22 *data);

void init_dht22(void);

#endif
