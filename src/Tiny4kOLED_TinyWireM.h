/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */
#ifndef TINY4KOLED_TINYWIREM_H
#define TINY4KOLED_TINYWIREM_H

#include <TinyWireM.h>  // Version with buffer bugfix: https://github.com/adafruit/TinyWireM
#include "Tiny4kOLED_common.h"

static bool tinywirem_beginTransmission(void);
static uint8_t tinywirem_endTransmission(void);

#ifndef TINY4KOLED_QUICK_BEGIN
static bool check (void) {
	const uint8_t noError = 0x00;
	tinywirem_beginTransmission();
	return (tinywirem_endTransmission()==noError);
}
#endif

static void tinywirem_begin(void) {
	TinyWireM.begin();
#ifndef TINY4KOLED_QUICK_BEGIN
	while (!check()) {
		delay(10);
	}
#endif
}

static bool tinywirem_beginTransmission(void) {
	TinyWireM.beginTransmission(SSD1306);
	return true;
}

static bool tinywirem_write(uint8_t byte) {
	return TinyWireM.write(byte);
}

static uint8_t tinywirem_endTransmission(void) {
	return TinyWireM.endTransmission();
}

SSD1306Device oled(&tinywirem_begin, &tinywirem_beginTransmission, &tinywirem_write, &tinywirem_endTransmission);

#endif
