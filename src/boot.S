.global _start

.section .text
.org 0x80000

.equ CORE_STACK_BASE,  0x00008000
.equ CORE_CANARY_ADDR, 0x00001000
.equ CORE_CANARY_VALUE, 0xDEADC0DE
.equ CORE_ID_MASK,     0xFF

_start:
    mrs x1, MPIDR_EL1
    and x1, x1, #CORE_ID_MASK
    cmp x1, #0
    b.ne CORE_PARK

    mov x0, #0
    orr x0, x0, #(1 << 10)
    orr x0, x0, #1
    msr SCR_EL3, x0
    isb

    mov x0, #1
    lsl x0, x0, #31
    msr HCR_EL2, x0
    isb

    mov x0, #CORE_STACK_BASE
    msr SP_EL1, x0

    mov x0, #0x3c5
    msr SPSR_EL3, x0

    adr x0, EL1_START
    msr ELR_EL3, x0
    eret

CORE_PARK:
    mov x9, x1
SECONDARY_WAIT:
    wfi
    b SECONDARY_WAIT

EL1_START:
    movz x9, #0xC0DE
    movk x9, #0xDEAD, lsl #16

    ldr x0, =CORE_CANARY_ADDR
    str w9, [x0]

    dsb sy
    isb

    ldr x10, =__bss_start
    ldr x11, =__bss_end
clear_bss:
    cmp x10, x11
    b.hs bss_cleared
    str xzr, [x10], #8
    b clear_bss

bss_cleared:

    b kernel_main
