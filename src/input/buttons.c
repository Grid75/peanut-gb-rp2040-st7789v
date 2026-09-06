#include "input/buttons.h"
#include "config.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

static uint8_t button_pins[] = {
    PIN_BTN_A, PIN_BTN_B, PIN_BTN_X, PIN_BTN_Y,
    PIN_BTN_L1, PIN_BTN_R1, PIN_BTN_START, PIN_BTN_SELECT
};

static uint32_t last_button_time[8] = {0};
static bool button_states[8] = {false};

// Initialize button inputs
void buttons_init(void) {
    for (int i = 0; i < 8; i++) {
        gpio_init(button_pins[i]);
        gpio_set_dir(button_pins[i], GPIO_IN);
        gpio_pull_up(button_pins[i]);  // Pull-up for active-low buttons
    }
}

// Check if button is held (raw read, no debounce)
bool button_held(uint8_t gpio_pin) {
    return !gpio_get(gpio_pin);  // Active low
}

// Check if button was just pressed (with debounce)
bool button_pressed(uint8_t gpio_pin) {
    if (button_held(gpio_pin)) {
        uint32_t now = to_ms_since_boot();
        // Find button index
        for (int i = 0; i < 8; i++) {
            if (button_pins[i] == gpio_pin) {
                if (!button_states[i] && (now - last_button_time[i]) > BUTTON_DEBOUNCE_MS) {
                    button_states[i] = true;
                    last_button_time[i] = now;
                    return true;
                }
                break;
            }
        }
    } else {
        // Reset state when button released
        for (int i = 0; i < 8; i++) {
            if (button_pins[i] == gpio_pin) {
                button_states[i] = false;
                break;
            }
        }
    }
    return false;
}

// Read current button state
gameboy_buttons_t buttons_read(void) {
    gameboy_buttons_t buttons = {0};

    buttons.up = button_held(GB_BTN_UP);
    buttons.down = button_held(GB_BTN_DOWN);
    buttons.left = button_held(GB_BTN_LEFT);
    buttons.right = button_held(GB_BTN_RIGHT);
    buttons.a = button_held(GB_BTN_A);
    buttons.b = button_held(GB_BTN_B);
    buttons.start = button_held(GB_BTN_START);
    buttons.select = button_held(GB_BTN_SELECT);

    return buttons;
}
