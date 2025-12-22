#define DISPLAY_ON 0xAF
#define DISPLAY_OFF 0xAE
#define INVERT 0xA7
#define COMMAND 0x00
#define OLED_ADDR_7BIT  0x3c
void i2c_init(void);
void i2c_send(void);
