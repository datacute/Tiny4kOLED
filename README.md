# Tiny4kOLED

This is a library for an ATTiny85 to use an SSD1306 powered, double buffered, 128x32 pixel OLED, over I<sup>2</sup>C.

The SSD1306 has enough RAM to support a 128 by 64 pixel display, and most SSD1306 controlled OLEDs are 128 by 64 pixels. However there are also other display sizes available. With a 128 by 32 pixel display, only half of the SSD1306's RAM is used, and the other half can be used as a frame buffer. This results in being able to have a slow ATTiny85 gradually update the display, then switch it into view when it is ready.

This library is an adaption of [DigisparkOLED](https://github.com/digistump/DigistumpArduino/tree/master/digistump-avr/libraries/DigisparkOLED)
which in turn is a modification of the [Tinusaur](http://tinusaur.org) project's [SSD1306xLED library](https://bitbucket.org/tinusaur/ssd1306xled).
I have extensively re-written it, with the following changes:

- Replaced digistump's [Wire](https://github.com/digistump/DigistumpArduino/tree/master/digistump-avr/libraries/Wire) with [TinyWireM](https://github.com/adafruit/TinyWireM) (Digistump's Wire is a rename of TinyWireM, but Adafruit's includes a necessary bug fix.)
- Fixed the sending of multiple bytes per I<sup>2</sup>C transmission.
- Changed the initialization to be two I<sup>2</sup>C transmissions, instead of using a transmission per command byte.
- Left the display off in the initialization, so that the display could be cleared before being shown.
- Altered the library to be for a 128x32 display.
- Added double buffering control code.
- Exposed all of the SSD1306 features.
- Optimised font usage, enabling custom fonts.
- Optimised code size.
- Exposed the blink and fade SSD1306 features documented in revision 1.2 of the SSD1306 Specification.
- Support three different I<sup>2</sup>C interfaces:
  - Spence Konde's Wire.h that comes with [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore)
  - Adafruit's [TinyWireM](https://github.com/adafruit/TinyWireM)
  - David Johnson-Davies / Technoblogy's [TinyI2C](https://github.com/technoblogy/tiny-i2c)

## Example Usage

```c
#include <Tiny4kOLED.h>
void setup() {
  // Send the initialization sequence to the oled. This leaves the display turned off
  oled.begin();
  oled.setFont(FONT8X16);
  // Clear the memory before turning on the display
  oled.clear();
  // Turn on the display
  oled.on();
  // Switch the half of RAM that we are writing to, to be the half that is non currently displayed
  oled.switchRenderFrame();
}

void loop() {
  updateDisplay();
  delay(50);
}

void updateDisplay() {
  // Clear the half of memory not currently being displayed.
  oled.clear();
  // Position the text cursor
  // In order to keep the library size small, text can only be positioned
  // with the top of the font aligned with one of the four 8 bit high RAM pages.
  // The Y value therefore can only have the value 0, 1, 2, or 3.
  // usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  oled.setCursor(0, 1);
  // Write text to oled RAM (which is not currently being displayed).
  oled.print(F("ms: "));
  // Write the number of milliseconds since power on.
  oled.print(millis());
  // Swap which half of RAM is being written to, and which half is being displayed.
  // This is equivalent to calling both switchRenderFrame and switchDisplayFrame.
  oled.switchFrame();
}
```

*<sub>The 4k in the project name is because 128 multiplied by 32 is 4096. This library is for a display with only 4096 pixels, not for UHD displays.</sub>*
