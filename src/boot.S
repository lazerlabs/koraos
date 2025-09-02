#include "mm.h"

// boot.S
.section ".text.boot"
.global _start

_start:
    // Ensure we're in EL1
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, master
    b       halt

master:
    ldr     x0, =bss_begin
    ldr     x1, =bss_end
    sub     x1, x1, x0
    bl      memzero

    mov     sp, #LOW_MEMORY

    // Record some debug breadcrumbs before handing off to C
    // x19: 'BOOT' marker, x20: core id, x21: __bss_start, x22: __bss_end, x23: bss_len, x24: stack ptr
    ldr     x19, =0x424F4F54          // 'BOOT'

    bl      kernel_main
    b       halt

halt:
    // Stamp registers so they are easy to spot in QEMU monitor (info registers)
    ldr     x19, =0x48414C54          // 'HALT'

    wfe
    b       halt
