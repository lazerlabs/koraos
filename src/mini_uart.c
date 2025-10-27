#include "mini_uart.h"
#include "utils.h"

#define PBASE               0x7E000000UL

#define GPIO_BASE           (PBASE + 0x200000UL)
#define GPFSEL1             (GPIO_BASE + 0x04)
#define GPPUD               (GPIO_BASE + 0x94)
#define GPPUDCLK0           (GPIO_BASE + 0x98)

#define AUX_BASE            (PBASE + 0x215000UL)
#define AUX_ENABLES         (AUX_BASE + 0x04)
#define AUX_MU_IO_REG       (AUX_BASE + 0x40)
#define AUX_MU_IER_REG      (AUX_BASE + 0x44)
#define AUX_MU_IIR_REG      (AUX_BASE + 0x48)
#define AUX_MU_LCR_REG      (AUX_BASE + 0x4C)
#define AUX_MU_LSR_REG      (AUX_BASE + 0x54)
#define AUX_MU_CNTL_REG     (AUX_BASE + 0x60)
#define AUX_MU_BAUD_REG     (AUX_BASE + 0x68)

#define BAUD_RATE_51        51U
#define MU_LSR_RX_READY     (1U << 0)
#define MU_LSR_TX_EMPTY     (1U << 5)
#define MU_FIFO_CLEAR       0x06U
#define MU_8BIT_MODE        0x01U
#define MU_TX_RX_ENABLE     0x03U

static inline void mmio_write(uintptr_t addr, uint32_t value) {
    *(volatile uint32_t *)addr = value;
}

static inline uint32_t mmio_read(uintptr_t addr) {
    return *(volatile uint32_t *)addr;
}

void uart_init(void) {
    uint32_t selector;

    mmio_write(AUX_ENABLES, 0);
    mmio_write(AUX_MU_CNTL_REG, 0);
    mmio_write(AUX_MU_IER_REG, 0);

    selector = mmio_read(GPFSEL1);
    selector &= ~((7U << 12) | (7U << 15));
    selector |= (2U << 12) | (2U << 15);
    mmio_write(GPFSEL1, selector);

    mmio_write(GPPUD, 0);
    delay(150);
    mmio_write(GPPUDCLK0, (1U << 14) | (1U << 15));
    delay(150);
    mmio_write(GPPUDCLK0, 0);

    mmio_write(AUX_MU_IIR_REG, MU_FIFO_CLEAR);
    mmio_write(AUX_MU_LCR_REG, MU_8BIT_MODE);
    mmio_write(AUX_MU_BAUD_REG, BAUD_RATE_51);

    mmio_write(AUX_ENABLES, 1);
    mmio_write(AUX_MU_CNTL_REG, MU_TX_RX_ENABLE);
}

void uart_putc(unsigned char c) {
    while (!(mmio_read(AUX_MU_LSR_REG) & MU_LSR_TX_EMPTY)) {
    }

    mmio_write(AUX_MU_IO_REG, (uint32_t)c);
}

unsigned char uart_getc(void) {
    while (!(mmio_read(AUX_MU_LSR_REG) & MU_LSR_RX_READY)) {
    }

    return (unsigned char)(mmio_read(AUX_MU_IO_REG) & 0xFFU);
}
