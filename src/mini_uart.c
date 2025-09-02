#include "gpio.h"
#include "utils.h"
#include "mini_uart.h"

#ifdef QEMU_TESTING
#include "peripherals/pl011.h"
#else
#include "peripherals/aux.h"
#endif

#define TXD 14
#define RXD 15

void uart_init() {
#ifdef QEMU_TESTING
    // PL011 on GPIO14/15 uses ALT0
    gpio_pin_set_func(TXD, GFAlt0);
    gpio_pin_set_func(RXD, GFAlt0);
    gpio_pin_enable(TXD);
    gpio_pin_enable(RXD);

    // Disable UART before config
    REGS_PL011->cr = 0;
    // Clear interrupts
    REGS_PL011->icr = 0x7FF;

    // Baud = 115200 assuming 48MHz UARTCLK
    REGS_PL011->ibrd = 26; // integer divisor
    REGS_PL011->fbrd = 3;  // fractional divisor

    // 8N1, FIFO enabled
    REGS_PL011->lcrh = (1 << 4) | (3 << 5);

    // Mask all interrupts
    REGS_PL011->imsc = 0;

    // Enable UART, TX, RX
    REGS_PL011->cr = (1 << 0) | (1 << 8) | (1 << 9);
#else
    // Mini UART on GPIO14/15 uses ALT5
    gpio_pin_set_func(TXD, GFAlt5);
    gpio_pin_set_func(RXD, GFAlt5);

    gpio_pin_enable(TXD);
    gpio_pin_enable(RXD);

    REGS_AUX->enables = 1;
    REGS_AUX->mu_control = 0;
    REGS_AUX->mu_ier = 0;
    REGS_AUX->mu_lcr = 3;
    REGS_AUX->mu_mcr = 0;

#if RPI_VERSION == 3
    REGS_AUX->mu_baud_rate = 270; // = 115200 @ 250 Mhz
#endif

#if RPI_VERSION == 4
    REGS_AUX->mu_baud_rate = 541; // = 115200 @ 500 Mhz
#endif

    REGS_AUX->mu_control = 3;
#endif

    uart_send('\r');
    uart_send('\n');
    uart_send('\n');
}

void uart_send(char c) {
#ifdef QEMU_TESTING
    // Wait until TX FIFO not full
    while (REGS_PL011->fr & (1 << 5)) {
    }
    REGS_PL011->dr = (uint32_t)c;
#else
    while(!(REGS_AUX->mu_lsr & 0x20));
    REGS_AUX->mu_io = c;
#endif
}

char uart_recv() {
#ifdef QEMU_TESTING
    // Wait until RX FIFO not empty
    while (REGS_PL011->fr & (1 << 4)) {
    }
    return (char)(REGS_PL011->dr & 0xFF);
#else
    while(!(REGS_AUX->mu_lsr & 1));
    return REGS_AUX->mu_io & 0xFF;
#endif
}

void uart_send_string(char *str) {
    while(*str) {
        if (*str == '\n') {
            uart_send('\r');
        }
        uart_send(*str);
        str++;
    }
}
