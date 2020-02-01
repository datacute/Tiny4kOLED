/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */
#ifndef TINY4KOLED_TINYI2C_H
#define TINY4KOLED_TINYI2C_H

#include <TinyI2CMaster.h>
#include "Tiny4kOLED_common.h"

static bool tinyi2c_send_start(void);
static uint8_t tinyi2c_send_stop(void);

static void tinyi2c_begin(void) {
	TinyI2C.init();
#ifndef TINY4KOLED_QUICK_BEGIN
	while (!tinyi2c_send_start()) {
		delay(10);
	}
	tinyi2c_send_stop();
#endif
}

static bool tinyi2c_send_start(void) {
	return TinyI2C.start(SSD1306, 0);
}

static bool tinyi2c_send_byte(uint8_t byte) {
	return TinyI2C.write(byte);
}

static uint8_t tinyi2c_send_stop(void) {
	TinyI2C.stop();
	return 0;
}

SSD1306Device oled(&tinyi2c_begin, &tinyi2c_send_start, &tinyi2c_send_byte, &tinyi2c_send_stop);

#endif
