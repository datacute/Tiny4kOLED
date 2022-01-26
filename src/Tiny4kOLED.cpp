/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */

// ----------------------------------------------------------------------------

#include "Tiny4kOLED_common.h"

#define SSD1306_COLUMNS 128
#define SSD1306_PAGES 4

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0x40

// ----------------------------------------------------------------------------

static uint8_t oledOffsetX = 0, oledOffsetY = 0; // pixels and pages
static uint8_t oledWidth = SSD1306_COLUMNS; // pixels and pages
static uint8_t oledPages = SSD1306_PAGES;
static const DCUnicodeFont *oledUnicodeFont = 0;
static uint8_t unicodeFontNum = 0;
static const DCfont *oledFont = 0;
static uint8_t oledX = 0, oledY = 0;
static uint8_t renderingFrame = 0xB0, drawingFrame = 0x40;
static uint8_t doubleSize = 0;
static uint8_t characterSpacing = 0;
static DCUnicodeCodepoint unicodeCodepoint = { 0 };
static uint8_t utf8Continuation = 0;

static void (*wireBeginFn)(void);
static bool (*wireBeginTransmissionFn)(void);
static bool (*wireWriteFn)(uint8_t byte);
static uint8_t (*wireEndTransmissionFn)(void);

static void (SSD1306Device::* decodeFn)(uint8_t c) = 0;
static void (SSD1306Device::* renderFn)(uint8_t c) = 0;
static uint8_t (*combineFn)(uint8_t x, uint8_t y, uint8_t b) = 0;
static uint8_t writesSinceSetCursor = 0;

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
	if (combineFn) byte = (*combineFn)(oledX + writesSinceSetCursor, oledY, byte);
	if (ssd1306_send_byte(byte) == 0) {
		ssd1306_send_stop();
		ssd1306_send_data_start();
		ssd1306_send_byte(byte);
	}
	writesSinceSetCursor++;
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

static uint8_t invertByte(uint8_t x, uint8_t y, uint8_t byte) {
	return byte ^ 0xff;
}

SSD1306Device::SSD1306Device(void (*wireBeginFunc)(void), bool (*wireBeginTransmissionFunc)(void), bool (*wireWriteFunc)(uint8_t byte), uint8_t (*wireEndTransmissionFunc)(void)) {
	wireBeginFn = wireBeginFunc;
	wireBeginTransmissionFn = wireBeginTransmissionFunc;
	wireWriteFn = wireWriteFunc;
	wireEndTransmissionFn = wireEndTransmissionFunc;
}

void SSD1306Device::begin(void) {
	begin(sizeof(tiny4koled_init_128x32r), tiny4koled_init_128x32r);
}

void SSD1306Device::begin(uint8_t init_sequence_length, const uint8_t init_sequence []) {
	ssd1306_begin();

	ssd1306_send_command_start();
	for (uint8_t i = 0; i < init_sequence_length; i++) {
		ssd1306_send_command_byte(pgm_read_byte(&init_sequence[i]));
	}
	ssd1306_send_stop();
}

void SSD1306Device::begin(uint8_t width, uint8_t height, uint8_t init_sequence_length, const uint8_t init_sequence []) {
	oledOffsetX = (128 - width) >> 1;
	oledOffsetY = 0;
	oledWidth = width;
	oledPages = height >> 3;
	begin(init_sequence_length,init_sequence);
}

void SSD1306Device::begin(uint8_t xOffset, uint8_t yOffset, uint8_t width, uint8_t height, uint8_t init_sequence_length, const uint8_t init_sequence []) {
	oledOffsetX = xOffset;
	oledOffsetY = yOffset >> 3;
	oledWidth = width;
	oledPages = height >> 3;
	begin(init_sequence_length,init_sequence);
}

void SSD1306Device::setPages(uint8_t pages) {
	oledPages = pages;
}

void SSD1306Device::setWidth(uint8_t width) {
	oledWidth = width;
}

void SSD1306Device::setHeight(uint8_t height) {
	oledPages = height >> 3;
}

void SSD1306Device::setOffset(uint8_t xOffset, uint8_t yOffset) {
	oledOffsetX = xOffset;
	oledOffsetY = yOffset >> 3;
}

void SSD1306Device::setRotation(uint8_t rotation) {
	uint8_t rotationBit = (rotation & 0x01);
	ssd1306_send_command2(0xC0 | (rotationBit << 3), 0xA0 | rotationBit);
}

void SSD1306Device::setFont(const DCfont *font) {
	setFontOnly(font);
	decodeFn = &SSD1306Device::decodeAsciiInternal;
	renderFn = &SSD1306Device::renderOriginalSize;
	doubleSize = 0;
}

void SSD1306Device::setUnicodeFont(const DCUnicodeFont *unicode_font) {
	oledUnicodeFont = unicode_font;
	unicodeFontNum = 0;
	setFontOnly(oledUnicodeFont->fonts[unicodeFontNum].font);
	decodeFn = &SSD1306Device::decodeUtf8Internal;
	renderFn = &SSD1306Device::renderOriginalSize;
	doubleSize = 0;
}

void SSD1306Device::setFontX2(const DCfont *font) {
	setFontOnly(font);
	decodeFn = &SSD1306Device::decodeAsciiInternal;
	renderFn = &SSD1306Device::renderDoubleSize;
	doubleSize = 1;
}

void SSD1306Device::setUnicodeFontX2(const DCUnicodeFont *unicode_font) {
	oledUnicodeFont = unicode_font;
	unicodeFontNum = 0;
	setFontOnly(oledUnicodeFont->fonts[unicodeFontNum].font);
	decodeFn = &SSD1306Device::decodeUtf8Internal;
	renderFn = &SSD1306Device::renderDoubleSize;
	doubleSize = 1;
}

void SSD1306Device::setFontX2Smooth(const DCfont *font) {
	setFontOnly(font);
	decodeFn = &SSD1306Device::decodeAsciiInternal;
	renderFn = &SSD1306Device::renderDoubleSizeSmooth;
	doubleSize = 1;
}

void SSD1306Device::setUnicodeFontX2Smooth(const DCUnicodeFont *unicode_font) {
	oledUnicodeFont = unicode_font;
	unicodeFontNum = 0;
	setFontOnly(oledUnicodeFont->fonts[unicodeFontNum].font);
	decodeFn = &SSD1306Device::decodeUtf8Internal;
	renderFn = &SSD1306Device::renderDoubleSizeSmooth;
	doubleSize = 1;
}

void SSD1306Device::setFontOnly(const DCfont *font) {
	oledFont = font;
	if (font->width == 0)
		characterSpacing = font->spacing;
	else
		characterSpacing = 0;
}

void SSD1306Device::setSpacing(uint8_t spacing) {
	characterSpacing = spacing;
}

void SSD1306Device::setCombineFunction(uint8_t (*combineFunc)(uint8_t, uint8_t, uint8_t)) {
	combineFn = combineFunc;
}

uint16_t SSD1306Device::getCharacterDataOffset(uint8_t c) {
	uint16_t c_index = (uint16_t)c - oledFont->first;
	if (c_index == 0) return 0;

	uint8_t w = oledFont->width;
	uint8_t h = oledFont->height;

	if (w > 0) return c_index * w * h;

	uint16_t offset = 0;
	uint16_t c_index16s = c_index >> 4;
	for (uint16_t pre_c = 0; pre_c < c_index16s; pre_c++)
	{
		uint16_t w16 = pgm_read_word(&(oledFont->widths16s[pre_c]));
		offset += w16;
	}
	for (uint16_t pre_c = c_index16s << 4; pre_c < c_index; pre_c++)
	{
		w = pgm_read_byte(&(oledFont->widths[pre_c]));
		offset += w;
	}
	return offset * h;
}

uint8_t SSD1306Device::getCharacterWidth(uint8_t c) {
	uint8_t w = oledFont->width;
	if (w == 0) {
		uint16_t c_index = (uint16_t)c - oledFont->first;
		w = pgm_read_byte(&(oledFont->widths[c_index]));
	}
	return w;
}

uint16_t SSD1306Device::getTextWidth(DATACUTE_F_MACRO_T *text) {
	PGM_P p = reinterpret_cast<PGM_P>(text);
	uint16_t totalWidth = 0;

	while (true) {
		unsigned char c = pgm_read_byte(p++);
		if (c == 0) break;
		totalWidth += getCharacterWidth(c);
		totalWidth += characterSpacing; // every character ends with whitespace
	}

	return totalWidth;
}

void SSD1306Device::setCursor(uint8_t x, uint8_t y) {
	ssd1306_send_command3(renderingFrame | ((y + oledOffsetY) & 0x07), 0x10 | (((x + oledOffsetX) & 0xf0) >> 4), (x + oledOffsetX) & 0x0f);
	oledX = x;
	oledY = y;
	writesSinceSetCursor = 0;
}

uint8_t SSD1306Device::getCursorX() {
	return oledX;
}

uint8_t SSD1306Device::getCursorY() {
	return oledY;
}

void SSD1306Device::clear(void) {
	fill(0x00);
}

void SSD1306Device::fill(uint8_t fill) {
	for (uint8_t m = 0; m < oledPages; m++) {
		setCursor(0, m);
		fillToEOP(fill);
	}
	setCursor(0, 0);
}

void SSD1306Device::newLine(uint8_t fontHeight) {
	uint8_t h = fontHeight << doubleSize;
	oledY+=h;
	if (oledY > oledPages - h) {
		oledY = oledPages - h;
	}
	setCursor(0, oledY);
}

void SSD1306Device::newLine(void) {
	newLine(oledFont->height);
}

size_t SSD1306Device::write(byte c) {
	if (decodeFn) (this->*decodeFn)(c);
	return 1;
}

void SSD1306Device::decodeAsciiInternal(uint8_t c) {
	if (c == '\r')
		return;

	if (c == '\n') {
		newLine(oledFont->height);
		return;
	}

	if (oledFont->first <= c &&
	    oledFont->last >= c)
  	(this->*renderFn)(c);
}

void SSD1306Device::renderOriginalSize(uint8_t c) {
	uint8_t w = getCharacterWidth(c);
	uint8_t h = oledFont->height;
	uint8_t spacing = characterSpacing;

	if (oledX > ((uint8_t)oledWidth - w)) {
		newLine(h);
	}

	if (oledX + w + spacing > (uint8_t)oledWidth) {
		spacing = 0;
	}

	uint16_t offset = getCharacterDataOffset(c);
	uint8_t line = h;
	do
	{
		ssd1306_send_data_start();
		for (uint8_t i = 0; i < w; i++) {
			ssd1306_send_data_byte(pgm_read_byte(&(oledFont->bitmap[offset++])));
		}
		repeatData(0, spacing);
		ssd1306_send_stop();
		if (line > 1) {
			setCursor(oledX, oledY + 1);
		}
		else {
			setCursor(oledX + w + spacing, oledY - (h - 1));
		}
	}
	while (--line);
}

uint8_t SSD1306Device::getExpectedUtf8Bytes(void) {
	return utf8Continuation;
}

void SSD1306Device::RenderUnicodeSpace(void) {
	uint8_t spaceWidth = (oledUnicodeFont->space_width + characterSpacing) << doubleSize;
	if (oledX > ((uint8_t)oledWidth - spaceWidth)) {
		newLine(oledFont->height);
	} else {
		uint8_t textHeight = oledFont->height << doubleSize;
		uint8_t line = textHeight;
		do
		{
			ssd1306_send_data_start();
			repeatData(0, spaceWidth);
			ssd1306_send_stop();
			if (line > 1) {
				setCursor(oledX, oledY + 1);
			}
			else {
				setCursor(oledX + spaceWidth, oledY - (textHeight - 1));
			}
		}
		while (--line);
	}
}

bool SSD1306Device::SelectUnicodeBlock(void) {
	if (oledUnicodeFont->fonts[unicodeFontNum].unicode_block == unicodeCodepoint.unicode.block && 
	    oledUnicodeFont->fonts[unicodeFontNum].unicode_plane == unicodeCodepoint.unicode.plane &&
	    oledUnicodeFont->fonts[unicodeFontNum].font->first <= unicodeCodepoint.unicode.offset &&
	    oledUnicodeFont->fonts[unicodeFontNum].font->last >= unicodeCodepoint.unicode.offset) return true;
	uint8_t fontNum = unicodeFontNum + 1;
	if (fontNum == oledUnicodeFont->num_fonts) fontNum = 0;
	while(fontNum != unicodeFontNum) {
		if (oledUnicodeFont->fonts[fontNum].unicode_block == unicodeCodepoint.unicode.block && 
		    oledUnicodeFont->fonts[fontNum].unicode_plane == unicodeCodepoint.unicode.plane &&
		    oledUnicodeFont->fonts[fontNum].font->first <= unicodeCodepoint.unicode.offset &&
		    oledUnicodeFont->fonts[fontNum].font->last >= unicodeCodepoint.unicode.offset) {
			unicodeFontNum = fontNum;
			setFontOnly(oledUnicodeFont->fonts[unicodeFontNum].font);
			return true;
		}
		if (fontNum++ == oledUnicodeFont->num_fonts) fontNum = 0;
	}
	return false;
}

void SSD1306Device::decodeUtf8Internal(uint8_t utf8byte) {
	if ((utf8byte & 0x80) == 0x00) { // U+0000 to U+007F (most common?)
		unicodeCodepoint.codepoint = utf8byte;
		utf8Continuation = 0;
	}
	else if ((utf8byte & 0xC0) == 0x80) { // continuation byte (second most common?)
		unicodeCodepoint.codepoint = (unicodeCodepoint.codepoint << 6) | (utf8byte & 0x3F);
		utf8Continuation--;
	}
	else if ((utf8byte & 0xE0) == 0xC0) { // U+0080 to U+07FF (third most common?)
		unicodeCodepoint.codepoint = utf8byte & 0x1F;
		utf8Continuation = 1;
	}
	else if ((utf8byte & 0xF0) == 0xE0) { // U+0800 to U+FFFF
		unicodeCodepoint.codepoint = utf8byte & 0x0F;
		utf8Continuation = 2;
	}
	else if ((utf8byte & 0xF8) == 0xF0) { // U+10000 to U+10FFFF
		unicodeCodepoint.codepoint = utf8byte & 0x07;
		utf8Continuation = 3;
	}

	if (utf8Continuation != 0) return;

	uint32_t c = unicodeCodepoint.codepoint;

	// Don't switch blocks for control characters, nor space

	if (c == '\n') {
		newLine(oledFont->height);
		return;
	}

	if (c < ' ') return;

	if (c == ' ') {
		RenderUnicodeSpace();
		return;
	}

	// If none of the fonts support the current unicode block, don't output anything.
	if (SelectUnicodeBlock()) {
		(this->*renderFn)(unicodeCodepoint.unicode.offset);
	}
}

static uint16_t ReadCharacterBits(uint8_t * cPtr, uint8_t w) {
  uint16_t resultBits = pgm_read_byte(cPtr);
  if (oledFont->height > 1) {
    // change type before shifting, as otherwise signed ints will be used.
    resultBits |= (uint16_t)pgm_read_byte(cPtr + w) << 8;
  }
  return resultBits;
}

static uint32_t Stretch(uint16_t x) {
  uint32_t x32 = (uint32_t)x;
  x32 = (x32<<8 | x32) & 0x00FF00FF;
  x32 = (x32<<4 | x32) & 0x0F0F0F0F;        // 0000abcd____efgh -> 0000abcd0000efgh
  x32 = (x32<<2 | x32) & 0x33333333;        // 00ab__cd00ef__gh -> 00ab00cd00ef00gh
  x32 = (x32<<1 | x32) & 0x55555555;        // 0a_b0c_d0e_f0g_h -> 0a0b0c0d0e0f0g0h
  return x32 | x32<<1;                      // aabbccddeeffgghh
}

void SSD1306Device::sendDoubleBits(uint32_t doubleBits) {
  ssd1306_send_data_byte(doubleBits);
  ssd1306_send_data_byte(doubleBits>>8);
  if (oledFont->height > 1) {
    ssd1306_send_data_byte(doubleBits>>16);
    ssd1306_send_data_byte(doubleBits>>24);
  }
}

void SSD1306Device::renderDoubleSize(uint8_t c) {
	uint16_t offset = getCharacterDataOffset(c);
	uint8_t w = getCharacterWidth(c);
	uint8_t h = oledFont->height;

	// change memory mode to advance pages, before columns
	setMemoryAddressingMode(1);
	uint8_t topPage = oledY + (renderingFrame & 0x04);
	setPageAddress(topPage, topPage + h + h - 1);

	ssd1306_send_data_start();
	uint8_t * cPtr = &(oledFont->bitmap[offset]);
	for (uint8_t col = 0 ; col < w; col++) {
		uint16_t col0 = ReadCharacterBits(cPtr + col, w);
		uint32_t col0L = Stretch(col0);
		sendDoubleBits(col0L);
		sendDoubleBits(col0L);
	}
	ssd1306_send_stop();

	setMemoryAddressingMode(2);
	setPageAddress(0,7);

	setCursor(oledX + (w + characterSpacing) * 2 , oledY);
}

void SSD1306Device::renderDoubleSizeSmooth(uint8_t c) {
	uint16_t offset = getCharacterDataOffset(c);
	uint8_t w = getCharacterWidth(c);
	uint8_t h = oledFont->height;

	// change memory mode to advance pages, before columns
	setMemoryAddressingMode(1);
	uint8_t topPage = oledY + (renderingFrame & 0x04);
	setPageAddress(topPage, topPage + h + h - 1);

	ssd1306_send_data_start();

	uint8_t * cPtr = &(oledFont->bitmap[offset]);
	uint16_t col0 = ReadCharacterBits(cPtr, w);
	uint32_t col0L, col0R, col1L, col1R;
	col0L = Stretch(col0);
	col0R = col0L;
	for (uint8_t col = 1 ; col < w; col++) {
		uint16_t col1 = ReadCharacterBits(cPtr + col, w);
		col1L = Stretch(col1);
		col1R = col1L;
		for (uint8_t i=0; i<16; i++) { // (15 pairs of bits in 8 bit line)
			for (uint8_t j=1; j<3; j++) {
				if (((col0>>i & 0b11) == (uint8_t)(3-j)) && ((col1>>i & 0b11) == j)) {
					col0R |= (uint32_t)1<<((i*2)+j);
					col1L |= (uint32_t)1<<((i*2)+3-j);
				}
			}
		}
		sendDoubleBits(col0L);
		sendDoubleBits(col0R);
		col0L = col1L; col0R = col1R; col0 = col1;
	}
	sendDoubleBits(col0L);
	sendDoubleBits(col0R);
	ssd1306_send_stop();
	setMemoryAddressingMode(2);
	setPageAddress(0,7);
	setCursor(oledX + (w + characterSpacing) * 2 , oledY);
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
	uint8_t x = oledX;
	uint8_t pagesToClear = oledFont->height << doubleSize;
	do
	{
		fillToEOP(fill);
		if (pagesToClear > 1) {
			setCursor(x,oledY + 1);
		}
	}
	while (--pagesToClear);
}

void SSD1306Device::clearToEOP(void) {
	fillToEOP(0x00);
}

void SSD1306Device::fillToEOP(uint8_t fill) {
	fillLength(fill, oledWidth - oledX);
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

void SSD1306Device::invertOutput(bool enable) {
	combineFn = enable ? &invertByte : NULL;
}	

void SSD1306Device::clipText(uint16_t startPixel, uint8_t width, DATACUTE_F_MACRO_T *text) {
	uint8_t h = oledFont->height;
	uint8_t w = oledFont->width;
	PGM_P p = reinterpret_cast<PGM_P>(text);
	uint8_t drawnColumns = 0;
	// It is currently up to the caller to make sure that the startPixel is still within the text.
	// This method needs to change to read each character in the string up to startPixel, to check for end of string
	// If beyond the end of the string, write 0s/spaces
	// However, it probably takes less bytes currently to simply put spaces at the beginning or end of the text.
	uint16_t charactersToSkip = startPixel / w;
	uint8_t initialSkip = startPixel % w;
	p += charactersToSkip;
	while (drawnColumns < width) {
		unsigned char c = pgm_read_byte(p++);
		if (c == 0) break;
		uint16_t offset = ((uint16_t)c - oledFont->first) * w * h;
		uint8_t line = h;
		do
		{
			offset += initialSkip;
			ssd1306_send_data_start();
			for (uint8_t i = 0; (i < w - initialSkip) && ((drawnColumns + i) < width); i++) {
				ssd1306_send_data_byte(pgm_read_byte(&(oledFont->bitmap[offset + i])));
			}
			offset += w - initialSkip;
			ssd1306_send_stop();
			if (h > 1) {
				if (line > 1) {
					setCursor(oledX, oledY + 1);
				}
				else {
					setCursor(oledX + w, oledY - (h - 1));
				}
			}
		} while (--line);
		drawnColumns += w - initialSkip;
		initialSkip = 0;
	}
}

void SSD1306Device::clipTextP(uint16_t startPixel, uint8_t width, DATACUTE_F_MACRO_T *text) {
	uint8_t h = oledFont->height;
	PGM_P p = reinterpret_cast<PGM_P>(text);
	uint8_t drawnColumns = 0;
	// It is currently up to the caller to make sure that the startPixel is still within the text.
	// This method needs to change to read each character in the string up to startPixel, to check for end of string
	// If beyond the end of the string, write 0s/spaces
	// However, it probably takes less bytes currently to simply put spaces at the beginning or end of the text.
	uint16_t initialSkip = startPixel;
	while (drawnColumns < width) {
		unsigned char c = pgm_read_byte(p++);
		if (c == 0) break;

		uint8_t spacing = characterSpacing;
		uint8_t w = getCharacterWidth(c);

		// start drawing after character and inter-character spacing?
		if (initialSkip >= w + spacing) {
			initialSkip -= w + spacing;
			// Need to read the next character
			continue;
		}

		// do we just need to draw spacing?
		if (initialSkip >= w) {
			initialSkip -= w;
			//if (spacing > initialSkip) {
				spacing -= initialSkip;
			//}
			if (drawnColumns + spacing > width) {
				spacing = width - drawnColumns;
			}
			//if (spacing > 0) {
				uint8_t line = h;
				do
				{
					ssd1306_send_data_start();
					repeatData(0, spacing);
					ssd1306_send_stop();
					if (h > 1) {
						if (line > 1) {
							setCursor(oledX, oledY + 1);
						}
						else {
							setCursor(oledX + spacing, oledY - (h - 1));
						}
					}
				} while (--line);
				drawnColumns += spacing;
			//}
			initialSkip = 0;
			continue;
		}

		// need to only put spacing on the end, if there is enough room
		uint8_t endOfCharacter = drawnColumns + w - initialSkip;
		if (endOfCharacter > width) {
			spacing = 0;
		} else {
			if (endOfCharacter + spacing > width) {
				spacing = width - endOfCharacter;
			}
		}

		uint16_t offset = getCharacterDataOffset(c);
		uint8_t line = h;
		do
		{
			offset += initialSkip;
			ssd1306_send_data_start();
			for (uint8_t i = 0; (i < w - initialSkip) && ((drawnColumns + i) < width); i++) {
				ssd1306_send_data_byte(pgm_read_byte(&(oledFont->bitmap[offset + i])));
			}
			repeatData(0, spacing);
			ssd1306_send_stop();
			offset += w - initialSkip;
			if (h > 1) {
				if (line > 1) {
					setCursor(oledX, oledY + 1);
				}
				else {
					setCursor(oledX + w + spacing, oledY - (h - 1));
				}
			}
		} while (--line);
		drawnColumns += w - initialSkip + spacing;
		initialSkip = 0;
	}
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

void SSD1306Device::setExternalIref(void) {
	ssd1306_send_command2(0xAD, 0x00);
}

void SSD1306Device::setInternalIref(bool bright) {
	ssd1306_send_command2(0xAD, ((bright & 0x01) << 5) | 0x10);
}

void SSD1306Device::off(void) {
	ssd1306_send_command(0xAE);
}

void SSD1306Device::on(void) {
	ssd1306_send_command(0xAF);
}

// 2. Scrolling Command Table

void SSD1306Device::scrollRight(uint8_t startPage, uint8_t interval, uint8_t endPage, uint8_t startColumn, uint8_t endColumn) {
	ssd1306_send_command7(0x26, 0x00, startPage + oledOffsetY, interval, endPage + oledOffsetY, startColumn, endColumn);
}

void SSD1306Device::scrollLeft(uint8_t startPage, uint8_t interval, uint8_t endPage, uint8_t startColumn, uint8_t endColumn) {
	ssd1306_send_command7(0x27, 0x00, startPage + oledOffsetY, interval, endPage + oledOffsetY, startColumn, endColumn);
}

void SSD1306Device::scrollRightOffset(uint8_t startPage, uint8_t interval, uint8_t endPage, uint8_t offset) {
	ssd1306_send_command6(0x29, 0x00, startPage + oledOffsetY, interval, endPage + oledOffsetY, offset);
}

void SSD1306Device::scrollLeftOffset(uint8_t startPage, uint8_t interval, uint8_t endPage, uint8_t offset) {
	ssd1306_send_command6(0x2A, 0x00, startPage + oledOffsetY, interval, endPage + oledOffsetY, offset);
}

void SSD1306Device::scrollContentRight(uint8_t startPage, uint8_t endPage, uint8_t startColumn, uint8_t endColumn) {
	ssd1306_send_command7(0x2C, 0x00, startPage + oledOffsetY, 0x01, endPage + oledOffsetY, startColumn + oledOffsetX, endColumn + oledOffsetX);
}

void SSD1306Device::scrollContentLeft(uint8_t startPage, uint8_t endPage, uint8_t startColumn, uint8_t endColumn) {
	ssd1306_send_command7(0x2D, 0x00, startPage + oledOffsetY, 0x01, endPage + oledOffsetY, startColumn + oledOffsetX, endColumn + oledOffsetX);
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
	ssd1306_send_command(0xC0 | ((direction & 0x01) << 3));
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

void SSD1306Device::enableChargePump(uint8_t voltage) {
	ssd1306_send_command2(0x8D, ((voltage | 0x14) & 0xD5));
}

void SSD1306Device::disableChargePump(void) {
	ssd1306_send_command2(0x8D, 0x10);
}

// ----------------------------------------------------------------------------
