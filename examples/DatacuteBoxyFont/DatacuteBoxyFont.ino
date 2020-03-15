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
//#include <Wire.h>

// To use the Adafruit's TinyWireM library:
//#include <TinyWireM.h>

// To use the TinyI2C library from https://github.com/technoblogy/tiny-i2c
//#include <TinyI2CMaster.h>

// The blue OLED screen requires a long initialization on power on.
// The code to wait for it to be ready uses 20 bytes of program storage space
// If you are using a white OLED, this can be reclaimed by uncommenting
// the following line (before including Tiny4kOLED.h):
//#define TINY4KOLED_QUICK_BEGIN

#include <Tiny4kOLED.h>
#include "boxyfont.h"
// ============================================================================

const char datacute1[17] = {(char)214,(char)196,(char)196,(char)191,(char)175,(char)175,(char)186,(char)175,(char)175,(char)175,(char)175,(char)175,(char)175,(char)186,(char)175,(char)175,(char)0};
const char datacute2[17] = {(char)186,(char)175,(char)175,(char)179,(char)218,(char)210,(char)215,(char)194,(char)183,(char)214,(char)196,(char)175,(char)196,(char)215,(char)210,(char)191,(char)0};
const char datacute3[17] = {(char)186,(char)175,(char)175,(char)179,(char)218,(char)182,(char)186,(char)218,(char)182,(char)186,(char)175,(char)179,(char)186,(char)186,(char)199,(char)217,(char)0};
const char datacute4[17] = {(char)211,(char)196,(char)196,(char)217,(char)192,(char)208,(char)208,(char)192,(char)208,(char)211,(char)196,(char)192,(char)208,(char)208,(char)211,(char)217,(char)0};

const char boxes1[16] = {(char)218,(char)194,(char)191,(char)175,(char)201,(char)203,(char)187,(char)175,(char)213,(char)209,(char)184,(char)175,(char)214,(char)210,(char)183,(char)0};
const char boxes2[16] = {(char)195,(char)197,(char)180,(char)175,(char)204,(char)206,(char)185,(char)175,(char)198,(char)216,(char)181,(char)175,(char)199,(char)215,(char)182,(char)0};
const char boxes3[16] = {(char)192,(char)193,(char)217,(char)175,(char)200,(char)202,(char)188,(char)175,(char)212,(char)207,(char)190,(char)175,(char)211,(char)208,(char)189,(char)0};
const char boxes4[16] = {(char)176,(char)177,(char)178,(char)219,(char)175,(char)220,(char)223,(char)175,(char)221,(char)222,(char)175,(char)196,(char)179,(char)205,(char)186,(char)0};

void setup() {
  // put your setup code here, to run once:

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
  // This example shows how to create and use your own font.
  // The font used here is of the codepage 437 box drawing characters.
  oled.setFont(&cp_437_box_drawing_font);
  datacute();
  oled.on();
  oled.switchRenderFrame();
  boxes();
}

void loop() {
  delay(10000);
  oled.switchDisplayFrame();
}

void datacute() {
  oled.clear();
  oled.setCursor(0, 0);
  oled.print(datacute1);
  oled.setCursor(0, 1);
  oled.print(datacute2);
  oled.setCursor(0, 2);
  oled.print(datacute3);
  oled.setCursor(0, 3);
  oled.print(datacute4);
}

void boxes() {
  oled.clear();
  oled.setCursor(4, 0);
  oled.print(boxes1);
  oled.setCursor(4, 1);
  oled.print(boxes2);
  oled.setCursor(4, 2);
  oled.print(boxes3);
  oled.setCursor(4, 3);
  oled.print(boxes4);
}
