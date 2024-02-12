/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */
/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */

// ----------------------------------------------------------------------------

#ifndef RP2040
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

// ----------------------------------------------------------------------------

/* Standard ASCII 8x16 proportional font */
const uint8_t Tiny4kOLED_font8x16 [] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00, //   0

  0xF8,0x00,
  0x33,0x30,                               // ! 1

  0x10,0x0C,0x06,0x10,0x0C,0x06,
  0x00,0x00,0x00,0x00,0x00,0x00, // " 2

  0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,
  0x04,0x3F,0x04,0x04,0x3F,0x04,0x04, // # 3

  0x70,0x88,0xFC,0x08,0x30,
  0x18,0x20,0xFF,0x21,0x1E, // $ 4

  0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,
  0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E, // % 5

  0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,
  0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10, // & 6

  0x10,0x16,0x0E,
  0x00,0x00,0x00, // ' 7

  0xE0,0x18,0x04,0x02,
  0x07,0x18,0x20,0x40, // ( 8

  0x02,0x04,0x18,0xE0,
  0x40,0x20,0x18,0x07, // ) 9

  0x40,0x40,0x80,0xF0,0x80,0x40,0x40,
  0x02,0x02,0x01,0x0F,0x01,0x02,0x02, // * 10

  
  0x00,0x00,0x00,0xF0,0x00,0x00,0x00,
  0x01,0x01,0x01,0x1F,0x01,0x01,0x01, // + 11

  0x00,0x00,0x00,
  0x80,0xB0,0x70, // , 12

  0x00,0x00,0x00,0x00,0x00,0x00,
  0x01,0x01,0x01,0x01,0x01,0x01, // - 13

  0x00,0x00,
  0x30,0x30, // . 14

  0x00,0x00,0x00,0x80,0x60,0x18,0x04,
  0x60,0x18,0x06,0x01,0x00,0x00,0x00, // / 15

  0xE0,0x10,0x08,0x08,0x10,0xE0,
  0x0F,0x10,0x20,0x20,0x10,0x0F, // 0 16

  0x10,0x10,0xF8,0x00,0x00,
  0x20,0x20,0x3F,0x20,0x20, // 1 17

  0x70,0x08,0x08,0x08,0x88,0x70,
  0x30,0x28,0x24,0x22,0x21,0x30, // 2 18

  0x30,0x08,0x88,0x88,0x48,0x30,
  0x18,0x20,0x20,0x20,0x11,0x0E, // 3 19

  0x00,0xC0,0x20,0x10,0xF8,0x00,
  0x07,0x04,0x24,0x24,0x3F,0x24, // 4 20

  0xF8,0x08,0x88,0x88,0x08,0x08,
  0x19,0x21,0x20,0x20,0x11,0x0E, // 5 21

  0xE0,0x10,0x88,0x88,0x18,0x00,
  0x0F,0x11,0x20,0x20,0x11,0x0E, // 6 22

  0x38,0x08,0x08,0xC8,0x38,0x08,
  0x00,0x00,0x3F,0x00,0x00,0x00, // 7 23

  0x70,0x88,0x08,0x08,0x88,0x70,
  0x1C,0x22,0x21,0x21,0x22,0x1C, // 8 24

  0xE0,0x10,0x08,0x08,0x10,0xE0,
  0x00,0x31,0x22,0x22,0x11,0x0F, // 9 25

  0xC0,0xC0,
  0x30,0x30, // : 26

  0x00,0x80,
  0x80,0x60, // ; 27

  0x00,0x80,0x40,0x20,0x10,0x08,
  0x01,0x02,0x04,0x08,0x10,0x20, // < 28

  0x40,0x40,0x40,0x40,0x40,0x40,0x40,
  0x04,0x04,0x04,0x04,0x04,0x04,0x04, // = 29

  0x08,0x10,0x20,0x40,0x80,0x00,
  0x20,0x10,0x08,0x04,0x02,0x01, // > 30

  0x70,0x48,0x08,0x08,0x08,0xF0,
  0x00,0x00,0x30,0x36,0x01,0x00, // ? 31

  0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,
  0x07,0x18,0x27,0x24,0x23,0x14,0x0B, // @ 32

  0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,
  0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20, // A 33

  0x08,0xF8,0x88,0x88,0x88,0x70,0x00,
  0x20,0x3F,0x20,0x20,0x20,0x11,0x0E, // B 34

  0xC0,0x30,0x08,0x08,0x08,0x08,0x38,
  0x07,0x18,0x20,0x20,0x20,0x10,0x08, // C 35

  0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,
  0x20,0x3F,0x20,0x20,0x20,0x10,0x0F, // D 36

  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,
  0x20,0x3F,0x20,0x20,0x23,0x20,0x18, // E 37

  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,
  0x20,0x3F,0x20,0x00,0x03,0x00,0x00, // F 38

  0xC0,0x30,0x08,0x08,0x08,0x38,0x00,
  0x07,0x18,0x20,0x20,0x22,0x1E,0x02, // G 39

  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,
  0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20, // H 40

  0x08,0x08,0xF8,0x08,0x08,
  0x20,0x20,0x3F,0x20,0x20, // I 41

  0x00,0x00,0x08,0x08,0xF8,0x08,0x08,
  0xC0,0x80,0x80,0x80,0x7F,0x00,0x00, // J 42

  0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,
  0x20,0x3F,0x20,0x01,0x26,0x38,0x20, // K 43

  0x08,0xF8,0x08,0x00,0x00,0x00,0x00,
  0x20,0x3F,0x20,0x20,0x20,0x20,0x30, // L 44

  0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,
  0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20, // M 45

  0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,
  0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00, // N 46

  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,
  0x0F,0x10,0x20,0x20,0x20,0x10,0x0F, // O 47

  0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,
  0x20,0x3F,0x21,0x01,0x01,0x01,0x00, // P 48

  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,
  0x0F,0x18,0x24,0x24,0x38,0x50,0x4F, // Q 49

  0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,
  0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20, // R 50

  0x70,0x88,0x08,0x08,0x08,0x38,
  0x38,0x20,0x21,0x21,0x22,0x1C, // S 51

  0x18,0x08,0x08,0xF8,0x08,0x08,0x18,
  0x00,0x00,0x20,0x3F,0x20,0x00,0x00, // T 52

  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,
  0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00, // U 53

  0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,
  0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00, // V 54

  0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,
  0x03,0x3C,0x07,0x00,0x07,0x3C,0x03, // W 55

  0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,
  0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20, // X 56

  0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,
  0x00,0x00,0x20,0x3F,0x20,0x00,0x00, // Y 57

  0x10,0x08,0x08,0x08,0xC8,0x38,0x08,
  0x20,0x38,0x26,0x21,0x20,0x20,0x18, // Z 58

  0xFE,0x02,0x02,0x02,
  0x7F,0x40,0x40,0x40, // [ 59

  0x0C,0x30,0xC0,0x00,0x00,0x00,
  0x00,0x00,0x01,0x06,0x38,0xC0, // \ 60

  0x02,0x02,0x02,0xFE,0x00,0x00,
  0x40,0x40,0x40,0x7F,0x00,0x00, // ] 61

  0x04,0x02,0x02,0x02,0x04,
  0x00,0x00,0x00,0x00,0x00, // ^ 62

  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80, // _ 63

  0x02,0x02,0x04,
  0x00,0x00,0x00, // ` 64

  0x00,0x80,0x80,0x80,0x80,0x00,0x00,
  0x19,0x24,0x22,0x22,0x22,0x3F,0x20, // a 65

  0x08,0xF8,0x00,0x80,0x80,0x00,0x00,
  0x00,0x3F,0x11,0x20,0x20,0x11,0x0E, // b 66

  0x00,0x00,0x80,0x80,0x80,0x00,
  0x0E,0x11,0x20,0x20,0x20,0x11, // c 67

  0x00,0x00,0x80,0x80,0x88,0xF8,0x00,
  0x0E,0x11,0x20,0x20,0x10,0x3F,0x20, // d 68

  0x00,0x80,0x80,0x80,0x80,0x00,
  0x1F,0x22,0x22,0x22,0x22,0x13, // e 69

  0x80,0x80,0xF0,0x88,0x88,0x88,0x18,
  0x20,0x20,0x3F,0x20,0x20,0x00,0x00, // f 70

  0x00,0x80,0x80,0x80,0x80,0x80,
  0x6B,0x94,0x94,0x94,0x93,0x60, // g 71

  0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,
  0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20, // h 72

  0x80,0x98,0x98,0x00,0x00,
  0x20,0x20,0x3F,0x20,0x20, // i 73

  0x00,0x00,0x80,0x98,0x98,
  0xC0,0x80,0x80,0x80,0x7F, // j 74

  0x08,0xF8,0x00,0x00,0x80,0x80,0x80,
  0x20,0x3F,0x24,0x02,0x2D,0x30,0x20, // k 75

  0x08,0x08,0xF8,0x00,0x00,
  0x20,0x20,0x3F,0x20,0x20, // l 76

  0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,
  0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F, // m 77

  0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,
  0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20, // n 78

  0x00,0x80,0x80,0x80,0x80,0x00,
  0x1F,0x20,0x20,0x20,0x20,0x1F, // o 79

  0x80,0x80,0x00,0x80,0x80,0x00,0x00,
  0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E, // p 80

  0x00,0x00,0x80,0x80,0x80,0x80,0x00,
  0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80, // q 81

  0x80,0x80,0x80,0x00,0x80,0x80,0x80,
  0x20,0x20,0x3F,0x21,0x20,0x00,0x01, // r 82

  0x00,0x80,0x80,0x80,0x80,0x80,
  0x33,0x24,0x24,0x24,0x24,0x19, // s 83

  0x80,0x80,0xE0,0x80,0x80,
  0x00,0x00,0x1F,0x20,0x20, // t 84

  0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,
  0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20, // u 85

  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,
  0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00, // v 86

  0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,
  0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00, // w 87

  0x80,0x80,0x00,0x80,0x80,0x80,
  0x20,0x31,0x2E,0x0E,0x31,0x20, // x 88

  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,
  0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00, // y 89

  0x80,0x80,0x80,0x80,0x80,0x80,
  0x21,0x30,0x2C,0x22,0x21,0x30, // z 90

  0x80,0x7C,0x02,0x02,
  0x00,0x3F,0x40,0x40, // { 91

  0xFF,
  0xFF, // | 92

  0x02,0x02,0x7C,0x80,
  0x40,0x40,0x3F,0x00, // } 93

  0x06,0x01,0x01,0x02,0x02,0x04,0x04,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ~ 94
};

const uint8_t Tiny4kOLED_font8x16_widths [] PROGMEM = {
  7,2,6,7,5,7,8,3,4,4,7,7,3,6,2,7,
  6,5,6,6,6,6,6,6,6,6,2,2,6,7,6,6,
  7,8,7,7,7,7,7,7,8,5,7,7,7,7,8,7,
  7,7,8,6,7,8,8,7,8,7,7,4,6,6,5,8,
  3,7,7,6,7,6,7,6,8,5,5,7,5,8,8,6,
  7,7,7,6,5,8,8,8,6,8,6,4,1,4,7
};

const uint16_t Tiny4kOLED_font8x16_widths_16s [] PROGMEM = {
  7+2+6+7+5+7+8+3+4+4+7+7+3+6+2+7,
  6+5+6+6+6+6+6+6+6+6+2+2+6+7+6+6,
  7+8+7+7+7+7+7+7+8+5+7+7+7+7+8+7,
  7+7+8+6+7+8+8+7+8+7+7+4+6+6+5+8,
  3+7+7+6+7+6+7+6+8+5+5+7+5+8+8+6,
  7+7+7+6+5+8+8+8+6+8+6+4+1+4+7
};

const DCfont Tiny4kOLEDfont8x16 = {
  (uint8_t *)Tiny4kOLED_font8x16,
  0, // character width in pixels
  2, // character height in pages (8 pixels)
  32,126, // ASCII extents
  (uint16_t *)Tiny4kOLED_font8x16_widths_16s,
  (uint8_t *)Tiny4kOLED_font8x16_widths,
  1 // spacing
  };

// for backwards compatibility
#define FONT8X16P (&Tiny4kOLEDfont8x16)
