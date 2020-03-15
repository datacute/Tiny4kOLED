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
// This example compiles to 4402 bytes of program storage space
// and 88 bytes of dynamic memory.
//#include <Wire.h>

// To use the Adafruit's TinyWireM library:
// (Saves about 350 bytes and 20 bytes of RAM over Wire.h)
// (If you see a strange dot pattern then upgrade the TinyWireM
//  library to get the buffer overflow fix.)
//#include <TinyWireM.h>

// To use the TinyI2C library from https://github.com/technoblogy/tiny-i2c
// (Saves about 570 bytes and 40 bytes of RAM over Wire.h)
//#include <TinyI2CMaster.h>

// The blue OLED screen requires a long initialization on power on.
// The code to wait for it to be ready uses 20 bytes of program storage space
// If you are using a white OLED, this can be reclaimed by uncommenting
// the following line (before including Tiny4kOLED.h):
//#define TINY4KOLED_QUICK_BEGIN

#include <Tiny4kOLED.h>

// ============================================================================

void setup() {
  // Put your own setup code here, to run once.

  // Send the initialization sequence to the oled. This leaves the display turned off
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

  // Clear the memory before turning on the display
  oled.clear();

  // Turn on the display
  oled.on();

  // Switch the half of RAM that we are writing to, to be the half that is non currently displayed
  oled.switchRenderFrame();
}

void loop() {
  // put your main code here, to run repeatedly:

  /* ------------------------------
   * Show screen with all pixels ON
   * ------------------------------
   */

  // Fill screen with color
  // As we're setting every pixel, there is no need to clear the previous contents.
  oled.fill(0xFF);

  // Swap which half of RAM is being written to, and which half is being displayed
  // This is equivalent to calling both switchRenderFrame and switchDisplayFrame
  oled.switchFrame();

  delay(1000);

  /* -------------------------------
   * Show screen with all pixels OFF
   * -------------------------------
   */

  // Clear the non-displayed half of the memory to all black
  oled.clear();

  // Swap which half of RAM is being written to, and which half is being displayed
  oled.switchFrame();

  delay(1000);

  /* -----------------------------------------
   * Show screen with two different font sizes
   * -----------------------------------------
   */

  // Clear the non-displayed half of the memory to all black
  // (The previous clear only cleared the other half of RAM)
  oled.clear();

  // The characters in the 8x16 font are 8 pixels wide and 16 pixels tall
  // 2 lines of 16 characters exactly fills 128x32
  oled.setFont(FONT8X16);

  // Position the cusror
  // usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  oled.setCursor(32, 0);

  // Write the text to oled RAM (which is not currently being displayed)
  // Wrap strings in F() to save RAM!
  oled.print(F("Datacute"));

  // The characters in the 6x8 font are 6 pixels wide and 8 pixels tall
  // 4 lines of 21 characters only fills 126x32
  oled.setFont(FONT6X8);

  // Position the cusror
  // Two rows down because the 8x16 font used for the last text takes two rows of 8 pixels
  oled.setCursor(13, 2);

  // Write the text to oled RAM (which is not currently being displayed)
  oled.print(F("Acute Information"));

  // Position the cusror
  // Cursor X is in pixels, and does not need to be a multiple of the font width
  oled.setCursor(16, 3);

  // Write the text to oled RAM (which is not currently being displayed)
  oled.print(F("Revelation Tools"));

  // Swap which half of RAM is being written to, and which half is being displayed
  oled.switchFrame();

  delay(3000);
}
