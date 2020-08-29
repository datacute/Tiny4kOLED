/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 * This example shows how the scrolling features work.
 * When scrolling, the double buffering of screens cannot be used.
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

void setup() {
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

  // Two fonts are supplied with this library, FONT8X16 and FONT6X8
  // Other fonts are available from the TinyOLED-Fonts library
  oled.setFont(FONT6X8);
  oled.clear();
  oled.on();
}

void loop() {

    oled.setCursor(0, 0);
    oled.invertOutput(false);
    oled.println("Normal");
    delay(500);

    oled.invertOutput(true);
    oled.print("Inverse");
    oled.clearToEOL();
    delay(500);

    for (;;)
    {
        oled.setCursor(0, 2);
        oled.invertOutput(true);
        oled.print(" ");

        delay(500);

        oled.setCursor(0, 2);
        oled.invertOutput(false);
        oled.print(" ");

        delay(500);
    }
}
