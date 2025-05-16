// test_display.cpp
// Simple standalone test for the LinuxSSD1306Display wrapper

#include <cstdio>
#include <unistd.h>
#include "LinuxSSD1306Display.h"

#define OLED_WIDTH    128
#define OLED_HEIGHT   64
#define I2C_SPEED     BCM2835_I2C_CLOCK_DIVIDER_626
#define I2C_ADDRESS   0x3C

int main() {
    // Instantiate the display (width, height, I²C speed divider, I²C address)
    LinuxSSD1306Display lcd(OLED_WIDTH, OLED_HEIGHT, I2C_SPEED, I2C_ADDRESS);

    // Initialize in debug mode to see library/version output
    if (!LCD_INIT(lcd)) {
        std::fprintf(stderr, "ERROR: Failed to initialize OLED display\n");
        return 1;
    }

    // Clear the buffer, write a message, then push to screen
    LCD_CLEAR(lcd);
    LCD_SET_CURSOR(lcd, 10, 10);
    LCD_PRINT(lcd, "Hello OLED!");
    LCD_DISPLAY(lcd);
    
    std::fprintf(stderr, "Text output to OLED.  Pausing 5 seconds...\n");

    // Keep the message on-screen for 5 seconds
    sleep(5);

    return 0;
}
