#include "display/st7789v.h"
#include "config.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <string.h>

static uint16_t frame_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];

// Helper: Write command byte
static void _write_command(uint8_t cmd) {
    gpio_put(PIN_DC, 0);  // DC = 0 for command
    spi_write_blocking(SPI_PORT, &cmd, 1);
}

// Helper: Write data byte(s)
static void _write_data(const uint8_t *data, uint32_t len) {
    gpio_put(PIN_DC, 1);  // DC = 1 for data
    spi_write_blocking(SPI_PORT, (uint8_t *)data, len);
}

// Initialize ST7789V display
void st7789v_init(void) {
    // Initialize SPI
    spi_init(SPI_PORT, SPI_BAUD);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);

    // Initialize control pins
    gpio_init(PIN_DC);
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_RST, GPIO_OUT);

    // Reset display
    gpio_put(PIN_RST, 0);
    sleep_ms(10);
    gpio_put(PIN_RST, 1);
    sleep_ms(120);

    // Initialize display commands
    _write_command(ST7789_SWRESET);
    sleep_ms(150);

    _write_command(ST7789_SLPOUT);
    sleep_ms(10);

    // Set color mode to 16-bit RGB565
    _write_command(ST7789_COLMOD);
    uint8_t colmod_data = 0x55;  // 16-bit color
    _write_data(&colmod_data, 1);
    sleep_ms(10);

    // Memory access control (normal orientation)
    _write_command(ST7789_MADCTL);
    uint8_t madctl_data = 0x00;
    _write_data(&madctl_data, 1);

    // Inversion on for better visibility
    _write_command(ST7789_INVON);

    // Turn on display
    _write_command(ST7789_DISPON);
    sleep_ms(10);

    // Clear display to black
    st7789v_fill(0x0000);
}

// Send command to display
void st7789v_cmd(uint8_t cmd) {
    _write_command(cmd);
}

// Send data to display
void st7789v_data(const uint8_t *data, uint32_t len) {
    _write_data(data, len);
}

// Set display window (for drawing region)
void st7789v_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column address set
    _write_command(ST7789_CASET);
    uint8_t caset_data[4] = {
        (x0 >> 8) & 0xFF, x0 & 0xFF,
        (x1 >> 8) & 0xFF, x1 & 0xFF
    };
    _write_data(caset_data, 4);

    // Row address set
    _write_command(ST7789_RASET);
    uint8_t raset_data[4] = {
        (y0 >> 8) & 0xFF, y0 & 0xFF,
        (y1 >> 8) & 0xFF, y1 & 0xFF
    };
    _write_data(raset_data, 4);
}

// Write frame buffer to display
void st7789v_write_frame(const uint16_t *frame_buffer) {
    // Set full window
    st7789v_set_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

    // Write RAM
    _write_command(ST7789_RAMWR);
    _write_data((const uint8_t *)frame_buffer, DISPLAY_WIDTH * DISPLAY_HEIGHT * 2);
}

// Convert RGB to RGB565
color_t color_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

// Set pixel color
void st7789v_set_pixel(uint16_t x, uint16_t y, color_t color) {
    if (x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT) {
        frame_buffer[y * DISPLAY_WIDTH + x] = color;
    }
}

// Fill display with solid color
void st7789v_fill(color_t color) {
    for (uint32_t i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        frame_buffer[i] = color;
    }
    st7789v_write_frame(frame_buffer);
}
