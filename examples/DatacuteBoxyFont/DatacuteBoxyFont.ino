#include <Tiny4kOLED.h>
#include "boxyfont.h"
// ============================================================================

const char datacute1[17] = {214,196,196,191,175,175,186,175,175,175,175,175,175,186,175,175,0};
const char datacute2[17] = {186,175,175,179,218,210,215,194,183,214,196,175,196,215,210,191,0};
const char datacute3[17] = {186,175,175,179,218,182,186,218,182,186,175,179,186,186,199,217,0};
const char datacute4[17] = {211,196,196,217,192,208,208,192,208,211,196,192,208,208,211,217,0};

const char boxes1[16] = {218,194,191,175,201,203,187,175,213,209,184,175,214,210,183,0};
const char boxes2[16] = {195,197,180,175,204,206,185,175,198,216,181,175,199,215,182,0};
const char boxes3[16] = {192,193,217,175,200,202,188,175,212,207,190,175,211,208,189,0};
const char boxes4[16] = {176,177,178,219,175,220,223,175,221,222,175,196,179,205,186,0};

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
