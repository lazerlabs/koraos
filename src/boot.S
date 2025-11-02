.global _start

.section .text

.equ CORE_STACK_BASE,  0x00008000
.equ CORE_CANARY_ADDR, 0x00001000
.equ CORE_CANARY_VALUE, 0xDEADC0DE
.equ CORE_ID_MASK,     0xFF

_start:
    // Park secondary cores
    mrs x1, MPIDR_EL1
    and x1, x1, #CORE_ID_MASK
    cbz x1, primary_core
    b CORE_PARK

primary_core:
    // Set up stack pointer
    ldr x0, =stack_top
    mov sp, x0

    // Clear BSS
    ldr x10, =__bss_start
    ldr x11, =__bss_end
clear_bss:
    cmp x10, x11
    b.hs bss_cleared
    str xzr, [x10], #8
    b clear_bss

bss_cleared:
    // Jump to kernel main
    bl kernel_main
    // If kernel_main returns, hang
    b hang

CORE_PARK:
    mov x9, x1
SECONDARY_WAIT:
    wfi
    b SECONDARY_WAIT

hang:
    wfi
    b hang
