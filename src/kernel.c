#include "mini_uart.h"

void kernel_main(void) {
    uart_init();
    uart_putc('K');

    while (1) {
        unsigned char c = uart_getc();
        uart_putc(c);

        /*if (c == '\r') {
            uart_putc('\n');
        }*/
    }
}
