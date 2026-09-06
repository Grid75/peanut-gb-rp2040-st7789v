#include "emulator/emulator.h"
#include "config.h"
#include "display/st7789v.h"
#include "pico/stdlib.h"

// Placeholder for Peanut-GB integration
// This is a minimal stub that will be filled in when Peanut-GB is integrated

static emulator_t emulator = {0};
static uint32_t frame_count = 0;
static uint16_t gb_framebuffer[GB_WIDTH * GB_HEIGHT] = {0};

// Game Boy color palette (DMG style - greenish)
static const color_t gb_palette[4] = {
    0xE0FF,  // Light - pale yellow
    0xA8D5,  // Light gray
    0x5285,  // Dark gray
    0x0000   // Black
};

// Initialize emulator
bool emulator_init(const uint8_t *rom_data, uint32_t rom_size) {
    if (!rom_data || rom_size == 0) {
        return false;
    }

    emulator.rom_data = (uint8_t *)rom_data;
    emulator.rom_size = rom_size;
    emulator.framebuffer = gb_framebuffer;
    emulator.gb_state = NULL;

    // TODO: Initialize Peanut-GB core
    // peanut_gb_init(&emulator.gb_state, rom_data, rom_size);

    frame_count = 0;

    return true;
}

// Run one frame of emulation
void emulator_run_frame(const gameboy_buttons_t *buttons) {
    // TODO: Update joypad input
    // peanut_gb_set_buttons(emulator.gb_state, buttons);

    // TODO: Run CPU cycles for one frame (~70224 cycles @ 60 FPS)
    // while (!peanut_gb_is_frame_ready(emulator.gb_state)) {
    //     peanut_gb_step(emulator.gb_state);
    // }

    // TODO: Get framebuffer from Peanut-GB
    // peanut_gb_get_framebuffer(emulator.gb_state, gb_framebuffer);

    frame_count++;
}

// Get current framebuffer
uint16_t *emulator_get_framebuffer(void) {
    return gb_framebuffer;
}

// Get frame count
uint32_t emulator_get_frame_count(void) {
    return frame_count;
}

// Cleanup emulator
void emulator_cleanup(void) {
    // TODO: Cleanup Peanut-GB
    // peanut_gb_cleanup(&emulator.gb_state);
}

// Helper: Convert GB framebuffer to display framebuffer with palette
// This scales/centers the 160x144 Game Boy display on the 240x240 ST7789V
void gb_to_display_framebuffer(uint16_t *display_fb, const uint8_t *gb_fb) {
    // Clear display buffer (black)
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        display_fb[i] = 0x0000;
    }

    // Copy GB framebuffer to center of display with palette conversion
    for (int y = 0; y < GB_HEIGHT; y++) {
        for (int x = 0; x < GB_WIDTH; x++) {
            // Get GB pixel color (0-3)
            uint8_t gb_color = gb_fb[y * GB_WIDTH + x];
            color_t color = gb_palette[gb_color & 3];

            // Write to centered position on display
            int disp_x = GB_X_OFFSET + x;
            int disp_y = GB_Y_OFFSET + y;
            display_fb[disp_y * DISPLAY_WIDTH + disp_x] = color;
        }
    }
}
