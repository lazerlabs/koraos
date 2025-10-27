# Setting Up clangd for KoraOS

## Quick Start

1. **Build the project to generate compile_commands.json:**
   ```bash
   ./build.sh
   ```

2. **Reload your editor/IDE** (or restart the clangd language server)

3. **Verify it's working:**
   - Open any C file (e.g., `src/kernel.c`)
   - Check that clangd no longer shows false errors
   - Hover over types and functions - you should see proper type information

## What Was Done

### Files Created:
1. **CMakeLists.txt** - Main CMake build configuration
2. **cmake/aarch64-toolchain.cmake** - Cross-compilation toolchain settings
3. **.clangd** - Configuration file telling clangd where to find compile_commands.json
4. **build.sh** - Convenient build script
5. **CMAKE_README.md** - Detailed CMake documentation

### Files Modified:
1. **src/linker.ld** - Updated to work with both Make and CMake object file naming

## How It Works

1. **CMake generates `compile_commands.json`** during the build process
2. **clangd reads this file** to understand:
   - Target architecture (aarch64-none-elf)
   - Compiler flags (-ffreestanding, -nostdlib, etc.)
   - Include paths
   - Preprocessor definitions (RPI_VERSION, QEMU_TESTING)
3. **Your IDE/editor** uses clangd to provide:
   - Accurate error checking
   - Code completion
   - Go-to-definition
   - Refactoring tools

## Verifying It Works

Check that these files exist after building:
```bash
ls -lh build/compile_commands.json
ls -lh build/kernel8.img
ls -lh build/kernel8.elf
```

View the compile commands:
```bash
cat build/compile_commands.json
```

You should see entries for each source file with proper flags like:
- `--target=aarch64-none-elf`
- `-mcpu=cortex-a72`
- `-I/Users/spider/dev/kora/koraos/include`

## Editor-Specific Setup

### VS Code
1. Install the "clangd" extension
2. Disable C/C++ IntelliSense: Add to settings.json:
   ```json
   {
     "C_Cpp.intelliSenseEngine": "Disabled",
     "clangd.path": "/opt/homebrew/opt/llvm/bin/clangd"
   }
   ```
3. Reload window (Cmd+Shift+P → "Developer: Reload Window")

### Neovim
With nvim-lspconfig:
```lua
require'lspconfig'.clangd.setup{
  cmd = {
    "/opt/homebrew/opt/llvm/bin/clangd",
    "--compile-commands-dir=build"
  }
}
```

### Other Editors
Most modern editors with clangd support will automatically detect `compile_commands.json` in the build directory thanks to the `.clangd` config file.

## Troubleshooting

### "Unknown type name" errors persist
- Make sure you've built at least once: `./build.sh`
- Restart your editor completely
- Check clangd logs for errors

### clangd can't find headers
- Verify `compile_commands.json` contains `-I/Users/spider/dev/kora/koraos/include`
- Check that the include directory exists

### Build fails with linker errors
- Make sure LLVM toolchain is installed: `brew install llvm`
- Verify ld.lld or clang is available: `which ld.lld`

## Using Both Make and CMake

Both build systems work independently:

**Make (original):**
```bash
make clean
make
```

**CMake (new):**
```bash
./build.sh
# or manually:
rm -rf build && mkdir build && cd build && cmake .. && cmake --build .
```

**For clangd to work, you must use CMake** at least once to generate `compile_commands.json`.

## Install to Boot Mount

To copy the kernel to a specific boot mount:
```bash
cd build
cmake --build . --target install_kernel
```

Or set BOOTMNT when building:
```bash
BOOTMNT=/mnt/boot ./build.sh
cd build && cmake --build . --target install_kernel
```

