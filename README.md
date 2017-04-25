# Tiny4kOLED
This is a library for an ATTiny85 to use an SSD1306 powered, double buffered, 128x32 pixel OLED, over I<sup>2</sup>C.

The SSD1306 has enough RAM to support a 128 by 64 pixel display, and most SSD1306 controlled OLEDs are 128 by 64 pixels. However there are also other display sizes available. With a 128 by 32 pixel display, only half of the SSD1306's RAM is used, and the other half can be used as a frame buffer. This results in being able to have a slow ATTiny85 gradually update the display, then switch it into view when it is ready.

This library is an adaption of [DigisparkOLED](https://github.com/digistump/DigistumpArduino/tree/master/digistump-avr/libraries/DigisparkOLED) with the following changes:
- Replaced [Wire](https://github.com/digistump/DigistumpArduino/tree/master/digistump-avr/libraries/Wire) with [TinyWireM](https://github.com/adafruit/TinyWireM) (Digispark's Wire is a rename of TinyWireM, but Adafruit's includes a necessary bug fix.)
- Fixed the sending of multiple bytes per I<sup>2</sup>C transmission
- Changed the initialization to be two I<sup>2</sup>C transmissions, instead of using a transmission per command byte.
- Left the display off in the initialization, so that the display could be cleared before being shown.
- Altered the library to be for a 128x32 display.
- Added double buffering control code.

*<sub>The 4k in the project name is because 128 multiplied by 32 is 4096. This library is for a display with only 4096 pixels, not for UHD displays.</sub>*
