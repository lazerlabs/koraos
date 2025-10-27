#!/bin/bash
# Build script for KoraOS using CMake

# Configuration
BUILD_DIR="build"
RPI_VERSION="${RPI_VERSION:-4}"
BOOTMNT="${BOOTMNT:-/workspace/build/boot}"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DRPI_VERSION="$RPI_VERSION" \
    -DBOOTMNT="$BOOTMNT" \
    -G "Unix Makefiles"

# Build
cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Build complete!"
echo "Kernel image: $BUILD_DIR/kernel8.img"
echo "compile_commands.json generated for clangd"

