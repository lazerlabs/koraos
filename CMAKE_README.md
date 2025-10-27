# CMake Build System for KoraOS

This project now supports CMake in addition to Make, which provides better IDE/LSP integration.

## Why CMake?

CMake generates a `compile_commands.json` file that clangd uses to understand your build configuration. This eliminates false errors in your IDE/LSP.

## Prerequisites

- **Clang/LLVM toolchain** with support for `aarch64-none-elf`
- **CMake** 3.20 or later
- **ld.lld** (LLVM linker) or clang with lld support
- **llvm-objcopy** for creating binary images

### macOS Installation
```bash
brew install cmake llvm
```

### Linux Installation
```bash
# Ubuntu/Debian
sudo apt install cmake clang lld llvm

# Arch Linux
sudo pacman -S cmake clang lld llvm
```

## Building

### Quick Build
```bash
./build.sh
```

### Manual Build
```bash
# Create build directory
mkdir -p build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build .
```

### Build Options

You can customize the build with these options:

```bash
cmake .. \
    -DRPI_VERSION=4 \
    -DBOOTMNT=/path/to/boot/mount
```

Or set environment variables:
```bash
RPI_VERSION=4 BOOTMNT=/mnt/boot ./build.sh
```

## IDE/LSP Configuration

### For clangd

The `.clangd` configuration file is already set up to use `compile_commands.json` from the build directory.

**Steps:**
1. Build the project at least once: `./build.sh`
2. Reload your IDE/editor (or restart clangd)
3. clangd will now use the correct flags from `build/compile_commands.json`

### VS Code
If using VS Code with clangd extension:
1. Install the "clangd" extension
2. Disable the default C/C++ IntelliSense: 
   - Set `"C_Cpp.intelliSenseEngine": "Disabled"` in settings
3. Build the project to generate `compile_commands.json`
4. Reload the window

### Other Editors
Most modern editors with clangd support will automatically detect `compile_commands.json` in the build directory.

## Output Files

After building, you'll find:
- `build/kernel8.elf` - ELF executable with debug symbols
- `build/kernel8.img` - Raw binary kernel image
- `build/kernel8.map` - Linker map file
- `build/compile_commands.json` - Compilation database for clangd
- `build/boot/kernel8-rpi4.img` - Kernel copied to boot directory

## Cleaning

```bash
rm -rf build
```

Or with Make-style command:
```bash
cmake --build build --target clean
```

## Using with Make

The original Makefile is still available. You can continue using it:
```bash
make
```

Both build systems are independent and can coexist.

## Troubleshooting

### clangd still showing errors

1. Make sure you've built the project at least once
2. Check that `build/compile_commands.json` exists
3. Restart your editor/IDE
4. Check clangd logs for errors

### Linker not found

If you get linker errors, make sure LLVM is installed and in your PATH:
```bash
# Check for ld.lld
which ld.lld

# Check for llvm-objcopy
which llvm-objcopy
```

On macOS with Homebrew:
```bash
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
```

### Wrong architecture errors

Make sure clang supports the `aarch64-none-elf` target:
```bash
clang --print-targets | grep aarch64
```

