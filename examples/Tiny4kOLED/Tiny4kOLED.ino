#include <Tiny4kOLED.h>
#include <TinyWireM.h>
// ============================================================================

void setup() {
  // put your setup code here, to run once:

  oled.begin();
  oled.clear();
  oled.on();

}

void loop() {
  
  // put your main code here, to run repeatedly:
  oled.fill(0xFF); //fill screen with color
  delay(1000);
  oled.clear(); //all black
  delay(1000);
  oled.setFont(FONT8X16); // 2 lines of 16 characters exactly fills 128x32
  //usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  oled.setCursor(32, 0);
  oled.print(F("Datacute")); //wrap strings in F() to save RAM!
  oled.setFont(FONT6X8); // 4 lines of 21 characters only fills 126x32
  oled.setCursor(13, 2); //two rows down because the 8x16 font takes two rows of 8
  oled.print(F("Acute Information"));
  oled.setCursor(16, 3); //cursor x does not need to be a multiple of the font width
  oled.print(F("Revelation Tools"));
  
  delay(3000);
}
