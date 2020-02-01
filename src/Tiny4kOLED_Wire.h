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

static bool wire_beginTransmission(void);
static uint8_t wire_endTransmission(void);

#ifndef TINY4KOLED_QUICK_BEGIN
inline static bool check (void) {
	const uint8_t noError = 0x00;
	wire_beginTransmission();
	return (wire_endTransmission()==noError);
}
#endif

static void wire_begin(void) {
	Wire.begin();
#ifndef TINY4KOLED_QUICK_BEGIN
	while (!check()) {
		delay(10);
	}
#endif
}

static bool wire_beginTransmission(void) {
	Wire.beginTransmission(SSD1306);
	return true;
}

static bool wire_write(uint8_t byte) {
	return Wire.write(byte);
}

static uint8_t wire_endTransmission(void) {
	return Wire.endTransmission();
}

SSD1306Device oled(&wire_begin, &wire_beginTransmission, &wire_write, &wire_endTransmission);

#endif
