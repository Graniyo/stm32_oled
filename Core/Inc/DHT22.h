#ifndef DHT22_H
#define DHT22_H

#include <stdint.h>

// Returverdier:
//  0 = OK
// -1 = Timeout (ingen respons fra sensor)
// -2 = Checksum feil
int get_dht22_data(int16_t *temperature, int16_t *humidity);

void init_dht22(void);

#endif
