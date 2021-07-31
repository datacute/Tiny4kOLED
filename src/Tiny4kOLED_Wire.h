/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */
#ifndef TINY4KOLED_WIRE_H
#define TINY4KOLED_WIRE_H

#include <Wire.h>
#include "Tiny4kOLED_common.h"

#ifndef DATACUTE_I2C_WIRE
#define DATACUTE_I2C_WIRE

static bool datacute_write_wire(uint8_t byte) {
	return Wire.write(byte);
}

static uint8_t datacute_read_wire(void) __attribute__((unused));
static uint8_t datacute_read_wire(void) {
	return Wire.read();
}

static void datacute_end_read_wire(void) __attribute__((unused));
static void datacute_end_read_wire(void) {}

static uint8_t datacute_endTransmission_wire(void) {
	return Wire.endTransmission();
}

#endif

static bool tiny4koled_beginTransmission_wire(void) {
	Wire.beginTransmission(SSD1306);
	return true;
}

#ifndef TINY4KOLED_QUICK_BEGIN
inline static bool tiny4koled_check_wire(void) {
	const uint8_t noError = 0x00;
	tiny4koled_beginTransmission_wire();
	return (datacute_endTransmission_wire()==noError);
}
#endif

static void tiny4koled_begin_wire(void) {
	Wire.begin();
#ifndef TINY4KOLED_QUICK_BEGIN
	while (!tiny4koled_check_wire()) {
		delay(10);
	}
#endif
}

#ifndef TINY4KOLED_NO_PRINT
SSD1306PrintDevice oled(&tiny4koled_begin_wire, &tiny4koled_beginTransmission_wire, &datacute_write_wire, &datacute_endTransmission_wire);
#else
SSD1306Device oled(&tiny4koled_begin_wire, &tiny4koled_beginTransmission_wire, &datacute_write_wire, &datacute_endTransmission_wire);
#endif

#endif
