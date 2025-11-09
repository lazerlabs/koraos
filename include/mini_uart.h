#pragma once

#include "common.h"

void uart_init(void);
void uart_putc(unsigned char c);
unsigned char uart_getc(void);

void uart_puts(const char *str);
int uart_readline(char *buffer, int max_len);
