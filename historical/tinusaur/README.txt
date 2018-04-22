SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays

-----------------------------------------------------------------------------------
 Copyright (c) 2016 Neven Boyanov, Tinusaur Team. All Rights Reserved.
 Distributed as open source software under MIT License, see LICENSE.txt file.
 Please, as a favor, retain the link http://tinusaur.org to The Tinusaur Project.
-----------------------------------------------------------------------------------

SSD1306xLED is a C library for working with the SSD1306 display driver to control dot matrix OLED/PLED 128x64 displays. It is intended to be used with the Tinusaur board but should also work with any other board based on ATtiny85 or similar microcontroller.

The SSD1306xLED library is written in plain C and does not require any additional libraries to function except those that come with the SDK.

This library has support for and tested work with the 8 MHz CPU clock mode.

Modules, folders and files Structure:

 - ssd1306xled          - the SSD1306xLED library
   - font6x8.h          - small 6x8 font
   - font8x16.h         - large 8x16 font
   - ssd1306xled.c      - library implementation
   - ssd1306xled.h      - library headers
   - ssd1306xled8x16.c  - large font 8x16 implementation
   - ssd1306xled8x16.h  - large font 8x16 headers
 - ssd1306xled_cn       - additional functions for handling some Chinese characters (currently does not work)
 - ssd1306xled_test     - simple testing program for the library
   - img0_128x64c1.h    - sample bitmap image 128x64
   - img1_128x64c1.h    - sample bitmap image 128x64
   - main.c             - sample code using the library
 - ssd1306xled_test_cn  - test for Chinese functions (currently does not work)


==== Links ====

Official Tinusaur Project website: http://tinusaur.org
Project SSD1306xLED page: http://tinusaur.org/projects/ssd1306xled/
Project SSD1306xLED source code: https://bitbucket.org/tinusaur/ssd1306xled

Twitter: https://twitter.com/tinusaur
Facebook: https://www.facebook.com/tinusaur

