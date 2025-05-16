// LinuxSSD1306Display.cpp
// Wrapper for SSD1306_OLED_RPI library, modeled on the CLOCK_DEMO example

#include "LinuxSSD1306Display.h"
#include <bcm2835.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "SSD1306_OLED.hpp"   // from SSD1306_OLED_RPi library


// constants for text height + optional spacing
static const uint8_t LCD_FONT_HEIGHT   = 8;   // pixels
static const uint8_t LCD_LINE_SPACING  = 2;   // blank pixels between lines

LinuxSSD1306Display::LinuxSSD1306Display(uint8_t width,
                                         uint8_t height,
                                         uint16_t i2c_speed,
                                         uint8_t i2c_address)
  : oled_(width, height)
  , width_(width)
  , height_(height)
  , speed_(i2c_speed)
  , address_(i2c_address)
  , debug_(false)
  , flipped_(true)
{}

LinuxSSD1306Display::~LinuxSSD1306Display() {
    // Power down and close I²C, matching the example's EndTests()
    oled_.OLEDPowerDown();
    oled_.OLED_I2C_OFF();
    bcm2835_close();

}

bool LinuxSSD1306Display::begin(bool debug) {
    debug_ = debug;

    // 1) Initialize bcm2835 library
    if (!bcm2835_init()) {
        std::fprintf(stderr, "Error: bcm2835_init() failed\n");
        return false;
    }

    // 2) Turn on I²C bus
    if (!oled_.OLED_I2C_ON()) {
        std::fprintf(stderr, "Error: OLED_I2C_ON() failed (Need root?)\n");
        bcm2835_close();
        return false;
    }

    // Optional debug info
    //if (debug_) {
    //    std::fprintf(stderr,
    //        "SSD1306 lib v%u, bcm2835 v%s\n",
    //        oled_.getLibVerNum(),
    //        bcm2835_version());
    //}

    // 3) Allocate and register our frame buffer
    //buffer_ = static_cast<uint8_t*>(std::malloc(bufferSize_));
    //if (!buffer_) {
    //    std::fprintf(stderr,
    //        "Error: malloc(%zu) failed\n",
    //        bufferSize_);
    //    return false;
    //}

    // 4) Initialize the OLED controller
    oled_.OLEDbegin(speed_, address_, debug_, flipped_);

    // 5) Clear the display RAM and our buffer
    oled_.OLEDSetBufferPtr(width_, height_, buffer_, bufferSize_);
    oled_.OLEDFillScreen(0x00, 0);
    oled_.OLEDclearBuffer();
    oled_.setTextColor(WHITE);

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
                                  const uint8_t* bitmap,
                                  bool invert) {
    oled_.OLEDBitmap(x, y, w, h, bitmap, invert);
}

void LinuxSSD1306Display::setCursor(int16_t x, int16_t y) {
    oled_.setCursor(x, y);
}

size_t LinuxSSD1306Display::print(const char* text) {
    return oled_.print(text);
}

bool LCD_INIT(LinuxSSD1306Display &lcd){
    return lcd.begin();
}

void LCD_CLEAR(LinuxSSD1306Display &lcd){
    lcd.clear();
}

void LCD_SET_CURSOR(LinuxSSD1306Display &lcd, uint8_t col, uint8_t row){
    lcd.setCursor((col),(row));
}
    
void LCD_SET_CURSOR_LINE(LinuxSSD1306Display &lcd, uint8_t col, uint8_t line){
    uint8_t row = line * (LCD_FONT_HEIGHT + LCD_LINE_SPACING);
    lcd.setCursor((col),(row));
}
    
void LCD_PRINT(LinuxSSD1306Display &lcd, const char* str){
    lcd.print((str));
}

void LCD_PRINT_NUMBER(LinuxSSD1306Display &lcd, int n){
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", n);
    const char* str = buf;
    lcd.print((str));
}
    
void LCD_DISPLAY(LinuxSSD1306Display &lcd){
    lcd.display();
}
    
void LCD_DRAW_XBM(LinuxSSD1306Display &lcd, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap, bool invert){
    lcd.drawXbm((x),(y),(w),(h),(bitmap),(invert));
}
