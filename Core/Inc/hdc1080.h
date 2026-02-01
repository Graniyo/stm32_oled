#ifndef HDC1080_H
#define HDC1080_H

#include <stdint.h>

// HDC1080 I2C address (7-bit)
#define HDC1080_ADDR 0x40

// HDC1080 registers
#define HDC1080_REG_TEMPERATURE 0x00
#define HDC1080_REG_HUMIDITY    0x01
#define HDC1080_REG_CONFIG      0x02
#define HDC1080_REG_SERIAL_ID1  0xFB
#define HDC1080_REG_SERIAL_ID2  0xFC
#define HDC1080_REG_SERIAL_ID3  0xFD
#define HDC1080_REG_MFR_ID      0xFE
#define HDC1080_REG_DEVICE_ID   0xFF

// Configuration register bits
#define HDC1080_CONFIG_RST      (1 << 15)  // Software reset
#define HDC1080_CONFIG_HEAT     (1 << 13)  // Heater enable
#define HDC1080_CONFIG_MODE     (1 << 12)  // Acquisition mode (0=temp or hum, 1=both)
#define HDC1080_CONFIG_BTST     (1 << 11)  // Battery status
#define HDC1080_CONFIG_TRES_14  (0 << 10)  // Temperature resolution 14 bit
#define HDC1080_CONFIG_TRES_11  (1 << 10)  // Temperature resolution 11 bit
#define HDC1080_CONFIG_HRES_14  (0 << 8)   // Humidity resolution 14 bit
#define HDC1080_CONFIG_HRES_11  (1 << 8)   // Humidity resolution 11 bit
#define HDC1080_CONFIG_HRES_8   (2 << 8)   // Humidity resolution 8 bit

// Expected ID values
#define HDC1080_MFR_ID_VALUE    0x5449  // "TI"
#define HDC1080_DEVICE_ID_VALUE 0x1050

typedef struct {
  int16_t temperature;    // Temperature in tenths of degrees Celsius
  uint16_t humidity;      // Humidity in tenths of percent
  uint16_t low_temp;
  uint16_t hi_temp;
  char temperature_text[8];
  char humidity_text[8];
  char hi_temp_text[8];
  char low_temp_text[8];
  uint8_t initialized;
} hdc1080_t;

// Initialize HDC1080 sensor
// Returns 0 on success, -1 on failure
int hdc1080_init(void);

// Read temperature only
// Returns temperature in tenths of degrees Celsius, or INT16_MIN on error
int16_t hdc1080_read_temperature(void);

// Read humidity only
// Returns humidity in tenths of percent, or -1 on error
int16_t hdc1080_read_humidity(void);

// Read both temperature and humidity
// Returns 0 on success, -1 on failure
int hdc1080_read(hdc1080_t *data);

// Read manufacturer ID (should return 0x5449)
uint16_t hdc1080_read_mfr_id(void);

// Read device ID (should return 0x1050)
uint16_t hdc1080_read_device_id(void);

#endif
