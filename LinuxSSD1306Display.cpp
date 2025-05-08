// LinuxSSD1306Display.cpp
#include "LinuxSSD1306Display.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

LinuxSSD1306Display::LinuxSSD1306Display(uint8_t width,
                                         uint8_t height,
                                         uint16_t i2c_speed,
                                         uint8_t i2c_address)
  : oled_(width, height)
  , speed_(i2c_speed)
  , address_(i2c_address)
  , debug_(false)
  , buffer_(nullptr)
  , bufferSize_(static_cast<size_t>(width) * (height / 8))
  , width_(width)
  , height_(height)
{}

LinuxSSD1306Display::~LinuxSSD1306Display() {
    // deregister and free
    if (buffer_) {
        delete[] buffer_;
        buffer_ = nullptr;
    }
    oled_.OLED_I2C_OFF();
    bcm2835_close();
}

bool LinuxSSD1306Display::begin(bool debug) {
    debug_ = debug;
    if (!bcm2835_init()) {
        std::fprintf(stderr, "bcm2835_init() failed\n");
        return false;
    }
    if (!oled_.OLED_I2C_ON()) {
        std::fprintf(stderr, "OLED_I2C_ON() failed\n");
        bcm2835_close();
        return false;
    }
    // perform controller init
    oled_.OLEDbegin(speed_, address_, debug_);

    // allocate and register our framebuffer
    buffer_ = new (std::nothrow) uint8_t[bufferSize_];
    if (!buffer_) {
        std::fprintf(stderr, "Failed to allocate OLED buffer (%zu bytes)\n", bufferSize_);
        return false;
    }
    // clear it initially
    std::memset(buffer_, 0x00, bufferSize_);
    if (!oled_.OLEDSetBufferPtr(width_, height_, buffer_, static_cast<uint16_t>(bufferSize_))) {
        std::fprintf(stderr, "OLEDSetBufferPtr() failed\n");
        return false;
    }

    return true;
}

void LinuxSSD1306Display::clear() {
    oled_.OLEDclearBuffer();
}

void LinuxSSD1306Display::display() {
    oled_.OLEDupdate();
}

void LinuxSSD1306Display::drawXbm(int16_t x, int16_t y,
                                  int16_t w, int16_t h,
                                  const uint8_t* bitmap) {
    oled_.OLEDBitmap(x, y, w, h, bitmap, false);
}

void LinuxSSD1306Display::setCursor(int16_t x, int16_t y) {
    oled_.setCursor(x, y);
}

size_t LinuxSSD1306Display::print(const char* text) {
    return oled_.print(text);
}
