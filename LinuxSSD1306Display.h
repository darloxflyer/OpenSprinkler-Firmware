// LinuxSSD1306Display.h
#pragma once

#include <cstdint>
#include <cstddef>
#include <bcm2835.h>
#include "SSD1306_OLED.hpp"

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
                 const uint8_t* bitmap);

    /// Move the text cursor to (x,y) in pixels
    void setCursor(int16_t x, int16_t y);

    /// Print a C-string at the current cursor
    size_t print(const char* text);

private:
    SSD1306    oled_;        ///< upstream driver
    uint16_t   speed_;       ///< I2C clock divider
    uint8_t    address_;     ///< I2C address
    bool       debug_;       ///< debug flag

    uint8_t*   buffer_;      ///< our framebuffer
    size_t     bufferSize_;  ///< = width * (height/8)
    uint8_t    width_, height_;
};
