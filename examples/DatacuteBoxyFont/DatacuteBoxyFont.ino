/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */

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
