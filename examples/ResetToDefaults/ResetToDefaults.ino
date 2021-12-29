/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 * The SSD1306 remembers many of its settings even when powered off.
 * After experimenting with various features,
 * it can be useful to reset all the settings to the default values,
 * which can be done by uploading and running this example.
 *
 */

#include <Tiny4kOLED.h>

void setup() {
  oled.begin(128, 64, sizeof(tiny4koled_init_defaults), tiny4koled_init_defaults);
  oled.enableChargePump(); // The default is off, but most boards need this.
  oled.setRotation(1);     // The default orientation is not the most commonly used.
  oled.clear();
  oled.setFont(FONT8X16CAPSP);
  oled.setCursor(0, 0);
  oled.print("DEFAULT SETTINGS");
  oled.on();
}

void loop() {
}
