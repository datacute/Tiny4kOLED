/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */
#include <stdint.h>
#include <Arduino.h>

#ifndef TINY4KOLEDCOMMON_H
#define TINY4KOLEDCOMMON_H

typedef struct {
	uint8_t *bitmap;      // character bitmaps data
	uint8_t width;        // character width in pixels
	uint8_t height;       // character height in pages (8 pixels)
	uint8_t first, last;  // ASCII extents
} DCfont;

// Two included fonts, The space isn't used unless it is needed
#include "font6x8.h"
#include "font8x16.h"
#include "font8x16digits.h"

// ----------------------------------------------------------------------------

#ifndef SSD1306
#define SSD1306		0x3C	// Slave address
#endif

// ----------------------------------------------------------------------------

class SSD1306Device: public Print {

	public:
		SSD1306Device(uint8_t ssd1306_i2c_address, void (*wireBeginFunc)(void), void (*wireBeginTransmissionFunc)(uint8_t ssd1306_i2c_address), bool (*wireWriteFunc)(uint8_t byte), void (*wireEndTransmissionFunc)(void));

		void begin(void);
		void begin(uint8_t init_sequence_length, const uint8_t init_sequence []);
		void switchRenderFrame(void);
		void switchDisplayFrame(void);
		void switchFrame(void);
		uint8_t currentRenderFrame(void);
		uint8_t currentDisplayFrame(void);
		void setFont(const DCfont *font);
		void setCursor(uint8_t x, uint8_t y);
		void newLine();
		void fill(uint8_t fill);
		void fillToEOL(uint8_t fill);
		void fillLength(uint8_t fill, uint8_t length);
		void clear(void);
		void clearToEOL(void);
		void bitmap(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[]);
		void startData(void);
		void sendData(const uint8_t data);
		void repeatData(uint8_t data, uint8_t length);
		void clearData(uint8_t length);
		void endData(void);

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
		void setColumnStartAddress(uint8_t startAddress);
		void setMemoryAddressingMode(uint8_t mode);
		void setColumnAddress(uint8_t startAddress, uint8_t endAddress);
		void setPageAddress(uint8_t startPage, uint8_t endPage);
		void setPageStartAddress(uint8_t startPage);

		// 4. Hardware Configuration (Panel resolution and layout related) Command Table

		void setDisplayStartLine(uint8_t startLine);
		void setSegmentRemap(uint8_t remap);
		void setMultiplexRatio(uint8_t mux);
		void setComOutputDirection(uint8_t direction);
		void setDisplayOffset(uint8_t offset);
		void setComPinsHardwareConfiguration(uint8_t alternative, uint8_t enableLeftRightRemap);

		// 5. Timing and Driving Scheme Setting Command table

		void setDisplayClock(uint8_t divideRatio, uint8_t oscillatorFrequency);
		void setPrechargePeriod(uint8_t phaseOnePeriod, uint8_t phaseTwoPeriod);
		void setVcomhDeselectLevel(uint8_t level);
		void nop(void);

		// 6. Advance Graphic Command table

		void fadeOut(uint8_t interval);
		void blink(uint8_t interval);
		void disableFadeOutAndBlinking(void);
		void enableZoomIn(void);
		void disableZoomIn(void);

		// Charge Pump Settings

		void enableChargePump(void);
		void disableChargePump(void);

		virtual size_t write(byte c);
		using Print::write;

	private:
		void newLine(uint8_t fontHeight);

};

// ----------------------------------------------------------------------------

#endif
