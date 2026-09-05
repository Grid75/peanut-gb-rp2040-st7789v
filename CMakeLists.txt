cmake_minimum_required(VERSION 3.13)

project(peanut_gb_rp2040)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

pico_sdk_init()

# Main executable
add_executable(peanut_gb_emulator
    src/main.c
    src/display/st7789v.c
    src/input/buttons.c
    src/emulator/emulator.c
)

# Include directories
target_include_directories(peanut_gb_emulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Pico SDK libraries
target_link_libraries(peanut_gb_emulator
    pico_stdlib
    pico_multicore
    hardware_spi
    hardware_gpio
    hardware_dma
)

# Enable USB output (for debugging)
pico_enable_stdio_usb(peanut_gb_emulator 1)
pico_enable_stdio_uart(peanut_gb_emulator 0)

# Create UF2 binary
pico_add_uf2_output(peanut_gb_emulator)
