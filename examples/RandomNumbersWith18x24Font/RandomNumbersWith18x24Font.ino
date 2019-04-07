/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */

#include <Tiny4kOLED.h>
#include "font18x24digits.h"

void setup() {
  oled.begin();               // Send the initialization sequence to the oled. This leaves the display turned off
  oled.clear();               // Clear the memory before turning on the display
  oled.on();                  // Turn on the display
  oled.switchRenderFrame();   // Switch the half of RAM that we are writing to, to be the half that is non currently displayed
}

void loop() {
  delay(50);
  updateDisplay();
}

void updateDisplay() {
  int value = random(1000, 9999);
  oled.setCursor(28, 0);
  oled.setFont(&TinyOLED4kfont18x24Digits);
  oled.print(value);
  oled.setCursor(7, 3);
  oled.setFont(FONT6X8);
  oled.print(F("This text is small."));
  oled.switchFrame();             // Swap which half of RAM is being written to, and which half is being displayed. This is equivalent to calling both switchRenderFrame and switchDisplayFrame.
  oled.clear();                   // Clear the half of memory not currently being displayed.
}
