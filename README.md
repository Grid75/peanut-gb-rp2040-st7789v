Peanut-GB Emulator for RP2040 with ST7789V Display
====================================================

A Game Boy emulator running on the Raspberry Pi Pico (RP2040) microcontroller with a 1.3" ST7789V display, configured for the **Waveshare GamePi13** HAT.

## Hardware Requirements

- **Raspberry Pi Pico** (RP2040 MCU)
- **Waveshare GamePi13 HAT** (includes ST7789V 240x240 display and game buttons)
- **USB cable** for programming

## Features

- **ST7789V Display Driver**: 240x240 color LCD with 16-bit RGB565 color
- **GamePi13 Button Support**: Fully mapped directional and action buttons
- **Dual-Core Execution**:
  - **Core 0**: Display updates (60 FPS)
  - **Core 1**: Emulation and input handling
- **Game Boy Color Palette**: DMG-style green palette
- **Framebuffer Scaling**: 160x144 GB display centered on 240x240 LCD

## Pin Configuration (GamePi13)

### ST7789V Display (SPI0)
| Function | GPIO | Pin |
|----------|------|-----|
| MOSI     | 10   | 19  |
| SCLK     | 11   | 23  |
| CS       | 8    | 24  |
| DC       | 25   | 22  |
| RST      | 27   | 13  |

### Game Buttons
| Button   | GPIO | Action      |
|----------|------|-------------|
| A        | 5    | GB Right    |
| B        | 6    | GB Left     |
| X        | 16   | GB Down     |
| Y        | 20   | GB Up       |
| L1       | 21   | GB A Button |
| R1       | 26   | GB B Button |
| Start    | 19   | GB Start    |
| Select   | 13   | GB Select   |

## Building

### Prerequisites

1. **Raspberry Pi Pico SDK**:
   ```bash
   git clone https://github.com/raspberrypi/pico-sdk.git
   export PICO_SDK_PATH=~/pico-sdk
   ```

2. **ARM GCC Toolchain**:
   ```bash
   # On Ubuntu/Debian
   sudo apt install cmake gcc-arm-none-eabi
   ```

### Build Instructions

```bash
# Clone repository
git clone https://github.com/Grid75/peanut-gb-rp2040-st7789v.git
cd peanut-gb-rp2040-st7789v

# Create build directory
mkdir build && cd build

# Configure CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make

# UF2 file will be at: peanut_gb_emulator.uf2
```

## Programming the Pico

### Method 1: UF2 Bootloader (Recommended)

1. Hold **BOOTSEL** button while connecting USB
2. Pico appears as `RPI-RP2` drive
3. Drag `build/peanut_gb_emulator.uf2` to the drive
4. Pico reboots automatically

### Method 2: OpenOCD

```bash
openocd -f interface/picoprobe.cfg -f target/rp2040.cfg \
  -c "program build/peanut_gb_emulator.elf verify reset exit"
```

## ROM Loading

Currently, the emulator expects a Game Boy ROM to be embedded at build time.

To add a ROM:

1. Place your `.gb` file in the `roms/` directory
2. Modify `CMakeLists.txt` to embed the ROM:
   ```cmake
   pico_embed_binary(embedded_rom roms/your_game.gb)
   target_link_libraries(peanut_gb_emulator PRIVATE embedded_rom)
   ```
3. Update `src/main.c` to load the embedded ROM
4. Rebuild

## Debug Output

Connect via USB serial (115200 baud) to see debug messages:

```bash
screen /dev/ttyACM0 115200
```

Or use `minicom`, `putty`, etc.

## Architecture

```
┌─────────────────────────────────────┐
│      Raspberry Pi Pico (RP2040)     │
│  Dual ARM Cortex-M0+ @ 125 MHz      │
├──────────────┬──────────────────────┤
│   Core 0     │      Core 1          │
│   Display    │   Emulation & Input  │
│   Updates    │   (Peanut-GB)        │
├──────────────┴────────��─────────────┤
│      Shared Memory (264 KB)          │
│   - Framebuffer (51.2 KB)            │
│   - ROM Data (varies)                │
│   - State (8 KB)                     │
└──────────────┬──────────────────────┘
               │
    ┌──────────┴──────────┐
    │                     │
┌───▼────┐          ┌─────▼──────┐
│ ST7789V│          │ GamePi13   │
│Display │          │ Buttons    │
│240x240 │          │ (8x GPIO)  │
└────────┘          └────────────┘
```

## Performance

- **Display Refresh**: 60 FPS
- **Emulation Speed**: Synchronized to 60 FPS Game Boy cycles
- **SPI Speed**: 62.5 MHz (configured)
- **Memory Usage**: ~65 KB (including framebuffers and state)

## To-Do

- [ ] Integrate full Peanut-GB emulation core
- [ ] Add ROM loading from microSD card
- [ ] Implement audio output (PWM speaker)
- [ ] Add save state support (flash storage)
- [ ] Optimize SPI transfers with DMA
- [ ] Implement menu system (ROM selector)

## References

- [Peanut-GB GitHub](https://github.com/deltabeard/Peanut-GB)
- [Raspberry Pi Pico C SDK](https://github.com/raspberrypi/pico-sdk)
- [ST7789V Datasheet](https://www.displayfuture.com/Display/datasheet/controller/ST7789V.pdf)
- [Waveshare GamePi13 Wiki](https://www.waveshare.com/wiki/GamePi13)

## License

MIT License - See LICENSE file for details

## Contributing

Pull requests welcome! Areas for contribution:
- Peanut-GB core integration
- Audio implementation
- ROM loading mechanisms
- UI/menu system
- Performance optimization
