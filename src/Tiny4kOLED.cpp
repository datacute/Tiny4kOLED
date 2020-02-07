/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */

// ----------------------------------------------------------------------------

#include "Tiny4kOLED_common.h"

#define SSD1306_PAGES 4

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0x40

// ----------------------------------------------------------------------------

// Some code based on "IIC_without_ACK" by http://www.14blog.com/archives/1358

static const uint8_t ssd1306_init_sequence [] PROGMEM = {	// Initialization Sequence
//	0xAE,			// Display OFF (sleep mode)
//	0x20, 0b10,		// Set Memory Addressing Mode
					// 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
					// 10=Page Addressing Mode (RESET); 11=Invalid
//	0xB0,			// Set Page Start Address for Page Addressing Mode, 0-7
	0xC8,			// Set COM Output Scan Direction
//	0x00,			// Set low nibble of column address
//	0x10,			// Set high nibble of column address
//	0x40,			// Set display start line address
//	0x81, 0x7F,		// Set contrast control register
	0xA1,			// Set Segment Re-map. A0=column 0 mapped to SEG0; A1=column 127 mapped to SEG0.
//	0xA6,			// Set display mode. A6=Normal; A7=Inverse
	0xA8, 0x1F,		// Set multiplex ratio(1 to 64)
//	0xA4,			// Output RAM to Display
					// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
//	0xD3, 0x00,		// Set display offset. 00 = no offset
//	0xD5, 0x80,		// --set display clock divide ratio/oscillator frequency
//	0xD9, 0x22,		// Set pre-charge period
	0xDA, 0x02,		// Set com pins hardware configuration
//	0xDB, 0x20,		// --set vcomh 0x20 = 0.77xVcc
	0x8D, 0x14		// Set DC-DC enable
};

static uint8_t oledPages = SSD1306_PAGES;
static const DCfont *oledFont = 0;
static uint8_t oledX = 0, oledY = 0;
static uint8_t renderingFrame = 0xB0, drawingFrame = 0x40;

static void (*wireBeginFn)(void);
static bool (*wireBeginTransmissionFn)(void);
static bool (*wireWriteFn)(uint8_t byte);
static uint8_t (*wireEndTransmissionFn)(void);

static void ssd1306_begin(void) {
	wireBeginFn();
}

static void ssd1306_send_start(void) {
	wireBeginTransmissionFn();
}

static bool ssd1306_send_byte(uint8_t byte) {
	return wireWriteFn(byte);
}

static void ssd1306_send_stop(void) {
	wireEndTransmissionFn();
}

static void ssd1306_send_command_start(void) {
	ssd1306_send_start();
	ssd1306_send_byte(SSD1306_COMMAND);
}

static void ssd1306_send_data_start(void) {
	ssd1306_send_start();
	ssd1306_send_byte(SSD1306_DATA);
}

static void ssd1306_send_command_byte(uint8_t byte) {
	if (ssd1306_send_byte(byte) == 0) {
		ssd1306_send_stop();
		ssd1306_send_command_start();
		ssd1306_send_byte(byte);
	}
}

static void ssd1306_send_data_byte(uint8_t byte) {
	if (ssd1306_send_byte(byte) == 0) {
		ssd1306_send_stop();
		ssd1306_send_data_start();
		ssd1306_send_byte(byte);
	}
}

static void ssd1306_send_command(uint8_t command) {
	ssd1306_send_command_start();
	ssd1306_send_byte(command);
	ssd1306_send_stop();
}

static void ssd1306_send_command2(uint8_t command1, uint8_t command2) {
	ssd1306_send_command_start();
	ssd1306_send_byte(command1);
	ssd1306_send_byte(command2);
	ssd1306_send_stop();
}

static void ssd1306_send_command3(uint8_t command1, uint8_t command2, uint8_t command3) {
	ssd1306_send_command_start();
	ssd1306_send_byte(command1);
	ssd1306_send_byte(command2);
	ssd1306_send_byte(command3);
	ssd1306_send_stop();
}

static void ssd1306_send_command6(uint8_t command1, uint8_t command2, uint8_t command3, uint8_t command4, uint8_t command5, uint8_t command6) {
	ssd1306_send_command_start();
	ssd1306_send_byte(command1);
	ssd1306_send_byte(command2);
	ssd1306_send_byte(command3);
	ssd1306_send_byte(command4);
	ssd1306_send_byte(command5);
	ssd1306_send_byte(command6);
	ssd1306_send_stop();
}

static void ssd1306_send_command7(uint8_t command1, uint8_t command2, uint8_t command3, uint8_t command4, uint8_t command5, uint8_t command6, uint8_t command7) {
	ssd1306_send_command_start();
	ssd1306_send_byte(command1);
	ssd1306_send_byte(command2);
	ssd1306_send_byte(command3);
	ssd1306_send_byte(command4);
	ssd1306_send_byte(command5);
	ssd1306_send_byte(command6);
	ssd1306_send_byte(command7);
	ssd1306_send_stop();
}

SSD1306Device::SSD1306Device(void (*wireBeginFunc)(void), bool (*wireBeginTransmissionFunc)(void), bool (*wireWriteFunc)(uint8_t byte), uint8_t (*wireEndTransmissionFunc)(void)) {
	wireBeginFn = wireBeginFunc;
	wireBeginTransmissionFn = wireBeginTransmissionFunc;
	wireWriteFn = wireWriteFunc;
	wireEndTransmissionFn = wireEndTransmissionFunc;
}

void SSD1306Device::begin(void) {
	begin(sizeof(ssd1306_init_sequence), ssd1306_init_sequence);
}

void SSD1306Device::begin(uint8_t init_sequence_length, const uint8_t init_sequence []) {
	ssd1306_begin();

	ssd1306_send_command_start();
	for (uint8_t i = 0; i < init_sequence_length; i++) {
		ssd1306_send_command_byte(pgm_read_byte(&init_sequence[i]));
	}
	ssd1306_send_stop();
}

void SSD1306Device::setPages(uint8_t pages) {
	oledPages = pages;
}

void SSD1306Device::setRotation(uint8_t rotation) {
	uint8_t rotationBit = (rotation & 0x01);
	ssd1306_send_command2(0xC0 | (rotationBit << 3), 0xA0 | rotationBit);
}

void SSD1306Device::setFont(const DCfont *font) {
	oledFont = font;
}

void SSD1306Device::setCursor(uint8_t x, uint8_t y) {
	ssd1306_send_command3(renderingFrame | (y & 0x07), 0x10 | ((x & 0xf0) >> 4), x & 0x0f);
	oledX = x;
	oledY = y;
}

void SSD1306Device::clear(void) {
	fill(0x00);
}

void SSD1306Device::fill(uint8_t fill) {
	for (uint8_t m = 0; m < oledPages; m++) {
		setCursor(0, m);
		fillToEOL(fill);
	}
	setCursor(0, 0);
}

void SSD1306Device::newLine(uint8_t fontHeight) {
	oledY+=fontHeight;
	if (oledY > oledPages - fontHeight) {
		oledY = oledPages - fontHeight;
	}
	setCursor(0, oledY);
}

void SSD1306Device::newLine(void) {
	newLine(oledFont->height);
}

size_t SSD1306Device::write(byte c) {
	if (!oledFont)
		return 1;

	if (c == '\r')
		return 1;
	
	uint8_t h = oledFont->height;

	if (c == '\n') {
		newLine(h);
		return 1;
	}

	uint8_t w = oledFont->width;

	if (oledX > ((uint8_t)128 - w)) {
		newLine(h);
	}

	uint16_t offset = ((uint16_t)c - oledFont->first) * w * h;
	uint8_t line = h;
	do
	{
		ssd1306_send_data_start();
		for (uint8_t i = 0; i < w; i++) {
			ssd1306_send_data_byte(pgm_read_byte(&(oledFont->bitmap[offset++])));
		}
		ssd1306_send_stop();
		if (h == 1) {
			oledX+=w;
		}
		else {
			if (line > 1) {
				setCursor(oledX, oledY + 1);
			}
			else {
				setCursor(oledX + w, oledY - (h - 1));
			}
		}
	}
	while (--line);
	return 1;
}

void SSD1306Device::bitmap(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[]) {
	uint16_t j = 0;
 	for (uint8_t y = y0; y < y1; y++) {
		setCursor(x0,y);
		ssd1306_send_data_start();
		for (uint8_t x = x0; x < x1; x++) {
			ssd1306_send_data_byte(pgm_read_byte(&bitmap[j++]));
		}
		ssd1306_send_stop();
	}
	setCursor(0, 0);
}

void SSD1306Device::clearToEOL(void) {
	fillToEOL(0x00);
}

void SSD1306Device::fillToEOL(uint8_t fill) {
	fillLength(fill, 128 - oledX);
}

void SSD1306Device::fillLength(uint8_t fill, uint8_t length) {
	if (length == 0) return;
	oledX += length;
	ssd1306_send_data_start();
	repeatData(fill, length);
	ssd1306_send_stop();
}

void SSD1306Device::startData(void) {
	ssd1306_send_data_start();
}

void SSD1306Device::sendData(const uint8_t data) {
	ssd1306_send_data_byte(data);
}

void SSD1306Device::repeatData(uint8_t data, uint8_t length) {
	for (uint8_t x = 0; x < length; x++) {
		ssd1306_send_data_byte(data);
	}
}

void SSD1306Device::clearData(uint8_t length) {
	repeatData(0, length);
}

void SSD1306Device::endData(void) {
	ssd1306_send_stop();
}

// Double Buffering Commands

void SSD1306Device::switchRenderFrame(void) {
	renderingFrame ^= 0x04;
}

void SSD1306Device::switchDisplayFrame(void) {
	drawingFrame ^= 0x20;
	ssd1306_send_command(drawingFrame);
}

void SSD1306Device::switchFrame(void) {
	switchDisplayFrame();
	switchRenderFrame();
}

uint8_t SSD1306Device::currentRenderFrame(void) {
	return (renderingFrame >> 2) & 0x01;
}

uint8_t SSD1306Device::currentDisplayFrame(void) {
	return (drawingFrame >> 5) & 0x01;
}

// 1. Fundamental Command Table

void SSD1306Device::setContrast(uint8_t contrast) {
	ssd1306_send_command2(0x81,contrast);
}

void SSD1306Device::setEntireDisplayOn(bool enable) {
	if (enable)
		ssd1306_send_command(0xA5);
	else
		ssd1306_send_command(0xA4);
}

void SSD1306Device::setInverse(bool enable) {
	if (enable)
		ssd1306_send_command(0xA7);
	else
		ssd1306_send_command(0xA6);
}

void SSD1306Device::off(void) {
	ssd1306_send_command(0xAE);
}

void SSD1306Device::on(void) {
	ssd1306_send_command(0xAF);
}

// 2. Scrolling Command Table

void SSD1306Device::scrollRight(uint8_t startPage, uint8_t interval, uint8_t endPage) {
	ssd1306_send_command7(0x26, 0x00, startPage, interval, endPage, 0x00, 0xFF);
}

void SSD1306Device::scrollLeft(uint8_t startPage, uint8_t interval, uint8_t endPage) {
	ssd1306_send_command7(0x27, 0x00, startPage, interval, endPage, 0x00, 0xFF);
}

void SSD1306Device::scrollRightOffset(uint8_t startPage, uint8_t interval, uint8_t endPage, uint8_t offset) {
	ssd1306_send_command6(0x29, 0x00, startPage, interval, endPage, offset);
}

void SSD1306Device::scrollLeftOffset(uint8_t startPage, uint8_t interval, uint8_t endPage, uint8_t offset) {
	ssd1306_send_command6(0x2A, 0x00, startPage, interval, endPage, offset);
}

void SSD1306Device::deactivateScroll(void) {
	ssd1306_send_command(0x2E);
}

void SSD1306Device::activateScroll(void) {
	ssd1306_send_command(0x2F);
}

void SSD1306Device::setVerticalScrollArea(uint8_t top, uint8_t rows) {
	ssd1306_send_command3(0xA3, top, rows);
}

// 3. Addressing Setting Command Table

void SSD1306Device::setColumnStartAddress(uint8_t startAddress) {
	ssd1306_send_command2(startAddress & 0x0F, startAddress >> 4);
}

void SSD1306Device::setMemoryAddressingMode(uint8_t mode) {
	ssd1306_send_command2(0x20, mode & 0x03);
}

void SSD1306Device::setColumnAddress(uint8_t startAddress, uint8_t endAddress) {
	ssd1306_send_command3(0x21, startAddress & 0x7F, endAddress & 0x7F);
}

void SSD1306Device::setPageAddress(uint8_t startPage, uint8_t endPage) {
	ssd1306_send_command3(0x22, startPage & 0x07, endPage & 0x07);
}

void SSD1306Device::setPageStartAddress(uint8_t startPage) {
	ssd1306_send_command(0xB0 | (startPage & 0x07));
}

// 4. Hardware Configuration (Panel resolution and layout related) Command Table

void SSD1306Device::setDisplayStartLine(uint8_t startLine) {
	ssd1306_send_command(0x40 | (startLine & 0x3F));
}

void SSD1306Device::setSegmentRemap(uint8_t remap) {
	ssd1306_send_command(0xA0 | (remap & 0x01));
}

void SSD1306Device::setMultiplexRatio(uint8_t mux) {
	ssd1306_send_command2(0xA8, (mux - 1) & 0x3F);
}

void SSD1306Device::setComOutputDirection(uint8_t direction) {
	ssd1306_send_command(0xC0 | ((direction & 0x01)<<3));
}

void SSD1306Device::setDisplayOffset(uint8_t offset) {
	ssd1306_send_command2(0xD3, offset & 0x3F);
}

void SSD1306Device::setComPinsHardwareConfiguration(uint8_t alternative, uint8_t enableLeftRightRemap) {
	ssd1306_send_command2(0xDA, ((enableLeftRightRemap & 0x01) << 5) | ((alternative & 0x01) << 4) | 0x02 );
}

// 5. Timing and Driving Scheme Setting Command table

void SSD1306Device::setDisplayClock(uint8_t divideRatio, uint8_t oscillatorFrequency) {
	ssd1306_send_command2(0xD5, ((oscillatorFrequency & 0x0F) << 4) | ((divideRatio -1) & 0x0F));
}

void SSD1306Device::setPrechargePeriod(uint8_t phaseOnePeriod, uint8_t phaseTwoPeriod) {
	ssd1306_send_command2(0xD9, ((phaseTwoPeriod & 0x0F) << 4) | (phaseOnePeriod & 0x0F));
}

void SSD1306Device::setVcomhDeselectLevel(uint8_t level) {
	ssd1306_send_command2(0xDB, (level & 0x07) << 4);
}

void SSD1306Device::nop(void) {
	ssd1306_send_command(0xE3);
}

// 6. Advance Graphic Command table

void SSD1306Device::fadeOut(uint8_t interval) {
	ssd1306_send_command2(0x23, (0x20 | (interval & 0x0F)));
}

void SSD1306Device::blink(uint8_t interval) {
	ssd1306_send_command2(0x23, (0x30 | (interval & 0x0F)));
}

void SSD1306Device::disableFadeOutAndBlinking(void) {
	ssd1306_send_command2(0x23, 0x00);
}

void SSD1306Device::enableZoomIn(void) {
	ssd1306_send_command2(0xD6, 0x01);
}

void SSD1306Device::disableZoomIn(void) {
	ssd1306_send_command2(0xD6, 0x00);
}

// Charge Pump Settings

void SSD1306Device::enableChargePump(void) {
	ssd1306_send_command2(0x8D, 0x14);
}

void SSD1306Device::disableChargePump(void) {
	ssd1306_send_command2(0x8D, 0x10);
}

// ----------------------------------------------------------------------------
