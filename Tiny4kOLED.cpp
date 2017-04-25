/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */

// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <avr/io.h>

#include <avr/pgmspace.h>

#include "Tiny4kOLED.h"
#include "font6x8.h"

#ifndef _nofont_8x16	// tBUG Optional removal to save code space
#include "font8x16.h"
#endif

#define SSD1306_PAGES 4
#define SSD1306_MAX_PAGE 3
#define SSD1306_MAX_PAGE_8x16 2

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0x40

// ----------------------------------------------------------------------------

// Some code based on "IIC_wtihout_ACK" by http://www.14blog.com/archives/1358

const uint8_t ssd1306_init_sequence [] PROGMEM = {	// Initialization Sequence
	0xAE,			// Display OFF (sleep mode)
	0x20, 0b00,		// Set Memory Addressing Mode
					// 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
					// 10=Page Addressing Mode (RESET); 11=Invalid
	0xB0,			// Set Page Start Address for Page Addressing Mode, 0-7
	0xC8,			// Set COM Output Scan Direction
	0x00,			// ---set low column address
	0x10,			// ---set high column address
	0x40,			// --set start line address
	0x81, 0x8F,		// Set contrast control register
	0xA1,			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped. 
	0xA6,			// Set display mode. A6=Normal; A7=Inverse
	0xA8, 0x1F,		// Set multiplex ratio(1 to 64)
	0xA4,			// Output RAM to Display
					// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
	0xD3, 0x00,		// Set display offset. 00 = no offset
	0xD5, 0x80,		// --set display clock divide ratio/oscillator frequency
	0xD9, 0x22,		// Set pre-charge period
	0xDA, 0x02,		// Set com pins hardware configuration		
	0xDB,			// --set vcomh
	0x20,			// 0x20,0.77xVcc
	0x8D, 0x14,		// Set DC-DC enable
	0xAF			// Display ON in normal mode
	
};

uint8_t oledFont, oledX, oledY = 0;

// Program:    5248 bytes

SSD1306Device::SSD1306Device(void){}


void SSD1306Device::begin(void)
{
	TinyWireM.begin();

	ssd1306_send_start(SSD1306_COMMAND);	
	for (uint8_t i = 0; i < sizeof (ssd1306_init_sequence); i++) {
		ssd1306_send_byte(SSD1306_COMMAND, pgm_read_byte(&ssd1306_init_sequence[i]));
	}
	ssd1306_send_stop();
	clear();
}


void SSD1306Device::setFont(uint8_t font)
{
	oledFont = font;
}

void SSD1306Device::ssd1306_send_start(uint8_t transmission_type) {
	TinyWireM.beginTransmission(SSD1306);
	TinyWireM.write(transmission_type);
}

void SSD1306Device::ssd1306_send_stop(void) {
	TinyWireM.endTransmission();
}

void SSD1306Device::ssd1306_send_byte(uint8_t transmission_type, uint8_t byte)
{
	if(TinyWireM.write(byte) == 0){
		ssd1306_send_stop();
		ssd1306_send_start(transmission_type);
		TinyWireM.write(byte);
	}
}

void SSD1306Device::ssd1306_send_command(uint8_t command)
{
	ssd1306_send_start(SSD1306_COMMAND);
	TinyWireM.write(command);
	ssd1306_send_stop();
}

void SSD1306Device::setCursor(uint8_t x, uint8_t y)
{
	ssd1306_send_start(SSD1306_COMMAND);
	TinyWireM.write(0xb0 + y);
	TinyWireM.write(((x & 0xf0) >> 4) | 0x10);
	TinyWireM.write(x & 0x0f);
	ssd1306_send_stop();
	oledX = x;
	oledY = y;
}

void SSD1306Device::clear(void)
{
	fill(0x00);
}

void SSD1306Device::fill(uint8_t fill)
{
	uint8_t m,n;
	for (m = 0; m < SSD1306_PAGES; m++)
	{
		ssd1306_send_command(0xb0 + m);	// page0 - page1
		ssd1306_send_command(0x00);		// low column start address
		ssd1306_send_command(0x10);		// high column start address
		ssd1306_send_start(SSD1306_DATA);
		for (n = 0; n < 128; n++)
		{
			ssd1306_send_byte(SSD1306_DATA, fill);
		}
		ssd1306_send_stop();
	}
	setCursor(0, 0);
}

size_t SSD1306Device::write(byte c) {
	uint8_t i = 0;
	uint8_t ci = c - 32;
	if(c == '\r')
		return 1;
	if(c == '\n'){
		if(oledFont == FONT6X8) {	//	tBUG
			oledY++;	
//			if ( oledY > SSD1306_MAX_PAGE) // tBUG
//				oledY = SSD1306_MAX_PAGE;
			}
		else {
			oledY+=2;	//tBUG  Large Font up by two
			if ( oledY > SSD1306_MAX_PAGE_8x16) // tBUG
				oledY = SSD1306_MAX_PAGE_8x16;
			}
		setCursor(0, oledY);
		return 1;
	}

	if(oledFont == FONT6X8){
		if (oledX > 122)
		{
			oledX = 0;
			oledY++;
			if ( oledY > SSD1306_MAX_PAGE) // tBUG
				oledY = SSD1306_MAX_PAGE;
			setCursor(oledX, oledY);
		}
		
		ssd1306_send_start(SSD1306_DATA);
		for (i= 0; i < 6; i++)
		{
			ssd1306_send_byte(SSD1306_DATA, pgm_read_byte(&ssd1306xled_font6x8[ci * 6 + i]));
		}
		ssd1306_send_stop();
		setCursor(oledX+6, oledY);
	}
#ifndef _nofont_8x16	// tBUG
	else{
		if (oledX > 120)
		{
			oledX = 0;
			oledY+=2;	//tBUG  Large Font up by two
//			oledY++;	
			if ( oledY > SSD1306_MAX_PAGE_8x16) // tBUG
				oledY = SSD1306_MAX_PAGE_8x16;
			setCursor(oledX, oledY);
		}
		
		ssd1306_send_start(SSD1306_DATA);
		for (i = 0; i < 8; i++)
		{
			TinyWireM.write(pgm_read_byte(&ssd1306xled_font8x16[ci * 16 + i]));
		}
		ssd1306_send_stop();
		setCursor(oledX, oledY + 1);
		ssd1306_send_start(SSD1306_DATA);
		for (i = 0; i < 8; i++)
		{
			TinyWireM.write(pgm_read_byte(&ssd1306xled_font8x16[ci * 16 + i + 8]));
		}
		ssd1306_send_stop();
		setCursor(oledX + 8, oledY - 1);
	}
#endif
	return 1;
}




void SSD1306Device::bitmap(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[])
{
	uint16_t j = 0;
	uint8_t y, x;
	// if (y1 % 8 == 0) y = y1 / 8; 	// else y = y1 / 8 + 1;		// tBUG :: this does nothing as y is initialized below
	//	THIS PARAM rule on y makes any adjustment here WRONG   //usage oled.bitmap(START X IN PIXELS, START Y IN ROWS OF 8 PIXELS, END X IN PIXELS, END Y IN ROWS OF 8 PIXELS, IMAGE ARRAY);
 	for (y = y0; y < y1; y++)
	{
		setCursor(x0,y);
		ssd1306_send_start(SSD1306_DATA);
		for (x = x0; x < x1; x++)
		{
			ssd1306_send_byte(SSD1306_DATA, pgm_read_byte(&bitmap[j++]));
		}
		ssd1306_send_stop();
	}
	setCursor(0, 0);
}


SSD1306Device oled;

// ----------------------------------------------------------------------------
