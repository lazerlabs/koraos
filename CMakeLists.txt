cmake_minimum_required(VERSION 3.20)

# Set the toolchain file before project()
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/aarch64-toolchain.cmake" CACHE PATH "Toolchain file")

project(KoraOS C ASM)

# Enable compile_commands.json generation for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build configuration
set(RPI_VERSION "4" CACHE STRING "Raspberry Pi version (3 or 4)")
set(BOOTMNT "/workspace/build/boot" CACHE PATH "Boot mount directory")

# Directories
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LINKER_SCRIPT "${SRC_DIR}/linker.ld")

# Common compiler flags
set(COMMON_FLAGS
    --target=aarch64-none-elf
    -mcpu=cortex-a72
    -ffreestanding
    -nostdlib
    -fno-builtin
    -fno-stack-protector
)

# C-specific flags
set(C_FLAGS
    ${COMMON_FLAGS}
    -Wall
    -mgeneral-regs-only
    -DRPI_VERSION=${RPI_VERSION}
    -DQEMU_TESTING
)

# Assembly flags
set(ASM_FLAGS
    ${COMMON_FLAGS}
)

# Collect source files
file(GLOB C_SOURCES "${SRC_DIR}/*.c")
file(GLOB ASM_SOURCES "${SRC_DIR}/*.S" "${SRC_DIR}/*.s")

# Create kernel executable
add_executable(kernel8.elf
    ${C_SOURCES}
    ${ASM_SOURCES}
)

# Set target properties
set_target_properties(kernel8.elf PROPERTIES
    SUFFIX ""
    LINK_DEPENDS ${LINKER_SCRIPT}
)

# Include directories
target_include_directories(kernel8.elf PRIVATE ${INCLUDE_DIR})

# Apply compiler flags
target_compile_options(kernel8.elf PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${C_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${ASM_FLAGS}>
)

# Linker flags
target_link_options(kernel8.elf PRIVATE
    ${COMMON_FLAGS}
    -Wl,-T,${LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_BINARY_DIR}/kernel8.map
)

# Create boot directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/boot)

# Custom command to convert ELF to binary image
add_custom_command(TARGET kernel8.elf POST_BUILD
    COMMAND llvm-objcopy -O binary $<TARGET_FILE:kernel8.elf> ${CMAKE_BINARY_DIR}/kernel8.img
    COMMENT "Creating kernel8.img binary"
    BYPRODUCTS ${CMAKE_BINARY_DIR}/kernel8.img
)

# Optional: Custom target to copy to boot mount
# This is a separate target so the build doesn't fail if BOOTMNT doesn't exist
add_custom_target(install_kernel
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BOOTMNT}
    COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_BINARY_DIR}/kernel8.img 
        ${BOOTMNT}/kernel8-rpi${RPI_VERSION}.img
    COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/config.txt 
        ${BOOTMNT}/config.txt
    DEPENDS kernel8.elf
    COMMENT "Copying kernel to boot mount: ${BOOTMNT}"
)

# Also create a local boot directory copy for convenience
add_custom_command(TARGET kernel8.elf POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/boot
    COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_BINARY_DIR}/kernel8.img 
        ${CMAKE_BINARY_DIR}/boot/kernel8-rpi${RPI_VERSION}.img
    COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/config.txt 
        ${CMAKE_BINARY_DIR}/boot/config.txt
    COMMENT "Copying kernel to local boot directory"
)

# Print configuration
message(STATUS "Building for Raspberry Pi ${RPI_VERSION}")
message(STATUS "Boot mount directory: ${BOOTMNT}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Linker script: ${LINKER_SCRIPT}")

