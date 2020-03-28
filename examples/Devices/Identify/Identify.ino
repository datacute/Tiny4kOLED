/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 * This example attempts to identify your screen.
 * When the screen shows a rectangle with a number in it, 
 * find that number in this source file to see how to use the
 * Tiny4kOLED library with your screen.
 *
 */

#include <Tiny4kOLED.h>

#define pinPrevious 3
#define pinNext 4

// The values shown here are the default values at reset.
// As this example changes lots of values,
// this sequence is sent between each init sequence
// The screen SHOWN as sequence 0 is sending this sequence,
// then the libraries default sequence (for a 128 x 32 screen)
static const uint8_t ssd1306_init_sequence0 [] PROGMEM = {  // Initialization Sequence
  0xAE,         // Display OFF (sleep mode)
  0x2E,         // Disable scroll
  0x20, 0b10,   // Set Memory Addressing Mode
                // 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
                // 10=Page Addressing Mode (RESET); 11=Invalid
  0xB0,         // Set Page Start Address for Page Addressing Mode, 0-7
  0xC0,         // Set COM Output Scan Direction
  0x00,         // Set low nibble of column address
  0x10,         // Set high nibble of column address
  0x40,         // Set display start line address
  0x81, 0x7F,   // Set contrast control register
  0xA0,         // Set Segment Re-map. A0=column 0 mapped to SEG0; A1=column 127 mapped to SEG0.
  0xA6,         // Set display mode. A6=Normal; A7=Inverse
  0xA8, 0x3F,   // Set multiplex ratio(1 to 64)
  0xA4,         // Output RAM to Display
                // 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
  0xD3, 0x00,   // Set display offset. 00 = no offset
  0xD5, 0x80,   // --set display clock divide ratio/oscillator frequency
  0xD9, 0x22,   // Set pre-charge period
  0xDA, 0x12,   // Set com pins hardware configuration
  0xDB, 0x20,   // --set vcomh 0x20 = 0.77xVcc
  0xAD, 0x00,   // Select external IREF
  0x8D, 0x10    // DC-DC disabled
};

// Initialization sequence for 128 x 32 screen
static const uint8_t ssd1306_init_sequence1 [] PROGMEM = {
  0xA8, 0x1F,   // Multiplex ratio
  0xDA, 0x02,   // HW pin configuration
  // If you are copying this, include the common_post_init_sequence values to here
};

// Initialization sequence for rotated 128 x 32 screen
static const uint8_t ssd1306_init_sequence2 [] PROGMEM = {
  0xC8,         // Set COM Output Scan Direction
  0xA1,         // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
  0xA8, 0x1F,   // Set multiplex ratio(1 to 64)
  0xDA, 0x02,   // HW pin configuration
  // If you are copying this, include the common_post_init_sequence values to here
};

// Initialization sequence for 128 x 64 screen
static const uint8_t ssd1306_init_sequence3 [] PROGMEM = {
  // If you are copying this, include the common_post_init_sequence values to here
};

// Initialization sequence for rotated 128 x 64 screen
static const uint8_t ssd1306_init_sequence4 [] PROGMEM = {
  0xC8,         // Set COM Output Scan Direction
  0xA1,         // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
  // If you are copying this, include the common_post_init_sequence values to here
};

// Initialization sequence for 64 x 48 screen
static const uint8_t ssd1306_init_sequence5 [] PROGMEM = {
  0xA8, 0x2F,   // Set multiplex ratio(1 to 64)
  // If you are copying this, include the common_post_init_sequence values to here
};

// Initialization sequence for rotated 64 x 48 screen
static const uint8_t ssd1306_init_sequence6 [] PROGMEM = {
  0xC8,         // Set COM Output Scan Direction
  0xA1,         // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
  0xA8, 0x2F,   // Set multiplex ratio(1 to 64)
  // If you are copying this, include the common_post_init_sequence values to here
};

// Initialization sequence for 72 x 40 screen
static const uint8_t ssd1306_init_sequence7 [] PROGMEM = {
  0xA8, 0x27,   // Set multiplex ratio(1 to 64)
  // If you are copying this, include the common_post_init_sequence values to here
};

// Initialization sequence for rotated 72 x 40 screen
static const uint8_t ssd1306_init_sequence8 [] PROGMEM = {
  0xC8,         // Set COM Output Scan Direction
  0xA1,         // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
  0xA8, 0x27,   // Set multiplex ratio(1 to 64)
  // If you are copying this, include the common_post_init_sequence values to here
};

// Initialization sequence for 64 x 32 screen
static const uint8_t ssd1306_init_sequence9 [] PROGMEM = {
  0xA8, 0x1F,   // Set multiplex ratio(1 to 64)
  // If you are copying this, include the common_post_init_sequence values to here
};

// Initialization sequence for rotated 64 x 32 screen
static const uint8_t ssd1306_init_sequence10 [] PROGMEM = {
  0xC8,         // Set COM Output Scan Direction
  0xA1,         // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
  0xA8, 0x1F,   // Set multiplex ratio(1 to 64)
  // If you are copying this, include the common_post_init_sequence values to here
};

const uint8_t numScreens = 11;

// Common Initialization sequence left at reset values
static const uint8_t common_post_init_sequence [] PROGMEM = {
  0xAD, 0x30,   // Select internal IREF and higher current
  0x8D, 0x14    // Set DC-DC enable 7.5V (We can't see the screen without the charge pump on)
};

uint8_t screen = 0;
uint8_t minx = 0;
uint8_t miny = 0;
uint8_t width = 128;
uint8_t height = 32;

struct {
  const uint8_t minx;
  const uint8_t miny;
  const uint8_t width;
  const uint8_t height;
  const uint8_t init_sequence_length;
  const uint8_t * init_sequence;
} const screens[numScreens] = {
  { 0, 0, 128, 32, sizeof(ssd1306_init_sequence0), ssd1306_init_sequence0},
  { 0, 0, 128, 32, sizeof(ssd1306_init_sequence1), ssd1306_init_sequence1},
  { 0, 0, 128, 32, sizeof(ssd1306_init_sequence2), ssd1306_init_sequence2},
  { 0, 0, 128, 64, sizeof(ssd1306_init_sequence3), ssd1306_init_sequence3},
  { 0, 0, 128, 64, sizeof(ssd1306_init_sequence4), ssd1306_init_sequence4},
  {32, 0,  64, 48, sizeof(ssd1306_init_sequence5), ssd1306_init_sequence5},
  {32, 0,  64, 48, sizeof(ssd1306_init_sequence6), ssd1306_init_sequence6},
  {28, 0,  72, 40, sizeof(ssd1306_init_sequence7), ssd1306_init_sequence7},
  {28, 0,  72, 40, sizeof(ssd1306_init_sequence8), ssd1306_init_sequence8},
  {32, 0,  64, 32, sizeof(ssd1306_init_sequence9), ssd1306_init_sequence9},
  {32, 0,  64, 32, sizeof(ssd1306_init_sequence10), ssd1306_init_sequence10}
};

void setup() {
  oled.setFont(FONT6X8);
  pinMode(pinPrevious, INPUT_PULLUP);
  pinMode(pinNext, INPUT_PULLUP);
}

void loop() {
  // Set everything back to default values at reset
  oled.begin(sizeof(ssd1306_init_sequence0), ssd1306_init_sequence0);

  if (screen == 0) {
    oled.begin();
  } else {
    oled.begin(screens[screen].init_sequence_length, screens[screen].init_sequence);
    oled.begin(sizeof(common_post_init_sequence), common_post_init_sequence);
  }
  drawScreen();
  oled.on();
  scrollScreen();
  nextScreen();
}

void drawScreen() {
  // Set entire memory to hatched
  for (uint8_t y = 0; y < 8; y++) {
    oled.setCursor(0, y);
    oled.startData();
    for (uint8_t x=0; x<128; x += 2) {
      oled.sendData(0b10101010);
      oled.sendData(0b01010101);
    }
    oled.endData();
  }

  oled.setCursor(minx, (miny / 8));
  oled.startData();
  oled.sendData(0b11111111);
  oled.repeatData(0b00000001, width - 2);
  oled.sendData(0b11111111);
  oled.endData();

  for (uint8_t y = 1; y < (height - 8) / 8; y++) {
    oled.setCursor(minx, y + (miny / 8));
    oled.startData();
    oled.sendData(0b11111111);
    oled.repeatData(0b00000000, width - 2);
    oled.sendData(0b11111111);
    oled.endData();
  }

  oled.setCursor(minx, (height + miny - 8) / 8);
  oled.startData();
  oled.sendData(0b11111111);
  oled.repeatData(0b10000000, width - 2);
  oled.sendData(0b11111111);
  oled.endData();

  oled.setCursor(8 + minx, 1 + (miny / 8));
  oled.print(screen);
  oled.setCursor(8 + minx, 2 + (miny / 8));
  oled.print(width);
  oled.print('x');
  oled.print(height);
}

void scrollScreen() {
  uint8_t startScrollPage = 1 + (miny / 8);
  uint8_t endSCrollPage = startScrollPage + 1;
  uint8_t startScrollColumn = 8 + minx;
  uint8_t endScrollColumn = startScrollColumn + width - 16;
  for (uint8_t x = 0; x < width - 16; x++)
  {
    delay(50);
    if (digitalRead(pinPrevious) == 0) {
      if (screen == 0) screen = numScreens - 2;
      else screen -= 2;
      break;
    }
    if (digitalRead(pinNext) == 0) break;
    oled.scrollContentRight(startScrollPage, endSCrollPage, startScrollColumn, endScrollColumn);
  }
}

void nextScreen() {
  screen += 1;
  if (screen == numScreens) screen = 0;
  minx = screens[screen].minx;
  miny = screens[screen].miny;
  width = screens[screen].width;
  height = screens[screen].height;
}