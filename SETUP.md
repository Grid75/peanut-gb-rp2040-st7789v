# Setup Guide: Peanut-GB on RP2040 with GamePi13

This guide walks through setting up the development environment and building the UF2 file for the GamePi13.

## System Setup (Ubuntu/Debian)

### 1. Install Dependencies

```bash
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential pkg-config
```

### 2. Download and Setup Pico SDK

```bash
# Create a workspace
mkdir ~/pico_dev && cd ~/pico_dev

# Clone Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# Set environment variable (add to ~/.bashrc for persistence)
export PICO_SDK_PATH=~/pico_dev/pico-sdk
```

### 3. Clone the Emulator Project

```bash
cd ~/pico_dev
git clone https://github.com/Grid75/peanut-gb-rp2040-st7789v.git
cd peanut-gb-rp2040-st7789v
```

## Building the Project

### Build Steps

```bash
# Navigate to project directory
cd ~/pico_dev/peanut-gb-rp2040-st7789v

# Create and enter build directory
mkdir build && cd build

# Configure the build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compile
make -j4

# Check output
ls -lh peanut_gb_emulator.uf2
```

Expected output:
```
[100%] Built target peanut_gb_emulator.uf2
```

The UF2 file is now ready at `build/peanut_gb_emulator.uf2`

## Flashing to Raspberry Pi Pico

### Method 1: Bootsel Mode (Easiest)

1. **Prepare Pico for programming**:
   - Locate the BOOTSEL button on the Pico (small button near USB port)
   - Hold BOOTSEL button
   - Connect USB cable to Pico while holding BOOTSEL
   - Release BOOTSEL
   - Pico should appear as `RPI-RP2` drive in your file manager

2. **Copy UF2 file**:
   ```bash
   # USB drive is usually mounted at /media/$USER/RPI-RP2
   cp build/peanut_gb_emulator.uf2 /media/$USER/RPI-RP2/
   ```

3. **Verify**:
   - Pico will reboot automatically
   - Connect USB for serial output: `screen /dev/ttyACM0 115200`
   - You should see boot messages

### Method 2: Using picotool

```bash
# Install picotool
sudo apt install picotool

# Put Pico in bootsel mode
# Then flash:
picotool load build/peanut_gb_emulator.uf2 -v

# And optionally reboot
picotool reboot
```

### Method 3: Using OpenOCD (Requires JTAG/SWD debugger)

```bash
# Install OpenOCD
sudo apt install openocd

# Connect SWD debugger, then:
openocd -f interface/picoprobe.cfg -f target/rp2040.cfg \
  -c "program build/peanut_gb_emulator.elf verify reset exit"
```

## Verifying the Build

### Serial Debug Output

Connect via USB and monitor output:

```bash
# Using screen (default 115200 baud)
screen /dev/ttyACM0 115200

# Or using minicom
minicom -D /dev/ttyACM0 -b 115200

# Or using picocom
picocom -b 115200 /dev/ttyACM0
```

Expected output:
```
=== Peanut-GB RP2040 Emulator ===
Initializing hardware...
Initializing ST7789V display...
Display initialized!
Loading Game Boy ROM...
Emulator initialized!
Starting dual-core execution...
Core 0: Display thread started
Core 1: Emulator thread started
```

### Visual Verification

- Display should show black background (no ROM loaded yet)
- Buttons should respond (press any button and check console if needed)

## GamePi13 Hardware Verification

### Wiring Check

Verify all connections before first power-on:

```
GamePi13 Pin → RP2040 GPIO
─────────────────────────
Display:
  DIN (MOSI)  → GPIO 10 (Pin 19)
  CLK         → GPIO 11 (Pin 23)
  CS          → GPIO 8  (Pin 24)
  DC          → GPIO 25 (Pin 22)
  RST         → GPIO 27 (Pin 13)
  GND         → GND (Pin 3, 8, 13, 18, 23, 28, 33, 38)
  VCC         → 3V3 (Pin 36)

Buttons (all pull-up to 3V3):
  A       → GPIO 5  (Pin 7)
  B       → GPIO 6  (Pin 9)
  X       → GPIO 16 (Pin 21)
  Y       → GPIO 20 (Pin 26)
  L1      → GPIO 21 (Pin 27)
  R1      → GPIO 26 (Pin 31)
  Start   → GPIO 19 (Pin 25)
  Select  → GPIO 13 (Pin 17)
```

### Test ROM

For testing, you can create a minimal test ROM or use a small Game Boy game you own.

## Troubleshooting

### Issue: Pico doesn't appear as RPI-RP2 drive

**Solution**: 
- Ensure USB cable is connected
- Try holding BOOTSEL for 2-3 seconds
- Use a different USB cable or USB port
- Check if Pico shows up with `lsusb`

### Issue: Build fails with "pico-sdk not found"

**Solution**:
```bash
# Ensure environment variable is set
export PICO_SDK_PATH=/path/to/pico-sdk

# Or modify CMakeLists.txt to hardcode the path:
# set(PICO_SDK_PATH /path/to/pico-sdk)
```

### Issue: Display shows only black screen

**Solution**:
- Check ST7789V wiring (DC, RST pins critical)
- Verify SPI pins (GPIO 10, 11, 8)
- Check serial output for error messages
- Verify 3.3V power supply to display

### Issue: Buttons not responding

**Solution**:
- Check GPIO pin connections
- Verify pull-up configuration in code
- Test with `screen` to see debug output
- Use multimeter to verify button continuity

### Issue: UF2 file doesn't transfer

**Solution**:
- Check USB drive is writable: `touch /media/$USER/RPI-RP2/test.txt`
- Try reformatting RPI-RP2 drive (careful!)
- Use `dd` instead of cp: `dd if=peanut_gb_emulator.uf2 of=/media/$USER/RPI-RP2/peanut_gb_emulator.uf2 bs=1M`

## Adding a Game Boy ROM

### Embedding ROM at Build Time

1. **Place ROM file**:
   ```bash
   mkdir -p roms
   cp ~/Downloads/pokemon.gb roms/
   ```

2. **Modify CMakeLists.txt**:
   ```cmake
   # Add before target_link_libraries
   pico_embed_binary(game_rom roms/pokemon.gb)
   
   target_link_libraries(peanut_gb_emulator
       PRIVATE game_rom
   )
   ```

3. **Update src/main.c**:
   ```c
   extern uint8_t embedded_rom[];
   extern uint32_t embedded_rom_size;
   
   // In main():
   if (!emulator_init(embedded_rom, embedded_rom_size)) {
       // error handling
   }
   ```

4. **Rebuild**:
   ```bash
   cd build
   make clean
   make
   ```

## Next Steps

1. **Integrate Peanut-GB**: Download and integrate the actual Peanut-GB emulation core
2. **Test with Games**: Add Game Boy ROMs and verify emulation
3. **Optimize Performance**: Profile and optimize hot paths
4. **Add Audio**: Implement PWM audio output
5. **Save States**: Add flash-based save state support

## Resources

- [Pico SDK Getting Started](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Peanut-GB Documentation](https://github.com/deltabeard/Peanut-GB)
- [GamePi13 Wiki](https://www.waveshare.com/wiki/GamePi13)

## Support

- Check `SETUP.md` troubleshooting section
- Review build output for error messages
- Check serial debug output
- Search GitHub issues in the project repo
