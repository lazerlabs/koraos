#include "gpio.h"
#include "utils.h"
#include "mini_uart.h"

#include "memory_access.h"

#define UART0_DR   (UART0_BASE + 0x00)
#define UART0_FR   (UART0_BASE + 0x18)
#define UART0_IBRD (UART0_BASE + 0x24)
#define UART0_FBRD (UART0_BASE + 0x28)
#define UART0_LCRH (UART0_BASE + 0x2C)
#define UART0_CR   (UART0_BASE + 0x30)


uint64_t get_uart_base(){
    return UART0_BASE;
}

void enable_uart() {
    write32(UART0_CR, 0x0);

    write32(UART0_IBRD, 1);
    write32(UART0_FBRD, 40);

    write32(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    write32(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_raw_putc(const char c) {
    while (read32(UART0_FR) & (1 << 5));
    write32(UART0_DR, c);
}

void uart_putc(const char c){
    uart_raw_putc(c);
}

void uart_puts(const char *s) {
    uart_raw_puts(s);
}

void uart_raw_puts(const char *s) {
    while (*s != '\0') {
        uart_raw_putc(*s);
        s++;
    }
}

void uart_puthex(uint64_t value) {
    bool started = false;
    uart_raw_putc('0');
    uart_raw_putc('x');
    for (int i = 60; i >= 0; i -= 4) {
        uint8_t nibble = (value >> i) & 0xF;
        char curr_char = nibble < 10 ? '0' + nibble : 'A' + (nibble - 10);
        if (started || curr_char != '0' || i == 0) {
            started = true;
            uart_raw_putc(curr_char);
        }
    }
}





/*
  -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

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

*/
