/*
 * Tiny4kOLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays
 *
 * Based on ssd1306xled, re-written and extended by Stephen Denne
 * from 2017-04-25 at https://github.com/datacute/Tiny4kOLED
 *
 */
/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * Copyright (c) 2016 Neven Boyanov, Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Please, as a favor, retain the link http://tinusaur.org to The Tinusaur Project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */

/****************************************************************
*
* ���ļ����ڴ�����룬ASCII�ַ�����ʹ�ñ��ļ����ṩ�ģ�
* ���ֺ�ͼƬ���밴���·�ʽȡģ��������������ʽ�����������;
* �뽫û���õ�������ע�͵����������Խ�ԼFLASH��
* 
*****************************************************************/

#include <avr/pgmspace.h>

/* 16*16 ����ȡ�ַ�ʽ��������������ʽ����������� */
const uint8_t ssd1306xled_font16x16cn [] PROGMEM = {
	0x00,0x02,0x02,0xFA,0xFA,0xAA,0xAA,0xFF,0xFF,0xAA,0xAA,0xFA,0xFA,0x02,0x02,0x00,
	0x00,0x42,0x72,0x72,0x3A,0x7A,0x42,0x4B,0x5B,0x52,0x62,0x62,0x13,0x77,0x66,0x00, // "��",0

	0x20,0x3C,0x1C,0xFF,0xFF,0xB0,0xB4,0x24,0x24,0x3F,0x3F,0xE4,0xE4,0x24,0x24,0x20,
	0x02,0x02,0x03,0xFF,0xFF,0x00,0x01,0x05,0x1D,0x59,0xC1,0xFF,0x7F,0x01,0x01,0x01, // "��",1

	0x00,0x00,0x00,0xF8,0xF8,0x48,0x4C,0x4F,0x4B,0x4A,0x48,0x48,0xF8,0xF8,0x00,0x00,
	0x00,0x00,0x00,0xFF,0xFF,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0xFF,0xFF,0x00,0x00, // "��",2

	0x20,0x24,0x24,0xE4,0xE4,0x24,0x24,0x24,0x30,0x10,0xFF,0xFF,0x10,0xF0,0xF0,0x00,
	0x08,0x1C,0x1F,0x0B,0x0C,0x0D,0x4F,0x6E,0x34,0x1C,0x0F,0x23,0x60,0x7F,0x3F,0x00, // "��",3

	0x80,0xC0,0x60,0xF8,0xFF,0x07,0x02,0x00,0xFF,0xFF,0xE0,0x70,0x3C,0x1C,0x08,0x00,
	0x00,0x00,0x00,0x7F,0x7F,0x04,0x06,0x03,0x3F,0x7F,0x40,0x40,0x40,0x78,0x78,0x00, // "��",4
};


const DCfont TinyOLED4kfont16x16cn = {
  (uint8_t *)ssd1306xled_font16x16cn,
  16, // character width in pixels
  2, // character height in pages (8 pixels)
  48,52, // ASCII extents
  0,0,0 // Unused for fixed width fonts
  };

