#include "common.h"
#include "mini_uart.h"

void kernel_main() {
    enable_uart();

    uart_puts("KorOS Initializing...\n");

#if RPI_VERSION == 3
    uart_puts("\tBoard: Raspberry PI 3\n");
#endif

#if RPI_VERSION == 4
    uart_puts("\tBoard: Raspberry PI 4\n");
#endif

    uart_puts("\n\nDone\n");

}

