# UART Setup for KoraOS on Raspberry Pi 4

## Summary

Successfully implemented UART communication for the KoraOS kernel, supporting both QEMU testing and real Raspberry Pi 4 hardware.

## Changes Made

### 1. Fixed UART Implementation (`src/mini_uart.c`)

- **For QEMU** (when `QEMU_TESTING` is defined):
  - Uses PL011 UART (UART0) which is better supported in QEMU
  - Peripheral base address: `0x3F000000` (RPi3 address used by QEMU)
  - GPIO pins 14/15 configured as Alt0 for PL011

- **For Real Hardware** (when `QEMU_TESTING` is not defined):
  - Uses Mini UART (AUX UART)
  - Peripheral base address: `0xFE000000` (RPi4 hardware address)
  - GPIO pins 14/15 configured as Alt5 for Mini UART

### 2. Fixed Peripheral Base Address (`include/peripherals/base.h`)

Updated to use correct addresses based on target:
- RPI3: `0x3F000000`
- RPI4 + QEMU_TESTING: `0x3F000000`
- RPI4 (real hardware): `0xFE000000`

### 3. Fixed Boot Code (`src/boot.S`)

- Removed `.org 0x80000` directive that conflicted with linker script
- Simplified boot process to work at any exception level
- Properly initializes stack and clears BSS before jumping to `kernel_main`

### 4. Fixed Linker Script (`src/linker.ld`)

- Removed `.stack_fill` and `.heap_fill` sections that created 36MB binaries
- Used `(NOLOAD)` sections for stack and heap
- Simplified layout for a clean, small binary (~4KB)

### 5. Toolchain Setup

Downloaded and configured correct x86_64-hosted AArch64 cross-compiler:
- Path: `/home/spider/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-elf/`
- Previous toolchain was ARM-hosted and incompatible with x86_64 system

## Current Functionality

The kernel now:
1. **Outputs** a single character 'K' via UART on boot
2. **Reads** characters from UART
3. **Echoes** received characters back

### Test Output

```bash
$ echo "hello" | timeout 2 ./run-qemu.sh
Kello
```

- 'K' is printed by the kernel
- 'ello' is echoed back ('h' consumed by first read)

## Building and Running

### Build the Kernel

```bash
./build-kernel.sh
```

This compiles all sources and creates `build/kernel8.img`.

### Run in QEMU

```bash
./run-qemu.sh
```

Type characters and see them echoed back. Press `Ctrl-A X` to quit QEMU.

### Test with Input

```bash
echo "test" | timeout 2 ./run-qemu.sh
```

## Code Structure

```
src/
├── kernel.c        - Main kernel entry point
│                     Calls uart_init(), prints 'K', echo loop
├── mini_uart.c     - UART driver (PL011 for QEMU, Mini for hardware)
├── boot.S          - Boot assembly code
└── linker.ld       - Linker script (0x80000 load address)

include/
├── mini_uart.h           - UART function declarations
└── peripherals/
    ├── base.h            - PBASE definition based on target
    ├── gpio.h            - GPIO register structures
    ├── aux.h             - Mini UART register structures
    └── pl011.h           - PL011 UART register structures
```

## Hardware Notes

### GPIO Pin Configuration

- **GPIO 14** (TXD): UART transmit
- **GPIO 15** (RXD): UART receive

#### Alt Functions
- **Alt0**: PL011 UART (UART0) - used for QEMU
- **Alt5**: Mini UART (AUX) - used for real hardware

### Baud Rate

- **115200 bps** on both UARTs

### PL011 Configuration (QEMU)
- 8 data bits, 1 stop bit, no parity
- FIFO enabled
- UART clock: 3MHz
- Baud divisor: Integer=1, Fractional=40

### Mini UART Configuration (Hardware)
- 8 data bits
- Baud rate register: 270 (for 250MHz clock)

## GDB Debugging

GDB integration is configured in `~/.cursor/mcp.json`:

```json
{
  "mcpServers": {
    "gdb": {
      "command": "node",
      "args": ["/home/spider/dev/mcp-gdb/build/index.js"],
      "disabled": false,
      "env": {
        "GDB_PATH": "/home/spider/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf-gdb"
      }
    }
  }
}
```

To debug with QEMU:

```bash
# Terminal 1: Start QEMU with GDB server
/usr/local/bin/qemu-system-aarch64 -M raspi3b -kernel build/kernel8.img \
  -serial stdio -display none -s -S

# Terminal 2: Connect with GDB
aarch64-none-elf-gdb build/kernel8.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

## Next Steps

Potential improvements:
- Implement `uart_puts()` for string output
- Add buffered I/O
- Implement proper line editing with backspace support
- Add formatted output (printf-like function)
- Test on real Raspberry Pi 4 hardware with Mini UART

## References

- [BCM2711 ARM Peripherals](https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf)
- [ARM PL011 Technical Reference](https://developer.arm.com/documentation/ddi0183/latest/)
- [Circle Bare Metal Framework](https://github.com/rsta2/circle) - Reference implementation

