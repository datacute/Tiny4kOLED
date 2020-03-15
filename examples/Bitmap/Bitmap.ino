/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */

// Choose your I2C implementation before including Tiny4kOLED.h
// The default is selected is Wire.h

// To use the Wire library:
//#include <Wire.h>

// To use the Adafruit's TinyWireM library:
//#include <TinyWireM.h>

// To use the TinyI2C library from https://github.com/technoblogy/tiny-i2c
//#include <TinyI2CMaster.h>

// The blue OLED screen requires a long initialization on power on.
// The code to wait for it to be ready uses 20 bytes of program storage space
// If you are using a white OLED, this can be reclaimed by uncommenting
// the following line (before including Tiny4kOLED.h):
//#define TINY4KOLED_QUICK_BEGIN

#include <Tiny4kOLED.h>
#include "SolomonSystech.h"

// ============================================================================

void setup() {
  // put your setup code here, to run once:

  oled.begin();

  // Two rotations are supported,
  // The begin() method sets the rotation to 1.
  //oled.setRotation(0);

  // Some newer devices do not contain an external current reference.
  // Older devices may also support using the internal curret reference,
  // which provides more consistent brightness across devices.
  // The internal current reference can be configured as either low current, or high current.
  // Using true as the parameter value choses the high current internal current reference,
  // resulting in a brighter display, and a more effective contrast setting.
  //oled.setInternalIref(true);

  // This example does not use double buffering.

  // To save space, the bitmap is cropped left and right,
  // intended to be drawn onto a clear screen
  oled.clear();

  // The display will show two bitmaps,
  // one of SOLOMON SYSTECH's logo
  // one of the text SOLOMON SYSTECH
  // The logo bitmap is 37 pixels wide
  // The text bitmap is 69 pixels wide
  // Both are 32 pixels (4 pages) high

  oled.bitmap(6, 0, 6 + 37, 4, solomon_systech_logo_bitmap);
  oled.bitmap(54, 0, 54 + 69, 4, solomon_systech_text_bitmap);

  // Now that the display is all setup, turn on the display
  oled.on();
}

void loop() {
  // This example only shows a static image on the display.
  // The microcontroller could be turned off now.
}
