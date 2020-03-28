/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 * This example shows a full screen rectangle,
 * writes the rectangle size inside the rectangle,
 * and scrolls the size off the screen.
 *
 */

#include <Tiny4kOLED.h>

uint8_t width = 128;
uint8_t height = 64;

void setup() {
  oled.begin(width, height, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
  oled.setFont(FONT8X16);
  drawScreen();
  oled.on();
}

void loop() {
  scrollScreen();
}

void drawScreen() {
  // Set entire memory to hatched - if you see any of this hatching, then the display is not initialised correctly.
  for (uint8_t y = 0; y < 8; y++) {
    oled.setCursor(0, y);
    oled.startData();
    for (uint8_t x=0; x<128; x += 2) {
      oled.sendData(0b10101010);
      oled.sendData(0b01010101);
    }
    oled.endData();
  }

  oled.setCursor(0, 0);
  oled.startData();
  oled.sendData(0b11111111);
  oled.repeatData(0b00000001, width - 2);
  oled.sendData(0b11111111);
  oled.endData();

  for (uint8_t y = 1; y < (height - 8) / 8; y++) {
    oled.setCursor(0, y);
    oled.startData();
    oled.sendData(0b11111111);
    oled.repeatData(0b00000000, width - 2);
    oled.sendData(0b11111111);
    oled.endData();
  }

  oled.setCursor(0, (height - 8) / 8);
  oled.startData();
  oled.sendData(0b11111111);
  oled.repeatData(0b10000000, width - 2);
  oled.sendData(0b11111111);
  oled.endData();

  oled.setCursor(8, 1);
  oled.print(width);
  oled.print('x');
  oled.print(height);
}

void scrollScreen() {
  uint8_t startScrollPage = 1;
  uint8_t endScrollPage = 2;
  uint8_t startScrollColumn = 8;
  uint8_t endScrollColumn = startScrollColumn + width - 16;
  for (uint8_t x = 0; x < width - 16; x++)
  {
    delay(50);
    oled.scrollContentRight(startScrollPage, endScrollPage, startScrollColumn, endScrollColumn);
  }
}
