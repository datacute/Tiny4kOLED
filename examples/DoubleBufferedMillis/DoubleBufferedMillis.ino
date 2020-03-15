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

// This example simply shows the number of milliseconds since reset
// To show the double-buffering effect, the location where the timer
// is displayed moves left and right, and the screen is completely cleared.

// The number increases, so if the location was not moving,
// we would not need to repeatedly clear and initialize the display.
// But we are doing so to demonstrate the capability of the double
// buffering code to result in a smooth animation

// Uncomment this line to disable the code performing the double buffering
//#define NO_DOUBLE_BUFFERING

uint8_t location = 0;
bool leftToRight = true;

void setup() {
  // Send the initialization sequence to the oled. This leaves the display turned off.
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
  // This example only uses a single font, so it can be set once here.
  // The characters in the 8x16 font are 8 pixels wide and 16 pixels tall.
  // 2 lines of 16 characters exactly fills 128x32.
  oled.setFont(FONT8X16);
  // Setup the first half of memory.
  updateDisplay();
#ifndef NO_DOUBLE_BUFFERING
  // Switch the half of RAM that we are writing to, to be the half that is non currently displayed.
  oled.switchRenderFrame();
  // Setup the second half of memory.
  updateDisplay();
  // Switch back to being ready to render on the first frame while displaying the second frame.
  oled.switchFrame();
#endif
  // Turn on the display.
  oled.on();
}

void loop() {
  delay(50);
  if (leftToRight) {
    location++;
    if (location == 60) {
      leftToRight = false;
    }
  } else {
    location--;
    if (location == 0) {
      leftToRight = true;
    }
  }
  updateDisplay();
#ifndef NO_DOUBLE_BUFFERING
  // Swap which half of RAM is being written to, and which half is being displayed.
  // This is equivalent to calling both switchRenderFrame and switchDisplayFrame.
  // To see the benefit of double buffering, try this code again with this line commented out.
  oled.switchFrame();
#endif
}

void updateDisplay() {
  // Clear whatever random data has been left in memory.
  oled.clear();
  // Position the text cursor
  // In order to keep the library size small, text can only be positioned
  // with the top of the font aligned with one of the four 8 bit high RAM pages.
  // The Y value therefore can only have the value 0, 1, 2, or 3.
  // usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  oled.setCursor(location, 0);
  // Write text to oled RAM.
  oled.print(F("ms:"));
  // Write the number of milliseconds since power on.
  oled.print(millis());
  // Write it again
  oled.setCursor(location, 2);
  oled.print(F("ms:"));
  oled.print(millis());
}
