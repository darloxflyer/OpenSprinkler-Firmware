// LinuxSSD1306Display.h
#pragma once

#include <cstdint>
#include <cstddef>
#include "SSD1306_OLED.hpp"

#define OLEDwidth 128
#define OLEDheight 64
#define FULLSCREEN (OLEDwidth * (OLEDheight/8))

class LinuxSSD1306Display {
public:
    /// @param width       Display width in pixels (e.g. 128)
    /// @param height      Display height in pixels (e.g. 64)
    /// @param i2c_speed   BCM2835 I2C clock divider (0 = default ~100 kHz)
    /// @param i2c_address 7-bit I2C address of the OLED (default SSD1306_ADDR)
    LinuxSSD1306Display(uint8_t width = 128,
                        uint8_t height = 64,
                        uint16_t i2c_speed = 0,
                        uint8_t i2c_address = SSD1306_ADDR);
    ~LinuxSSD1306Display();

    /// Initialize bcm2835 + OLED, and register the framebuffer
    bool begin(bool debug = false);

    /// Clear the internal framebuffer
    void clear();

    /// Push the framebuffer out to the panel
    void display();

    /// Draw a monochrome XBM bitmap into the framebuffer
    void drawXbm(int16_t x, int16_t y,
                 int16_t w, int16_t h,
                 const uint8_t* bitmap,
                 bool invert);

    /// Move the text cursor to (x,y) in pixels
    void setCursor(int16_t x, int16_t y);

    /// Print a C-string at the current cursor
    size_t print(const char* text);
    

private:
    SSD1306    oled_;        ///< upstream driver
    uint16_t   speed_;       ///< I2C clock divider
    uint8_t    address_;     ///< I2C address
    bool       debug_;       ///< debug flag
    bool       flipped_;     ///< Flip display 180-degrees

    uint8_t    buffer_[FULLSCREEN];      ///< our framebuffer
    size_t     bufferSize_ = FULLSCREEN;  ///< = width * (height/8)
    uint8_t    width_, height_;
};

extern LinuxSSD1306Display lcd;

bool LCD_INIT(LinuxSSD1306Display &lcd);
void LCD_CLEAR(LinuxSSD1306Display &lcd);
void LCD_SET_CURSOR(LinuxSSD1306Display &lcd, uint8_t col, uint8_t row);
void LCD_SET_CURSOR_LINE(LinuxSSD1306Display &lcd, uint8_t col, uint8_t line);
void LCD_PRINT(LinuxSSD1306Display &lcd, const char* s);
void LCD_PRINT_NUMBER(LinuxSSD1306Display &lcd, int n);
void LCD_DISPLAY(LinuxSSD1306Display &lcd);
void LCD_DRAW_XBM(LinuxSSD1306Display &lcd, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap, bool invert);
