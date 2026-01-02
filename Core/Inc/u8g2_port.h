#pragma once

#include "u8g2.h"

u8g2_t* u8g2_port_init(void);
void u8g2_port_clear(void);
void u8g2_port_draw_test(void);
void u8g2_port_update(void);
void u8g2_port_test_pattern(void);
