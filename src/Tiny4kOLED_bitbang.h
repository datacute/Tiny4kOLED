/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 * This file adds support for the I2C implementation from https://github.com/evilnick/evilOLED
 *
 */
#ifndef TINY4KOLED_BITBANG_H
#define TINY4KOLED_BITBANG_H

#include "Tiny4kOLED_common.h"

#ifndef DATACUTE_I2C_BITBANG
#define DATACUTE_I2C_BITBANG

static bool datacute_write_bitbang(uint8_t byte) {
	char i;
	for (i = 0; i < 8; i++)
	{
		if ((byte << i) & 0x80) {
			PORTB |= (1 << PB0);//digitalWrite(SDA, HIGH);
		}
		else {
			PORTB &= ~(1 << PB0);//digitalWrite(SDA, LOW);
		}
		PORTB |= (1 << PB2);//digitalWrite(SCL, HIGH);
		PORTB &= ~(1 << PB2);//digitalWrite(SCL, LOW);
	}

	PORTB |= (1 << PB0);//digitalWrite(SDA, HIGH);
	PORTB |= (1 << PB2);//digitalWrite(SCL, HIGH);
	PORTB &= ~(1 << PB2);//digitalWrite(SCL, LOW);
	return 1;
}

static uint8_t datacute_read_bitbang(void) __attribute__((unused));
static uint8_t datacute_read_bitbang(void) {
	return 0;
}

static void datacute_end_read_bitbang(void) __attribute__((unused));
static void datacute_end_read_bitbang(void) {
	PORTB &= ~(1 << PB2);//digitalWrite(SCL, LOW);
	PORTB &= ~(1 << PB0);//digitalWrite(SDA, LOW);
	PORTB |= (1 << PB2);//digitalWrite(SCL, HIGH);
	PORTB |= (1 << PB0);//digitalWrite(SDA, HIGH);
}

static uint8_t datacute_endTransmission_bitbang(void) {
	PORTB &= ~(1 << PB2);//digitalWrite(SCL, LOW);
	PORTB &= ~(1 << PB0);//digitalWrite(SDA, LOW);
	PORTB |= (1 << PB2);//digitalWrite(SCL, HIGH);
	PORTB |= (1 << PB0);//digitalWrite(SDA, HIGH);
	return 0;
}

#endif

static bool tiny4koled_beginTransmission_bitbang(void) {
	PORTB |= (1 << PB2);//digitalWrite(SCL, HIGH);
	PORTB |= (1 << PB0);//digitalWrite(SDA, HIGH);
	PORTB &= ~(1 << PB0);//digitalWrite(SDA, LOW);
	PORTB &= ~(1 << PB2);//digitalWrite(SCL, LOW);
	datacute_write_bitbang(SSD1306<<1);
	return 0;
}

static void tiny4koled_begin_bitbang(void) {
	pinMode(SCL, OUTPUT);
	pinMode(SDA, OUTPUT);
}


#ifndef TINY4KOLED_NO_PRINT
SSD1306PrintDevice oled(&tiny4koled_begin_bitbang, &tiny4koled_beginTransmission_bitbang, &datacute_write_bitbang, &datacute_endTransmission_bitbang);
#else
SSD1306Device oled(&tiny4koled_begin_bitbang, &tiny4koled_beginTransmission_bitbang, &datacute_write_bitbang, &datacute_endTransmission_bitbang);
#endif

#endif
