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
// ============================================================================

void setup() {
  // put your setup code here, to run once:

  // This example is for a 128x64 screen
  oled.begin(128, 64, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);

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

  oled.clear();
  normalSize();
  doubleSize();
  oled.on();
}

void loop() {
  delay(10000);
}

void normalSize() {
  oled.setFont(FONT6X8P);
  oled.setCursor(0, 1);
  oled.print("Normal Size");

  oled.setFont(FONT8X16P);
  oled.setCursor(64, 0);
  oled.print("Datacute");
}

void doubleSize() {
  oled.setFontX2(FONT6X8P);
  oled.setCursor(0, 2);
  oled.print("Double Size");

  oled.setFontX2(FONT8X16P);
  oled.setCursor(0, 4);
  oled.print("Datacute");
}
