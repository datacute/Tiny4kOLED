/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 * This file adds support for the I2C implementation from https://github.com/technoblogy/tiny-i2c
 *
 */
#ifndef TINY4KOLED_TINYI2C_H
#define TINY4KOLED_TINYI2C_H

#include <TinyI2CMaster.h>
#include "Tiny4kOLED_common.h"

#ifndef DATACUTE_I2C_TINYI2C
#define DATACUTE_I2C_TINYI2C

static bool datacute_write_tinyi2c(uint8_t byte) {
	return TinyI2C.write(byte);
}

static uint8_t datacute_read_tinyi2c(void) __attribute__((unused));
static uint8_t datacute_read_tinyi2c(void) {
	return TinyI2C.read();
}

static void datacute_end_read_tinyi2c(void) __attribute__((unused));
static void datacute_end_read_tinyi2c(void) {
	TinyI2C.stop();
}

static uint8_t datacute_endTransmission_tinyi2c(void) {
	TinyI2C.stop();
	return 0;
}

#endif

static bool tiny4koled_beginTransmission_tinyi2c(void) {
	return TinyI2C.start(SSD1306, 0);
}

static void tiny4koled_begin_tinyi2c(void) {
	TinyI2C.init();
#ifndef TINY4KOLED_QUICK_BEGIN
	while (!tiny4koled_beginTransmission_tinyi2c()) {
		delay(10);
	}
	datacute_endTransmission_tinyi2c();
#endif
}

#ifndef TINY4KOLED_NO_PRINT
SSD1306PrintDevice oled(&tiny4koled_begin_tinyi2c, &tiny4koled_beginTransmission_tinyi2c, &datacute_write_tinyi2c, &datacute_endTransmission_tinyi2c);
#else
SSD1306Device oled(&tiny4koled_begin_tinyi2c, &tiny4koled_beginTransmission_tinyi2c, &datacute_write_tinyi2c, &datacute_endTransmission_tinyi2c);
#endif

#endif
