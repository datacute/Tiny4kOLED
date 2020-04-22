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

// The F macro is not able to be used outside of methods.
// The following two lines place a string in flash memory,
// and get a reference to it as though it had been created as
// F("This is an example of scrolling text. ")
// while also allowing sizeof(textToScrollData) to return
// the length of the string (including the null terminating character).
const char textToScrollData[] PROGMEM = { "This is an example of scrolling text. " };
DATACUTE_F_MACRO_T * textToScroll = FPSTR(textToScrollData);

uint16_t nextRowOfTextToDraw;

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
  oled.setFont(FONT8X16);

  // This example does not use the double buffering ability of 128x32 OLED screens
  // so the display should be set up, rather than just cleared, before turing on
  setupInitialDisplay();

  // If the text to scroll needs to start on-screen, it can be initialized,
  // clipped to the scrolling window
  oled.setCursor(5, 1);
  // Start drawing the text from the beginning pixel, clip at 118 pixels.
  oled.clipText(0, 118, textToScroll);
  nextRowOfTextToDraw = 118;

  oled.on();
}

void loop() {
  // Content scrolling is controlled by the microcontroller.
  // Each request to scroll content results in the content shifting one pixel left or right

  // The parameters are start page, end page, start column, end column
  oled.scrollContentLeft(1, 2, 5, 122);

  // However the scroll happens after the command is given, and a delay is required before
  // the edge of the scrolled content can be safely overwritten.
  // The length of the delay is 2 divided by the refresh rate.
  // So if the refresh rate is 100 Hz, then the delay required is 20ms.
  // Unfortunately the refresh rate is not easy to determine.
  // The SSD1306 is capable of refresh rates between 4 and 489 Hz.
  // The default refresh rate for a 128x32 screen is 215.5 Hz, requiring a delay of 9.3ms
  // The default refresh rate for a 128x64 screen is 107.8 Hz, requiring a delay of 18.6ms
  delay(10);

  // Draw one column of the text at the right hand end of the scrolling window.
  oled.setCursor(122, 1);
  oled.clipText(nextRowOfTextToDraw++, 1, textToScroll);

  // Wrap the text...
  // sizeof(textToScrollData) is 1 byte longer than the text
  // (in includes the end of string marker)
  // The font width is 8 pixels
  if (nextRowOfTextToDraw >= (sizeof(textToScrollData) - 1) * 8) {
    nextRowOfTextToDraw = 0;
  }

  // Fastest scrolling speed is slightly too quick, lets add another delay:
  delay(20);
}

void setupInitialDisplay() {
  // Draw a border around the screen, to demonstrate scrolling the region within it.
  oled.setCursor(0, 0);
  oled.startData();
  oled.sendData(0xFF);
  oled.sendData(0xFF);
  oled.sendData(0x7F);
  oled.sendData(0x3F);
  oled.sendData(0x1F);
  oled.sendData(0x0F);
  oled.sendData(0x07);
  oled.repeatData(0x03, 128-14);
  oled.sendData(0x07);
  oled.sendData(0x0F);
  oled.sendData(0x1F);
  oled.sendData(0x3F);
  oled.sendData(0x7F);
  oled.sendData(0xFF);
  oled.sendData(0xFF);
  oled.endData();

  oled.setCursor(0, 1);
  oled.startData();
  oled.sendData(0xFF);
  oled.sendData(0xFF);
  oled.repeatData(0x00, 128-4);
  oled.sendData(0xFF);
  oled.sendData(0xFF);
  oled.endData();

  oled.setCursor(0, 2);
  oled.startData();
  oled.sendData(0xFF);
  oled.sendData(0xFF);
  oled.repeatData(0x00, 128-4);
  oled.sendData(0xFF);
  oled.sendData(0xFF);
  oled.endData();

  oled.setCursor(0, 3);
  oled.startData();
  oled.sendData(0xFF);
  oled.sendData(0xFF);
  oled.sendData(0xFE);
  oled.sendData(0xFC);
  oled.sendData(0xF8);
  oled.sendData(0xF0);
  oled.sendData(0xE0);
  oled.repeatData(0xC0, 128-14);
  oled.sendData(0xE0);
  oled.sendData(0xF0);
  oled.sendData(0xF8);
  oled.sendData(0xFC);
  oled.sendData(0xFE);
  oled.sendData(0xFF);
  oled.sendData(0xFF);
  oled.endData();
}
