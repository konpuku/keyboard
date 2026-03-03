#pragma once

#include_next <mcuconf.h>

// Enable I2C1 for OLED display (GP2 = SDA, GP3 = SCL)
#undef RP_I2C_USE_I2C1
#define RP_I2C_USE_I2C1 TRUE
