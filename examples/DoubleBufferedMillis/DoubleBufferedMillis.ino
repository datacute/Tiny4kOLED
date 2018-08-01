/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */

#include <Tiny4kOLED.h>

// ============================================================================

void setup() {
  // Send the initialization sequence to the oled. This leaves the display turned off.
  oled.begin();
  // This example only uses a single font, so it can be set once here.
  // The characters in the 8x16 font are 8 pixels wide and 16 pixels tall.
  // 2 lines of 16 characters exactly fills 128x32.
  oled.setFont(FONT8X16);
  // Setup the first half of memory.
  initDisplay();
  // Switch the half of RAM that we are writing to, to be the half that is non currently displayed.
  oled.switchRenderFrame();
  // Setup the second half of memory.
  initDisplay();
  // Call your own display updating code.
  updateDisplay();
  // Turn on the display.
  oled.on();
}

void loop() {
  delay(50);
  updateDisplay();
}

void initDisplay() {
  // Clear whatever random data has been left in memory.
  oled.clear();
  // Position the text cursor
  // In order to keep the library size small, text can only be positioned
  // with the top of the font aligned with one of the four 8 bit high RAM pages.
  // The Y value therefore can only have the value 0, 1, 2, or 3.
  // usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  oled.setCursor(0, 1);
  // Write text to oled RAM (which is not currently being displayed).
  oled.print(F("ms:"));
}

void updateDisplay() {
  // Position the text cursor
  oled.setCursor(32, 1);
  // Write the number of milliseconds since power on.
  // The number increases, so always overwrites any stale data.
  // This means we do not need to repeatedly clear and initialize the display.
  oled.print(millis());
  // Swap which half of RAM is being written to, and which half is being displayed.
  // This is equivalent to calling both switchRenderFrame and switchDisplayFrame.
  oled.switchFrame();
}
