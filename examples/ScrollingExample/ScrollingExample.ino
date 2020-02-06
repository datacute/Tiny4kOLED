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

#include <Tiny4kOLED.h>

void setup() {
  oled.begin();
  oled.setFont(FONT6X8);

  // To cear all the memory, clear both rendering frames:
  oled.clear();
  oled.switchRenderFrame();
  oled.clear();
  oled.switchRenderFrame();

  // The SSD1306 refers to 8 rows of pixels as a 'page'

  // Page 0: One line of text at the top that is not scrolling
  oled.setCursor(13,0); // 13 pixels to center the following text
  oled.print(F("Scrolling Example")); // 17 characters x 6 pixels = 102 pixels

  // Page 1: One lines of graphics
  // For this example, the scrolled area is set up below to start after 11 rows.
  // The top three rows of pixels in this page are not scrolling
  // The bottom five rows of pixels are scrolling vertically
  oled.setCursor(0,1);
  oled.fillToEOL(0x02);

  // Pages 2 to 6: Five lines of text that are scrolling vertically
  oled.setCursor(0,2);
  oled.print(F("Top eleven rows fixed"));
  oled.setCursor(0,3);
  oled.print(F("Fifty rows vertically"));
  oled.setCursor(0,4);
  oled.print(F("+ 3 rows horizontally"));
  oled.setCursor(0,5);
  oled.print(F("Tiny4kOLED example by"));
  oled.setCursor(80,6);
  oled.print(F("Datacute"));

  // Page 7: Graphics at the bottom.
  // This page of 8 rows of pixels is all scrolling horizontally
  // For this example, the vertical scroll only extends down 5 rows into this page.
  // The effect is that it looks like only 5 rows are scrolling horizontally.
  // Consequently none of the data bytes below are larger than 0x1F
  oled.setCursor(0,7);
  oled.startData();
  for (uint8_t i = 0; i < 16; i++) {
    oled.sendData(0x02);
    oled.sendData(0x02);
    oled.sendData(0x02);
    oled.sendData(0x0C);
    oled.sendData(0x10);
    oled.sendData(0x10);
    oled.sendData(0x10);
    oled.sendData(0x0C);
  }
  oled.endData();

  // Scroll the bottom graphics on page 7
  // 'interval' is how quickly it scrolls.
  // 'offset' determines how far to scroll vertically for each horizontal movement
  // startPage = 7, interval = 1, endPage = 7, offset = 1
  oled.scrollLeftOffset(7,1,7,1);
  // 11 rows in the top fixed (non-scrolling) area
  // Vertically scroll 50 rows
  // The remaining 3 rows are not included in the scroll, and are never visible.
  oled.setVerticalScrollArea(11, 50);
  oled.activateScroll();
  oled.on();
}

void loop() {
  // The scrolling happens automatically, and is performed by rearranging the RAM contents.
}
