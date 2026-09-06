# CI/CD Pipeline Documentation

## Overview

This project includes a comprehensive GitHub Actions CI/CD pipeline that automatically:

- **Builds** the UF2 firmware for Raspberry Pi Pico
- **Lints** code for quality issues
- **Releases** binaries when you create a version tag
- **Reports** build status

## Pipeline Stages

### 1. Build Stage ⚙️

**Trigger:** Push to `main` or `develop`, PR, or manual dispatch

**What it does:**
1. Checks out code
2. Installs ARM GCC toolchain and CMake
3. Clones Pico SDK
4. Configures CMake build
5. Compiles firmware with optimizations
6. Verifies UF2 file generated
7. Uploads artifacts

**Artifacts stored:**
- `peanut_gb_emulator.uf2` (90 days)
- `peanut_gb_emulator.elf` (30 days) - for debugging

### 2. Lint Stage 🔍

**Trigger:** Same as Build

**Checks:**
- `cppcheck` - Static code analysis for bugs
- `clang-format` - Code formatting verification

**Note:** Lint issues don't block the build (non-fatal)

### 3. Release Stage 🚀

**Trigger:** When you create a git tag starting with `v` (e.g., `v1.0.0`, `v1.0.1`)

**What it does:**
1. Downloads UF2 and ELF artifacts
2. Creates a GitHub Release
3. Attaches the binaries
4. Generates release notes

## How to Use the Pipeline

### ✅ Automatic Builds (Every Push)

Every time you push to `main` or `develop`, the pipeline automatically builds:

```bash
# Make changes
git add .
git commit -m "Add feature X"
git push origin main
```

**To check the build:**
1. Go to your GitHub repo
2. Click **Actions** tab
3. Find your commit
4. Click to see detailed logs
5. Download `peanut_gb_emulator_uf2` artifact

### 📦 Creating a Release

When you're ready to release version 1.0.0:

```bash
# Tag your commit
git tag -a v1.0.0 -m "Release version 1.0.0"

# Push tag to GitHub
git push origin v1.0.0
```

**Pipeline will:**
1. Build the firmware
2. Create a Release in the **Releases** tab
3. Attach UF2 and ELF files
4. Users can download directly from Releases page

### 🎮 Manual Builds

To manually trigger a build without pushing:

1. Go to **Actions** tab
2. Click **Build Peanut-GB UF2 for RP2040**
3. Click **Run workflow** dropdown
4. Select your branch
5. Click **Run workflow**

The build starts immediately.

### 📅 Weekly Automatic Builds

The pipeline runs every Sunday at midnight UTC to verify the build stays working.

## Accessing Build Artifacts

### From Actions Tab (All Builds)

1. Go to **Actions** → **Build Peanut-GB UF2 for RP2040**
2. Click the latest workflow run
3. Scroll down to **Artifacts** section
4. Download `peanut_gb_emulator_uf2`

### From Releases Tab (Tagged Releases)

For official releases:

1. Go to **Releases** tab
2. Find your release (e.g., v1.0.0)
3. Download `peanut_gb_emulator.uf2` directly
4. Also includes `peanut_gb_emulator.elf` for debugging

## Configuration

### Branches

Workflows run on `main` and `develop`.

To add/remove branches, edit `.github/workflows/build.yml`:

```yaml
on:
  push:
    branches: [ main, develop, feature-branch ]  # Add here
```

### Schedule

Weekly build runs **Sunday at 00:00 UTC**.

To change, edit the cron expression:

```yaml
schedule:
  - cron: '0 0 * * 0'  # '0 0 * * 5' = Friday at midnight
```

[Cron syntax reference](https://crontab.guru/)

### Artifact Retention

- UF2: 90 days
- ELF: 30 days

To change, edit `retention-days` in build.yml.

## Build Status Badge

Add to README.md to show build status:

```markdown
![Build Status](https://github.com/Grid75/peanut-gb-rp2040-st7789v/workflows/Build%20Peanut-GB%20UF2%20for%20RP2040/badge.svg)
```

## Troubleshooting

### Build Failed ❌

1. Click the failed workflow run
2. Expand **Build Project** step
3. Look for error messages

**Common issues:**

| Error | Solution |
|-------|----------|
| SDK clone timeout | Retry - temporary network issue |
| CMake error | Check `CMakeLists.txt` syntax |
| Compilation error | Check C code for syntax errors |
| UF2 not created | Ensure `pico_add_uf2_output()` called |

### Artifacts Not Found

The UF2 might not be created if:
- Build failed (check logs)
- Output filename wrong in `CMakeLists.txt`
- `pico_add_uf2_output()` not called

### Release Not Created

Make sure:
1. Tag starts with `v` (e.g., `v1.0.0` not `1.0.0`)
2. Build passed before release stage
3. Artifacts uploaded successfully

## Workflow File Location

The workflow is stored at:
```
.github/workflows/build.yml
```

To modify it:
1. Go to this file on GitHub
2. Click edit icon (pencil)
3. Make changes
4. Commit directly to `main`

Changes take effect immediately for the next build.

## Performance

| Stage | Time |
|-------|------|
| First build | 8-10 min (SDK clone) |
| Subsequent builds | 3-5 min (cached) |
| Lint checks | 1 min |
| **Total** | **~10-15 min** |

## Example Release Notes

When you create tag `v1.0.0`, the release will look like:

```
Peanut-GB RP2040 Release v1.0.0

Ready-to-flash UF2 firmware for Raspberry Pi Pico with Waveshare GamePi13.

Installation
1. Hold BOOTSEL button on Pico
2. Connect USB
3. Copy peanut_gb_emulator.uf2 to RPI-RP2 drive

See SETUP.md for details.
```

## Next Steps

1. **Push to main** to trigger first build
2. **Check Actions tab** for build status
3. **Download UF2** from artifacts
4. **Flash to Pico** using BOOTSEL method
5. **Create tags** for releases: `git tag -a v1.0.0 -m "Release"`

## Environment Variables

Pipeline automatically sets:
- `PICO_SDK_PATH=$HOME/pico-sdk`
- `CMAKE_BUILD_TYPE=Release`
- `GITHUB_TOKEN` (for releases)

## Further Reading

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Pico SDK Build Guide](https://github.com/raspberrypi/pico-sdk)
- [UF2 Format Specification](https://github.com/microsoft/uf2)
