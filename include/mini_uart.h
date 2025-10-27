#pragma once

void uart_init();
char uart_recv();
void uart_send(char c);
void uart_send_string(char *str);


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

#define UART0_BASE 0xFE201000

uint64_t get_uart_base();
void enable_uart();
void uart_puts(const char *s);
void uart_putc(const char c);
void uart_puthex(uint64_t value);

void uart_raw_putc(const char c);
void uart_raw_puts(const char *s);

#ifdef __cplusplus
}
#endif
