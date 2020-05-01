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
- Exposed all of the SSD1306 features, with example code (in v1.2).
- Optimised font usage, enabling custom fonts. (See [TinyOLED-Fonts](https://github.com/datacute/TinyOLED-Fonts))
- Optimised code size.
- Exposed the blink and fade SSD1306 features documented in revision 1.2 of the SSD1306 Specification.
- v1.1 Support three different I<sup>2</sup>C interfaces:
  - Spence Konde's Wire.h that comes with [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore)
  - Adafruit's [TinyWireM](https://github.com/adafruit/TinyWireM)
  - David Johnson-Davies / Technoblogy's [TinyI2C](https://github.com/technoblogy/tiny-i2c)
- v1.3 Added back support for 128x64 screens, including double buffering when 'zoom' is enabled (32 lines are each drawn twice).
- v1.4 Added support for SSD1306B documented features:
  - Ability to select internal current reference, providing consistent, bright displays. 72x40 displays typically do not include an external current reference.
  - Ability to select the voltage used by the internal charge pump (results in a minor difference in brightness).
  - Ability to horizontally scroll a portion of the display by one pixel (`scrollContentLeft` and `scrollContentRight`).
- v1.5 Added init sequences and offsets for 128x64, 128x32, 72x40, 64x48, and 64x32 resolution screens.

## 128x64 / 128x32 / 72x40 / 64x48 / 64x32

The screens with a resolution of 128 by 64 pixels support a feature called 'zoom' where each row is drawn twice. This also only uses half the memory, and so the double buffering technique can be used when the screen is in that mode.

The double buffering technique doesn't have to be used, and library works with SSD1306 screens of different resolutions, however the default init sequence in this library is for a screen with 32 rows of pixel. Custom init sequences can be supplied. An empty init sequence can also be used with `oled.begin(0,0);` so the SSD1306 can be initialized by your own code.

The `clear` and `fill` commands, and the wrapping of lines of text by this library, use the screen's height in `pages` (a `page` is 8 rows of pixels). This library defaults the number of `pages` to 4. For screens with other geometries call the `setPages` method, or use the `begin` settings appropriate for your display.

The following `begin` parameters are supported for the various display resolutions.
The `b` at the end means `b`right. The `r` at the end means `r`otated.

```c
    // this is equivalent to tiny4koled_init_128x32r
    oled.begin();

    oled.begin(128, 64, sizeof(tiny4koled_init_128x64), tiny4koled_init_128x64);
    oled.begin(128, 64, sizeof(tiny4koled_init_128x64b), tiny4koled_init_128x64b);
    oled.begin(128, 64, sizeof(tiny4koled_init_128x64r), tiny4koled_init_128x64r);
    oled.begin(128, 64, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);

    oled.begin(128, 32, sizeof(tiny4koled_init_128x32), tiny4koled_init_128x32);
    oled.begin(128, 32, sizeof(tiny4koled_init_128x32b), tiny4koled_init_128x32b);
    oled.begin(128, 32, sizeof(tiny4koled_init_128x32r), tiny4koled_init_128x32r);
    oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);

    oled.begin(72, 40, sizeof(tiny4koled_init_72x40), tiny4koled_init_72x40);
    oled.begin(72, 40, sizeof(tiny4koled_init_72x40b), tiny4koled_init_72x40b);
    oled.begin(72, 40, sizeof(tiny4koled_init_72x40r), tiny4koled_init_72x40r);
    oled.begin(72, 40, sizeof(tiny4koled_init_72x40br), tiny4koled_init_72x40br);

    oled.begin(64, 48, sizeof(tiny4koled_init_64x48), tiny4koled_init_64x48);
    oled.begin(64, 48, sizeof(tiny4koled_init_64x48b), tiny4koled_init_64x48b);
    oled.begin(64, 48, sizeof(tiny4koled_init_64x48r), tiny4koled_init_64x48r);
    oled.begin(64, 48, sizeof(tiny4koled_init_64x48br), tiny4koled_init_64x48br);

    oled.begin(64, 32, sizeof(tiny4koled_init_64x32), tiny4koled_init_64x32);
    oled.begin(64, 32, sizeof(tiny4koled_init_64x32b), tiny4koled_init_64x32b);
    oled.begin(64, 32, sizeof(tiny4koled_init_64x32r), tiny4koled_init_64x32r);
    oled.begin(64, 32, sizeof(tiny4koled_init_64x32br), tiny4koled_init_64x32br);
```

## Example Usage

```c
// Choose your I2C implementation before including Tiny4kOLED.h
// The default is selected is Wire.h

// To use the Wire library:
//#include <Wire.h>

// To use the Adafruit's TinyWireM library:
//#include <TinyWireM.h>

// To use the TinyI2C library from https://github.com/technoblogy/tiny-i2c
//#include <TinyI2CMaster.h>

// To use the SoftI2CMaster library from https://github.com/felias-fogg/SoftI2CMaster
// #define SCL_PIN PINB2
// #define SCL_PORT PORTB
// #define SDA_PIN PINB0
// #define SDA_PORT PORTB
// #include <SoftWire.h>

// The blue OLED screen requires a long initialization on power on.
// The code to wait for it to be ready uses 20 bytes of program storage space
// If you are using a white OLED, this can be reclaimed by uncommenting
// the following line (before including Tiny4kOLED.h):
//#define TINY4KOLED_QUICK_BEGIN

#include <Tiny4kOLED.h>

void setup() {
  // Send the initialization sequence to the oled. This leaves the display turned off
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
