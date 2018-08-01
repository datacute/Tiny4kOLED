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

static void tinyi2c_begin(void) {
	TinyI2C.init();
}

static void tinyi2c_send_start(uint8_t i2c_address) {
	TinyI2C.start(i2c_address, 0);
}

static bool tinyi2c_send_byte(uint8_t byte) {
	return TinyI2C.write(byte);
}

static void tinyi2c_send_stop(void) {
	TinyI2C.stop();
}


#include "Tiny4kOLED_common.h"
SSD1306Device oled(SSD1306, &tinyi2c_begin, &tinyi2c_send_start, &tinyi2c_send_byte, &tinyi2c_send_stop);

#endif
