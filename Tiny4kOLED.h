/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */
#include <stdint.h>
#include <Arduino.h>
#include <TinyWireM.h>  // Version with buffer bugfix: https://github.com/adafruit/TinyWireM

#ifndef TINY4KOLED_H
#define TINY4KOLED_H

// #define _nofont_8x16
#ifndef _nofont_8x16
#define FONT8X16	1
#endif
#define FONT6X8		0

// ----------------------------------------------------------------------------

#ifndef SSD1306
#define SSD1306		0x3C	// Slave address
#endif

// ----------------------------------------------------------------------------

class SSD1306Device: public Print {

	public:
		SSD1306Device(void);
		void begin(void);
		void switchRenderFrame(void);
		void switchDisplayFrame(void);
		void switchFrame(void);
		void setFont(uint8_t font);
		void setCursor(uint8_t x, uint8_t y);
		void fill(uint8_t fill);
		void fillToEOL(uint8_t fill);
		void fillLength(uint8_t fill, uint8_t length);
		void clear(void);
		void clearToEOL(void);
		void bitmap(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[]);

		// 1. Fundamental Command Table

		void setContrast(uint8_t contrast);
		void setEntireDisplayOn(bool enable);
		void setInverse(bool enable);
		void off(void);
		void on(void);
		
		// 2. Scrolling Command Table
		
		void scrollRight(uint8_t startPage, uint8_t interval, uint8_t endPage);
		void scrollLeft(uint8_t startPage, uint8_t interval, uint8_t endPage);
		void scrollRightOffset(uint8_t startPage, uint8_t interval, uint8_t endPage, uint8_t offset);
		void scrollLeftOffset(uint8_t startPage, uint8_t interval, uint8_t endPage, uint8_t offset);
		void deactivateScroll(void);
		void activateScroll(void);
		void setVerticalScrollArea(uint8_t top, uint8_t rows);
		
		// 3. Addressing Setting Command Table

		// 4. Hardware Configuration (Panel resolution and layout related) Command Table
		
		void setDisplayStartLine(uint8_t startLine);
		void setSegmentRemap(uint8_t remap);
		void setMultiplexRatio(uint8_t mux);
		void setComOutputDirection(uint8_t direction);
		void setDisplayOffset(uint8_t offset);
		void setComPinsHardwareConfiguration(uint8_t alternative, uint8_t enableLeftRightRemap);
		void setDisplayClock(uint8_t divideRatio, uint8_t oscillatorFrequency);
		void setPrechargePeriod(uint8_t phaseOnePeriod, uint8_t phaseTwoPeriod);
		void setVcomhDeselectLevel(uint8_t level);
		void nop(void);

		virtual size_t write(byte c);
		using Print::write;

	private:
		void ssd1306_send_command(uint8_t command);
		void ssd1306_send_command2(uint8_t command1, uint8_t command2);
		void ssd1306_send_start(uint8_t transmission_type);
		void ssd1306_send_byte(uint8_t transmission_type, uint8_t byte);
		void ssd1306_send_stop();
};

extern SSD1306Device oled;

// ----------------------------------------------------------------------------

#endif
