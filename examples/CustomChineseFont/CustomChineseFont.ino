/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */

#include <Tiny4kOLED.h>
#include "font16x16cn.h"
// ============================================================================

void setup() {
  // put your setup code here, to run once:

  oled.begin();
  oled.clear();
  oled.setFont(&TinyOLED4kfont16x16cn);
  oled.setCursor(10, 1);
  oled.print(F("01234"));
  oled.on();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}
