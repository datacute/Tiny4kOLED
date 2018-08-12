/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */
#ifndef TINY4KOLED_H
#define TINY4KOLED_H

#if defined(TinyWireM_h)
#include "Tiny4kOLED_TinyWireM.h"
#elif defined(TinyI2CMaster_h)
#include "Tiny4kOLED_tiny-i2c.h"
#elif defined(TwoWire_h)
#include "Tiny4kOLED_Wire.h"
#else
#include "Tiny4kOLED_TinyWireM.h"
#endif

#endif
