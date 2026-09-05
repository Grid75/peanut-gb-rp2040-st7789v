#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "display/st7789v.h"
#include "input/buttons.h"
#include "emulator/emulator.h"
#include "config.h"
#include <stdio.h>

// Game Boy ROM data - TODO: Load from storage
// For now, a minimal 32KB template ROM
extern uint8_t embedded_rom[];
extern uint32_t embedded_rom_size;

// Frame timing
static uint32_t frame_time_us = 1000000 / FRAME_RATE;  // 16.67ms per frame @ 60 FPS

// Display update function (runs on core 0)
void core0_main(void) {
    printf("Core 0: Display thread started\n");

    uint16_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];

    while (1) {
        // Get current emulator framebuffer
        uint16_t *gb_fb = emulator_get_framebuffer();

        // Convert GB framebuffer (160x144) to display framebuffer (240x240)
        // with centering and palette conversion
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                // Check if pixel is in GB display area
                if (x >= GB_X_OFFSET && x < (GB_X_OFFSET + GB_WIDTH) &&
                    y >= GB_Y_OFFSET && y < (GB_Y_OFFSET + GB_HEIGHT)) {
                    // Copy from GB framebuffer
                    int gb_x = x - GB_X_OFFSET;
                    int gb_y = y - GB_Y_OFFSET;
                    display_buffer[y * DISPLAY_WIDTH + x] = gb_fb[gb_y * GB_WIDTH + gb_x];
                } else {
                    // Black border
                    display_buffer[y * DISPLAY_WIDTH + x] = 0x0000;
                }
            }
        }

        // Write to display
        st7789v_write_frame((const uint16_t *)display_buffer);

        sleep_us(frame_time_us);
    }
}

// Emulation function (runs on core 1)
void core1_main(void) {
    printf("Core 1: Emulator thread started\n");

    // Initialize buttons
    buttons_init();

    // Main emulation loop
    while (1) {
        uint64_t frame_start = time_us_64();

        // Read button input
        gameboy_buttons_t buttons = buttons_read();

        // Run one frame of emulation
        emulator_run_frame(&buttons);

        // Frame timing - maintain 60 FPS
        uint64_t frame_elapsed = time_us_64() - frame_start;
        if (frame_elapsed < frame_time_us) {
            sleep_us(frame_time_us - frame_elapsed);
        }
    }
}

int main() {
    stdio_init_all();
    printf("\n=== Peanut-GB RP2040 Emulator ===\n");
    printf("Initializing hardware...\n");

    // Initialize display
    printf("Initializing ST7789V display...\n");
    st7789v_init();
    st7789v_fill(0x0000);  // Clear to black

    // Show splash message
    printf("Display initialized!\n");

    // Load Game Boy ROM
    printf("Loading Game Boy ROM...\n");
    // TODO: Load ROM from flash or storage
    // For now, use embedded ROM
    if (!emulator_init(embedded_rom, embedded_rom_size)) {
        printf("ERROR: Failed to initialize emulator\n");
        st7789v_fill(0xF800);  // Red on error
        while (1) {
            tight_loop_contents();
        }
    }

    printf("Emulator initialized!\n");
    printf("Starting dual-core execution...\n");

    // Launch core 1 for emulation
    multicore_launch_core1(core1_main);

    // Run core 0 for display updates
    core0_main();

    return 0;
}

// Stub for embedded ROM - TODO: Replace with actual ROM data
uint8_t embedded_rom[] = {0};
uint32_t embedded_rom_size = 0;
