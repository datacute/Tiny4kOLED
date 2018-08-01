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

static void tinywirem_begin(void) {
	TinyWireM.begin();
}

static void tinywirem_beginTransmission(uint8_t i2c_address) {
	TinyWireM.beginTransmission(i2c_address);
}

static bool tinywirem_write(uint8_t byte) {
	return TinyWireM.write(byte);
}

static void tinywirem_endTransmission(void) {
	TinyWireM.endTransmission();
}


#include "Tiny4kOLED_common.h"
SSD1306Device oled(SSD1306, &tinywirem_begin, &tinywirem_beginTransmission, &tinywirem_write, &tinywirem_endTransmission);

#endif
