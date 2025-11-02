# Quick Start - CMake + clangd for KoraOS

## 🎯 Goal Achieved
✅ clangd/LSP now recognizes the build correctly  
✅ No more false errors in your editor  
✅ Full code intelligence (completion, go-to-definition, refactoring)

## 🚀 To Fix LSP Errors (3 steps)

```bash
# 1. Build with CMake (generates compile_commands.json)
./build.sh

# 2. Reload your editor (or restart clangd)
# VS Code: Cmd+Shift+P → "Developer: Reload Window"
# Neovim: :LspRestart
# Other: Just restart the editor

# 3. Open any .c file and verify errors are gone!
```

## 📁 What Was Created

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | CMake build configuration |
| `cmake/aarch64-toolchain.cmake` | Cross-compilation setup |
| `.clangd` | Points clangd to compile_commands.json |
| `build.sh` | Easy build script |
| `build/compile_commands.json` | ⭐ The magic file clangd needs |

## 🔧 Building

```bash
# Quick build (builds QEMU + hardware images)
./build.sh

# Only build the QEMU testing image
./build.sh --qemu

# Hardware-focused build with Release optimizations
./build.sh --variant hw --release

# Or use CMake directly
mkdir -p build && cd build
cmake ..
cmake --build .
```

## ✅ Verification

```bash
# Check the key file exists
ls -lh build/compile_commands.json
# Should show: 3.5K file

# Check kernel built
ls -lh build/kernel8.img
# Should show: 37M file

# View what clangd sees
head build/compile_commands.json
# Should show compiler flags with --target=aarch64-none-elf
```

## 📚 More Information

- **SETUP_CLANGD.md** - Detailed setup and troubleshooting
- **CMAKE_README.md** - Full CMake documentation
- **MIGRATION_SUMMARY.md** - Complete technical details

## 💡 Tips

- The original Makefile still works independently
- CMake and Make can coexist peacefully
- Always build with CMake at least once to generate compile_commands.json
- If you change compiler flags, rebuild with CMake to update compile_commands.json
- Both build systems produce identical kernel images

## 🐛 Troubleshooting

**Still seeing errors?**
1. Make sure `build/compile_commands.json` exists
2. Completely restart your editor (not just reload)
3. Check clangd is using the right clang: `/opt/homebrew/opt/llvm/bin/clangd`

**Build failed?**
```bash
# Install LLVM if missing
brew install llvm cmake

# Verify tools are available
which clang ld.lld llvm-objcopy
```

## 🎉 Success Indicators

When working correctly, you should see:
- ✅ No red squiggles on `#include "common.h"`
- ✅ Hover over `uart_puts` shows function signature
- ✅ Cmd/Ctrl+Click on function names jumps to definition
- ✅ Code completion suggests functions from headers
- ✅ No errors about "unknown type name" for your types

---

**Ready to go?** Just run `./build.sh` and reload your editor! 🚀

