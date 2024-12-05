#ifndef SSD1306_DISPLAY_H
#define SSD1306_DISPLAY_H

#if defined(USE_SSD1306)

#include <iostream>
#include <ctime>
#include <bcm2835.h>
#include "SSD1306_OLED.hpp"
#include "Bitmap_test_data.hpp" // Test data for bitmaps

#define myOLEDwidth  128
#define myOLEDheight 64
#define FULLSCREEN (myOLEDwidth * (myOLEDheight/8))
uint8_t screenBuffer[FULLSCREEN];
#define LCD_STD 0  // Standard LCD
#define LCD_I2C 1

class SSD1306Display : public SSD1306 {
public:
	SSD1306Display(int16_t oledwidth, int16_t oledheight) : SSD1306(oledwidth, oledheight) {
		cx = 0;
		cy = 0;
		for (unsigned char i = 0;i < NUM_CUSTOM_ICONS;i++) custom_chars[i] = NULL;
	}
	void begin() {
		const uint16_t I2C_Speed = BCM2835_I2C_CLOCK_DIVIDER_626; //  bcm2835I2CClockDivider enum , see readme.
		const uint8_t I2C_Address = 0x3C;
		bool I2C_debug = false;
		SSD1306Display::OLED_I2C_ON();
		bcm2835_delay(500);
		SSD1306Display::OLEDbegin(I2C_Speed, I2C_Address, I2C_debug);
		SSD1306Display::OLEDInvert(1);
	}
	void clear() {
		SSD1306::clear();
	}
	void clear(int start, int end) {
		SSD1306Display::fillRect(0, 0, 128, 64, BLACK);
	}

	uint8_t type() { return LCD_I2C; }
	void noBlink() {/*no support*/ }
	void blink() {/*no support*/ }
	void setCursor(uint8_t col, int8_t row) {
		/* assume 4 lines, the middle two lines
			 are row 0 and 1 */
		cy = (row + 1) * fontHeight;
		cx = col * fontWidth;
	}
	void noBacklight() {/*no support*/ }
	void backlight() {/*no support*/ }
	size_t write(uint8_t c) {
		setColor(BLACK);
		fillRect(cx, cy, fontWidth, fontHeight);
		setColor(WHITE);

		if (c < NUM_CUSTOM_ICONS && custom_chars[c] != NULL) {
			drawXbm(cx, cy, fontWidth, fontHeight, (const unsigned char*)custom_chars[c]);
		}
		else {
			drawString(cx, cy, String((char)c));
		}
		cx += fontWidth;
		if (auto_display) display();  // todo: not very efficient
		return 1;
	}
	size_t write(const char* s) {
		uint8_t nc = strlen(s);
		setColor(BLACK);
		fillRect(cx, cy, fontWidth * nc, fontHeight);
		setColor(WHITE);
		drawString(cx, cy, String(s));
		cx += fontWidth * nc;
		if (auto_display) display();	// todo: not very efficient
		return nc;
	}
	void createChar(unsigned char idx, PGM_P ptr) {
		if (idx >= 0 && idx < NUM_CUSTOM_ICONS) custom_chars[idx] = ptr;
	}
	void setAutoDisplay(bool v) { auto_display = v; }
private:
	bool auto_display = true;
	uint8_t cx, cy;
	uint8_t fontWidth, fontHeight;
	PGM_P custom_chars[NUM_CUSTOM_ICONS];
};


#endif

#endif // SSD1306_DISPLAY_H

