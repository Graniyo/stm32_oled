#include "hdc1080.h"
#include "i2c.h"
#include "delay.h"

// Conversion time for 14-bit resolution (both temp and humidity)
#define HDC1080_CONVERSION_TIME_MS 15

int hdc1080_init(void) {
  // Verify device ID
  uint16_t device_id = hdc1080_read_device_id();
  if (device_id != HDC1080_DEVICE_ID_VALUE) {
    return -1;
  }

  // Configure: acquisition mode = both temp and humidity, 14-bit resolution
  uint8_t config_data[3];
  config_data[0] = HDC1080_REG_CONFIG;
  uint16_t config = HDC1080_CONFIG_MODE | HDC1080_CONFIG_TRES_14 | HDC1080_CONFIG_HRES_14;
  config_data[1] = (config >> 8) & 0xFF;
  config_data[2] = config & 0xFF;

  i2c_send(HDC1080_ADDR, config_data, 3);

  return 0;
}

int16_t hdc1080_read_temperature(void) {
  uint8_t raw[2];

  // Trigger temperature measurement
  i2c_write_reg(HDC1080_ADDR, HDC1080_REG_TEMPERATURE);

  // Wait for conversion
  delay_ms(HDC1080_CONVERSION_TIME_MS);

  // Read result
  i2c_read(HDC1080_ADDR, raw, 2);

  // Convert to temperature in tenths of degrees Celsius
  // Formula: T = (raw / 65536) * 165 - 40
  // For tenths: T = (raw * 1650 / 65536) - 400
  uint16_t raw_temp = ((uint16_t)raw[0] << 8) | raw[1];
  int32_t temp = ((int32_t)raw_temp * 1650 / 65536) - 400;

  return (int16_t)temp;
}

int16_t hdc1080_read_humidity(void) {
  uint8_t raw[2];

  // Trigger humidity measurement
  i2c_write_reg(HDC1080_ADDR, HDC1080_REG_HUMIDITY);

  // Wait for conversion
  delay_ms(HDC1080_CONVERSION_TIME_MS);

  // Read result
  i2c_read(HDC1080_ADDR, raw, 2);

  // Convert to humidity in tenths of percent
  // Formula: H = (raw / 65536) * 100
  // For tenths: H = (raw * 1000 / 65536)
  uint16_t raw_hum = ((uint16_t)raw[0] << 8) | raw[1];
  int32_t hum = (int32_t)raw_hum * 1000 / 65536;

  return (int16_t)hum;
}

int hdc1080_read(hdc1080_t *data) {
  uint8_t raw[4];

  // Trigger measurement (both temp and humidity in sequence mode)
  i2c_write_reg(HDC1080_ADDR, HDC1080_REG_TEMPERATURE);

  // Wait for conversion
  delay_ms(HDC1080_CONVERSION_TIME_MS);

  // Read both temperature and humidity (4 bytes)
  i2c_read(HDC1080_ADDR, raw, 4);

  // Convert temperature
  uint16_t raw_temp = ((uint16_t)raw[0] << 8) | raw[1];
  int32_t temp = ((int32_t)raw_temp * 1650 / 65536) - 400;
  data->temperature = (int16_t)temp;

  // Convert humidity
  uint16_t raw_hum = ((uint16_t)raw[2] << 8) | raw[3];
  int32_t hum = (int32_t)raw_hum * 1000 / 65536;
  data->humidity = (uint16_t)hum;

  return 0;
}

uint16_t hdc1080_read_mfr_id(void) {
  uint8_t raw[2];

  i2c_write_reg(HDC1080_ADDR, HDC1080_REG_MFR_ID);
  delay_ms(1);
  i2c_read(HDC1080_ADDR, raw, 2);

  return ((uint16_t)raw[0] << 8) | raw[1];
}

uint16_t hdc1080_read_device_id(void) {
  uint8_t raw[2];

  i2c_write_reg(HDC1080_ADDR, HDC1080_REG_DEVICE_ID);
  delay_ms(1);
  i2c_read(HDC1080_ADDR, raw, 2);

  return ((uint16_t)raw[0] << 8) | raw[1];
}
