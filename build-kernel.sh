#!/bin/bash
# Simple build script for KoraOS kernel using GNU toolchain

set -e

# Toolchain configuration
TOOLCHAIN_PATH="/home/spider/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-elf/bin"
export PATH="$TOOLCHAIN_PATH:$PATH"


## With ARM GNU Toolchain
#CC=aarch64-none-elf-gcc
#LD=aarch64-none-elf-ld
#OBJCOPY=aarch64-none-elf-objcopy
# Build flags
#CFLAGS="-mcpu=cortex-a72 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -Wall -mgeneral-regs-only -Iinclude -DRPI_VERSION=4 -DQEMU_TESTING"

# With LLVM Toolchain
CC=clang
LD=ld.lld
OBJCOPY=llvm-objcopy

# Build flags
CFLAGS="--target=aarch64-none-elf -mcpu=cortex-a72 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -Wall -mgeneral-regs-only -Iinclude -DRPI_VERSION=4 -DQEMU_TESTING"   

LDFLAGS="-T src/linker.ld"

# Build directory
BUILD_DIR="build"
mkdir -p "$BUILD_DIR"

echo "Building KoraOS kernel..."

# Compile C files
echo "  Compiling C sources..."
for src in src/*.c; do
    obj="$BUILD_DIR/$(basename ${src%.c}.o)"
    echo "    $src -> $obj"
    $CC $CFLAGS -c "$src" -o "$obj"
done

# Compile assembly files
echo "  Compiling assembly sources..."
for src in src/*.S; do
    obj="$BUILD_DIR/$(basename ${src%.S}.o)"
    echo "    $src -> $obj"
    $CC $CFLAGS -c "$src" -o "$obj"
done

# Link
echo "  Linking..."
$LD $LDFLAGS -o "$BUILD_DIR/kernel8.elf" $BUILD_DIR/*.o

# Create binary
echo "  Creating binary image..."
$OBJCOPY "$BUILD_DIR/kernel8.elf" -O binary "$BUILD_DIR/kernel8.img"

SIZE=$(du -h "$BUILD_DIR/kernel8.img" | cut -f1)
echo ""
echo "✓ Build complete!"
echo "  Kernel image: $BUILD_DIR/kernel8.img ($SIZE)"
echo ""
echo "Run with: ./run-qemu.sh"

