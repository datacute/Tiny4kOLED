/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Copyright (c) 2024 BogDan Vatra
 *
 * This file adds support for the I2C implementation for RP2040
 *
 */
#ifndef TINY4KOLED_RP2040
#define TINY4KOLED_RP2040

#include "Tiny4kOLED_common.h"
#include <hardware/i2c.h>
#include <hardware/gpio.h>
#include <vector>

#ifndef SSD1306_I2C_CLK
#define SSD1306_I2C_CLK 400
#endif

#ifndef SSD1306_I2C_SDA
#warning "SSD1306_I2C_SDA not defined, defaulting to PICO_DEFAULT_I2C_SCL_PIN"
#define SSD1306_I2C_SDA PICO_DEFAULT_I2C_SDA_PIN
#endif

#ifndef SSD1306_I2C_SCL
#warning "SSD1306_I2C_SCL not defined, defaulting to PICO_DEFAULT_I2C_SCL_PIN"
#define SSD1306_I2C_SCL PICO_DEFAULT_I2C_SCL_PIN
#endif

#define SSD1306_I2C_ADDR _u(0x3C)

static std::vector<uint8_t> i2c_buffer;
static bool datacute_write_rp2040(uint8_t byte) {
    i2c_buffer.push_back(byte);
    return true;
}

static uint8_t datacute_endTransmission_rp2040(void)
{
    if (i2c_buffer.empty())
        return 0;
    auto res = i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, i2c_buffer.data(), i2c_buffer.size(), false);
    return res == int(i2c_buffer.size()) ? 0 : 1;
}

static bool tiny4koled_beginTransmission_rp2040(void)
{
    i2c_buffer.clear();
    return true;
}

static void tiny4koled_begin_rp2040(void) {
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    gpio_set_function(SSD1306_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_I2C_SDA);
    gpio_pull_up(SSD1306_I2C_SCL);
    i2c_buffer.reserve(32);
}

#ifndef TINY4KOLED_NO_PRINT
SSD1306PrintDevice oled(&tiny4koled_begin_rp2040, &tiny4koled_beginTransmission_rp2040, &datacute_write_rp2040, &datacute_endTransmission_rp2040);
#else
SSD1306Device oled(&tiny4koled_begin_rp2040, &tiny4koled_beginTransmission_rp2040, &datacute_write_rp2040, &datacute_endTransmission_rp2040);
#endif

#endif // TINY4KOLED_RP2040_RP2040
