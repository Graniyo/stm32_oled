#include <stdint.h>

#define DISPLAY_ON 0xAF
#define DISPLAY_OFF 0xAE
#define INVERT 0xA7
#define COMMAND 0x00
#define OLED_ADDR_7BIT 0x3c

void i2c_init(void);
void clear_flags();
void i2c_send(uint8_t address, uint8_t data[], uint8_t length);
