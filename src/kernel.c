#include "mini_uart.h"
#include "console.h"

void kernel_main(void) {
    uart_init();
    uart_putc('K');
    uart_puts("Hello, world!\n");

    console_init(); 
    console_run();

}
